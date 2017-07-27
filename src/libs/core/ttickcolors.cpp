/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#include "ttickcolors.h"
#include <QtCore/QtMath>


QColor TtickColors::startColor = Qt::green;
QColor TtickColors::middleColor = Qt::yellow;
QColor TtickColors::endColor = Qt::red;
QColor TtickColors::totalColor = QColor(117, 21, 86); // brown
QColor TtickColors::disabledColor = Qt::gray;


TtickColors::TtickColors(QObject* parent) :
  QObject(parent),
  m_divisor(6.0)
{}


void TtickColors::setWidth(qreal w) {
  if (w != m_width) {
    m_width = w;
    resize(m_width);
    emit widthChanged();
  }
}


void TtickColors::setDivisor(qreal d) {
  if (d != m_divisor) {
    m_divisor = d;
    resize(m_width);
    emit divisorChanged();
  }
}


void TtickColors::resize(qreal w) {
  int m_ticksCount = qFloor(w / m_divisor) + 1;
    m_tickColors.clear();
  for (int i = 0; i < m_ticksCount; i++) {
    if (i < qFloor(m_ticksCount * 0.3))
      m_tickColors << gradColorAtPoint(0.0, w * 0.3, startColor, middleColor, (i) * (w / m_ticksCount));
    else if (i < qFloor(m_ticksCount * 0.9))
      m_tickColors << gradColorAtPoint(w * 0.3, w * 0.9, middleColor, endColor, (i) * (w / m_ticksCount));
    else
      m_tickColors << gradColorAtPoint(w * 0.9, w, endColor, totalColor, (i) * (w / m_ticksCount));
  }
}


QColor TtickColors::gradColorAtPoint(qreal lineX1, qreal lineX2, const QColor& startC, const QColor& endC, qreal posC) {
  qreal segmentLength = qSqrt((lineX2 - lineX1) * (lineX2 - lineX1));
  qreal pdist = qSqrt((posC - lineX1) * (posC - lineX1));
  qreal ratio = pdist / segmentLength;
  int red = qBound(0, (int)(ratio * endC.red() + ( 1 - ratio) * startC.red()), 255);
  int green = qBound(0, (int)(ratio * endC.green() + (1 - ratio) * startC.green()), 255);
  int blue = qBound(0, (int)(ratio * endC.blue() + (1 - ratio) * startC.blue()), 255);
  return QColor(red, green, blue);
}
