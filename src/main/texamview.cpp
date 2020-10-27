/***************************************************************************
 *   Copyright (C) 2011-2020 by Tomasz Bojczuk                             *
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


#include "texamview.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>


TexamView* TexamView::m_instance = nullptr;

TexamView::TexamView(QQuickItem *parent) :
  QQuickItem(parent),
  m_exam(nullptr)
{
  if (m_instance) {
    qDebug() << "TexamView instance already exists";
    return;
  }

  m_instance = this;

  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &TexamView::countTime);
}


TexamView::~TexamView()
{
  m_instance = nullptr;
}


int TexamView::correctAnswers() const { return m_exam ? m_exam->corrects() : 0; }
int TexamView::halfAnswers() const { return m_exam ? m_exam->halfMistaken() : 0; }
int TexamView::wrongAnswers() const { return m_exam ? m_exam->mistakes() : 0; }

QString TexamView::reactText() const {
  if (!m_exam || m_exam->count() <1)
    return QLatin1String(" 0.0 ");
  return QString("%1").arg(Texam::formatReactTime((m_showReact ? m_questionTime.elapsed() / 100 : 0) + m_exam->curQ()->time));
}
QString TexamView::effectiveness() const { return m_effectivenessText; }
QString TexamView::averText() const {
  return m_exam && m_exam->count() ? Texam::formatReactTime(m_exam->averageReactonTime()) : QLatin1String(" 0.0 ");
}
QString TexamView::totalTimeText() const {
  return m_exam && m_exam->count() ? formatedTotalTime(m_startExamTime * 1000 + m_totalTime.elapsed()) : QStringLiteral(" 0:00:00 ");
}


bool TexamView::isExercise() const {
  return m_exam ? m_exam->isExercise() : false;
}


void TexamView::updateExam() {
  if (m_exam) {
    m_exam->setTotalTime(totalTime());
  }
}


void TexamView::questionStart() {
  m_questionTime.start();
  m_answerDuration = 0;
  m_showReact = true;
  countTime();
}


quint16 TexamView::questionTime() {
  return quint16(qRound(static_cast<qreal>(m_questionTime.elapsed()) / 100.0));
}


void TexamView::questionStop() {
  m_showReact = false;
  m_answerDuration += m_questionTime.elapsed();
  auto t = static_cast<quint16>(qRound(static_cast<qreal>(m_answerDuration) / 100.0));
  if (m_exam->melodies()) {
      m_exam->curQ()->time += t; // total time of all attempts
      m_exam->curQ()->lastAttempt()->setTotalTime(t);
  } else
      m_exam->curQ()->time = t; // just elapsed time of single answer
  if (isVisible())
    emit reactTextChanged();
}


void TexamView::pause() {
  m_answerDuration += m_questionTime.elapsed();
  doNotdisplayTime();
}


void TexamView::go() {
  m_questionTime.start();
  displayTime();
}


void TexamView::startExam(Texam* exam, int totalNr) {
  m_exam = exam;
  m_startExamTime = static_cast<int>(m_exam->totalTime());
  m_showReact = false;
  m_totalTime.start();
  countTime();
  answered();
  if (isVisible())
    emit averTextChanged();

  m_totalNr = totalNr;
  updateLabels();
  emit isExerciseChanged();
}


void TexamView::answered() {
  questionCountUpdate();
  effectUpdate();
  reactTimesUpdate();
}


void TexamView::reactTimesUpdate() {
  if (m_exam && isVisible()) {
    emit averTextChanged();
  }
}


void TexamView::effectUpdate() {
  if (m_exam && isVisible()) {
    m_effectivenessText = QString("<b>%1 %</b>").arg(qRound(m_exam->effectiveness()));
    if (m_exam->count() && m_exam->melodies() && !m_exam->curQ()->answered() && 
        m_exam->curQ()->attemptsCount() && m_exam->curQ()->lastAttempt()->mistakes.size())
          m_effectivenessText += QString(" <font size=\"2\">(%1 %)</font>").arg(qRound(m_exam->curQ()->effectiveness()));
    emit effectivenessChanged();
  }
}


void TexamView::questionCountUpdate() {
  if (m_exam && isVisible()) {
    emit answersNumberChanged();
  }
}


void TexamView::terminate() {
  m_progressValue = 0;
  m_progressMax = 0;
  m_answersText = QStringLiteral("0");
  m_totalText = QStringLiteral("0");
  emit valuesUpdated();
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TexamView::countTime() {
  if (isVisible()) {
    if (m_showReact)
      emit reactTextChanged();
    emit totalTimeTextChanged();
  }
}


void TexamView::updateLabels() {
  int remained = qMax(0, m_totalNr + m_exam->penalty() - m_exam->count());
  int alreadyAnswered = m_exam->count();
  if (m_exam->melodies() && m_exam->count() && !m_exam->curQ()->answered())
    alreadyAnswered = qMax(0, alreadyAnswered - 1);
  m_answersText = QString("%1 + %2").arg(alreadyAnswered).arg(remained);
  m_totalText = QString(" %1 (%2)").arg(m_totalNr + m_exam->penalty()).arg(m_exam->penalty());
  m_progressMax = m_totalNr + m_exam->penalty();
  if (remained)
    m_progressValue = m_exam->count();
  else
    m_progressValue = m_totalNr + m_exam->penalty();
  if (isVisible())
    emit valuesUpdated();
}


QString TexamView::answersHint() const {
  if (m_exam) {
      int alreadyAnswered = m_exam->count();
      if (m_exam->melodies() && m_exam->count() && !m_exam->curQ()->answered())
        alreadyAnswered = qMax(0, alreadyAnswered - 1);
      return tr("Answered questions") + QString(": <b>%1</b>, ").arg(alreadyAnswered)
              + tr("Unanswered questions", "could be also: 'rest of the questions' or 'remaining questions'")
              + QString(": <b>%1</b> ").arg(qMax(0, m_totalNr + m_exam->penalty() - m_exam->count()));
  } else
      return QString();
}


QString TexamView::summaryHint() const {
  if (m_exam) {
      return tr("Total questions in this exam") + QString(": <b>%1</b> ").arg(m_totalNr + m_exam->penalty())
              + QLatin1String(" (") + tr("penalties") + QString(": <b>%1</b>)").arg(m_exam->penalty());
  } else
      return QString();
}


QString TexamView::progressHint() const {
  if (m_exam) {
      if (m_exam->isFinished())
        return examFinishedTxt();
      else
        return progressExamTxt() + QString(": <b>%1 %</b>").arg((m_progressValue * 100) / m_progressMax);
  } else
      return QString();
}


QString TexamView::correctHint() const {
  return TexTrans::corrAnswersNrTxt() + QString(": <b>%1</b>").arg(correctAnswers());
}


QString TexamView::halfHint() const {
  return TexTrans::halfMistakenTxt() + QString(": <b>%1</b>").arg(halfAnswers());
}


QString TexamView::wrongHint() const {
  return TexTrans::mistakesNrTxt() + QString(": <b>%1</b>").arg(wrongAnswers());
}


QString TexamView::effectHint() const {
  QString eff = effectiveness();
  return (eff.contains(QLatin1String("(")) ? tr("Exam effectiveness (this question effectiveness)") : TexTrans::effectTxt())
      + QLatin1String(": ") + effectiveness();
}


QString TexamView::averageHint() const {
  return TexTrans::averAnsverTimeTxt() + QString(": <b>%1</b>").arg(averText());
}


QString TexamView::answerTimeTxt() const {
  return tr("Answer time");
}


QString TexamView::examTimeTxt() const {
  return tr("Exam time");
}


QString TexamView::resultsTxt() const {
  return tr("Exam results");
}
