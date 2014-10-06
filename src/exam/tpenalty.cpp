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
#include "texamview.h"
#include "tprogresswidget.h"
#include <exam/texam.h>
#include <QDebug>



Tpenalty::Tpenalty(Texam* exam, TexecutorSupply* supply, TexamView* examView, TprogressWidget* progress) :
	QObject(),
	m_examView(examView),
	m_progress(progress),
	m_exam(exam),
	m_supply(supply),
	m_blackQuestNr(-1), m_blackNumber(-1),
	m_penalCount(0),
	m_penalStep(65535)
{
	if (m_exam->isExercise()) { // Do not count penalties in exercising mode
			m_exam->setFinished(); // to avoid adding penalties in exercising
			m_supply->setFinished();
			m_progress->hide();
			m_examView->hide();
	} else {
			m_progress->show();
			m_examView->show();
			if (m_exam->isFinished()) {
					m_supply->setFinished();
					qDebug() << "Exam was finished";
				} else {
						int remained = (m_supply->obligQuestions() + m_exam->penalty()) - m_exam->count();
						remained = qMax(0, remained);
						if (remained < m_exam->blackCount()) {
							m_exam->increasePenaltys(m_exam->blackCount() - remained);
							qDebug() << "penalties number adjusted:" << m_exam->blackCount() - remained;
// 							m_progress->activate(m_exam, m_supply->obligQuestions());
						}
						if (remained == 0 && m_exam->blackCount() == 0) {
							m_supply->setFinished();
							m_exam->setFinished();
							qDebug() << "Finished exam was detected";
						}
				}
			m_examView->displayTime();
	}
	m_progress->activate(m_exam, m_supply->obligQuestions());
	m_examView->startExam(m_exam);
	updatePenalStep();
}


void Tpenalty::nextQuestion() {
	m_penalCount++;
	m_blackQuestNr = -1;
}


bool Tpenalty::ask() {
	if (m_exam->melodies()) {
		if (m_exam->blackNumbers()->size() && m_penalCount > m_penalStep) {
			m_penalCount = 0;
			int idInList = qrand() % m_exam->blackNumbers()->size();;
			m_blackNumber = m_exam->blackNumbers()->at(idInList);
			qDebug() << "black Number" << m_blackNumber << m_exam->blackNumbers();
			m_exam->blackNumbers()->removeAt(idInList);
			if (m_blackNumber != -1) {
				TQAunit blackU = m_exam->answList()->operator[](m_blackNumber);
				m_exam->curQ() = blackU;
				m_exam->curQ().unsetAnswered();
				m_exam->curQ().addMelody(m_exam->answList()->operator[](m_blackNumber).melody(), TQAunit::e_otherUnit, m_blackNumber);
				qDebug() << "penalty melody" << m_blackNumber;
				m_exam->curQ().time = 0;
				m_exam->curQ().setMistake(TQAunit::e_correct);
				return true;
			}
		}
	} else {
		if (m_exam->blackCount() && m_penalCount > m_penalStep) {
			qDebug("penalty");
			m_penalCount = 0;
			m_blackQuestNr = qrand() % m_exam->blacList()->size();
			m_exam->curQ() = m_exam->blacList()->operator[](m_blackQuestNr);
			m_exam->curQ().unsetAnswered();
			m_exam->curQ().time = 0;
			m_exam->curQ().setMistake(TQAunit::e_correct);
			return true;
		}
	}
	return false;
}


void Tpenalty::checkAnswer() {
	if (!m_exam->isExercise() && !m_exam->melodies()) {
		if (!m_exam->curQ().isCorrect() && !m_exam->isFinished()) { // finished exam hasn't got black list
					m_exam->blacList()->append(m_exam->curQ());
					if (m_exam->curQ().isNotSoBad())
						m_exam->blacList()->last().time = 65501;
					else
						m_exam->blacList()->last().time = 65502;
		}
	}
	if (!m_exam->melodies())
		m_exam->curQ().setAnswered();
	m_exam->sumarizeAnswer();
	if (!m_exam->melodies()) // when melody question counters are not ready here, setMelodyPenalties() will do it.
		m_examView->questionCountUpdate();	
	m_examView->reactTimesUpdate();
	m_examView->effectUpdate();
	if (!m_exam->isExercise()) {
		releaseBlackList();
		m_progress->progress();
		if (!m_exam->curQ().isCorrect())
				updatePenalStep();
		checkForCert();
	}
}


void Tpenalty::setMelodyPenalties() {
	if (m_exam->count() == 0)
		return;
	if (m_exam->curQ().answered())
		return;
	m_exam->curQ().setAnswered();
	if (!m_exam->isExercise() && m_exam->melodies()) {
		if (!m_exam->curQ().isCorrect() && !m_exam->isFinished()) {
				m_exam->addPenalties();
				if ((m_supply->obligQuestions() + m_exam->penalty() - m_exam->count()) > 0)
					m_penalStep = (m_supply->obligQuestions() + m_exam->penalty() - m_exam->count()) / m_exam->blackNumbers()->size();
				else
					m_penalStep = 0; // only penalties questions remained to ask in this exam
		}
		m_progress->progress();
		checkForCert();
	}
	m_examView->questionCountUpdate();
}



void Tpenalty::releaseBlackList() {
	if (!m_exam->melodies() && m_blackQuestNr != -1) { // decrease black list
		if (m_exam->blacList()->operator[](m_blackQuestNr).time == 65502)
        m_exam->blacList()->operator[](m_blackQuestNr).time--; // remains one penalty
		else
        m_exam->blacList()->removeAt(m_blackQuestNr); // delete - penalties cleared
	}
}


void Tpenalty::setBlackQuestion() {
	m_blackQuestNr = m_exam->blacList()->count() - 1;
}


void Tpenalty::checkForCert() {
	if (!m_supply->wasFinished() && m_exam->count() >= (m_supply->obligQuestions() + m_exam->penalty()) ) { // maybe enough
		if (m_exam->blackCount()) {
				m_exam->increasePenaltys(m_exam->blackCount());
				qDebug() << "penalties increased. Can't finish this exam yet.";
		} else {
				m_exam->setFinished();
				m_progress->setFinished();
				emit certificate();
				m_supply->setFinished();
		}
	}
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

//######################################################################
//#################   FORWARDED METHODS OF TexamView       #############
//######################################################################
void Tpenalty::pauseTime() {
	m_examView->pause();
}


void Tpenalty::continueTime() {
	m_examView->go();
}


void Tpenalty::updateExamTimes() {
	if (m_exam->melodies())
			setMelodyPenalties();
	m_examView->updateExam();
}


void Tpenalty::stopTimeView() {
	m_examView->stopExam();
}


void Tpenalty::startQuestionTime() {
	m_examView->questionStart();
}


void Tpenalty::stopQuestionTime() {
	m_examView->questionStop();
}






