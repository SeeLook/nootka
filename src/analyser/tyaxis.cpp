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
#include <QApplication>
#include <QDebug>


TYaxis::TYaxis() :
  m_yScale(1),
  m_topTick(10),
  m_maxVal(11),
  m_multi(1),
  m_lenght(200)
{

    QFontMetrics metrics = QApplication::font();
    m_textPosOffset = metrics.boundingRect("X").height() / 2 - 1;
}

TYaxis::~TYaxis()
{}


void TYaxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    QRectF rect = boundingRect();
    qreal half = rect.width() /  2.0;
    painter->drawLine(half, 0, half, rect.height());
    painter->drawLine(half, 0, 2, 7);
    painter->drawLine(half, 0, rect.width() - 2, 7);
//     painter->drawText(rect.width() - 30, 0, "Axis");
    
    qreal maxT = m_maxVal;
    qreal multi = m_multi;
    while (maxT > 99) {
      multi = multi*10;
      maxT = maxT / 10;
    }
    int top = int(maxT) + 1 ;
    int loop = top;
    int multi2 = 1;

    if (top > 9) {
        loop = top / 10;
        multi2 = 10;
    }

    double yScale = (m_lenght - 15) / top;
    double nearTop = top * yScale;
    double step = (nearTop / top)  ;
//    qDebug() << top << loop << yScale << nearTop << step << multi2;
    bool halfTick = false;
    if (step > m_textPosOffset*2)
        halfTick = true;
    for (int i = 1; i <= loop; i++) {
        painter->drawLine(half, m_lenght - i*step*multi2, 1, m_lenght - i*step*multi2);
        painter->drawText(half + 3, m_lenght - i*step*multi2 + m_textPosOffset, QString::number(i*multi*multi2));
        if (halfTick) {
            painter->drawLine(half, m_lenght - (double)(i-0.5)*step*multi2, 1, m_lenght - (double)(i-0.5)*step*multi2);
            painter->drawText(half + 3, m_lenght - (double)(i-0.5)*step*multi2 + m_textPosOffset, QString::number((double)(i-0.5)*multi*multi2));
        }
    }
    if (loop != top) {
        painter->drawLine(half, m_lenght - nearTop, 1, m_lenght - nearTop);
        painter->drawText(half + 3, m_lenght - nearTop + m_textPosOffset, QString::number(top*multi));
    }
}

QRectF TYaxis::boundingRect() const
{
    QRectF rect(0 ,0, 10, m_lenght);
//     rect.translate(1, -m_lenght);
    return rect;
}

