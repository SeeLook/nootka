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

#include "tcombinedanim.h"


TcombinedAnim::TcombinedAnim(QGraphicsItem* item, QObject* parent) :
	TabstractAnim(item, parent),
	m_moving(0),
	m_scaling(0),
	m_coloring(0),
	m_morphing(0)
{
		installTimer();
}

void TcombinedAnim::setMoving(const QPointF& start, const QPointF& stop) {
		m_startMov = start;
		m_stopMov = stop;
		if (!m_moving) {
				m_moving = new TmovedAnim(item(), this);
				prepareAnim(m_moving);
		}
}


void TcombinedAnim::setScaling(qreal scaleEnd, qreal scaleMid) {
		m_scaleEnd = scaleEnd;
		m_scaleMid = scaleMid;
		if (!m_scaling) {
				m_scaling = new TscaledAnim(item(), this);
				prepareAnim(m_scaling);
		}
}


void TcombinedAnim::setColoring(const QColor& endColor, const QColor& midColor) {
		m_endColor = endColor;
		m_midColor = midColor;
		if (!m_coloring) {
			m_coloring = new TcoloredAnim(item(), this);
			prepareAnim(m_coloring);
		}
}


void TcombinedAnim::setMorphing(const QLineF& line, qreal width, bool toLine) {
		m_line = line;
		m_lineWidth = width;
		m_toLine = toLine;
		if (!m_morphing) {
			m_morphing = new TmorphedAnim(qgraphicsitem_cast<QGraphicsEllipseItem*>(item()));
			prepareAnim(m_morphing);
		}
}



void TcombinedAnim::startAnimations() {
	if (m_moving)
		m_moving->startMoving(m_startMov, m_stopMov);
	if (m_scaling)
		m_scaling->startScaling(m_scaleEnd, m_scaleMid);
	if (m_coloring)
		m_coloring->startColoring(m_endColor, m_midColor);
	if (m_morphing)
		m_morphing->startMorphing(m_line, m_lineWidth, m_toLine);
	m_doEmit = true;
}


/** We are sending only single signal about animation finish. */
void TcombinedAnim::finishSlot() {
	if (m_doEmit) { // to avoid emitting finished() signal more than once
		emit finished();
		m_doEmit = false;
	}
}


void TcombinedAnim::prepareAnim(TabstractAnim* anim) {
	anim->setDuration(duration()); // set the same duration like this
	anim->setTimer(timer()); // connect this timer
	connect(anim, SIGNAL(finished()), this, SLOT(finishSlot()));
}



