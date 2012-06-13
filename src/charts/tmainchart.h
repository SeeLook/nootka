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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TMAINCHART_H
#define TMAINCHART_H

#include "tchart.h"

class TexamLevel;
class Tnote;

typedef QList<TQAunit*> TanswerListPtr; // definition of TQAunit list type


class Texam;

/** This class paints different types of charts with an exam data.
 */


class TmainChart : public Tchart
{
  Q_OBJECT
  
public:
  
  TmainChart(Texam *exam, Tsettings &settings, QWidget* parent = 0);
  virtual ~TmainChart();
  
  
  QList<Tnote> getTheSame(short noteNr, TexamLevel *level = 0);
      /** Calculates average reaction time of a given list.
       * If skipWrong = true, wrong answers are not taken to average. */
  double calcAverTime(TanswerListPtr &answers, bool skipWrong = false);
      /** Merges elements of each list ti single list of poiters to TQAunit. */
  TanswerListPtr mergeListOfLists(QList<TanswerListPtr> &listOfLists);
      /** converts QList with TQAunit to QList with pointer to them*/
  TanswerListPtr convertToPointers(QList<TQAunit> *examList);
  
      /** Divides given list on lists with correct & almost good answers and wrong ones. */
  void divideGoodAndBad(QList<TQAunit> *list, TanswerListPtr &goodList, TanswerListPtr &badList);
      /** Returns the lists of list of the same notes. 
       * The last list is a list of ignored answers - no notes eighter in answer nor questions. */
  QList<TanswerListPtr> sortByNote(TanswerListPtr &answList);
  
  QList<TanswerListPtr> sortByFret(TanswerListPtr &answList);
  
  
private:
      /** Performs common elements for all kinds of charts. */
  void prepareChart(int maxX);
  
private:
  Texam *m_exam;
  TmainLine *m_mainLine;
  Tsettings &m_settings;
};

#endif // TMAINCHART_H
