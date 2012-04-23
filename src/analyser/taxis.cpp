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

#include "taxis.h"
#include <QPainter>

Taxis::Taxis() :
  m_width(200)
{

}

Taxis::~Taxis()
{}


void Taxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  QRectF rect = boundingRect();
  qreal half = rect.height() /  2.0;
  painter->drawLine(0, half, rect.width(), half);
  painter->drawLine(rect.width(), half, rect.width() - 7, 2);
  painter->drawLine(rect.width(), half, rect.width() - 7, rect.height() - 2);
  painter->drawText(rect.width() - 30, 0, "Axis");
  int b = m_width / 10 - 1;
  for (int i=1; i < b; i++) {
    painter->drawLine(i*10, half, i*10, half + 4);
  }
  
}

QRectF Taxis::boundingRect() const
{
  QRectF rect(0 ,0, m_width, 10);
  return rect;
}

void Taxis::setWidth(qreal w) {
  m_width = w;
  update(boundingRect());
}

