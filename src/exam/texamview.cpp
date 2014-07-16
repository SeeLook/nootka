/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include <QtWidgets>

#define SPACE_GAP (7)


QString borderStyleTxt = "border: 1px solid palette(Text); border-radius: 4px;";


TexamView::TexamView(QWidget *parent) :
    QWidget(parent)
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

    m_corrLab->setStatusTip(TexTrans::corrAnswersNrTxt());
    m_halfLab->setStatusTip(TexTrans::halfMistakenTxt() + "<br>" + TexTrans::halfMistakenAddTxt());
    m_mistLab->setStatusTip(TexTrans::mistakesNrTxt());
    m_effLab->setStatusTip(TexTrans::effectTxt());
    m_averTimeLab->setStatusTip(TexTrans::averAnsverTimeTxt() + " " + TexTrans::inSecondsTxt());
    m_averTimeLab->setAlignment(Qt::AlignCenter);
    m_reactTimeLab->setStatusTip(TexTrans::reactTimeTxt() + " " + TexTrans::inSecondsTxt());
    m_reactTimeLab->setAlignment(Qt::AlignCenter);
    m_totalTimeLab->setStatusTip(TexTrans::totalTimetxt());
    m_totalTimeLab->setAlignment(Qt::AlignCenter);

    m_timer = new QTimer(this);
		connect(m_timer, SIGNAL(timeout()), this, SLOT(countTime()));
}

void TexamView::setStyleBg(QString okBg, QString wrongBg, QString notBadBg) {
    m_corrLab->setStyleSheet(okBg + borderStyleTxt);
    m_mistLab->setStyleSheet(wrongBg + borderStyleTxt);
    m_halfLab->setStyleSheet(notBadBg + borderStyleTxt);
}

void TexamView::questionStart() {
    m_reactTime.start();
    m_showReact = true;
    m_questNr++;
    countTime();
}

quint16 TexamView::questionTime() {
     return qRound(m_reactTime.elapsed() / 100.0);
}


quint16 TexamView::questionStop() {
    m_showReact = false;
    quint16 t = qRound(m_reactTime.elapsed() / 100.0);
    m_reactTimeLab->setText(" " + Texam::formatReactTime(t) + " ");
    return t;
}


void TexamView::pause() {
		m_pausedAt = m_reactTime.elapsed();
}


void TexamView::go() {
		m_reactTime.start();
		m_reactTime = m_reactTime.addMSecs(-m_pausedAt);		
}



void TexamView::startExam(int passTimeInSec, int questNumber, int averTime, int mistakes, int halfMist) {
    m_questNr = questNumber;
    m_totElapsedTime = passTimeInSec;
    m_totalTime = QTime(0, 0);
    m_averTime = averTime;
    m_mistakes = mistakes;
    m_halfMistakes = halfMist;
    m_okCount = m_questNr - m_mistakes;
    m_showReact = false;
    m_totalTime.start();
    m_totalTime.restart();
    countTime();
    setAnswer();
    m_averTimeLab->setText(" " + Texam::formatReactTime(qRound(m_averTime)) + " ");
}


void TexamView::setAnswer(TQAunit* answer) {
    if (answer) {
      if (!answer->isCorrect()) {
        if (answer->isWrong())
          m_mistakes++;
        else
          m_halfMistakes++;
      }
      if (!answer->isWrong()) {
					m_okCount++;
					m_averTime = (m_averTime * (m_okCount -1) + answer->time) / m_okCount;
			}
    }
    m_mistLab->setText(QString("%1").arg(m_mistakes));
    if (m_halfMistakes) {
				m_halfLab->show();
				m_halfLab->setText(QString("%1").arg(m_halfMistakes));
    }
    m_corrLab->setText(QString("%1").arg(m_questNr - m_mistakes - m_halfMistakes));
    m_effect = Texam::effectiveness(m_questNr, m_mistakes, m_halfMistakes);
		m_effLab->setText(QString("<b>%1 %</b>").arg(qRound(m_effect)));
		m_averTimeLab->setText(" " + Texam::formatReactTime(qRound(m_averTime)) + " ");
}


void TexamView::resizeEvent(QResizeEvent* ) {
  
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
//    m_reactTimeLab->setFixedWidth(s * 3);
//    m_averTimeLab->setFixedWidth(s * 3);
//     m_totalTimeLab->setFixedWidth(s * 5);
//     setMinimumHeight(okGr->height() + 5);

}

void TexamView::countTime() {
    if (m_showReact)
        m_reactTimeLab->setText(QString(" %1 ").arg(Texam::formatReactTime(m_reactTime.elapsed() / 100)));
    m_totalTimeLab->setText(" " + formatedTotalTime(m_totElapsedTime * 1000 + m_totalTime.elapsed()) + " ");
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

