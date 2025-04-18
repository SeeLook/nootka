/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "timportscore.h"
#include "score/tdummychord.h"
#include "score/tnoteitem.h"
#include "score/tscoreobject.h"
#include "tchunk.h"
#include "tmelody.h"

#include <QtCore/qthread.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlengine.h>
#include <QtQuick/qquickitem.h>

#include <QtCore/qdebug.h>

#define SIX_DUR (6) // duration of Sixteenth - lowest supported rhythmic value

TimportScore *TimportScore::m_instance = nullptr;
int TimportScore::m_splitEveryBarNr = 0;

TimportScore::TimportScore(const QString &xmlFileName, Tmelody *melody, QObject *parent)
    : QObject(parent)
    , m_melody(melody)
{
    if (m_instance) {
        qDebug() << "[TimportScore] instance already exists!";
        return;
    }
    m_instance = this;
    m_mainThread = thread();
    m_xmlThread = new TxmlThread(xmlFileName, melody);
    connect(m_xmlThread, &TxmlThread::musicXmlRead, this, &TimportScore::musicXmlReadySlot);
}

TimportScore::TimportScore(const QString &xmlFileName, QObject *parent)
    : QObject(parent)
{
    if (m_instance) {
        qDebug() << "[TimportScore] instance already exists!";
        return;
    }
    m_instance = this;
    m_mainThread = thread();
    m_xmlThread = new TxmlThread(xmlFileName);
    m_melody = m_xmlThread->mainMelody();
    connect(m_xmlThread, &TxmlThread::musicXmlRead, this, &TimportScore::musicXmlReadySlot);
}

TimportScore::~TimportScore()
{
    if (m_xmlThread)
        delete m_xmlThread;
    qDeleteAll(m_parts);
    m_instance = nullptr;
}

void TimportScore::runXmlThread()
{
    if (m_xmlThread) {
        /**
         * When @p Tmelody invokes @p TimportScore methods from another thread
         * @p TimportScore has to be also in this thread,
         * but after music XML is parsed @p TimportScore backs to main thread
         * due to QML requires @p TmelodyPart-s it there.
         */
        moveToThread(m_xmlThread->thread());
        m_xmlThread->start();
    }
}

void TimportScore::addNote(int partId, int staff, int voice, const Tchunk &note, bool skip)
{
    if (partId == m_parts.count() + 1)
        m_parts << new TmelodyPart(nullptr, partId, staff, voice);
    else if (partId > m_parts.count()) {
        qDebug() << "[TimportScore] Wrong parts order in the score file!" << partId << m_parts.count();
        return;
    }
    if (m_lastPart && partId > m_lastPart->part())
        copyBowings();
    auto staffPart = m_parts[partId - 1];
    if (staff == staffPart->count() + 1)
        staffPart->parts << new TmelodyPart(staffPart, partId, staff, voice);
    else if (staff > staffPart->count()) {
        qDebug() << "[TimportScore] Wrong staves order in the score file!" << staff << staffPart->count();
        return;
    }
    auto voicePart = staffPart->parts[staff - 1];
    if (voice > voicePart->count()) {
        while (voicePart->count() < voice) {
            auto p = new TmelodyPart(voicePart, partId, staff, voicePart->count() + 1);
            // Create part name text by using part name obtained from <part-list><score-part><part-name>
            // or just 'part number' if not that tag in musicXML.
            if (partId <= IMPORT_SCORE->partNames().size())
                p->setPartName(IMPORT_SCORE->partNames()[partId - 1]);
            if (p->partName().isEmpty())
                p->setPartName(tr("part", "it is a part of a musical score, like violin part or piano part, also called as 'voice'")
                               + QString(": %1").arg(partId));
            p->setPartName(p->partName() + QLatin1String(", ") + tr("staff") + QString(": %1").arg(staff) + QLatin1String(", ")
                           + tr("voice", "like in multivocal piece") + QString(": %1").arg(voice));
            voicePart->parts << p;
        }
    }

    if (!skip) {
        auto snippPart = voicePart->parts[voice - 1];
        if (snippPart->parts.isEmpty())
            snippPart->parts << new TmelodyPart(snippPart, partId, staff, voice);
        else {
            if (voicePart->splitBarNr() > 0 && snippPart->parts.last()->melody()->lastMeasure().isFull()
                && snippPart->parts.last()->melody()->measuresCount() % voicePart->splitBarNr() == 0)
                snippPart->parts << new TmelodyPart(snippPart, partId, staff, voice);
        }

        auto currSnipp = snippPart->parts.last();
        if (!currSnipp->melody()) {
            auto m = new Tmelody(m_melody->title(), m_melody->key());
            m->setComposer(m_melody->composer());
            m->setMeter(m_melody->meter()->meter());
            m->setTempo(m_melody->tempo());
            m->setClef(m_melody->clef());
            currSnipp->setMelody(m);
        }
        currSnipp->melody()->addNote(note);
        if (note.bowing()) {
            Tmeasure &lm = currSnipp->melody()->lastMeasure();
            // QRect(X: bar nr, Y: notes duration from the bar beginning, W: beaming code).
            m_bowings << QRect(lm.number() - 1, lm.duration() - note.duration(), static_cast<int>(note.bowing()), 0);
        }
        m_lastPart = currSnipp;
    }
    setHasMoreParts(partId > 1 || staff > 1 || voice > 1);
}

