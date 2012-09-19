/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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

#include "sorting.h"
#include "tqaunit.h"
#include "texamlevel.h"


double calcAverTime(TanswerListPtr& answers, bool skipWrong) {
  if (answers.isEmpty())
    return 0.0;
  double result = 0.0;
  int cnt = 0; // number of answers in average
  for (int i = 0; i < answers.size(); i++) {
    if (skipWrong && (answers.operator[](i)->wrongNote() || answers.operator[](i)->wrongPos()) ) 
      continue; // skip wrong answer
    else {
      result += answers.operator[](i)->time;
      cnt++;
    }
  }
  return result / cnt;
}


QList<Tnote> getTheSame(short int noteNr, TexamLevel* level) {
  Tnote workNote(noteNr); // natural or sharp by default
  QList<Tnote> nList;
  nList << workNote;
  bool doFlats = true, doDblAcc = true, doSharps = true;
  if (level){
    doFlats = level->withFlats;
    doDblAcc = level->withDblAcc;
    doSharps = level->withSharps;
  }
  Tnote xNote;
  if (doSharps) {
    xNote = workNote.showWithSharp();
    if (workNote != xNote) {
      nList.prepend(xNote);
    }
  }
  if (nList.size() == 1) { // sharp not found
    if (doDblAcc) {
      xNote = workNote.showWithDoubleSharp();
      if (workNote != xNote) {
        nList.prepend(xNote);
      }
    }
  }
  if (doFlats) {
    xNote = workNote.showWithFlat();
    if (workNote != xNote) {
      nList.append(xNote);
    }
  }
  if (doDblAcc && nList.last().acidental != -1) { // flat not found
    xNote = workNote.showWithDoubleFlat();
    if (workNote != xNote) {
      nList.append(xNote);
    }
  }
  return nList;
}


void divideGoodAndBad(QList< TQAunit >* list, TanswerListPtr& goodList, TanswerListPtr& badList) {
  for (int i = 0; i < list->size(); i++) {
    if (list->operator[](i).wrongNote() || list->operator[](i).wrongPos())
      badList << &list->operator[](i);
    else
      goodList << &list->operator[](i);
  }
}



QList<TanswerListPtr> sortByNote(TanswerListPtr& answList, TexamLevel *level, bool &hasListUnrelated) {
  QList<TanswerListPtr> result;
  for (short i = level->loNote.getChromaticNrOfNote(); i <= level->hiNote.getChromaticNrOfNote(); i++) {
    QList<Tnote> theSame = getTheSame(i, level);
    for (int j = 0; j < theSame.size(); j++) {
      TanswerListPtr noteList;
      for (int k = 0; k < answList.size(); k++) {
        if (answList.operator[](k)->qa.note == theSame[j]) {
            if (answList.operator[](k)->questionAs != TQAtype::e_asFretPos || 
              answList.operator[](k)->answerAs != TQAtype::e_asFretPos)
                           noteList << answList.operator[](k);   
        }
      }
      if (!noteList.isEmpty()) {
        result << noteList;
      }
    }
  }
  if (level->questionAs.isFret() && level->answersAs[2].isFret()) {
      TanswerListPtr ignoredList; // ignore answers without notes
      for (int k = 0; k < answList.size(); k++)
            if (answList.operator[](k)->questionAs == TQAtype::e_asFretPos && 
              answList.operator[](k)->questionAs == TQAtype::e_asFretPos)
                      ignoredList << answList.operator[](k);
      if (!ignoredList.isEmpty()) {
        result << ignoredList; // add ignoredList at the end
        hasListUnrelated = true;
//         qDebug() << ignoredList.size();
      }
  }
  return result;
}



QList< TanswerListPtr > sortByFret(TanswerListPtr& answList, TexamLevel *level, bool& hasListUnrelated) {
  QList<TanswerListPtr> result;
  TanswerListPtr unrelatedList;
  for (int f = level->loFret; f <= level->hiFret; f++) {
    // search all list for each fret in level's fret range
    TanswerListPtr fretList;
    for (int i = 0; i < answList.size(); i++) {
      if (answList.operator[](i)->questionAs == TQAtype::e_asFretPos ||
          answList.operator[](i)->answerAs == TQAtype::e_asFretPos ||
          answList.operator[](i)->answerAs == TQAtype::e_asSound) { // is a question related to guitar
        if (f == answList.operator[](i)->qa.pos.fret())
            fretList << answList.operator[](i);
      } else {
          if (f == level->loFret) // feed unrelated in first loop only
              unrelatedList << answList.operator[](i);
      }
    }
    if (!fretList.isEmpty())
      result << fretList;
  }
  if (!unrelatedList.isEmpty()) {
      result << unrelatedList; // add unrelatedList at the end of list
      hasListUnrelated = true;
  }
  return result;
}



QList< TanswerListPtr > sortByKeySignature(TanswerListPtr& answList, TexamLevel *level, bool &hasListUnrelated) {
  QList<TanswerListPtr> result;
  TanswerListPtr unrelatedList;
  for (int k = level->loKey.value(); k <= level->hiKey.value(); k++) {
        TanswerListPtr majors, minors;
    for (int i = 0; i < answList.size(); i++) {
        if (answList.operator[](i)->questionAs == TQAtype::e_asNote || answList.operator[](i)->answerAs == TQAtype::e_asNote) {
            if (answList.operator[](i)->key.value() == k) {
              if (answList.operator[](i)->key.isMinor())
                  minors << answList.operator[](i);
              else
                  majors << answList.operator[](i);
            }
        } else {
          if (k == level->loKey.value())
            unrelatedList << answList.operator[](i);
        }
    }
    bool tmpBool;
    if (!majors.isEmpty()) {
      QList<TanswerListPtr> majSorted = sortByNote(majors, level, tmpBool);
      TanswerListPtr mS;
      for (int i = 0; i < majSorted.size(); i++)
        for (int j = 0; j < majSorted[i].size(); j++)
          mS << majSorted[i].operator[](j);
      result << mS;
    }
    if (!minors.isEmpty()) {
      QList<TanswerListPtr> minSorted = sortByNote(minors, level, tmpBool);
      TanswerListPtr mS;
      for (int i = 0; i < minSorted.size(); i++)
        for (int j = 0; j < minSorted[i].size(); j++)
          mS << minSorted[i].operator[](j);
      result << mS;
    }
  }
  if (!unrelatedList.isEmpty()) {
      result << unrelatedList; // add unrelatedList at the end of list
      hasListUnrelated = true;
  }
  return result;
}


QList< TanswerListPtr > sortByAccidental(TanswerListPtr& answList, bool& hasListUnrelated) {

}


TanswerListPtr mergeListOfLists(QList<TanswerListPtr>& listOfLists) {
  TanswerListPtr result;
  for (int i = 0; i < listOfLists.size(); i++)
    for (int j = 0; j < listOfLists[i].size(); j++)
      result << listOfLists[i].operator[](j);    
    return result;
}



TanswerListPtr convertToPointers(QList<TQAunit> *examList) {
    TanswerListPtr result;
    for (int i = 0; i< examList->size(); i++)
        result << &examList->operator [](i);
    return result;
}







