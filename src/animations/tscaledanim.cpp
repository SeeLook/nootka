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

#include "tscaledanim.h"


TscaledAnim::TscaledAnim(QGraphicsItem* item, QObject* parent) :
	TabstractAnim(item, parent)
{
		installTimer();
}


void TscaledAnim::startScaling(qreal endScale, qreal midScale) {
		m_beginScale = item()->scale();
		m_endScale = endScale;
		m_midScale = midScale;
		m_stepCount = duration() / CLIP_TIME;
		m_currentStep = -1;
		if (m_midScale >= 0.0) {
			m_stepCount = m_stepCount / 2;
			m_scaleToGo = m_midScale;
		} else
			m_scaleToGo = m_endScale;
		
		timer()->start(CLIP_TIME);
		animationRoutine();
}


void TscaledAnim::animationRoutine() {
		m_currentStep++;
		if (m_currentStep <= m_stepCount) {
				item()->setScale(m_beginScale + easyValue((qreal)m_currentStep / (qreal)m_stepCount) * (m_scaleToGo - m_beginScale));
		} else if (m_midScale >= 0.0) { // second part of an animation - scale goes from mid val to end val
				m_scaleToGo = m_endScale;
				m_stepCount = duration() / CLIP_TIME - (duration() / CLIP_TIME) / 2;
				m_currentStep = -1;
				m_beginScale = item()->scale();
				m_midScale = -1.0; // reset it to stop performing second part second time
				animationRoutine(); // perform it immediately...
		} else {
				timer()->stop();
				emit finished();
		}
}




