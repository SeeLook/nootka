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
#include "tmelody.h"
#include "tchunk.h"
#include "score/tscoreobject.h"
#include "score/tnoteitem.h"
#include "score/tdummychord.h"

#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlengine.h>
#include <QtQuick/qquickitem.h>
#include <QtCore/qdebug.h>


TimportScore* TimportScore::m_instance = nullptr;
int           TimportScore::m_splitEveryBarNr = 0;


TimportScore::TimportScore(Tmelody* melody, QObject *parent) :
  QObject(parent),
  m_melody(melody)
{
  if (m_instance) {
    qDebug() << "[TimportScore] instance already exists!";
    return;
  }
  m_instance = this;
}


TimportScore::~TimportScore()
{
  qDeleteAll(m_parts);
  m_instance = nullptr;
}


void TimportScore::addNote(int partId, int staff, int voice, const Tchunk &note, bool skip) {
  if (partId == m_parts.count() + 1)
      m_parts << new TmelodyPart(nullptr, partId, staff, voice);
  else if (partId > m_parts.count()) {
      qDebug() << "[TimportScore] Wrong parts order in the score file!" << partId << m_parts.count();
      return;
  }
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
        p->setPartName(tr("part", "like part of a score") + QString(": %1").arg(partId));
      p->setPartName(p->partName() + QLatin1String(", ") + tr("staff") + QString(": %1").arg(staff)
                    + QLatin1String(", ") + tr("voice") + QString(": %1").arg(voice));
      voicePart->parts << p;
    }
  }

  if (!skip) {
    auto snippPart = voicePart->parts[voice - 1];
    if (snippPart->parts.isEmpty())
        snippPart->parts << new TmelodyPart(snippPart, partId, staff, voice);
    else {
      if (voicePart->splitBarNr() > 0 && snippPart->parts.last()->melody()->lastMeasure().isFull()
          && snippPart->parts.last()->melody()->measuresCount() % voicePart->splitBarNr() == 0) {
          snippPart->parts << new TmelodyPart(snippPart, partId, staff, voice);
        }
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
    m_lastPart = currSnipp;
  }
  if (!m_hasMoreParts)
    m_hasMoreParts = partId > 1 || staff > 1 || voice > 1;
}


void TimportScore::addChordNote(const Tchunk& note) {
  if (m_lastPart && m_lastPart->melody())
    m_lastPart->addChordNote(m_lastPart, note);
  else
    qDebug() << "[TimportScore] Cannot add chord note to not existing part/melody.";
}


void TimportScore::sumarize() {
  for (auto p : m_parts) {
    for (auto s : p->parts) {
      for (auto v : s->parts) {
        if (v->count())
          m_partsModel << v;
      }
    }
  }
}


void TimportScore::setSplitBarNr(int splitNr) {
  if (splitNr != m_splitEveryBarNr) {
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


void TimportScore::addPartName(const QString& pn) {
  m_partNames << pn;
}


void TimportScore::setContextObject(QObject* c) {
  m_contextObj = c;
}


//#################################################################################################
//###################               TpartMelody        ############################################
//#################################################################################################

TmelodyPart::TmelodyPart(TmelodyPart* parent, int partId, int staffNr, int voiceNr) :
  QObject(parent),
  m_partId(partId),
  m_staffNr(staffNr),
  m_voiceNr(voiceNr)
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


void TmelodyPart::setMelody(Tmelody* m) {
  if (m != m_melody) {
    m_melody = m;
    emit melodyChanged();
  }
}


void TmelodyPart::setScoreObject(TscoreObject* sObj) {
  m_scoreObj = sObj;
  if (m_melody) {
    m_scoreObj->setMelody(m_melody);
    for (int c = 0; c < chords.count(); ++c) {
      auto noteSeg = m_scoreObj->note(chords[c]->noteNr());
      TdummyChord* chIt;
      if (chords[c]->dummyChord()) {
          chIt = chords[c]->dummyChord();
      } else {
          QQmlComponent comp(m_scoreObj->qmlEngine(), QUrl(QStringLiteral("qrc:/score/DummyChord.qml")));
          auto con = QQmlEngine::contextForObject(IMPORT_SCORE->contextObj());
          chIt = qobject_cast<TdummyChord*>(comp.create(con));
          chIt->setParent(IMPORT_SCORE);
      }
      chIt->setParentItem(noteSeg);
      chIt->setChord(chords[c]);
    }
  }
}


void TmelodyPart::setSelected(bool sel) {
  if (sel != m_selected) {
    m_selected = sel;
    emit selectedChanged();
  }
}


void TmelodyPart::setSplitBarNr(int splitNr) {
  if (splitNr != m_splitBarNr) {
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
        QList<Tmelody*> splitList;
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


int TmelodyPart::key() const {
  if (m_melody)
    return static_cast<int>(m_melody->key().value());
  else if (!parts.isEmpty() && parts.first()->melody())
    return static_cast<int>(parts.first()->melody()->key().value());

  return 0;
}


void TmelodyPart::setKey(int k) {
  if (m_melody)
    m_melody->setKey(TkeySignature(static_cast<char>(k)));
  else if (!parts.isEmpty()) {
    for (auto p : parts) {
      if (p->melody())
        p->melody()->setKey(TkeySignature(static_cast<char>(k)));
    }
  }
}


void TmelodyPart::addChordNote(TmelodyPart* part, const Tchunk& n) {
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


QList<QObject*> TmelodyPart::snippets() {
  QList<QObject*> s;
  for (auto p : parts)
    s << qobject_cast<QObject*>(p);
  return s;
}

//#################################################################################################
//###################                TalaChord         ############################################
//#################################################################################################

TalaChord::TalaChord(TmelodyPart* mp)
{
  part = mp;
  if (mp->melody()) {
      m_noteNr = part->melody()->length() - 1;
      auto fChN = mp->melody()->note(m_noteNr);
      m_notes.addNote(Tchunk(Tnote(fChN->p(), Trhythm(Trhythm::NoRhythm)), fChN->t()));
  } else
      qDebug() << "[TalaChord] FIXME! No melody in the part!";
}


void TalaChord::setDummyChord(TdummyChord* dCh) {
  m_dummyChord = dCh;
}
