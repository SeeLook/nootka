/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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
  return m_exam && m_exam->count() ? QString("%1").arg(Texam::formatReactTime(m_questionTime.elapsed() / 100 + m_exam->curQ()->time)) : QLatin1String(" 0.0 ");
}
QString TexamView::effectiveness() const { return m_effectivenessText; }
QString TexamView::averText() const {
  return m_exam && m_exam->count() ? Texam::formatReactTime(m_exam->averageReactonTime()) : QLatin1String(" 0.0 ");
}
QString TexamView::totalTimeText() const {
  return m_exam && m_exam->count() ? formatedTotalTime(m_startExamTime * 1000 + m_totalTime.elapsed()) : QStringLiteral(" 0:00:00 ");
}


void TexamView::updateExam() {
  if (m_exam) {
    m_exam->setTotalTime(totalTime());
  }
}


void TexamView::questionStart() {
  m_questionTime.start();
  m_showReact = true;
  countTime();
}


quint16 TexamView::questionTime() {
  return quint16(qRound(m_questionTime.elapsed() / 100.0));
}


void TexamView::questionStop() {
  m_showReact = false;
  quint16 t = quint16(qRound(m_questionTime.elapsed() / 100.0));
  if (m_exam->melodies()) {
    m_exam->curQ()->time += t; // total time of all attempts
    m_exam->curQ()->lastAttempt()->setTotalTime(t);
  } else
    m_exam->curQ()->time = t; // just elapsed time of single answer
  if (isVisible()) {
    emit reactTextChanged();
//     m_reactTimeLab->setText(space + Texam::formatReactTime(m_exam->curQ()->time) + space);
  }
}


void TexamView::pause() {
  m_pausedAt = m_questionTime.elapsed();
  doNotdisplayTime();
}


void TexamView::go() {
  m_questionTime.start();
  m_questionTime = m_questionTime.addMSecs(-m_pausedAt);
  displayTime();
}


void TexamView::startExam(Texam* exam, int totalNr) {
  m_exam = exam;
  m_totalTime = QTime(0, 0);
  m_startExamTime = int(m_exam->totalTime());
  m_showReact = false;
  m_totalTime.start();
  countTime();
  answered();
  emit averTextChanged();

  m_totalNr = totalNr;
  updateLabels();
//   m_averTimeLab->setText(space + Texam::formatReactTime(m_exam->averageReactonTime()) + space);
//   if (m_exam->melodies()) {
//     m_effLab->setStatusTip(tr("Effectiveness of whole exam (and effectiveness of current question)."));
//     m_halfLab->setStatusTip(TexTrans::halfMistakenTxt());
//   } else {
//     m_effLab->setStatusTip(TexTrans::effectTxt());
//     m_halfLab->setStatusTip(TexTrans::halfMistakenTxt() + QLatin1String("<br>") + TexTrans::halfMistakenAddTxt());
//   }
}


void TexamView::answered() {
  questionCountUpdate();
  effectUpdate();
  reactTimesUpdate();
}


void TexamView::reactTimesUpdate() {
  if (m_exam && isVisible()) {
    emit averTextChanged();
//     m_averTimeLab->setText(space + Texam::formatReactTime(m_exam->averageReactonTime()) + space);
  }
}


void TexamView::effectUpdate() {
  if (m_exam && isVisible()) {
    m_effectivenessText = QString("<b>%1 %</b>").arg(qRound(m_exam->effectiveness()));
    if (m_exam->count() && m_exam->melodies() && !m_exam->curQ()->answered() && 
        m_exam->curQ()->attemptsCount() && m_exam->curQ()->lastAttempt()->mistakes.size())
          m_effectivenessText += QString(" <font size=\"2\">(%1 %)</font>").arg(qRound(m_exam->curQ()->effectiveness()));
      // It will display current melody question effectiveness only if it makes a sense
//     m_effLab->setText(effText);
    emit effectivenessChanged();
  }
}


void TexamView::questionCountUpdate() {
  if (m_exam && isVisible()) {
//     m_mistLab->setText(QString("%1").arg(m_exam->mistakes()));
//     if (m_exam->halfMistaken()) {
//         m_halfLab->show();
//         m_halfLab->setText(QString("%1").arg(m_exam->halfMistaken()));
//     }
//     m_corrLab->setText(QString("%1").arg(m_exam->corrects()));
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
    if (m_showReact) {
//         m_reactTimeLab->setText(QString(" %1 ").arg(Texam::formatReactTime(m_questionTime.elapsed() / 100 + m_exam->curQ()->time)));
//       m_reactionTimeText = QString(" %1 ").arg(Texam::formatReactTime(m_questionTime.elapsed() / 100 + m_exam->curQ()->time));
      emit reactTextChanged();
    }
    emit totalTimeTextChanged();
//     m_totalTimeLab->setText(space + formatedTotalTime(m_startExamTime * 1000 + m_totalTime.elapsed()) + space);
  }
}


void TexamView::updateLabels() {
  int remained = qMax(0, m_totalNr + m_exam->penalty() - m_exam->count());
  int alreadyAnswered = m_exam->count();
  if (m_exam->melodies() && m_exam->count() && !m_exam->curQ()->answered())
    alreadyAnswered = qMax(0, alreadyAnswered - 1);
  m_answersText = QString("%1 + %2").arg(alreadyAnswered).arg(remained);
//   m_answLab->setStatusTip(tr("Answered questions") + QString(": %1").arg(alreadyAnswered) +
//         "<br>" + tr("Unanswered questions", "could be also: 'rest of the questions' or 'remaining questions'") + QString(": %1 ").arg(remained)  );
  m_totalText = QString(" %1 (%2)").arg(m_totalNr + m_exam->penalty()).arg(m_exam->penalty());
//   m_totalLab->setStatusTip(tr("Total questions in this exam") + QString(": %1 ").arg(m_totalNr + m_exam->penalty()) +
//     "<br>(" + tr("penalties") + QString(": %1)").arg(m_exam->penalty()));
  m_progressMax = m_totalNr + m_exam->penalty();
  if (remained) {
      m_progressValue = m_exam->count();
//     m_bar->setStatusTip(progressExamTxt() + QLatin1String("<br>") + m_bar->text());
  } else {
      m_progressValue = m_totalNr + m_exam->penalty();
//     if (m_exam->isFinished()) 
//       m_bar->setStatusTip(examFinishedTxt());
  }
  emit valuesUpdated();
}


