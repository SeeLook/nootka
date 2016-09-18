/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#include "texamview.h"
#include <exam/tqaunit.h>
#include <exam/texam.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>
#include <QtWidgets>

#define SPACE_GAP (7)


QString borderStyleTxt = "border: 1px solid palette(shadow); border-radius: 4px;";


TexamView::TexamView(QWidget *parent) :
    QWidget(parent),
    m_exam(0)
{
  setStatusTip(tr("Exam results"));
  
  QHBoxLayout *mainLay = new QHBoxLayout;

  mainLay->addStretch();
  QHBoxLayout *okMistLay = new QHBoxLayout;
  m_corrLab = new QLabel(this);
  okMistLay->addWidget(m_corrLab, 0, Qt::AlignRight);
  okMistLay->addSpacing(SPACE_GAP);
  m_halfLab = new QLabel(this);
  okMistLay->addWidget(m_halfLab, 0, Qt::AlignRight);
  m_halfLab->hide();
  okMistLay->addSpacing(SPACE_GAP);
  m_mistLab = new QLabel(this);
  okMistLay->addWidget(m_mistLab, 0, Qt::AlignRight);
  mainLay->addLayout(okMistLay);    
  mainLay->addStretch();

  m_effLab = new QLabel(this);
  m_effLab->setStyleSheet(borderStyleTxt);
  mainLay->addWidget(m_effLab);
  mainLay->addStretch();

  m_averTimeLab = new QLabel(this);
  m_averTimeLab->setStyleSheet(borderStyleTxt);
  mainLay->addWidget(m_averTimeLab);
  mainLay->addStretch();

  m_reactTimeLab = new QLabel(this);
  m_reactTimeLab->setStyleSheet(borderStyleTxt);
  mainLay->addWidget(m_reactTimeLab);

  mainLay->addStretch();

  m_totalTimeLab = new QLabel(this);
  m_totalTimeLab->setStyleSheet(borderStyleTxt);
  mainLay->addWidget(m_totalTimeLab);
  mainLay->addStretch();

  setLayout(mainLay);

  clearResults();

  m_corrLab->setAlignment(Qt::AlignCenter);
  m_corrLab->setStatusTip(TexTrans::corrAnswersNrTxt());
  m_halfLab->setAlignment(Qt::AlignCenter);
  m_mistLab->setStatusTip(TexTrans::mistakesNrTxt());
  m_mistLab->setAlignment(Qt::AlignCenter);
  m_averTimeLab->setStatusTip(TexTrans::averAnsverTimeTxt() + " " + TexTrans::inSecondsTxt());
  m_averTimeLab->setAlignment(Qt::AlignCenter);
  m_reactTimeLab->setStatusTip(TexTrans::reactTimeTxt() + " " + TexTrans::inSecondsTxt());
  m_reactTimeLab->setAlignment(Qt::AlignCenter);
  m_totalTimeLab->setStatusTip(TexTrans::totalTimetxt());
  m_totalTimeLab->setAlignment(Qt::AlignCenter);

  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(countTime()));
}


void TexamView::updateExam() {
	if (m_exam) {
		m_exam->setTotalTime(totalTime());
	}
}


void TexamView::setStyleBg(const QString& okBg, const QString& wrongBg, const QString& notBadBg) {
    m_corrLab->setStyleSheet(okBg + borderStyleTxt);
    m_mistLab->setStyleSheet(wrongBg + borderStyleTxt);
    m_halfLab->setStyleSheet(notBadBg + borderStyleTxt);
}


void TexamView::questionStart() {
	m_questionTime.start();
	m_showReact = true;
	countTime();
}


quint16 TexamView::questionTime() {
	return qRound(m_questionTime.elapsed() / 100.0);
}


