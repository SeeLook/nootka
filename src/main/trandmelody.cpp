/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
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

#include "trandmelody.h"
#include <music/tmelody.h>
#include <music/trhythm.h>
#include <music/trtmgroup.h>

#include <QtCore/qrandom.h>

#include <QtCore/qdebug.h>

/**
 * Generates random melody in following way:
 * If @p inKey is required creates list of notes in given key only
 * Random algorithm works with phrases
 * 1. Randomizes length of phrase (half of total length max)
 * 2. Randomizes first note in the phrase (from whole list - scale)
 * 3. Randomizes direction (ascending/descending)
 * 4. Takes notes from the list starting from random first one in random direction.
 * 5. etc, etc, until melody @p len is fulfilled
 * 6. Looks for tonic note at the end if required (@p onTonic)
 */
void getRandomMelodyNG(QList<TQAgroup> &qList, Tmelody *mel, int len, bool inKey, bool onTonic, int maxStep)
{
    QList<TQAgroup> *qListPtr = &qList;
    QList<TQAgroup> inKeyList;
    if (inKey) { // create list with notes in required key signature only
        for (TQAgroup qa : qList) {
            TQAgroup g;
            g.note = mel->key().inKey(qa.note);
            if (g.note.isValid()) {
                g.pos() = qa.pos();
                inKeyList << g;
            }
        }
        if (inKeyList.isEmpty())
            qDebug() << "[getRandomMelodyNG] Question list has no any note in key" << mel->key().getName();
        else
            qListPtr = &inKeyList;
    }

    auto *random = QRandomGenerator::global();

    /**
     * Randomize main melody step. 2 - is diatonic, 4 is for steps by third
     * It makes melodies more different
     */
    int melodyStep = (maxStep == 0 || maxStep > 4) ? (random->bounded(5) > 2 ? 4 : 2) : 2;

    while (mel->length() < len) {
        int phLen = len < 4 ? len : qBound(2, 2 + random->bounded(len / 2 - 1), len);
        int dir = random->bounded(2) == 1 ? 1 : -1; // direction of melody (ascending or descending)
        int noteId = random->bounded(qListPtr->size());
        if (maxStep && mel->length() > 0) {
            int loopGuard = 0;
            while (loopGuard < qListPtr->size() && qAbs(qListPtr->at(noteId).note.chromatic() - mel->note(mel->length() - 1)->p().chromatic()) > maxStep) {
                noteId += dir;
                if (noteId < 0)
                    noteId = qListPtr->size() - 1;
                else if (noteId >= qListPtr->size())
                    noteId = 0;
                loopGuard++;
            }
            if (loopGuard >= qListPtr->size()) { // interval impossible to found, skip this loop then
                qDebug() << "[getRandomMelodyNG] interval impossible to found:" << maxStep;
                continue;
            }
        }
        int notesCnt = 0;
        TQAgroup *lastNote = nullptr;
        while (notesCnt < phLen && noteId < qListPtr->size() && mel->length() < len) {
            auto curQA = &qListPtr->operator[](noteId);
            if (lastNote) {
                if (qAbs(curQA->note.chromatic() - lastNote->note.chromatic()) < melodyStep) {
                    noteId += dir;
                    if (noteId < 0 || noteId == qListPtr->size())
                        break;
                    continue;
                }
            }
            lastNote = curQA;
            mel->addNote(Tchunk(curQA->note, curQA->pos()));
            notesCnt++;
            noteId += dir;
            if (noteId < 0 || noteId == qListPtr->size())
                break;
        }
    }
    if (onTonic) {
        auto tonic = mel->key().tonicNote();
        QList<int> tonicList;
        for (int n = 0; n < qListPtr->size(); ++n) {
            auto qa = &qListPtr->operator[](n);
            if (qa->note.note() == tonic.note() && qa->note.alter() == tonic.alter())
                tonicList << n;
        }
        if (tonicList.isEmpty())
            qDebug() << "Tonic note of" << mel->key().getName() << "was not found";
        else {
            int tonicRandNr = tonicList[random->bounded(tonicList.size())];
            mel->note(mel->length() - 1)->g() = qListPtr->operator[](tonicRandNr).pos();
            mel->note(mel->length() - 1)->p() = qListPtr->operator[](tonicRandNr).note;
        }
    }
}

