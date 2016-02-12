/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
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


#include "tprogresswidget.h"
#include <tinitcorelib.h>
#include <tcolor.h>
#include <exam/texam.h>
#include <QtWidgets/QtWidgets>



TprogressWidget::TprogressWidget(QWidget* parent) :
  QWidget(parent),
  m_totalNr(0)
{
  QHBoxLayout *lay = new QHBoxLayout;
#if defined (Q_OS_ANDROID)
    lay->setContentsMargins(0, 0, 0, 0);
#endif
  m_answLab = new QLabel(zeroLabTxt(), this);
  m_answLab->setStyleSheet( "border: 1px solid palette(Text); border-radius: 4px;" + Tcolor::bgTag(Tcore::gl()->EnotBadColor));
  lay->addWidget(m_answLab);
  m_bar = new QProgressBar(this);
  m_bar->setValue(0);
  lay->addWidget(m_bar);
  m_totalLab = new QLabel(zeroLabTxt(), this);
  m_totalLab->setStyleSheet(QLatin1String("border: 1px solid palette(Text); border-radius: 4px;") + Tcolor::bgTag(Tcore::gl()->EanswerColor));
  lay->addWidget(m_totalLab);
  setLayout(lay);
  setStatusTip(progressExamTxt());
}


void TprogressWidget::activate(Texam* exam, int totalNr) {
	m_exam = exam;
	setFinished();
	m_totalNr = totalNr;
	updateLabels();
}


void TprogressWidget::progress() {
  updateLabels();
}


void TprogressWidget::setFinished() {
   if (m_exam->isFinished())
     m_bar->setStatusTip(examFinishedTxt());
}


void TprogressWidget::terminate() {
  m_answLab->setText(zeroLabTxt());
  m_answLab->setStatusTip(QString());
  m_totalLab->setText(zeroLabTxt());
  m_totalLab->setStatusTip(QString());
  m_bar->reset();
  m_bar->setValue(0);
  m_bar->setStatusTip(progressExamTxt());
}

void TprogressWidget::resize(int fontSize) {
    QFont f = font();
    f.setPointSize(fontSize);
    m_answLab->setFont(f);
    m_totalLab->setFont(f);
}

//#############################################################################
// PROTECTED
//#############################################################################

void TprogressWidget::updateLabels() {
  int remained = qMax(0, m_totalNr + m_exam->penalty() - m_exam->count());
	int alreadyAnswered = m_exam->count();
	if (m_exam->melodies() && m_exam->count() && !m_exam->curQ()->answered())
		alreadyAnswered = qMax(0, alreadyAnswered - 1);
  m_answLab->setText(QString("%1 + %2").arg(alreadyAnswered).arg(remained));
  m_answLab->setStatusTip(tr("Answered questions") + QString(": %1").arg(alreadyAnswered) +
        "<br>" + tr("Unanswered questions", "could be also: 'rest of the questions' or 'remaining questions'") + QString(": %1 ").arg(remained)  );
  m_totalLab->setText(QString(" %1 (%2)").arg(m_totalNr + m_exam->penalty()).arg(m_exam->penalty()));
  m_totalLab->setStatusTip(tr("Total questions in this exam") + QString(": %1 ").arg(m_totalNr + m_exam->penalty()) +
    "<br>(" + tr("penalties") + QString(": %1)").arg(m_exam->penalty()));
  m_bar->setMinimum(0);
  m_bar->setMaximum(m_totalNr + m_exam->penalty());
  if (remained) {
    m_bar->setValue(m_exam->count());
    m_bar->setStatusTip(progressExamTxt() + QLatin1String("<br>") + m_bar->text());
  } else {
    m_bar->setValue(m_totalNr + m_exam->penalty());
    if (m_exam->isFinished()) 
      m_bar->setStatusTip(examFinishedTxt());
  }
}


QString TprogressWidget::zeroLabTxt() {
  return QStringLiteral("<span style=\"color: transparent;\">0 + </span>0<span style=\"color: transparent;\">0</span>");
}



