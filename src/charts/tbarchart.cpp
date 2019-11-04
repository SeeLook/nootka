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
#include <exam/texam.h>


TbarChart::TbarChart(QObject* parent) :
  TmainChart(parent)
{
}


void TbarChart::init() {
  p_chartSett.separateWrong = false;
  sort();
  if (p_chartSett.order != e_byMistake) {
      qreal maxTime = 0.0;
      for (int i = 0; i < p_sortedLists.size(); i++) {
        maxTime = qMax(maxTime, p_sortedLists[i].averTime());
      }
      setYaxisParams(maxTime / 10.0, Tchart::e_timeInSec);
  } else { // For e_byMistake we are looking for highest amount of questions in a group
      int maxSize = 0;
      for (int i = 0; i < p_sortedLists.size(); i++) {
        maxSize = qMax(maxSize, p_sortedLists[i].size());
      }
      setYaxisParams(static_cast<qreal>(maxSize), e_questionNr);
  }
  prepareChart(p_sortedLists.size() - (p_hasListUnrelated ? 1 : 0));
}

