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
  m_yScale(1),
  m_topTick(10),
  m_maxVal(10),
  m_multi(1),
  m_lenght(200)
{

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
    while ((maxT / 10.0) >= 1) {
      multi = multi*10;
      maxT = maxT / 10;
    }
    double top = ((int)(maxT / multi) +1 ) * multi;
    double step = (m_lenght - 10) / top;
    qDebug() << top << step;
    for (int i = 1; i <= top; i++) {
        painter->drawLine(half, i*step, 1, i*step);
    }
}

QRectF TYaxis::boundingRect() const
{
    QRectF rect(0 ,0, 10, m_lenght);
//     rect.translate(1, -m_lenght);
    return rect;
}

