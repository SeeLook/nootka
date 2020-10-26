/***************************************************************************
 *   Copyright (C) 2014-2020 by Tomasz Bojczuk                             *
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

#include "tpenalty.h"
#include "texecutorsupply.h"
#include "texamview.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <QtCore/qdebug.h>



Tpenalty::Tpenalty(Texam* exam, TexecutorSupply* supply) :
  QObject(),
  m_exam(exam),
  m_supply(supply),
  m_blackQuestNr(-1), m_blackNumber(-1),
  m_penalStep(65535),
  m_penalCount(0)
{
  if (m_exam->isExercise()) { // Do not count penalties in exercising mode
      m_exam->setFinished(); // to avoid adding penalties in exercising
      m_supply->setFinished();
  } else {
      if (m_exam->isFinished()) {
          m_supply->setFinished();
          qDebug() << "Exam was finished";
      } else {
          int remained = (m_supply->obligQuestions() + m_exam->penalty()) - m_exam->count();
          remained = qMax(0, remained);
          if (remained < m_exam->blackCount()) {
            m_exam->increasePenaltys(m_exam->blackCount() - remained);
            qDebug() << "[Tpenalty] penalties number adjusted:" << m_exam->blackCount() - remained;
          }
          if (remained == 0 && m_exam->blackCount() == 0) {
            m_supply->setFinished();
            m_exam->setFinished();
            qDebug() << "[Tpenalty] Finished exam was detected";
          }
        }
      RESULTS->displayTime();
  }
  RESULTS->startExam(m_exam, m_supply->obligQuestions());
  updatePenalStep();
}


void Tpenalty::nextQuestion() {
  m_penalCount++;
  m_blackQuestNr = -1;
  m_blackNumber = -1;
}


bool Tpenalty::ask() {
  if (m_exam->melodies()) {
      if (m_exam->blackNumbers()->size() && m_penalCount > m_penalStep) {
        m_penalCount = 0;
        int idInList = qrand() % m_exam->blackNumbers()->size();
        m_blackNumber = m_exam->blackNumbers()->at(idInList);
        qDebug() << "[Tpenalty]" << "penalty melody";
        m_exam->blackNumbers()->removeAt(idInList);
        if (m_blackNumber != -1) {
          m_exam->curQ()->copy(*m_exam->answList()->operator[](m_blackNumber)); // copy black question into last unit
          m_exam->curQ()->unsetAnswered();
          m_exam->curQ()->addMelody(m_exam->answList()->operator[](m_blackNumber)->melody(), TQAunit::e_srcOtherUnit, m_blackNumber);
          m_exam->curQ()->time = 0;
          m_exam->curQ()->setMistake(TQAunit::e_correct);
          return true;
        }
      }
  } else {
      if (m_exam->blackCount() && m_penalCount > m_penalStep) {
        qDebug() << "[Tpenalty]" << "penalty question";
        m_penalCount = 0;
        m_blackQuestNr = qrand() % m_exam->blacList()->size();
        m_exam->curQ()->copy(m_exam->blacList()->operator[](m_blackQuestNr));
        m_exam->curQ()->unsetAnswered();
        m_exam->curQ()->time = 0;
        m_exam->curQ()->setMistake(TQAunit::e_correct);
        return true;
      }
  }
  return false;
}


void Tpenalty::checkAnswer() {
  if (!m_exam->isExercise() && !m_exam->melodies()) {
    if (!m_exam->curQ()->isCorrect() && !m_exam->isFinished()) { // finished exam hasn't got black list
      m_exam->blacList()->append(*m_exam->curQ());
      m_exam->blacList()->last().time = m_exam->curQ()->isNotSoBad() ? 65501 : 65502;
    }
  }
  if (!m_exam->melodies()) // we don't know is melody question answered here - user will decide...
    m_exam->curQ()->setAnswered();
  m_exam->sumarizeAnswer();
  if (!m_exam->melodies()) // when melody question counters are not ready here, setMelodyPenalties() will do it.
    RESULTS->questionCountUpdate();
  RESULTS->reactTimesUpdate();
  RESULTS->effectUpdate();
  if (!m_exam->isExercise()) {
    releaseBlackList();
    RESULTS->progress();
    if (!m_exam->curQ()->isCorrect())
      updatePenalStep();
    checkForCert();
  }
}


void Tpenalty::newAttempt() {
  m_exam->newAttempt();
  RESULTS->effectUpdate();
}


void Tpenalty::setMelodyPenalties() {
  if (m_exam->count() == 0)
    return;

  if (m_exam->curQ()->answered())
    return; // It happens when continued exam starts - last question has been answered already

  m_exam->curQ()->setAnswered();
  if (m_exam->melodies()) {
    if (!m_exam->curQ()->isCorrect() && !m_exam->isFinished()) {
      m_exam->addPenalties();
      if (m_exam->level()->isMelodySet() && !m_exam->level()->randOrderInSet)
        m_penalStep = -1;
      else
        updatePenalStep();
    }
    if (!m_exam->isExercise()) {
      RESULTS->progress();
      checkForCert();
    }
  }
  if (!m_exam->isExercise()) {
    RESULTS->questionCountUpdate(); // counters are not used/visible for exercises
    RESULTS->effectUpdate();
  }
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
        qDebug() << "[Tpenalty] penalties increased. Can't finish this exam yet.";
    } else {
        m_exam->setFinished();
        emit certificate();
        m_supply->setFinished();
    }
  }
}


void Tpenalty::updatePenalStep() {
  if (m_supply->wasFinished())
    return;

  if ((m_exam->melodies() && m_exam->blackNumbers()->isEmpty()) || (!m_exam->melodies() && m_exam->blacList()->isEmpty()))
      m_penalStep = 65535;
  else {
      if ((m_supply->obligQuestions() + m_exam->penalty() - m_exam->count()) > 0)
        m_penalStep = (m_supply->obligQuestions() + m_exam->penalty() - m_exam->count())
                      / (m_exam->melodies() ? m_exam->blackNumbers()->size() : m_exam->blackCount());
      else
        m_penalStep = 0; // only penalties questions remained to ask in this exam
  }
}

//######################################################################
//#################   FORWARDED METHODS OF TexamView       #############
//######################################################################
void Tpenalty::pauseTime() {
  RESULTS->pause();
}


void Tpenalty::continueTime() {
  RESULTS->go();
}


void Tpenalty::updateExamTimes() {
  RESULTS->updateExam();
}


void Tpenalty::stopTimeView() {
  RESULTS->stopExam();
}


void Tpenalty::startQuestionTime() {
  RESULTS->questionStart();
}


void Tpenalty::stopQuestionTime() {
  RESULTS->questionStop();
}


quint32 Tpenalty::elapsedTime() {
  return RESULTS->questionTime();
}






