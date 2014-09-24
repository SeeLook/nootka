/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#include "tpenalty.h"
#include "texecutorsupply.h"
#include <exam/texam.h>
#include <QDebug>



Tpenalty::Tpenalty(Texam* exam, TexecutorSupply* supply) :
	m_exam(exam),
	m_supply(supply),
	m_blackQuestNr(-1),
	m_penalCount(0),
	m_penalStep(65535)
{
	updatePenalStep();
}


bool Tpenalty::ask() {
	if (m_exam->blackCount() && m_penalCount > m_penalStep) {
			qDebug("penalty");
			m_penalCount = 0;
			m_blackQuestNr = qrand() % m_exam->blacList()->size();
			m_exam->curQ() = m_exam->blacList()->operator[](m_blackQuestNr);
			m_exam->curQ().time = 0;
			m_exam->curQ().setMistake(TQAunit::e_correct);
			return true;
	} else
			return false;
}


void Tpenalty::releaseBlackList() {
	if (m_blackQuestNr != -1) { // decrease black list
		if (m_exam->blacList()->operator[](m_blackQuestNr).time == 65502)
        m_exam->blacList()->operator[](m_blackQuestNr).time--; // remains one penalty
		else
        m_exam->blacList()->removeAt(m_blackQuestNr); // delete - penalties cleared
	}
}


void Tpenalty::setBlackQuestion() {
	m_blackQuestNr = m_exam->blacList()->count() - 1;
}



void Tpenalty::updatePenalStep() {
	if (m_supply->wasFinished())
			return;
	if (m_exam->blacList()->isEmpty())
		m_penalStep = 65535;
	else {
		if ((m_supply->obligQuestions() + m_exam->penalty() - m_exam->count()) > 0)
			m_penalStep = (m_supply->obligQuestions() + m_exam->penalty() - m_exam->count()) / m_exam->blackCount();
		else
			m_penalStep = 0; // only penalties questions remained to ask in this exam
	}
}

