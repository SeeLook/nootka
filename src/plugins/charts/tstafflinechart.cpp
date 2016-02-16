/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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

#include "tstafflinechart.h"
#include <QPainter>
#include <QGraphicsEffect>
#include <qstyleoption.h>


#define DISTANCE (2)

TstaffLineChart::TstaffLineChart()
{
  m_vector.setX(1);
  m_vector.setY(1);
}


void TstaffLineChart::setLine(QPointF from, QPointF to) {
  m_vector.setX(qRound(to.x() - from.x()));
  m_vector.setY(qRound(to.y() - from.y()));
  setPos(qRound(from.x()), qRound(from.y()));
}


void TstaffLineChart::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(widget)
  painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter->setPen(QPen(option->palette.text().color(), 0.5));
  for (qreal i = -2.0; i < 3.0; i++)
    painter->drawLine(0.0, i * DISTANCE, m_vector.x(), m_vector.y() + i * DISTANCE);
}


QPainterPath TstaffLineChart::shape() const {
	QPainterPath path;
	path.addRect(boundingRect());
	return path;
}


QRectF TstaffLineChart::boundingRect() const {
  QRectF rect(0, qMin<qreal>(-2 * DISTANCE, m_vector.y() -2 * DISTANCE), m_vector.x(), qAbs<qreal>(m_vector.y()) + (DISTANCE * 4));
  return rect;
}