TrhythmList getRandomRhythm(int meter, int barCount, quint32 basicMask, quint32 dotsMask, int rtmDiversity)
{
    TrhythmList rList;
    /** 1. In @p barGroups store groups types (duple or triple) in every measure of current @p meter  */
    Tmeter m(static_cast<Tmeter::Emeter>(meter));
    QList<quint8> durList;
    m.fillMeterGroups(durList);
    QList<int> barGroups;
    for (int d = 0; d < durList.count(); ++d) {
        int diff;
        if (d == 0)
            diff = durList.first();
        else
            diff = durList[d] - durList[d - 1];
        if (diff == 24)
            barGroups << 2;
        else if (diff == 36)
            barGroups << 3;
    }

    auto *random = QRandomGenerator::global();

    /** 2. Create lists: @p dupleList & @p tripleList each with the same grouping values. Also get special groups (whole, half and so) */
    bool meterHasDuple = barGroups.contains(2);
    bool meterHasTriple = barGroups.contains(3);
    int barDuration = m.duration();
    QList<TrtmGroup> dupleList, tripleList, specialList;
    quint32 mask = 1;
    for (int i = 1; i < 22; ++i) {
        if (basicMask && basicMask & mask) {
            TrtmGroup basicGr(static_cast<TrtmGroup::ErtmGroup>(i));
            if (meterHasDuple && basicGr.isDuple())
                dupleList << basicGr;
            else if (meterHasTriple && basicGr.isTriple())
                tripleList << basicGr;
            else if (basicGr.isSpecial()
                     && ((basicGr.group() == TrtmGroup::Gr_1 && barDuration >= 96) || (basicGr.group() == TrtmGroup::Gr_2 && barDuration >= 48)))
                specialList << basicGr;
        }
        if (i < 15 && dotsMask && dotsMask & mask) {
            TrtmGroup dotsGr(static_cast<TrtmGroup::ErtmGroup>(i + 21));
            if (meterHasDuple && dotsGr.isDuple())
                dupleList << dotsGr;
            else if (meterHasTriple && dotsGr.isTriple())
                tripleList << dotsGr;
            else if (dotsGr.isSpecial()
                     && ((dotsGr.group() == TrtmGroup::Gr_1dot && barDuration >= 144) || (dotsGr.group() == TrtmGroup::Gr_2dot && barDuration >= 72)))
                specialList << dotsGr;
        }
        mask <<= 1;
    }
    if ((meterHasDuple && dupleList.isEmpty()) || (meterHasTriple && tripleList.isEmpty())) {
        if (!specialList.isEmpty()) {
            if (meterHasDuple && dupleList.isEmpty()) {
                if (basicMask & 1 && barDuration >= 96)
                    dupleList << TrtmGroup(TrtmGroup::Gr_1);
                if (basicMask & 2 && barDuration >= 48)
                    dupleList << TrtmGroup(TrtmGroup::Gr_2);
            }
            if (meterHasTriple && tripleList.isEmpty()) {
                if (dotsMask & 23 && barDuration >= 144)
                    tripleList << TrtmGroup(TrtmGroup::Gr_1dot);
                if (dotsMask & 24 && barDuration >= 72)
                    tripleList << TrtmGroup(TrtmGroup::Gr_2dot);
            }
        }
    }

    if (meterHasDuple && dupleList.isEmpty()) {
        qDebug() << "[getRandomRhythm]" << "FIXME! Melody rhythm needs rhythm group which is not set! Added quarter to save the situation.";
        dupleList << TrtmGroup(TrtmGroup::Gr_4);
    }
    if (meterHasTriple && tripleList.isEmpty()) {
        qDebug() << "[getRandomRhythm]" << "FIXME! Melody rhythm needs rhythm group which is not set! Added quarter with dot to save the situation.";
        tripleList << TrtmGroup(TrtmGroup::Gr_4dot);
    }

    /** 3. Depending on rhythm diversity value (@p rtmDiversity) remove excessive rhythmic groups from the lists */
    //   const int dupleMaxNr = 7, tripleNr = 23;
    qreal totalGroupsNr =
        qMax(1.0, static_cast<qreal>(dupleList.count() + tripleList.count() + specialList.count()) * (static_cast<qreal>(rtmDiversity) / 10.0));
    qreal div = (dupleList.isEmpty() ? 0.0 : 1.0) + (tripleList.isEmpty() ? 0.0 : 1.0) + (specialList.isEmpty() ? 0.0 : 1.0);
    int grCount = 0;
    if (!dupleList.isEmpty()) {
        grCount = qRound(qMax(1.0, totalGroupsNr / div));
        while (dupleList.count() > grCount)
            dupleList.removeAt(random->bounded(dupleList.count()));
        div--;
        totalGroupsNr -= dupleList.count();
    }
    if (!tripleList.empty() && totalGroupsNr > 0.0 && div > 0.0) {
        grCount = qRound(qMax(1.0, totalGroupsNr / div));
        while (tripleList.count() > grCount)
            tripleList.removeAt(random->bounded(tripleList.count()));
        div--;
        totalGroupsNr -= tripleList.count();
    }
    if (!specialList.isEmpty() && totalGroupsNr > 0.0 && div > 0.0) {
        grCount = qRound(qMax(1.0, totalGroupsNr / div));
        while (specialList.count() > grCount)
            specialList.removeAt(random->bounded(dupleList.count()));
    }

    /** 4. Determine measures where to put longer (special) notes to imitate end of a phrase and/or period */
    int pharaseLen = 2;
    if (barCount > 16)
        pharaseLen = 8;
    else if (barCount >= 8)
        pharaseLen = 4;
    if (barDuration > 48 && pharaseLen > 2) // shorter phrase for "longer" measures
        pharaseLen /= 2;

    /** 5. Fulfil rhythm list with random groups from the lists */
    int barNr = 0;
    for (int m = 0; m < barCount; ++m) {
        barNr++;
        if (!specialList.isEmpty() && (barNr % pharaseLen == 0 || m == barCount - 1)) { // end of phrase/period or last bar- put longer rhythmic value here
            TrtmGroup specialGr = specialList[specialList.count() == 1 ? 0 : random->bounded(specialList.count())];
            int dur = specialGr.duration();
            if (dur < barDuration) {
                int restDur = barDuration - dur;
                if (restDur % 24 == 0 && !dupleList.isEmpty()) { // duple
                    for (int b = 0; b < restDur / 24; ++b)
                        rList.append(dupleList[dupleList.count() == 1 ? 0 : random->bounded(dupleList.count())].rhythm());
                } else if (restDur % 36 == 0 && !tripleList.isEmpty()) { // triple
                    for (int b = 0; b < restDur / 36; ++b)
                        rList.append(tripleList[tripleList.count() == 1 ? 0 : random->bounded(tripleList.count())].rhythm());
                } else {
                    rList.append(Trhythm::resolve(restDur));
                }
            }
            rList.append(specialGr.rhythm());
        } else {
            for (int b = 0; b < barGroups.count(); ++b) {
                if (barGroups[b] == 2)
                    rList.append(dupleList[dupleList.count() == 1 ? 0 : random->bounded(dupleList.count())].rhythm());
                else
                    rList.append(tripleList[tripleList.count() == 1 ? 0 : random->bounded(tripleList.count())].rhythm());
            }
        }
    }
    return rList;
}

bool mergeRhythmAndMelody(const TrhythmList &rList, Tmelody *melody)
{
    if (rList.count() == melody->length()) {
        for (int r = 0; r < rList.count(); ++r) {
            melody->note(r)->p().setRhythm(rList[r]);
        }
        return true;
    } else
        qDebug() << "[Random melody]" << "Can't merge melody and rhythm due to mismatch note number!";
    return false;
}
