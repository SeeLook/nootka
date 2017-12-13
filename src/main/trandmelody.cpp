/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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
#include <music/tchunk.h>
#include <QtCore/qdebug.h>
#include <QtCore/qdatetime.h>


void getRandomMelody(QList<TQAgroup>& qList, Tmelody* mel, int len, bool inKey, bool onTonic) {
  qsrand(QDateTime::currentDateTime().toTime_t());
  for (int i = 0; i < len; ++i) {
    int randVal = qrand() % qList.size();
    Tnote pitch = qList[randVal].note;
    TfingerPos fPos = qList[randVal].pos;
    if (inKey) { // note has to be in key signature declared in melody
      if (mel->key().inKey(pitch).note == 0) { // if it is not
        int tryCount = 0;
        int it = randVal;
        while (tryCount < qList.size()) { // find another one
          it++;
          if (it == qList.size())
            it = 0;
          Tnote tryNote = mel->key().inKey(qList[it].note);
          if (tryNote.note != 0) {
            pitch = tryNote;
            fPos = qList[it].pos;
            break;
          }
          tryCount++;
        }
      }
    }
    if (pitch.alter == 1 && mel->key().value() < 0) // flats key signature
      pitch = pitch.showWithFlat(); // so prefer flats
    else if (pitch.alter == -1 && mel->key().value() > 0) // sharps key signature
      pitch = pitch.showWithSharp(); // so prefer sharps
    Tchunk note(pitch, fPos);
    mel->addNote(note);
  }
  if (onTonic) {
    int cnt = -1, i = (qrand() % qList.size()) - 1; // start iteration from random value
    while (cnt < qList.size()) {
      i++;
      if (i >= qList.size())
        i = 0;
      cnt++;
      auto tonic = mel->key().tonicNote();
      bool theSame = false;
      if (tonic.compareNotes(qList[i].note, true))
        theSame = true;
      else {
        Tnote tonicConverted;
        if (tonic.alter == 1)
          tonicConverted = tonic.showWithFlat();
        else if (tonic.alter == -1)
          tonicConverted = tonic.showWithSharp();
        else
          continue;
        if (tonicConverted.compareNotes(qList[i].note, true)) {
          theSame = true;
          tonic = tonicConverted;
        }
      }
      if (theSame) {
        tonic.octave = qList[i].note.octave;
        if (tonic.alter == 1 && mel->key().value() < 0) // flats key signature
          tonic = tonic.showWithFlat(); // so prefer flats
        mel->lastMeasure().lastNote().p() = tonic;
        mel->lastMeasure().lastNote().g() = qList[i].pos;
        break;
      }
    }
    if (cnt == qList.size())
      qDebug() << "Tonic note of" << mel->key().getName() << "was not found";
  }
}


/**
 * Generates random melody with following way:
 * If @p inKey is required creates list of notes in given key only
 * Random algorithm works with phrases
 * 1. Randomizes length of phrase (half of total length max)
 * 2. Randomizes first note in the phrase (from whole list - scale)
 * 3. Randomizes direction (ascending/descending)
 * 4. Takes notes from the list starting from random first one in random direction.
 * 5. etc, etc, until melody @p len is fulfilled
 * 6. Looks for tonic note at the end if required (@p onTonic)
 */
void getRandomMelodyNG(QList<TQAgroup>& qList, Tmelody* mel, int len, bool inKey, bool onTonic) {
  QList<TQAgroup>* qListPtr = &qList;
  QList<TQAgroup> inKeyList;
  if (inKey) { // create list with notes in required key signature only
    for (TQAgroup qa : qList) {
      TQAgroup g;
      g.note = mel->key().inKey(qa.note);
      if (g.note.isValid()) {
        g.pos = qa.pos;
        inKeyList << g;
      }
    }
    if (inKeyList.isEmpty())
      qDebug() << "[getRandomMelodyNG] Question list has no any note in key" << mel->key().getName();
    else
      qListPtr = &inKeyList;
  }

  qsrand(QDateTime::currentDateTime().toTime_t());
  while (mel->length() < len) {
    int phLen = len < 4 ? len : qBound(2, 2 + qrand() % (len / 2 - 1), len);
    int dir = qrand() % 2 == 1 ? 1 : -1; // direction of melody (ascending or descending)
    int noteId = qrand() % qListPtr->size();
    int notesCnt = 0;
    while (notesCnt < phLen && noteId < qListPtr->size() && mel->length() < len) {
      auto curQA = &qListPtr->operator[](noteId);
      mel->addNote(Tchunk(curQA->note, curQA->pos));
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
      if (qa->note.note == tonic.note && qa->note.alter == tonic.alter)
        tonicList << n;
    }
    if (tonicList.isEmpty())
      qDebug() << "Tonic note of" << mel->key().getName() << "was not found";
    else {
      int tonicRandNr = tonicList[qrand() % tonicList.size()];
      mel->note(mel->length() - 1)->g() = qListPtr->operator[](tonicRandNr).pos;
      mel->note(mel->length() - 1)->p() = qListPtr->operator[](tonicRandNr).note;
    }
  }
}