void TexamView::questionStop() {
	m_showReact = false;
	quint16 t = qRound(m_questionTime.elapsed() / 100.0);
	if (m_exam->melodies()) {
		m_exam->curQ()->time += t; // total time of all attempts
		m_exam->curQ()->lastAttempt()->setTotalTime(t);
	} else
		m_exam->curQ()->time = t; // just elapsed time of single answer
	if (isVisible())
		m_reactTimeLab->setText(" " + Texam::formatReactTime(m_exam->curQ()->time) + " ");
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


void TexamView::startExam(Texam* exam) {
	m_exam = exam;
	m_totalTime = QTime(0, 0);
	m_startExamTime = m_exam->totalTime();
	m_showReact = false;
	m_totalTime.start();
	countTime();
	answered();
	m_averTimeLab->setText(" " + Texam::formatReactTime(m_exam->averageReactonTime()) + " ");
	if (m_exam->melodies()) {
		m_effLab->setStatusTip(tr("Effectiveness of whole exam (and effectiveness of current question)."));
		m_halfLab->setStatusTip(TexTrans::halfMistakenTxt());
	} else {
		m_effLab->setStatusTip(TexTrans::effectTxt());
		m_halfLab->setStatusTip(TexTrans::halfMistakenTxt() + "<br>" + TexTrans::halfMistakenAddTxt());
	}
}


void TexamView::answered() {
	questionCountUpdate();
	effectUpdate();
	reactTimesUpdate();
}


void TexamView::setFontSize(int s) {
  QFont f = m_reactTimeLab->font();
  f.setPointSize(s);
  m_reactTimeLab->setFont(f);
  m_averTimeLab->setFont(f);
  m_totalTimeLab->setFont(f);
  m_mistLab->setFont(f);
  m_corrLab->setFont(f);
  m_halfLab->setFont(f);
  m_effLab->setFont(f);
  m_sizeHint.setWidth(m_effLab->fontMetrics().width("0") * 35 + layout()->spacing() * 8);
  m_sizeHint.setHeight(m_effLab->fontMetrics().height() + m_effLab->contentsMargins().top() * 2);
}


//######################################################################
//#######################     PUBLIC SLOTS   ###########################
//######################################################################
void TexamView::reactTimesUpdate() {
	if (m_exam && isVisible())
		m_averTimeLab->setText(" " + Texam::formatReactTime(m_exam->averageReactonTime()) + " ");
}


void TexamView::effectUpdate() {
	if (m_exam && isVisible()) {
		QString effText = QString("<b>%1 %</b>").arg(qRound(m_exam->effectiveness()));
		if (m_exam->count() && m_exam->melodies() && !m_exam->curQ()->answered() && 
				m_exam->curQ()->attemptsCount() && m_exam->curQ()->lastAttempt()->mistakes.size())
			effText += QString(" <small>(%1 %)</small>").arg(qRound(m_exam->curQ()->effectiveness()));
			// It will display current melody question effectiveness only if it makes a sense
		m_effLab->setText(effText);
	}
}


void TexamView::questionCountUpdate() {
	if (m_exam && isVisible()) {
		m_mistLab->setText(QString("%1").arg(m_exam->mistakes()));
		if (m_exam->halfMistaken()) {
				m_halfLab->show();
				m_halfLab->setText(QString("%1").arg(m_exam->halfMistaken()));
		}
		m_corrLab->setText(QString("%1").arg(m_exam->corrects()));
	}
}



void TexamView::countTime() {
	if (isVisible()) {
		if (m_showReact)
				m_reactTimeLab->setText(QString(" %1 ").arg(Texam::formatReactTime(m_questionTime.elapsed() / 100 + m_exam->curQ()->time)));
		m_totalTimeLab->setText(" " + formatedTotalTime(m_startExamTime * 1000 + m_totalTime.elapsed()) + " ");
	}
}


void TexamView::clearResults() {
    m_corrLab->setText("0");
    m_mistLab->setText("0");
    m_halfLab->setText("0");
    m_effLab->setText("<b>100%</b>");
    m_averTimeLab->setText(" 0.0 ");
    m_reactTimeLab->setText(" 0.0 ");
    m_totalTimeLab->setText(" 0:00:00 ");
}

