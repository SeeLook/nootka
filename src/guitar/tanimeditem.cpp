/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tanimeditem.h"
#include <QTimer>



TanimedItem::TanimedItem(QGraphicsItem* item, QObject* parent) :
	QObject(parent),
	m_item(item), 
	m_currStep(0)
{
	setDuration(150); // default duration - 150 ms
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(movingSlot()));
}


void TanimedItem::startMoving(const QPointF &start, const QPointF &stop) {
	m_startPos = start;
	m_endPos = stop;
	m_timer->start(CLIP_TIME);
	movingSlot();
}


void TanimedItem::movingSlot() {
	m_currStep++;
	if (m_currStep <= m_step) {
			qreal xx, yy;
			xx = m_startPos.x() + ((m_endPos.x() - m_startPos.x()) / (qreal)m_step) * m_currStep;
			yy = m_startPos.y() + ((m_endPos.y() - m_startPos.y()) / (qreal)m_step) * m_currStep;
			m_item->setPos(xx, yy);
	} else {
		m_timer->stop();
		m_item->setPos(m_endPos);
		m_currStep = 0;
		emit finished();
	}
}






