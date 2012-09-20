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

#ifndef SORTING_H
#define SORTING_H

#include <QList>
#include "tqatype.h"

class Tnote;
class TQAunit;
class TexamLevel;


typedef QList<TQAunit*> TanswerListPtr; // definition of TQAunit list type

  QList<Tnote> getTheSame(short noteNr, TexamLevel *level = 0);
      /** Calculates average reaction time of a given list.
       * If skipWrong = true, wrong answers are not taken to average. */
  double calcAverTime(TanswerListPtr &answers, bool skipWrong = false);
      /** Merges elements of each list to single list of pointers to TQAunit. */
  TanswerListPtr mergeListOfLists(QList<TanswerListPtr> &listOfLists);
      /** converts QList with TQAunit to QList with pointer to them*/
  TanswerListPtr convertToPointers(QList<TQAunit> *examList);
  
      /** Given TanswerListPtr someList divides on two lists. 
       * First with questions of given type, second with answers. If both - answer has priority.
       * Then, two lists are added to result list.
       * We have to be sure that all given TQAunits have question or answer of given type. */
  void divideQuestionsAndAnswers(QList<TanswerListPtr> &result, TanswerListPtr &someList, TQAtype::Etype type);
  
      /** Divides given list on lists with correct & almost good answers and wrong ones. */
  void divideGoodAndBad(QList<TQAunit> *list, TanswerListPtr &goodList, TanswerListPtr &badList);
      /** Returns the lists of list of the same notes. 
       * The last list is a list of ignored answers - no notes eighter in answer nor questions.*/
  QList<TanswerListPtr> sortByNote(TanswerListPtr& answList, TexamLevel* level, bool& hasListUnrelated);
  
  QList<TanswerListPtr> sortByFret(TanswerListPtr& answList, TexamLevel* level, bool& hasListUnrelated);
  
  QList<TanswerListPtr> sortByKeySignature(TanswerListPtr& answList, TexamLevel* level, bool& hasListUnrelated);
  
  QList<TanswerListPtr> sortByAccidental(TanswerListPtr &answList, bool& hasListUnrelated);
  
//   QList<TanswerListPtr> divideQuestionsAndAnswers(TanswerListPtr &answList);
  

  

#endif // SORTING_H
