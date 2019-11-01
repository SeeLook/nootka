/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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

#include "tbarchart.h"
// #include "tyaxis.h"
// #include "txaxis.h"
#include "tbar.h"
#include <exam/texam.h>
#include <QtCore/qdebug.h>


TbarChart::TbarChart(QQuickItem* parent) :
  TmainChart(parent)
{
}


TbarChart::~TbarChart(){}


void TbarChart::init() {
  p_chartSett.separateWrong = false;
  sort();
  if (p_chartSett.order != Tchart::e_byMistake) {
      qreal maxTime = 0.0;
      for (int i = 0; i < p_sortedLists.size(); i++) {
        maxTime = qMax(maxTime, p_sortedLists[i].averTime());
      }
      setMaxValue(maxTime / 10.0);
      setUnit(Tchart::e_timeInSec);
//       yAxis->setMaxValue((double)maxTime / 10.0);
  } else { // For e_byMistake we are looking for highest amount of questions in a group
      int maxSize = 0;
      for (int i = 0; i < p_sortedLists.size(); i++) {
        maxSize = qMax(maxSize, p_sortedLists[i].size());
      }
      setMaxValue(static_cast<qreal>(maxSize));
      setUnit(Tchart::e_questionNr);
//       yAxis->setMaxValue((double)maxTime);
//       yAxis->setUnit(TYaxis::e_questionNr);
  }

//   xAxis->setAnswersForBarChart(p_sortedLists); // TODO
  prepareChart(p_sortedLists.size() - (p_hasListUnrelated ? 1 : 0));

//   int lastItem = p_sortedLists.size();
//   if (p_hasListUnrelated)
//     lastItem--; // don't show unrelated
//   for(int i = 0; i < lastItem; i++) {
//     Tbar *bar;
//     if (p_chartSett.order != Tchart::e_byMistake)
//       bar = new Tbar(yAxis->axisFactor() * (p_sortedLists[i].averTime() / 10.0), &p_sortedLists[i]);
//     else // Y value is number of questions, bar generates tip and has to know about type to skip some data
//       bar = new Tbar(yAxis->axisFactor() * (p_sortedLists[i].size()), &p_sortedLists[i], Tbar::e_mistakes);
//     setParentForItem(bar);
//     bar->setPos(xAxis->mapValue(i + 1) + xAxis->pos().x(), yAxis->boundingRect().height() - 2);
//   }
}