void TimportScore::addChordNote(const Tchunk &note)
{
    if (m_lastPart && m_lastPart->melody()) {
        m_lastPart->addChordNote(m_lastPart, note);
        setHasMoreParts(true);
    } else
        qDebug() << "[TimportScore] Cannot add chord note to not existing part/melody.";
}

void TimportScore::setUnsupported(int partId, int staff, int voice, int error)
{
    if (m_lastPart && m_lastPart->part() == partId && m_lastPart->staff() == staff && m_lastPart->voice() == voice)
        m_lastPart->setUnsupported(error);
    else
        qDebug() << "[TimportScore] part, staff or score mismatch with last part";
}

void TimportScore::sumarize()
{
    copyBowings();
    for (auto p : m_parts) {
        for (auto s : p->parts) {
            for (auto v : s->parts) {
                if (v->count())
                    m_partsModel << v;
            }
        }
    }
}

/**
 * @p splitNr = 0 means merge every voice snippets into single part
 */
void TimportScore::setSplitBarNr(int splitNr)
{
    if (splitNr != m_splitEveryBarNr || splitNr == 0) {
        m_splitEveryBarNr = splitNr;
        if (m_instance) {
            for (auto p : *m_instance->parts()) {
                for (auto s : p->parts) {
                    for (auto v : s->parts) {
                        if (v->count())
                            v->setSplitBarNr(m_splitEveryBarNr);
                    }
                }
            }
        }
    }
}

void TimportScore::selectNoteInChords(int noteNr, bool fromTop)
{
    for (auto p : m_parts) {
        for (auto s : p->parts) {
            for (auto v : s->parts) {
                if (v->count())
                    v->selectNoteInChords(noteNr, fromTop);
            }
        }
    }
}

void TimportScore::addPartName(const QString &pn)
{
    m_partNames << pn;
    if (m_partNames.count() == 2) // we are already sure that import dialog will be necessary
        setHasMoreParts(true);
}

void TimportScore::setContextObject(QObject *c)
{
    m_contextObj = c;
}

void TimportScore::keyChanged(const TkeySignature &newKey)
{
    setHasMoreParts(true);
    if (m_lastPart) {
        copyBowings();
        for (auto staffPart : m_parts[m_lastPart->part() - 1]->parts) { // split all staves
            for (auto voicePart : staffPart->parts) { // and all voices
                if (!voicePart->parts.isEmpty()) {
                    auto lastSnip = voicePart->parts.last();
                    if (lastSnip->melody() && lastSnip->melody()->length()) {
                        // when there are notes in last snipped melody - add new snipped to this voice
                        auto m = newSnippet(voicePart, lastSnip->part(), lastSnip->staff(), lastSnip->voice(), m_lastPart->melody());
                        m->setKey(newKey); // and set new key in its melody
                    } else // or just set new key in empty melody
                        lastSnip->melody()->setKey(newKey);
                }
            }
        }
    } else
        qDebug() << "[TimportScore] Something wrong with XML!";
}

void TimportScore::meterChanged(const Tmeter &newMeter)
{
    setHasMoreParts(true);
    if (m_lastPart) {
        copyBowings();
        for (auto staffPart : m_parts[m_lastPart->part() - 1]->parts) { // split all staves
            for (auto voicePart : staffPart->parts) { // and all voices
                if (!voicePart->parts.isEmpty()) {
                    auto lastSnip = voicePart->parts.last();
                    if (lastSnip->melody() && lastSnip->melody()->length()) {
                        // when there are notes in last snipped melody - add new snipped to this voice
                        auto m = newSnippet(voicePart, lastSnip->part(), lastSnip->staff(), lastSnip->voice(), m_lastPart->melody());
                        m->setMeter(newMeter.meter()); // and set new meter in its melody
                    } else // or just set new meter in empty melody
                        lastSnip->melody()->setMeter(newMeter.meter());
                }
            }
        }
    } else
        qDebug() << "[TimportScore] Something wrong with XML!";
}

