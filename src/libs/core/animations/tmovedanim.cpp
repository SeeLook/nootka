/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tmovedanim.h"
// #include <QDebug>


TmovedAnim::TmovedAnim(QGraphicsItem* item, QObject* parent) :
	TabstractAnim(item, parent),
	m_currStep(0),
	m_line(0)
{
	m_line = new QGraphicsLineItem();
	if (item->type() == m_line->type()) {
		delete m_line;
		m_line = qgraphicsitem_cast<QGraphicsLineItem*>(item);
	} else {
		delete m_line;
		m_line = 0;
	}
}


void TmovedAnim::startMoving(const QPointF &start, const QPointF &stop) {
	installTimer();
	m_step = duration() / CLIP_TIME;
	m_startPos = start;
	m_endPos = stop;
	timer()->start(CLIP_TIME);
	item()->show();
	animationRoutine();
}


void TmovedAnim::animationRoutine() {
	m_currStep++;
	if (m_currStep <= m_step) {
			qreal xx, yy, easingcurve;
			easingcurve = easyValue((qreal)m_currStep / (qreal)m_step);
			xx = m_startPos.x() + ((m_endPos.x() - m_startPos.x()) / (qreal)m_step) * m_currStep * easingcurve;
			yy = m_startPos.y() + ((m_endPos.y() - m_startPos.y()) / (qreal)m_step) * m_currStep * easingcurve;
			if (m_line) {
				qreal /*xStep,*/ yStep; // line moves only on y axis
// 				xStep = (m_endPos.x() - m_startPos.x()) / (qreal)m_step;
				yStep = (m_endPos.y() - m_startPos.y()) / (qreal)m_step;
				m_line->setLine(m_line->line().p1().x(), m_line->line().p1().y() + yStep, 
												m_line->line().p2().x(), m_line->line().p2().y() + yStep);
			} else
					item()->setPos(xx, yy);
	} else {
		if (m_line)
			m_line->setLine(m_line->line().p1().x(), m_endPos.y(), m_line->line().p2().x(), m_endPos.y());
		else
			item()->setPos(m_endPos);
		m_currStep = 0;
		stopAnim();
	}
}






