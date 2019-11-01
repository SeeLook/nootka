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
#include "txaxis.h"
#include "tyaxis.h"
#include "tabstractaxis.h"
#include "tgroupedqaunit.h"
#include <exam/textrans.h>

#include <QtCore/qdebug.h>
#include "checktime.h"


Tchart::Tchart(QQuickItem* parent) :
  QQuickItem(parent)
{
//   scene = new QGraphicsScene(this);

//   p_bgRect = scene->addRect(0.0, 0.0, 100.0, 350.0, Qt::NoPen, Qt::NoBrush);

//   yAxis = new TYaxis();
//   yAxis->setParentItem(p_bgRect);
//   yAxis->setLength(300);
//   yAxis->setMaxValue(9);
//   yAxis->setPos(45, 0);
//   yAxis->setZValue(55);

//   xAxis = new TXaxis();
//   xAxis->setParentItem(p_bgRect);
//   xAxis->setLength(550);
//   xAxis->setPos(52, yAxis->boundingRect().height() - 7);
}


void Tchart::setMaxValue(qreal m, bool allowHalf) {
  qreal multi1 = 1.0;
  m_maxValue = m;
  qreal maxT = m_maxValue;
  while (maxT > 99.0) {
    multi1 *= 10.0;
    maxT = maxT / 10.0;
  }
  int topVal = static_cast<int>(maxT) + 1;
  int loopCnt = topVal;
  qreal multi2 = 1.0;
  if (topVal > 9) {
    loopCnt = topVal / 10.0;
    multi2 = 10.0;
  }
  qreal shift = allowHalf && (m_unit == e_timeInSec || m_unit == e_prepareTime) && multi2 > 1.0 ? 0.5 : 1.0;
  m_yTickList.clear();
  for (qreal i = shift; i <= static_cast<qreal>(loopCnt); i += shift) {
    m_yTickList << i * multi1 * multi2;
  }
//   axisScale = ((length() - (2 * arrowSize)) / (m_top * m_multi));
//   if (allowHalf) { // check is enough place for half ticks
//     if ( ((mapValue((m_loop - 1) * m_multi * m_multi2) - mapValue(m_loop * m_multi * m_multi2))) > m_textPosOffset * 4)
//       m_halfTick = true;
//   }
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