void TimportScore::clefChanged(Tclef::EclefType newClef)
{
    setHasMoreParts(true);
    if (m_lastPart) {
        copyBowings();
        for (auto staffPart : m_parts[m_lastPart->part() - 1]->parts) { // split all staves
            for (auto voicePart : staffPart->parts) {
                if (!voicePart->parts.isEmpty()) {
                    auto lastSnip = voicePart->parts.last();
                    if (lastSnip->melody() && lastSnip->melody()->length()) {
                        // when there are notes in last snipped melody - add new snipped to this voice
                        auto m = newSnippet(voicePart, lastSnip->part(), lastSnip->staff(), lastSnip->voice(), m_lastPart->melody());
                        m->setClef(newClef); // and set new clef in its melody
                    } else // or just set new clef in empty melody
                        lastSnip->melody()->setClef(newClef);
                }
            }
        }
    } else
        qDebug() << "[TimportScore] Something wrong with XML!";
}

bool TimportScore::xmlReadFinished() const
{
    return m_xmlThread && m_xmlThread->isFinished();
}

void TimportScore::arpeggiateChords()
{
    for (auto p : m_parts) {
        for (auto s : p->parts) {
            for (auto v : s->parts) {
                if (v->count())
                    v->arpeggiateChords();
            }
        }
    }
}

void TimportScore::fillPartialBar(int partId)
{
    for (auto s : m_parts[partId - 1]->parts) {
        for (auto v : s->parts)
            v->fillPartialBar();
    }
}

// #################################################################################################
// ###################                PROTECTED         ############################################
// #################################################################################################

/**
 * Common routine which appends a new @p TmelodyPart to given @p voicePart
 * and adds melody to that new part, then returns pointer to it.
 */
Tmelody *TimportScore::newSnippet(TmelodyPart *voicePart, int partId, int staffNr, int voiceNr, Tmelody *melody)
{
    voicePart->parts << new TmelodyPart(voicePart, partId, staffNr, voiceNr);
    auto m = new Tmelody(melody->title(), melody->key());
    m->setComposer(melody->composer());
    m->setMeter(melody->meter()->meter());
    m->setTempo(melody->tempo());
    m->setClef(melody->clef());
    voicePart->parts.last()->setMelody(m);
    return m;
}

void TimportScore::setHasMoreParts(bool moreParts)
{
    if (!m_hasMoreParts && moreParts) {
        m_hasMoreParts = true;
        emit wantDialog();
    }
}

void TimportScore::copyBowings()
{
    if (m_lastPart && !m_bowings.isEmpty()) {
        auto p = m_parts[m_lastPart->part() - 1];
        for (auto s : p->parts) {
            // TODO: We are copying bowings for every staff!
            // It is valid for bandoneon but not for violin (so far unsupported)
            for (auto v : s->parts) {
                if (v->count()) {
                    auto snipp = v->parts.last();
                    for (QRect &bow : m_bowings) {
                        if (bow.x() < snipp->melody()->measuresCount()) { // X: bar number
                            Tmeasure &m = snipp->melody()->measure(bow.x());
                            int dur = 0;
                            for (int n = 0; n < m.count(); ++n) {
                                if (dur >= bow.y()) { // Y: duration where bowing occurs
                                    // width: bowing itself
                                    m.note(n).setBowing(static_cast<Ttechnical::EbowDirection>(bow.width()));
                                    break;
                                }
                                dur += m.note(n).duration();
                            }
                        }
                    }
                }
            }
        }
        m_bowings.clear();
    }
}

void TimportScore::musicXmlReadySlot()
{
    // All is back to main thread when parts are ready
    moveToThread(m_mainThread);
    for (auto p : m_parts) // parts have no parent, so move them as well
        p->moveToThread(m_mainThread);
    emit xmlWasRead();
}

// #################################################################################################
// ###################               TpartMelody        ############################################
// #################################################################################################

TmelodyPart::TmelodyPart(TmelodyPart *parent, int partId, int staffNr, int voiceNr)
    : QObject(parent)
    , m_partId(partId)
    , m_staffNr(staffNr)
    , m_voiceNr(voiceNr)
{
    m_splitBarNr = TimportScore::splitEveryBarNr();
}

