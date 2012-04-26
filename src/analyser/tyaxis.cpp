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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tyaxis.h"
#include <QPainter>
#include <QDebug>


TYaxis::TYaxis() :
  m_topTick(10),
  m_maxVal(11),
  m_multi(1),
  m_halfTick(false)
{
    m_textPosOffset = (rectBoundText("X").height() / 2) -3;
}

void TYaxis::setMaxValue(qreal val) {
    m_maxVal = val;
    qreal maxT = m_maxVal;
    while (maxT > 99) {
      m_multi = m_multi*10;
      maxT = maxT / 10;
    }
    m_top = int(maxT) + 1 ;
    m_loop = m_top;
    m_multi2 = 1;
    if (m_top > 9) {
        m_loop = m_top / 10;
        m_multi2 = 10;
    }
    axisScale = (length() - (2 * axisWidth)) / m_top;
    m_nearTop = m_top * axisScale;
    m_step = (m_nearTop / m_top);
    if (m_step > m_textPosOffset*2)
        m_halfTick = true;

}

void TYaxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QRectF rect = boundingRect();
    qreal half = rect.width() /  2.0;
    painter->drawLine(half, 0, half, rect.height());
    drawArrow(painter, QPointF(half, 0), false);
    
    for (int i = 1; i <= m_loop; i++) {
        painter->drawLine(half, length() - i*m_step*m_multi2, 1, length() - i*m_step*m_multi2);
        painter->drawText(half + 3, length() - i*m_step*m_multi2 + m_textPosOffset, QString::number(i*m_multi*m_multi2));
        if (m_halfTick) {
            painter->drawLine(half, length() - (double)(i-0.5)*m_step*m_multi2, 1, length() - (double)(i-0.5)*m_step*m_multi2);
            painter->drawText(half + 3, length() - (double)(i-0.5)*m_step*m_multi2 + m_textPosOffset, QString::number((double)(i-0.5)*m_multi*m_multi2));
        }
    }
    if (m_loop != m_top) {
        painter->drawLine(half, length() - m_nearTop, 1, length() - m_nearTop);
        painter->drawText(half + 3, length() - m_nearTop + m_textPosOffset, QString::number(m_top*m_multi));
    }
}

QRectF TYaxis::boundingRect()
{
    QRectF rect(0 ,0, axisWidth , length());
//     rect.translate(1, -m_lenght);
    return rect;
}

