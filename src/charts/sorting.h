/***************************************************************************
 *   Copyright (C) 2012-2018 by Tomasz Bojczuk                             *
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

#ifndef SORTING_H
#define SORTING_H

#include <QtCore/qlist.h>
#include <exam/tqatype.h>
#include "tgroupedqaunit.h"

class Tnote;
class TQAunit;
class Tlevel;


  QString qaSymbol(TQAtype::Etype type);

  QList<Tnote> getTheSame(short noteNr, Tlevel *level = nullptr);

      /**
       * Calculates average reaction time of a given list.
       * If @p skipWrong = true, wrong answers are not taken to average.
       */
  double calcAverTime(TgroupedQAunit &answers, bool skipWrong = false);

      /**
       * Merges elements of each list to single list of pointers to @p TQAunit.
       */
  TgroupedQAunit mergeListOfLists(QList<TgroupedQAunit> &listOfLists);

      /**
       * converts list with @class TQAunit pointers (usually answers list) to @class TgroupedQAunit.
       */
  void convertToGroupedQAunit(QList<TQAunit*>* examList, TgroupedQAunit& groupped);

      /**
       * Given TgroupedQAunit someList is divided on two lists:
       * - First with questions of given type
       * - second with answers.
       * !!! If both - answer has priority !!!
       * Then, two lists are added to result list.
       * We have to be sure that all given TQAunits have question or answer of given type.
       */
  void divideQuestionsAndAnswers(QList<TgroupedQAunit> &result, TgroupedQAunit &someList, TQAtype::Etype type);

      /**
       * Divides given list on lists with correct & almost good answers and wrong ones.
       */
  void divideGoodAndBad(QList<TQAunit*>* list, TgroupedQAunit& goodList, TgroupedQAunit& badList);

      /**
       * Returns the lists of list of the same notes. 
       * The last list is a list of ignored answers - no notes either in answer nor questions.
       */
  QList<TgroupedQAunit> sortByNote(TgroupedQAunit& answList, Tlevel* level, bool& hasListUnrelated);

  QList<TgroupedQAunit> sortByFret(TgroupedQAunit& answList, Tlevel* level, bool& hasListUnrelated);

  QList<TgroupedQAunit> sortByKeySignature(TgroupedQAunit& answList, Tlevel* level, bool& hasListUnrelated);

      /**
       * @p kindOfAccidList is created during sorting by accidental.
       * Each element corresponds with element of sortByAccidental List
       * and keeps appropriate accidental.
       * For unrelated its value is 3
       */
  QList<TgroupedQAunit> sortByAccidental(TgroupedQAunit& answList, Tlevel* level,
                                         bool& hasListUnrelated, QList<char> &kindOfAccidList);
  QList<TgroupedQAunit> sortByQAtype(TgroupedQAunit& answList, Tlevel* level, bool& hasListUnrelated);
  QList<TgroupedQAunit> sortByMisakes(TgroupedQAunit& answList, Tlevel* level, bool& hasListUnrelated);

      /**
       * converts given value to nootka string.
       */
  QString accidToNotka(char acc, int fontSize = 20);

      /**
       * Returns '?' or '!' depends on answer type.
       */
  QString getWasInAnswOrQuest(TQAtype::Etype type, TQAunit* question);

      /**
       * Returns a text 'Key signatures gave by user' when all cases are fulfilled or empty string
       */
  QString wereKeys(bool manualKeys, TQAtype::Etype answerType);

//   QList<TanswerListPtr> divideQuestionsAndAnswers(TanswerListPtr &answList);




#endif // SORTING_H