TmelodyPart::~TmelodyPart()
{
    if (!chords.isEmpty()) {
        qDeleteAll(chords);
    }
    if (m_melody)
        delete m_melody;
    if (!parts.isEmpty()) {
        qDeleteAll(parts);
    }
}

void TmelodyPart::setMelody(Tmelody *m)
{
    if (m != m_melody) {
        m_melody = m;
        emit melodyChanged();
    }
}

void TmelodyPart::setScoreObject(TscoreObject *sObj)
{
    m_scoreObj = sObj;
    if (m_melody) {
        m_scoreObj->setMelody(m_melody);
        for (int c = 0; c < chords.count(); ++c) {
            auto noteSeg = m_scoreObj->note(chords[c]->noteNr());
            TdummyChord *chIt;
            if (chords[c]->dummyChord()) {
                chIt = chords[c]->dummyChord();
            } else {
                QQmlComponent comp(m_scoreObj->qmlEngine(), QUrl(QStringLiteral("qrc:/score/DummyChord.qml")));
                auto con = QQmlEngine::contextForObject(IMPORT_SCORE->contextObj());
                chIt = qobject_cast<TdummyChord *>(comp.create(con));
                chIt->setParent(IMPORT_SCORE);
            }
            chIt->setParentItem(noteSeg);
            chIt->setChord(chords[c]);
        }
    }
}

void TmelodyPart::setSelected(bool sel)
{
    if (sel != m_selected) {
        m_selected = sel;
        emit selectedChanged();
    }
}

void TmelodyPart::setSplitBarNr(int splitNr)
{
    if (splitNr != m_splitBarNr || (m_splitBarNr == 0 && parts.count() > 1)) {
        m_splitBarNr = splitNr;
        if (!parts.isEmpty() && parts.first()->melody()) {
            // append all melodies to the first one
            auto firstPart = parts.first();
            for (int m = 1; m < parts.size(); ++m) {
                auto p = parts[m];
                firstPart->melody()->appendMelody(p->melody());
                if (!p->chords.isEmpty()) {
                    int lastNoteNr = firstPart->melody()->length() - p->melody()->length();
                    for (auto ch : p->chords)
                        ch->setNoteNr(lastNoteNr + ch->noteNr());
                    while (!p->chords.isEmpty()) {
                        firstPart->chords.append(p->chords.takeFirst());
                        firstPart->chords.last()->part = firstPart;
                    }
                }
            }
            // delete the rest of the parts
            int partsCount = parts.count();
            for (int m = 1; m < partsCount; ++m)
                delete parts.takeLast();
            // then divide by new bars number. if any
            bool hasChords = !firstPart->chords.isEmpty();
            if (m_splitBarNr) {
                QList<Tmelody *> splitList;
                firstPart->melody()->split(m_splitBarNr, splitList);
                int len = firstPart->melody()->length();
                if (!splitList.isEmpty()) {
                    for (auto m : splitList) {
                        auto mp = new TmelodyPart(this, m_partId, m_staffNr, m_voiceNr);
                        parts << mp;
                        mp->setMelody(m);
                        // split chords into just divided parts
                        if (hasChords) {
                            int c = 0;
                            while (c < firstPart->chords.count()) {
                                int nr = firstPart->chords[c]->noteNr();
                                // lookup by note number a chord is attached to
                                if (nr >= len && nr < len + m->length()) {
                                    // take the chord from first part, add to actual one and fix attached note number
                                    mp->chords << firstPart->chords.takeAt(c);
                                    mp->chords.last()->setNoteNr(mp->chords.last()->noteNr() - len);
                                    mp->chords.last()->part = mp;
                                } else if (nr >= len + mp->melody()->length())
                                    break;
                                else
                                    c++;
                            }
                        }
                        len += m->length();
                    }
                }
            }
            emit melodyChanged();
        }
        emit splitBarNrChanged();
    }
}

int TmelodyPart::key() const
{
    if (m_melody)
        return static_cast<int>(m_melody->key().value());
    else if (!parts.isEmpty() && parts.first()->melody())
        return static_cast<int>(parts.first()->melody()->key().value());

    return 0;
}

void TmelodyPart::setKey(int k)
{
    if (m_melody)
        m_melody->setKey(TkeySignature(static_cast<char>(k)));
    else if (!parts.isEmpty()) {
        for (auto p : parts) {
            if (p->melody())
                p->melody()->setKey(TkeySignature(static_cast<char>(k)));
        }
    }
}

