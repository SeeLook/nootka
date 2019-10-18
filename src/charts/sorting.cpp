/***************************************************************************
 *   Copyright (C) 2012-2019 by Tomasz Bojczuk                             *
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

#include "sorting.h"
#include <exam/tqaunit.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <tfingerpos.h>
#include <tnoofont.h>
#include <qtr.h>

#include <QtGui/qguiapplication.h>


QString qaTypeText(int qaType) {
  switch (qaType) {
    case 0: return qTR("Texam", "as note on the staff");
    case 1: return qTR("Texam", "as note name");
    case 2: return qTR("Texam", "on instrument");
    case 3: return qTR("Texam", "as played sound");
    default: return QString();
  }
}


QString qaSymbol(TQAtype::Etype type) {
  switch (type) {
    case TQAtype::e_onScore : return QStringLiteral("s");
    case TQAtype::e_asName : return QStringLiteral("c");
    case TQAtype::e_onInstr : return QStringLiteral("g");
    case TQAtype::e_asSound : return QStringLiteral("n");
  }
  return QString();
}


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


QList<Tnote> getTheSame(short int noteNr, Tlevel* level) {
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
  if (doDblAcc && nList.last().alter() != -1) { // flat not found
    xNote = workNote.showWithDoubleFlat();
    if (workNote != xNote) {
      nList.append(xNote);
    }
  }
  return nList;
}


void divideGoodAndBad(QList<TQAunit*> *list, TgroupedQAunit& goodList, TgroupedQAunit& badList) {
  for (int i = 0; i < list->size(); i++) {
    auto q = list->operator[](i);
    if (q->wrongNote() || q->wrongPos())
      badList.addQAunit(q, i + 1);
    else
      goodList.addQAunit(q, i + 1);
  }
}


QList<TgroupedQAunit> sortByNote(TgroupedQAunit& answList, Tlevel *level, bool &hasListUnrelated) {
  QList<TgroupedQAunit> result;
  for (short i = level->loNote.chromatic(); i <= level->hiNote.chromatic(); i++) {
    QList<Tnote> theSame = getTheSame(i, level);
    for (int j = 0; j < theSame.size(); j++) {
      TgroupedQAunit noteList;
      Tnote& n = theSame[j];
      for (int k = 0; k < answList.size(); k++) {
        TqaPtr& q = answList[k];
        if (q.qaPtr->qa.note == n) {
            if (q.qaPtr->questionAs != TQAtype::e_onInstr || q.qaPtr->answerAs != TQAtype::e_onInstr)
                noteList.addQAunit(q);
        }
      }
      if (!noteList.isEmpty()) {
        noteList.resume(n.toRichText(), QLatin1String("<b>") + noteList.for_a_note() + QLatin1String(" <big>")
                                        + n.toRichText() + QLatin1String("</big></b>"));
        result << noteList;
      }
    }
  }
  if (level->questionAs.isOnInstr() && level->answersAs[2].isOnInstr()) {
    TgroupedQAunit ignoredList; // ignore answers without notes
    for (int k = 0; k < answList.size(); k++) {
      TqaPtr& q = answList[k];
      if (q.qaPtr->questionAs == TQAtype::e_onInstr && q.qaPtr->questionAs == TQAtype::e_onInstr) {
        q.grNr = result.count();
        ignoredList.addQAunit(q);
      }
    }
    if (!ignoredList.isEmpty()) {
      result << ignoredList; // add ignoredList at the end
      hasListUnrelated = true;
    }
  }
  return result;
}


QList<TgroupedQAunit> sortByFret(TgroupedQAunit& answList, Tlevel *level, bool& hasListUnrelated) {
  QList<TgroupedQAunit> result;
  TgroupedQAunit unrelatedList;
  for (int f = level->loFret; f <= level->hiFret; f++) {
    // search all list for each fret in level's fret range
    TgroupedQAunit fretList;
    for (int i = 0; i < answList.size(); i++) {
      TqaPtr& q = answList[i];
      if (q.qaPtr->questionAs == TQAtype::e_onInstr || q.qaPtr->answerAs == TQAtype::e_onInstr || q.qaPtr->answerAs == TQAtype::e_asSound) { // is a question related to guitar
          if (f == q.qaPtr->qa.pos().fret())
            fretList.addQAunit(q);
      } else {
          if (f == level->loFret) // feed unrelated in first loop only
            unrelatedList.addQAunit(q);
      }
    }
    if (!fretList.isEmpty()) {
      fretList.resume(TfingerPos::romanFret(f), QLatin1String("<b>") + fretList.for_a_fret() + QLatin1String(" <big>")
                                                + QString("%1").arg(f) + QLatin1String("</big></b>"));
      result << fretList;
    }
  }
  if (!unrelatedList.isEmpty()) {
    result << unrelatedList; // add unrelatedList at the end
    hasListUnrelated = true;
  }
  return result;
}


QList<TgroupedQAunit> sortByKeySignature(TgroupedQAunit& answList, Tlevel *level, bool &hasListUnrelated) {
  QList<TgroupedQAunit> result;
  TgroupedQAunit unrelatedList;
  for (int k = level->loKey.value(); k <= level->hiKey.value(); k++) {
    TgroupedQAunit majors, minors;
    for (int i = 0; i < answList.size(); i++) {
      TqaPtr& q = answList[i];
      if (q.qaPtr->questionOnScore() || q.qaPtr->answerOnScore()) {
          if (q.qaPtr->key.value() == k) {
            if (q.qaPtr->key.isMinor())
              minors.addQAunit(q);
            else
              majors.addQAunit(q);
          }
      } else {
        if (k == level->loKey.value())
          unrelatedList.addQAunit(q);
      }
    }
    bool tmpBool;
    if (!majors.isEmpty()) {
      if (!majors.first()->melody()) {
        QList<TgroupedQAunit> majSorted = sortByNote(majors, level, tmpBool);
        TgroupedQAunit mS = mergeListOfLists(majSorted);
        divideQuestionsAndAnswers(result, mS, TQAtype::e_onScore);
      } else
          divideQuestionsAndAnswers(result, majors, TQAtype::e_onScore);
    }
    if (!minors.isEmpty()) {
      if (!minors.first()->melody()) {
        QList<TgroupedQAunit> minSorted = sortByNote(minors, level, tmpBool);
        TgroupedQAunit mS = mergeListOfLists(minSorted);
        divideQuestionsAndAnswers(result, mS, TQAtype::e_onScore);
      } else
          divideQuestionsAndAnswers(result, minors, TQAtype::e_onScore);
    }
  }
  for (int i = 0; i < result.size(); i++) {
    QString desc = result[i].list.first().qaPtr->key.getName() + QLatin1String("<br>") + getWasInAnswOrQuest(TQAtype::e_onScore, result[i].first());
    result[i].resume(desc, QLatin1String("<b>") + TgroupedQAunit::for_a_key() + QLatin1String("<big>  ")
                          + result[i].first()->key.getName() + QLatin1String("</big></b>")
                          + wereKeys(level->manualKey, result[i].list.first().qaPtr->answerAs));
  }
  if (!unrelatedList.isEmpty()) {
    result << unrelatedList; // add unrelatedList at the end
    hasListUnrelated = true;
  }
  return result;
}


QString wereKeys(bool manualKeys, TQAtype::Etype answerType) {
  QString wereK;
  if (manualKeys && answerType == TQAtype::e_onScore)
    wereK = QLatin1String("<br><i>(") + QApplication::translate("TlinearChart", "Key signatures given by user") + QLatin1String(")</i>");
  return wereK;
}


QList<TgroupedQAunit> sortByAccidental(TgroupedQAunit& answList, Tlevel* level, bool& hasListUnrelated, QList<char>& kindOfAccidList) {
  QList<TgroupedQAunit> result;
  TgroupedQAunit accidsArray[6]; // 0 - bb, 1 - b, 2 - none, 3 - #, 4 - x, 5 - unrelated
  for (int i = 0; i < answList.size(); i++) {
    TqaPtr& q = answList[i];
    if (q.qaPtr->questionAs == TQAtype::e_onScore || q.qaPtr->questionAs == TQAtype::e_asName ||
        q.qaPtr->answerAs == TQAtype::e_onScore || q.qaPtr->answerAs == TQAtype::e_asName) {
          accidsArray[q.qaPtr->qa.note.alter() + 2].addQAunit(q);
          if (q.qaPtr->qa_2.note.note() && q.qaPtr->qa_2.note.alter() != q.qaPtr->qa.note.alter())
            accidsArray[q.qaPtr->qa_2.note.alter() + 2].addQAunit(q);
    } else
          accidsArray[5].addQAunit(q);
  }
  bool tmpBool;
  for (int i = 0; i < 6; i++) {
    if (!accidsArray[i].isEmpty()) {
      QList<TgroupedQAunit> sorted = sortByNote(accidsArray[i], level, tmpBool);
      TgroupedQAunit accidList = mergeListOfLists(sorted);
      QString fullDesc;
      if (i - 2)
        fullDesc = QLatin1String("<b>") + TgroupedQAunit::for_an_accid() + QLatin1String("</b><big>  ") + accidToNotka(i -2) + QLatin1String("</big>");
            else
        fullDesc = QLatin1String("<b>") + QApplication::translate("TlinearChart", "for notes without accidentals") + QLatin1String("</b>");
      accidList.resume(accidToNotka(i - 2), fullDesc);
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

QList<TgroupedQAunit> sortByQAtype(TgroupedQAunit& answList, Tlevel* level, bool& hasListUnrelated) {
  QList<TgroupedQAunit> result;
  TgroupedQAunit qaTypesArr[4][4]; 
  for (int i = 0; i < answList.size(); i++) {
    TqaPtr& q = answList[i];
    switch (q.qaPtr->questionAs) {
      case TQAtype::e_onScore :
        switch (q.qaPtr->answerAs) {
          case TQAtype::e_onScore :
            qaTypesArr[0][0].addQAunit(q);
            break;
          case TQAtype::e_asName :
            qaTypesArr[0][1].addQAunit(q);
            break;
          case TQAtype::e_onInstr :
            qaTypesArr[0][2].addQAunit(q);
            break;
          case TQAtype::e_asSound :
            qaTypesArr[0][3].addQAunit(q);
            break;
        }
        break;
      case TQAtype::e_asName :
        switch (q.qaPtr->answerAs) {
          case TQAtype::e_onScore :
            qaTypesArr[1][0].addQAunit(q);
            break;
          case TQAtype::e_asName :
            qaTypesArr[1][1].addQAunit(q);
            break;
          case TQAtype::e_onInstr :
            qaTypesArr[1][2].addQAunit(q);
            break;
          case TQAtype::e_asSound :
            qaTypesArr[1][3].addQAunit(q);
            break;
        }
        break;
      case TQAtype::e_onInstr :
        switch (q.qaPtr->answerAs) {
          case TQAtype::e_onScore :
            qaTypesArr[2][0].addQAunit(q);
            break;
          case TQAtype::e_asName :
            qaTypesArr[2][1].addQAunit(q);
            break;
          case TQAtype::e_onInstr :
            qaTypesArr[2][2].addQAunit(q);
            break;
          case TQAtype::e_asSound :
            qaTypesArr[2][3].addQAunit(q);
            break;
        }
        break;
      case TQAtype::e_asSound :
        switch (q.qaPtr->answerAs) {
          case TQAtype::e_onScore :
            qaTypesArr[0][0].addQAunit(q);
            break;
          case TQAtype::e_asName :
            qaTypesArr[0][1].addQAunit(q);
            break;
          case TQAtype::e_onInstr :
            qaTypesArr[0][2].addQAunit(q);
            break;
          case TQAtype::e_asSound :
            qaTypesArr[0][3].addQAunit(q);
            break;
        }
        break;
    }
  }
  for (int q = 0; q < 4; q++) {
    for (int a = 0; a < 4; a++) {
      TgroupedQAunit& g = qaTypesArr[q][a];
      if (!g.isEmpty()) {
        QString fDesc;
        if (level->canBeMelody()) {
            if (g.first()->questionAs == TQAtype::e_onScore)
              fDesc = TexTrans::playMelodyTxt();
            else
              fDesc = TexTrans::writeMelodyTxt();
        } else {
            fDesc = TexTrans::questionsTxt() + QLatin1String(" ") + qaTypeText(g.first()->questionAs) + QLatin1String("<br>") +
                  TexTrans::answersTxt() + QLatin1String(" ") + qaTypeText(g.first()->answerAs);
        }
        g.resume( // short: symbols of types, full: texts (bold)
        TnooFont::span(qaSymbol(g.first()->questionAs), 22) + TnooFont::span(qaSymbol(g.first()->answerAs), 22),
                QLatin1String("<b>") + fDesc + QLatin1String("</b>"));
        result << g;
      }
    }
  }
  hasListUnrelated = false;
  return result;
}


QList<TgroupedQAunit> sortByMisakes(TgroupedQAunit& answList, Tlevel* level, bool& hasListUnrelated) {
  QList<TgroupedQAunit> result;
  TgroupedQAunit mistakesArr[12];
  QStringList mistakesDesc;
  mistakesDesc << QApplication::translate("AnswerText", "correct notes")           // 0
               << QApplication::translate("AnswerText", "wrong notes")             // 1
               << QApplication::translate("AnswerText", "wrong accidentals")       // 2
               << QApplication::translate("AnswerText", "wrong octaves")           // 3
               << QApplication::translate("AnswerText", "correct key signatures")  // 4
               << QApplication::translate("AnswerText", "wrong key signatures")    // 5
               << "correct styles"                                                 // 6 NOT IMPLEMENTED
               << "wrong styles"                                                   // 7 NOT IMPLEMENTED
               << QApplication::translate("AnswerText", "correct positions")       // 8
               << QApplication::translate("AnswerText", "wrong positions")         // 9
               << QApplication::translate("AnswerText", "wrong strings")           // 10
               << QApplication::translate("AnswerText", "out of tune")             // 11
  ;
  for (int i = 0; i < answList.size(); i++) {
    TqaPtr& q = answList[i];
    if (q.qaPtr->isCorrect()) {
      if (q.qaPtr->answerAs == TQAtype::e_onScore || q.qaPtr->answerAs == TQAtype::e_asName
                          || q.qaPtr->answerAs == TQAtype::e_asSound) {
          mistakesArr[0].addQAunit(q); // correct note
          if (level->useKeySign && level->manualKey && q.qaPtr->answerAs == TQAtype::e_onScore)
            mistakesArr[4].addQAunit(q); // correct key signature
          // TODO grab correct style here
      } else
          mistakesArr[8].addQAunit(q); // correct position
  } else { // correct, wrongNote & wrongPos exclude themself
      if (q.qaPtr->wrongNote()) {
          mistakesArr[1].addQAunit(q);
          /** TODO Unfortunately, when a mistake is cardinal it doesn't store was key signature correct
          * To have correct key but wrong note, TexamExecutor::checkAnswer() has to implement this discrimination */
      } else {
          if (q.qaPtr->wrongPos())
              mistakesArr[9].addQAunit(q);
          else { // meanwhile rest of mistakes can occur together
              if (q.qaPtr->wrongAccid())
                mistakesArr[2].addQAunit(q);
              if (q.qaPtr->wrongKey())
                mistakesArr[5].addQAunit(q);
              if (q.qaPtr->wrongOctave())
                mistakesArr[3].addQAunit(q);
              if (q.qaPtr->wrongStyle())
                mistakesArr[7].addQAunit(q);
              if (q.qaPtr->wrongString())
                mistakesArr[10].addQAunit(q);
              if (q.qaPtr->wrongIntonation())
                mistakesArr[11].addQAunit(q);
          }
      }
    }
  }
  for (int m = 0; m < 12; m++) {
    if (!mistakesArr[m].isEmpty()) {
      mistakesArr[m].resume(mistakesDesc[m].replace(QLatin1String(" "), QLatin1String("<br>")), mistakesDesc[m]);
      result << mistakesArr[m];
    }
  }
  hasListUnrelated = false;
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
  for (int i = 0; i < listOfLists.size(); i++) {
    for (int j = 0; j < listOfLists[i].size(); j++)
      result.addQAunit(listOfLists[i].operator[](j));
  }
  return result;
}


void convertToGroupedQAunit(QList<TQAunit*>* examList, TgroupedQAunit& groupped) {
  for (int i = 0; i< examList->size(); i++)
    groupped.addQAunit(examList->operator[](i), i + 1);
}


QString accidToNotka(char acc, int fontSize) {
  QString result;
  switch (acc) {
    case -2:
      result = TnooFont::span(QLatin1String("B"), fontSize); break;
    case -1:
      result = TnooFont::span(QLatin1String("b"), fontSize); break;
    case 1:
      result = TnooFont::span(QLatin1String("#"), fontSize); break;
    case 2:
      result = TnooFont::span(QLatin1String("x"), fontSize); break;
    default:
      result = QApplication::translate("chartStats", "none"); break;
  }
  return result;
}


QString getWasInAnswOrQuest(TQAtype::Etype type, TQAunit* question) {
  QString hintText;
  if (question->answerAs == type)
    hintText += QLatin1String("!");
  else
    hintText += QLatin1String("?");
  return TnooFont::span(hintText);
}



