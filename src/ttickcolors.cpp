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
  QObject(parent)
{

}


void TtickColors::resize(qreal w) {
  int m_ticksCount = qFloor(w / 6);
//    m_hiTickStep = ((float)height() * 0.66) / m_ticksCount;
    m_tickColors.clear();
  for (int i = 0; i < m_ticksCount; i++) {
      if (i <= m_ticksCount * 0.2)
        m_tickColors << startColor;
        else if (i <= m_ticksCount * 0.5)
          m_tickColors << gradColorAtPoint(w * 0.2, w * 0.55, startColor, middleColor,
                                           (i + 1) * (w / m_ticksCount));
        else if (i <= m_ticksCount * 0.6)
            m_tickColors << middleColor;
        else if ( i <= m_ticksCount * 0.8)
            m_tickColors << gradColorAtPoint(w * 0.6, w * 0.82,
                                         middleColor, endColor, (i + 1) * (w / m_ticksCount));
        else
            m_tickColors << gradColorAtPoint(w * 0.8, w,
                                         endColor, totalColor, (i + 1) * (w / m_ticksCount));
    }
}


QColor TtickColors::gradColorAtPoint(float lineX1, float lineX2, QColor startC, QColor endC, float posC) {
  float segmentLength = qSqrt((lineX2 - lineX1) * (lineX2 - lineX1));
  double pdist = qSqrt((posC - lineX1) * (posC - lineX1));
  double ratio = pdist / segmentLength;
  int red = qBound(0, (int)(ratio * endC.red() + ( 1 - ratio) * startC.red()), 255);
  int green = qBound(0, (int)(ratio * endC.green() + (1 - ratio) * startC.green()), 255);
  int blue = qBound(0, (int)(ratio * endC.blue() + (1 - ratio) * startC.blue()), 255);
  return QColor(red, green, blue);
}