void TmelodyPart::addChordNote(TmelodyPart *part, const Tchunk &n)
{
    Tchunk chordNote(n);
    chordNote.p().setRhythm(Trhythm(Trhythm::NoRhythm));
    if (!chords.isEmpty() && chords.last()->noteNr() == part->melody()->length() - 1)
        chords.last()->add(chordNote);
    else {
        chords << new TalaChord(part);
        chords.last()->add(chordNote);
        auto m = chords.last()->notes();
        m->setClef(m_melody->clef());
        m->setKey(m_melody->key());
    }
}

void TmelodyPart::selectNoteInChords(int noteNr, bool fromTop)
{
    for (auto snippPart : parts) {
        if (!snippPart->chords.isEmpty()) {
            for (auto alaCh : snippPart->chords) {
                // We cannot be sure that notes in the cord are in order.
                // Actually they are but better safe than sorry, so:
                // 1. Create map <chromatic nr, note nr in chord>
                QMap<int, int> chromMap;
                for (int n = 0; n < alaCh->count(); ++n)
                    chromMap[alaCh->notes()->note(n)->p().chromatic()] = n;
                // 2. sort it by note pitch (obtained order is reversed (ascending)) and we want descending
                auto sorted = chromMap.values();
                // so swap meaning of fromTop variable
                int s = fromTop ? qMax(alaCh->count() - noteNr, 0) : qMin(noteNr, alaCh->count()) - 1;
                // 3. select note but filtered by sorted list
                alaCh->dummyChord()->setSelected(sorted[s]);
            }
        }
    }
}

void TmelodyPart::arpeggiateChord(TalaChord *alaChord)
{
    if (alaChord->notes()->note(0)->p().rhythm() == Trhythm::NoRhythm) {
        if (!alaChord->setRhythm())
            return;
    }

    int chordId = 0;
    // find ID number of the chord in the list
    for (int c = 0; c < chords.count(); ++c) {
        if (alaChord == chords[c]) {
            chordId = c;
            break;
        }
    }
    QList<Tchunk> chunks;
    alaChord->notes()->toList(chunks);
    // squeeze notes of this dummy chord into melody
    m_melody->swapWithNotes(alaChord->noteNr(), chunks);
    // then remove this dummy chord (from the list and its QML item)
    alaChord->dummyChord()->deleteLater();
    chords.removeAt(chordId);
    chordId--;
    if (m_scoreObj)
        m_scoreObj->setMelody(m_melody);
    for (int c = 0; c < chords.count(); ++c) {
        auto ch = chords[c];
        if (c > chordId) // fix note number ID of the chord after adding notes to the melody
            ch->setNoteNr(ch->noteNr() + chunks.count() - 1);
        if (m_scoreObj) // fix parent of TdummyChord after melody was reloaded
            ch->dummyChord()->setParentItem(m_scoreObj->note(ch->noteNr()));
    }
}

void TmelodyPart::arpeggiateChords()
{
    for (auto snipp : parts) {
        if (!snipp->chords.isEmpty()) {
            if (snipp->unsupported()) // skip unsupported
                continue;

            QList<TalaChord *> shortChords; // list of chords impossible to arpeggiate
            // Iterating backwards so appending chord notes doesn't affect note id number
            // of other chords - all occur earlier
            for (int c = snipp->chords.count() - 1; c >= 0; --c) {
                auto chord = snipp->chords[c];
                if (!chord->setRhythm()) {
                    shortChords.prepend(chord); // backward iteration - add at beginning
                    chord->dummyChord()->setParentItem(nullptr); // HACK parent TnoteItem will not be the same
                    continue;
                }
                QList<Tchunk> chunks;
                chord->notes()->toList(chunks);
                snipp->melody()->swapWithNotes(chord->noteNr(), chunks);
                delete chord->dummyChord();
                chord->setDummyChord(nullptr);
                if (!shortChords.isEmpty()) { // fix note ids
                    for (auto shortCh : shortChords)
                        shortCh->setNoteNr(shortCh->noteNr() + chunks.count() - 1);
                }
            }

            if (snipp->score())
                snipp->score()->setMelody(snipp->melody());
            if (shortChords.isEmpty()) {
                qDeleteAll(snipp->chords);
                snipp->chords.clear();
            } else {
                for (int c = 0; c < snipp->chords.count(); ++c) {
                    auto ch = snipp->chords[c];
                    if (ch->dummyChord()) {
                        if (snipp->score()) // fix parent of TdummyChord after melody was reloaded
                            ch->dummyChord()->setParentItem(snipp->score()->note(ch->noteNr()));
                    } else
                        delete snipp->chords.takeAt(c);
                }
            }
        }
    }
}

