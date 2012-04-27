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
#include <QApplication>

Taxis::Taxis() :
    m_width(200),
    m_font(QApplication::font())
{

}

Taxis::~Taxis()
{}


void Taxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
  QRectF rect = boundingRect();
//  qreal half = rect.height() /  2.0;
  painter->drawLine(0, 5, rect.width(), 5);
  painter->drawLine(rect.width(), 5, rect.width() - 7, 2);
  painter->drawLine(rect.width(), 5, rect.width() - 7, 9);
  painter->drawText(rect.width() - 30, 0, "Axis");
  int b = m_width / 50 - 1;
  for (int i=1; i <= b; i++) {
    painter->drawLine(i*50, 5, i*50, 9);
    QString tt = QString("Q %1").arg(i);
    QRectF rr = outRect(tt);
    painter->drawText(QRectF(i*50 - rr.width()/2, 15, rr.width(), rr.height()), Qt::AlignCenter, tt);
  }
  
}

QRectF Taxis::boundingRect() const
{
  QRectF rect(0 ,0, m_width, 40);
  return rect;
}

void Taxis::setWidth(qreal w) {
  m_width = w;
  update(boundingRect());
}

QRectF Taxis::outRect(QString txt) {
    const int padd = 2;
    QFontMetrics metrics = m_font;
    QRectF rect = metrics.boundingRect(txt);
    rect.adjust(-padd, -padd, +padd, +padd);
    rect.translate(-rect.center());
    return rect;
}
