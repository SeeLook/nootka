/***************************************************************************
 *   Copyright (C) 2012-2018 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tabstractaxis.h"


/* static*/
const int TabstractAxis::tickSize = 4;
const int TabstractAxis::axisWidth = 15;
const int TabstractAxis::arrowSize = 9;

void TabstractAxis::drawArrow(QPainter *painter, QPointF endPoint, bool isHorizontal) {
  QPointF points[3];
  points[0] = endPoint;
  if (isHorizontal) {
      points[1] = QPointF(endPoint.x() - arrowSize, endPoint.y() - tickSize);
      points[2] = QPointF(endPoint.x() - arrowSize, endPoint.y() + tickSize);
  } else {
      points[1] = QPointF(endPoint.x() - tickSize, endPoint.y() + arrowSize);
      points[2] = QPointF(endPoint.x() + tickSize, endPoint.y() + arrowSize);
  }
  painter->setBrush(QBrush(painter->pen().color()));
  painter->drawPolygon(points, 3);
}


TabstractAxis::TabstractAxis() :
  m_font(qApp->font()),
  m_length(200),
  axisScale(1)
{
}

void TabstractAxis::setLength(qreal len) {
  m_length = len;
//   update(boundingRect());
}

void TabstractAxis::setFont(QFont f) {
  m_font = f;
//   update(boundingRect());
}


QRectF TabstractAxis::rectBoundText(QString txt) const{
  const int padd = 2;
  QFontMetrics metrics(m_font);
  QRectF rect = metrics.boundingRect(txt);
  rect.adjust(-padd, -padd, +padd, +padd);
  rect.translate(-rect.center());
  return rect;
}



