/***************************************************************************
 *   Copyright (C) 2013-2021 by Tomasz Bojczuk                             *
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

#include "texercises.h"
#include <tglobals.h>
#include <exam/texam.h>

#include <QtCore/qdebug.h>


Texercises::Texercises(Texam* exam, QObject* parent) :
  QObject(parent),
  m_exam(exam)
{
  m_exam->setExercise();
}


void Texercises::setSuggestionEnabled(int qaPosibilities, bool melody) {
  if (qaPosibilities > 0) {
      m_max = qMax(qaPosibilities, melody ? 5 : 10); // not less than 10 or 5 if melodies
      m_checkNow = true;
      m_checkInFuture = true; // global setting is true when it comes here
      m_currentGood = 0;
      m_prevMistake = 0;
  } else {
      m_checkNow = false;
      m_checkInFuture = false;
  }
}


bool Texercises::checkAnswer() {
  if (!m_checkNow || !m_exam->curQ()->answered())
    return false;

  if (m_exam->curQ()->isCorrect()) {
      m_currentGood++;
      m_prevMistake++;
  } else {
      if (m_currentGood > m_max / 2 && m_prevMistake > m_max / 2) // when there were enough good answers
        m_currentGood = m_max / 2; // forgive single mistake
      else
        m_currentGood = 0;
      m_prevMistake = 0; // but reset mistake counter, so after next wrong answer whole cycle will be required
  }

  if (m_currentGood >= m_max) {// call for suggestion dialog when amount of proper answers is sufficient.
    emit wantMessage(GLOB->waitForCorrect() && m_exam->melodies());
    m_wantMessageEmitted = true;
    return true;
  }
  return false;
}


bool Texercises::wantsUserExam(int whatInt) {
  if (!m_wantMessageEmitted) { // TODO It should never happened - clear it out if so.
    qDebug() << "[Texercises] FIXME! - wantsUserExam() invoked without suggestion dialog!";
    return false;
  }
  m_wantMessageEmitted = false;
  if (whatInt < -1 || whatInt > e_neverEver) {
      qDebug() << "[Texercises] User wants impossible things!" << whatInt;
      m_currentGood = 0;
  } else {
      auto what = static_cast<Esuggest>(whatInt);
      switch(what) {
        case e_readyToExam:
          m_readyToExam = true;
          break;
        case e_forAmoment:
          m_currentGood = 0;
          break;
        case e_checkEntireMelody:
          GLOB->setWaitForCorrect(false);
          m_currentGood = 0;
          break;
        case e_notThisTime:
          m_checkNow = false;
          break;
        case e_neverEver:
          m_checkInFuture = false;
          m_checkNow = false;
          break;
        default:
          m_currentGood = 0;
          break;
      }
  }
  return m_readyToExam;
}








