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


#include "tchart.h"
#include "tgroupedqaunit.h"
#include <exam/textrans.h>


Tchart::Tchart(QObject* parent) :
  QObject(parent)
{
}


void Tchart::setYaxisParams(qreal maxV, Tchart::Eunit yUnit) {
  m_maxValue = maxV;
  qreal maxYtick = m_maxValue, step = 1.0;
  if (yUnit == e_effectiveness) {
      maxYtick = 110.0;
      step = 25.0;
  } else {
      if (m_maxValue < 2.0) {
          maxYtick = 2.0;
          if (yUnit == e_timeInSec || yUnit == e_prepareTime)
            step = 0.5;
      } else if (m_maxValue < 8.0) {
          // step = 1.0; max is max
      } else if (m_maxValue < 11.0) {
          step = 2.0;
      } else if (m_maxValue < 30.0)  {
//           maxYtick = m_maxValue < 21.0 ? 20.0 : 30.0;
          step = 5.0;
      } else if (m_maxValue < 60.0) {
          step = 10.0;
          maxYtick = static_cast<qreal>(static_cast<int>(m_maxValue / 10.0) * 10);
      } else if (m_maxValue < 111.0) {
          step = yUnit == e_timeInSec || yUnit == e_prepareTime ? 30.0 : 25.0;
      } else if (m_maxValue < 601.0) {
          step = yUnit == e_timeInSec || yUnit == e_prepareTime ? 60.0 : 100.0;
      } else if (m_maxValue < 1000.0) {
          step = yUnit == e_timeInSec || yUnit == e_prepareTime ? 300.0 : 250.0;
      } else if (m_maxValue < 10000.0) {
          step = m_maxValue < 7001.0 ? 1000.0 : 2500.0;
      } else {
          step = yUnit == e_timeInSec || yUnit == e_prepareTime ? 7200.0 : 10000.0;
      }
  }
  for (qreal i = step; i < maxYtick + 0.1; i += step) {
    m_yTickList << i;
  }
  setUnit(yUnit);
}


void Tchart::setUnit(Tchart::Eunit unit) {
  switch (unit) {
    case e_timeInSec:
      m_unitDesc = QObject::tr("time [s]", "unit of Y axis");
      break;
    case e_questionNr:
      m_unitDesc = questionsNumberTxt() + QLatin1String(" [ ]");
      break;
    case e_prepareTime:
      m_unitDesc = prepareTimeTxt() + QLatin1String(" [s]");
      break;
    case e_attemptsCount:
      m_unitDesc = attemptsNumberTxt() + QLatin1String(" [ ]");
      break;
    case e_effectiveness:
      m_unitDesc = TexTrans::effectTxt().toLower() + QLatin1String(" [%]");
      break;
    case e_playedCount:
      m_unitDesc = playedNumberTxt() + QLatin1String(" [ ]");
  }
  m_unit = unit;
}
