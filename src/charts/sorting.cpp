/***************************************************************************
 *   Copyright (C) 2012-2013 by Tomasz Bojczuk                             *
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
#include "tquestionaswdg.h"
#include "tfingerpos.h"
#include <QDebug>

double calcAverTime(TgroupedQAunit& answers, bool skipWrong) {
  if (answers.isEmpty())
    return 0.0;
  double result = 0.0;
  int cnt = 0; // number of answers in average
  for (int i = 0; i < answers.size(); i++) {
    if (skipWrong && (answers[i].qaPtr->wrongNote() || answers[i].qaPtr->wrongPos()) ) 
      continue; // skip wrong answer
    else {
      result += answers[i].qaPtr->time;
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


void divideGoodAndBad(QList<TQAunit> *list, TgroupedQAunit& goodList, TgroupedQAunit& badList) {
  for (int i = 0; i < list->size(); i++) {
    if (list->operator[](i).wrongNote() || list->operator[](i).wrongPos())
      badList.addQAunit(&list->operator[](i), i);
    else
      goodList.addQAunit(&list->operator[](i), i);
  }
}


QList<TgroupedQAunit> sortByNote(TgroupedQAunit& answList, TexamLevel *level, bool &hasListUnrelated) {
  QList<TgroupedQAunit> result;
  for (short i = level->loNote.getChromaticNrOfNote(); i <= level->hiNote.getChromaticNrOfNote(); i++) {
    QList<Tnote> theSame = getTheSame(i, level);
    for (int j = 0; j < theSame.size(); j++) {
      TgroupedQAunit noteList;
      for (int k = 0; k < answList.size(); k++) {
        if (answList[k].qaPtr->qa.note == theSame[j]) {
            if (answList[k].qaPtr->questionAs != TQAtype::e_asFretPos || answList[k].qaPtr->answerAs != TQAtype::e_asFretPos)
                noteList.addQAunit(answList[k].qaPtr, answList[k].nr);
        }
      }
      if (!noteList.isEmpty()) {
        noteList.resume(theSame[j].toRichText());
        result << noteList;
      }
    }
  }
  if (level->questionAs.isFret() && level->answersAs[2].isFret()) {
      TgroupedQAunit ignoredList; // ignore answers without notes
      for (int k = 0; k < answList.size(); k++)
            if (answList[k].qaPtr->questionAs == TQAtype::e_asFretPos && 
              answList[k].qaPtr->questionAs == TQAtype::e_asFretPos)
                      ignoredList.addQAunit(answList[k]);
      if (!ignoredList.isEmpty()) {
        result << ignoredList; // add ignoredList at the end
        hasListUnrelated = true;
//         qDebug() << ignoredList.size();
      }
  }
  return result;
}


QList<TgroupedQAunit> sortByFret(TgroupedQAunit& answList, TexamLevel *level, bool& hasListUnrelated) {
  QList<TgroupedQAunit> result;
  TgroupedQAunit unrelatedList;
  for (int f = level->loFret; f <= level->hiFret; f++) {
    // search all list for each fret in level's fret range
    TgroupedQAunit fretList;
    for (int i = 0; i < answList.size(); i++) {
      if (answList[i].qaPtr->questionAs == TQAtype::e_asFretPos ||
          answList[i].qaPtr->answerAs == TQAtype::e_asFretPos ||
          answList[i].qaPtr->answerAs == TQAtype::e_asSound) { // is a question related to guitar
        if (f == answList[i].qaPtr->qa.pos.fret())
            fretList.addQAunit(answList[i]);
      } else {
          if (f == level->loFret) // feed unrelated in first loop only
              unrelatedList.addQAunit(answList[i]);
      }
    }
    if (!fretList.isEmpty()) {
      fretList.resume(TfingerPos::romanFret(f));
      result << fretList;
    }
  }
  if (!unrelatedList.isEmpty()) {
      result << unrelatedList; // add unrelatedList at the end of list
      hasListUnrelated = true;
  }
  return result;
}


QList<TgroupedQAunit> sortByKeySignature(TgroupedQAunit& answList, TexamLevel *level, bool &hasListUnrelated) {
  QList<TgroupedQAunit> result;
  TgroupedQAunit unrelatedList;
  for (int k = level->loKey.value(); k <= level->hiKey.value(); k++) {
        TgroupedQAunit majors, minors;
    for (int i = 0; i < answList.size(); i++) {
        if (answList[i].qaPtr->questionAs == TQAtype::e_asNote || answList[i].qaPtr->answerAs == TQAtype::e_asNote) {
            if (answList[i].qaPtr->key.value() == k) {
              if (answList[i].qaPtr->key.isMinor())
                  minors.addQAunit(answList[i]);
              else
                  majors.addQAunit(answList[i]);
            }
        } else {
          if (k == level->loKey.value())
            unrelatedList.addQAunit(answList[i]);
        }
    }
    bool tmpBool;
    if (!majors.isEmpty()) {
      QList<TgroupedQAunit> majSorted = sortByNote(majors, level, tmpBool);
      TgroupedQAunit mS = mergeListOfLists(majSorted);
      divideQuestionsAndAnswers(result, mS, TQAtype::e_asNote);
    }
    if (!minors.isEmpty()) {
      QList<TgroupedQAunit> minSorted = sortByNote(minors, level, tmpBool);
      TgroupedQAunit mS = mergeListOfLists(minSorted);
      divideQuestionsAndAnswers(result, mS, TQAtype::e_asNote);
    }
  }
  if (!unrelatedList.isEmpty()) {
      result << unrelatedList; // add unrelatedList at the end of list
      hasListUnrelated = true;
  }
  return result;
}


QList<TgroupedQAunit> sortByAccidental(TgroupedQAunit& answList, TexamLevel* level,
                                         bool& hasListUnrelated, QList< char >& kindOfAccidList) {
  QList<TgroupedQAunit> result;
  TgroupedQAunit accidsArray[6]; // 0 - bb, 1 - b, 2 - none, 3 - #, 4 - x, 5 - unrelated
  for (int i = 0; i < answList.size(); i++) {
//    bool accidFound = false;
    if (answList[i].qaPtr->questionAs == TQAtype::e_asNote || answList[i].qaPtr->questionAs == TQAtype::e_asName ||
      answList[i].qaPtr->answerAs == TQAtype::e_asNote || answList[i].qaPtr->answerAs == TQAtype::e_asName) {
        accidsArray[answList[i].qaPtr->qa.note.acidental + 2].addQAunit(answList[i]);
        if (answList[i].qaPtr->qa_2.note.note && answList[i].qaPtr->qa_2.note.acidental != answList[i].qaPtr->qa.note.acidental)
            accidsArray[answList[i].qaPtr->qa_2.note.acidental + 2].addQAunit(answList[i]);
    } else
        accidsArray[5].addQAunit(answList[i]);
  }
  bool tmpBool;
  for (int i = 0; i < 6; i++) {
    if (!accidsArray[i].isEmpty()) {
      QList<TgroupedQAunit> sorted = sortByNote(accidsArray[i], level, tmpBool);
      TgroupedQAunit accidList = mergeListOfLists(sorted);
      accidList.resume(accidToNotka(i - 2));
      result << accidList;
      kindOfAccidList << (i - 2);
    }
  }
  if (accidsArray[5].isEmpty())
    hasListUnrelated = false;
  else
    hasListUnrelated = true;
  return result;
}



void divideQuestionsAndAnswers(QList<TgroupedQAunit>& result, TgroupedQAunit& someList, TQAtype::Etype type) {
  TgroupedQAunit inQuest, inAnsw; 
  for (int i = 0; i < someList.size(); i++) {
    if (someList[i].qaPtr->answerAs == type)
      inAnsw.addQAunit(someList[i]);
    else 
      inQuest.addQAunit(someList[i]);
  }
  if (!inQuest.isEmpty())
    result << inQuest;
  if (!inAnsw.isEmpty())
    result << inAnsw;
}


TgroupedQAunit mergeListOfLists(QList<TgroupedQAunit>& listOfLists) {
  TgroupedQAunit result;
  for (int i = 0; i < listOfLists.size(); i++)
    for (int j = 0; j < listOfLists[i].size(); j++)
      result.addQAunit(listOfLists[i].operator[](j));
    
  return result;
}


TgroupedQAunit convertToPointers(QList<TQAunit> *examList) {
    TgroupedQAunit result;
    for (int i = 0; i< examList->size(); i++)
        result.addQAunit(&examList->operator[](i), i);
//         result << &examList->operator [](i);
    return result;
}


QString accidToNotka(char acc, int fontSize) {
  QString result = "";
  switch (acc) {
      case -2:
        result = TquestionAsWdg::spanNootka("B", fontSize); break;
      case -1:
        result = TquestionAsWdg::spanNootka("b", fontSize); break;
      case 0:
        break;
      case 1:
        result = TquestionAsWdg::spanNootka("#", fontSize); break;
      case 2:
        result = TquestionAsWdg::spanNootka("x", fontSize); break;
  }
  return result;
}





