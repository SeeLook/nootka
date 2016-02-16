/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#include "tfadeanim.h"
#include <QGraphicsItem>


TfadeAnim::TfadeAnim(QGraphicsItem* item, QObject* parent) :
  TabstractAnim(item, parent)
{
  
}


void TfadeAnim::startFade(qreal endOpacity, qreal midOpacity) {
	m_startOp = item()->opacity();
	m_endOp = endOpacity;
	m_midOp = midOpacity;
	int stepNr = duration() / CLIP_TIME;
	if (m_midOp != -1) {
			stepNr = stepNr / 2;
			m_opacityToGo = m_midOp;
	} else
			m_opacityToGo = m_endOp;
	initAnim(-1, stepNr);
}


void TfadeAnim::animationRoutine() {
  nextStep();
  if (currentStep() <= stepsNumber()) {
      item()->setOpacity(m_startOp + (m_opacityToGo - m_startOp) * easyValue((qreal)currentStep() / (qreal)stepsNumber()));
  } else if (m_midOp != -1) {
			m_opacityToGo = m_endOp;
			setStepNumber(duration() / CLIP_TIME - (duration() / CLIP_TIME) / 2);
			resetStepCounter();
			m_startOp = item()->opacity();
			m_midOp = -1;
			animationRoutine();
  } else {
			item()->setOpacity(m_endOp);
      stopAnim();
	}
}
