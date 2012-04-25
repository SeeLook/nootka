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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tabstarctaxis.h"
#include <QApplication>
#include <QPainter>


/* static*/
int TabstarctAxis::m_tickSize = 4;
int TabstarctAxis::m_axisWidth = 10;
int TabstarctAxis::m_arrowSize = 7;

void TabstarctAxis::drawArrow(QPainter &painter, QPointF endPoint) {
    QPointF points[3];
    points[0] = endPoint;
    points[1] = QPointF(endPoint.x() - m_arrowSize, endPoint.x() - m_tickSize);
    points[2] = QPointF(endPoint.x() - m_arrowSize, endPoint.x() + m_tickSize);
    painter.drawPolygon(points, 3);
}




TabstarctAxis::TabstarctAxis() :
    m_font(QApplication::font()),
    m_length(200)
{
}

void TabstarctAxis::setLength(qreal len) {
    m_length = len;
    update(boundingRect());
}

void TabstarctAxis::setFont(QFont f) {
    m_font = f;
    update(boundingRect());
}


QRectF TabstarctAxis::rectBoundText(QString txt) {
    const int padd = 2;
    QFontMetrics metrics = m_font;
    QRectF rect = metrics.boundingRect(txt);
    rect.adjust(-padd, -padd, +padd, +padd);
    rect.translate(-rect.center());
    return rect;
}



