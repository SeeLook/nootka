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


#include "tlinearchart.h"
#include "sorting.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/tattempt.h>


TlinearChart::TlinearChart(QObject* parent) :
  TmainChart(parent)
{
}


void TlinearChart::init() {
// Determine y value type and its maximal value to prepare Y axis
  switch (p_chartSett.yValue) {
    case Tchart::e_YprepareTime: {
      quint32 prepTime = 0;
      for(int i = 0; i < p_currExam->count(); i++)
        prepTime = qMax(prepTime, p_currExam->question(i)->attempt(0)->prepareTime());
      setYaxisParams(static_cast<qreal>(prepTime) / 10.0, e_prepareTime);
      break;
    }
    case Tchart::e_YattemptsCount: {
      int attemptsNr = 0;
      for(int i = 0; i < p_currExam->count(); i++)
        attemptsNr = qMax(attemptsNr, p_currExam->question(i)->attemptsCount());
      setYaxisParams(static_cast<qreal>(attemptsNr), e_attemptsCount);
      break;
    }
    case Tchart::e_YplayedCount: {
      int playedNr = 0;
      for(int i = 0; i < p_currExam->count(); i++)
        playedNr = qMax(playedNr, p_currExam->question(i)->totalPlayBacks());
      setYaxisParams(static_cast<qreal>(playedNr), e_playedCount);
      break;
    }
    case Tchart::e_Yeffectiveness: {
      // 110% looks good cause 100% is real maximum and happens often, so all chart goes little down
      setYaxisParams(110.0, e_effectiveness);
      break;
    }
    default: {
      quint16 maxTime = 0;
      for(int i = 0; i < p_currExam->count(); i++)
        maxTime = qMax(maxTime, p_currExam->question(i)->time);
      setYaxisParams(static_cast<qreal>(maxTime) / 10.0, e_timeInSec);
      break;
    }
  }

  if (p_chartSett.order == e_byNumber) {
    prepareChart(p_currExam->count());
    qreal tSum = 0.0;
    int okCount = 0;
    for(int i = 0; i < p_currExam->count(); i++) {
      qreal t = static_cast<qreal>(p_currExam->question(i)->time);
      if (!p_currExam->question(i)->isWrong() || (p_currExam->question(i)->isWrong() && p_chartSett.inclWrongAnsw)) {
        okCount++;
        tSum += t;
      }
      p_averChunks << (okCount ? tSum / static_cast<qreal>(okCount) : 0.0);
    }
  }

  if (p_chartSett.order == e_byNote || p_chartSett.order == e_byFret ||
          p_chartSett.order == e_byKey || p_chartSett.order == e_byAccid ||
          p_chartSett.order == e_byQuestAndAnsw || p_chartSett.order == e_byMistake) {
      sort();
      int ln = 0;
      for (int i = 0; i < p_sortedLists.size(); i++)
        ln += p_sortedLists[i].size();
      prepareChart(ln);
  }
}


