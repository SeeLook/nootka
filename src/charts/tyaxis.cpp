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
#include "tqaunit.h"
#include <QPainter>
#include <QWidget>
#include <QDebug>


TYaxis::TYaxis() :
  m_maxVal(11),
  m_multi(1),
  m_halfTick(false)
{
    m_textPosOffset = (rectBoundText("X").height() / 4);
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
    axisScale = ((length() - (2 * arrowSize)) / (m_top*m_multi));
    // check is enought place for half ticks
    if ( ((mapValue((m_loop-1)*m_multi*m_multi2) - mapValue(m_loop*m_multi*m_multi2))) > m_textPosOffset*4)
        m_halfTick = true;
//     qDebug() << m_top << axisScale << m_top*axisScale << length() - (2 * arrowSize) << length() << m_loop << mapValue(m_top) << m_multi << m_multi2;
}

void TYaxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)

    qreal half = axisWidth / 2;
    QColor bg = widget->palette().base().color();
    bg.setAlpha(200);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawRect(0, 0, axisWidth * 3, length() - 1);
    painter->setPen(QPen(widget->palette().text().color(), 2));
    painter->drawLine(half, 0, half, length());
    drawArrow(painter, QPointF(half, 0), false);
    
    double shift = 1.0;
    if (m_halfTick) shift = 0.5;
    for (double i = shift; i <= m_loop; i=i+shift) {
        double v= i*m_multi*m_multi2;
//         qDebug() << i << v << mapValue(v);
        painter->drawLine(half, mapValue(v), half - tickSize, mapValue(v));
        painter->drawText(half + 3, mapValue(v) + m_textPosOffset, QString::number(i*m_multi*m_multi2));
    }
    // paint top tick only if there is free room
    if ( ((mapValue(m_loop*m_multi*m_multi2) - mapValue(m_top*m_multi)) ) > m_textPosOffset*4) {
        painter->drawLine(half, mapValue(m_top*m_multi), half - tickSize, mapValue(m_top*m_multi));
        painter->drawText(half + 3, mapValue(m_top*m_multi) + m_textPosOffset, QString("%1[s]").arg(m_top*m_multi));
    }
}

QRectF TYaxis::boundingRect()
{
    QRectF rect(0 , 0, axisWidth + rectBoundText(QString::number(m_maxVal)).width() , length());
//     rect.translate(1, -length());
    return rect;
}

 void TYaxis::getYforGrid(QList< double >& yList) {
  yList.clear();
  for (double i = 1; i <= m_loop; i++) {
    yList << mapValue(i*m_multi*m_multi2);
  }
}