void TmelodyPart::fillPartialBar()
{
    for (auto snipp : parts) {
        if (snipp->melody()) {
            if (snipp->melody()->measuresCount() == 1) {
                Tmeasure &m = snipp->melody()->measure(0);
                auto rests = Trhythm::resolve(m.meter().duration() - m.duration());
                for (auto r : rests) {
                    r.setRest(true);
                    snipp->melody()->prepend(Tchunk(Tnote(0, 0, 0, r), Ttechnical(NO_TECHNICALS)));
                }
            }
        }
    }
}

QList<QObject *> TmelodyPart::snippets()
{
    QList<QObject *> s;
    for (auto p : parts)
        s << qobject_cast<QObject *>(p);
    return s;
}

// #################################################################################################
// ###################                TalaChord         ############################################
// #################################################################################################

TalaChord::TalaChord(TmelodyPart *mp)
{
    part = mp;
    if (mp->melody()) {
        m_noteNr = part->melody()->length() - 1;
        auto fChN = mp->melody()->note(m_noteNr);
        m_notes.addNote(Tchunk(Tnote(fChN->p(), Trhythm(Trhythm::NoRhythm)), fChN->t()));
    } else
        qDebug() << "[TalaChord] FIXME! No melody in the part!";
}

void TalaChord::setDummyChord(TdummyChord *dCh)
{
    m_dummyChord = dCh;
}

bool TalaChord::setRhythm()
{
    if (m_notes.note(0)->p().rhythm() != Trhythm::NoRhythm)
        return true; // all was set already

    if (!canArpeggiate())
        return false;

    int dur = part->melody()->note(m_noteNr)->p().duration();
    m_notes.setMeter(part->melody()->meter()->meter());
    int lastNoteDur = dur - (count() - 1) * SIX_DUR;
    auto notesAtEnd = Trhythm::resolve(lastNoteDur);
    for (int n = 0; n < count() - 1; ++n)
        m_notes.note(n)->p().setRhythm(Trhythm::Sixteenth);
    if (notesAtEnd.isEmpty()) {
        // error
    } else {
        auto last = m_notes.note(count() - 1);
        if (notesAtEnd.count() > 1)
            notesAtEnd.last().setTie(Trhythm::e_tieStart);
        last->p().setRhythm(notesAtEnd.last());
        for (int r = notesAtEnd.count() - 2; r >= 0; --r) {
            if (r == 0)
                notesAtEnd[r].setTie(Trhythm::e_tieEnd);
            else
                notesAtEnd[r].setTie(Trhythm::e_tieCont);
            m_notes.addNote(Tchunk(Tnote(last->p(), notesAtEnd[r]), last->t()));
        }
    }
    return true;
}

void TalaChord::arpeggiateChord()
{
    part->arpeggiateChord(this);
}

bool TalaChord::canArpeggiate() const
{
    return count() * SIX_DUR <= part->melody()->note(m_noteNr)->p().duration();
}

// #################################################################################################
// ###################                TxmlThread        ############################################
// #################################################################################################

TxmlThread::TxmlThread(const QString &xmlFileName, QObject *parent)
    : QObject(parent)
    , m_xmlFileName(xmlFileName)
    , m_thread(new QThread)
{
    m_melody = new Tmelody();
    m_melodyCreated = true;
    commonConstructor();
}

TxmlThread::TxmlThread(const QString &xmlFileName, Tmelody *m, QObject *parent)
    : QObject(parent)
    , m_xmlFileName(xmlFileName)
    , m_thread(new QThread)
{
    m_melody = m;
    commonConstructor();
}

TxmlThread::~TxmlThread()
{
    delete m_thread;
    if (m_melodyCreated)
        delete m_melody;
}

void TxmlThread::start()
{
    m_thread->start();
}

bool TxmlThread::isFinished() const
{
    return m_thread->isFinished();
}

void TxmlThread::commonConstructor()
{
    moveToThread(m_thread);
    connect(m_thread, &QThread::started, this, [=] {
        m_melody->grabFromMusicXml(m_xmlFileName);
        m_thread->quit();
    });
    connect(m_thread, &QThread::finished, this, &TxmlThread::musicXmlRead);
}
