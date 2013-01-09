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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TMAINCHART_H
#define TMAINCHART_H

#include "tchart.h"
#include "tgroupedqaunit.h"

class TexamLevel;
class Tnote;
class Texam;

/** This is global class for bar and linear charts.
 * It performs some common methods
 */
class TmainChart : public Tchart
{
  Q_OBJECT 
public:
  
  TmainChart(Texam *exam, Tsettings &settings, QWidget* parent = 0);
  virtual ~TmainChart();
  

protected:
    /** Sorts exam data by params given in chartSett. 
     * Initialises sortedLists, hasListUnrelated and kindOfAccids. */
  void sort();
      /** Performs common elements for all kinds of charts. */
  void prepareChart(int maxX);

  Tsettings chartSett;
  Texam *currExam;
  TmainLine *m_mainLine;
      /** Returns true if list contains unrelated list of questions. */
  bool hasListUnrelated;
  TgroupedQAunit goodAnsw, badAnsw;
  QList<TgroupedQAunit> sortedLists;
  int goodSize; // number of lists with good answers
  QList<char> kindOfAccids;
  
protected slots:
  void sceneMoved();
  void updateSceneAfterMove();

};

#endif // TMAINCHART_H
