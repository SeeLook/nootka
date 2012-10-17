/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
#include "tqaunit.h"
#include "texam.h"
#include <QtGui>

#define SPACE_GAP (7)


const QString TexamView::halfMistakenTxt() { return tr("'Not So Bad' answers"); } 
const QString TexamView::halfMistakenAddTxt() { return tr("(counted as half of a mistake)"); }

QString borderStyleTxt = "border: 1px solid palette(Text); border-radius: 4px;";

//QGroupBox *okGr;

TexamView::TexamView(QWidget *parent) :
    QWidget(parent)
{
//     setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    setStatusTip(tr("Exam results"));
    
    QHBoxLayout *mainLay = new QHBoxLayout;

    mainLay->addStretch();
    QHBoxLayout *okMistLay = new QHBoxLayout;
    m_corrLab = new QLabel(this);
    okMistLay->addWidget(m_corrLab, 0, Qt::AlignRight);
    okMistLay->addSpacing(SPACE_GAP);
    m_halfLab = new QLabel(this);
    okMistLay->addWidget(m_halfLab, 0, Qt::AlignRight);
    okMistLay->addSpacing(SPACE_GAP);
    m_mistLab = new QLabel(this);
    okMistLay->addWidget(m_mistLab, 0, Qt::AlignRight);
//     okMistLay->addStretch(1);
//    okGr = new QGroupBox(this);
//    okGr->setLayout(okMistLay);
//    mainLay->addWidget(okGr);
    mainLay->addLayout(okMistLay);    
    mainLay->addStretch();

    m_effLab = new QLabel(this);
    m_effLab->setStyleSheet(borderStyleTxt);
//     QVBoxLayout *effLay = new QVBoxLayout;
//     effLay->addWidget(m_effLab, 0, Qt::AlignCenter);
//    QGroupBox *effGr = new QGroupBox(this);
//    effGr->setLayout(effLay);
     mainLay->addWidget(m_effLab);
//    mainLay->addWidget(effGr);
    mainLay->addStretch();

    m_averTimeLab = new QLabel(this);
    m_averTimeLab->setStyleSheet(borderStyleTxt);
//     QVBoxLayout *averLay = new QVBoxLayout;
//     averLay->addWidget(m_averTimeLab);
//    QGroupBox *averGr = new QGroupBox(this);
//    averGr->setLayout(averLay);
     mainLay->addWidget(m_averTimeLab);
//    mainLay->addWidget(averGr);
    mainLay->addStretch();

    m_reactTimeLab = new QLabel(this);
    m_reactTimeLab->setStyleSheet(borderStyleTxt);
//     QVBoxLayout *reactLay = new QVBoxLayout;
//     reactLay->addWidget(m_reactTimeLab);
//    QGroupBox *reactGr = new QGroupBox(this);
//    reactGr->setLayout(reactLay);
     mainLay->addWidget(m_reactTimeLab);
//    mainLay->addWidget(reactGr);
    mainLay->addStretch();

    m_totalTimeLab = new QLabel(this);
    m_totalTimeLab->setStyleSheet(borderStyleTxt);
//     QVBoxLayout *totalLay = new QVBoxLayout;
//     totalLay->addWidget(m_totalTimeLab);
//    QGroupBox *totalGr = new QGroupBox(this);
//    totalGr->setLayout(totalLay);
     mainLay->addWidget(m_totalTimeLab);
//    mainLay->addWidget(totalGr);
    mainLay->addStretch();

    setLayout(mainLay);

    clearResults();

    m_corrLab->setStatusTip(corrAnswersNrTxt());
    m_halfLab->setStatusTip(halfMistakenTxt() + "<br>" + halfMistakenAddTxt());
    m_mistLab->setStatusTip(mistakesNrTxt());
    m_effLab->setStatusTip(effectTxt());
    //    effGr->setStatusTip(effectTxt());
    m_averTimeLab->setStatusTip(averAnsverTimeTxt() + " " + inSecondsTxt());
    //    averGr->setStatusTip(averAnsverTimeTxt() + " " + inSecondsTxt());
    m_averTimeLab->setAlignment(Qt::AlignCenter);
    m_reactTimeLab->setStatusTip(reactTimeTxt() + " " + inSecondsTxt());
    //    reactGr->setStatusTip(reactTimeTxt() + " " + inSecondsTxt());
    m_reactTimeLab->setAlignment(Qt::AlignCenter);
    m_totalTimeLab->setStatusTip(totalTimetxt());
    //    totalGr->setStatusTip(totalTimetxt());
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

quint16 TexamView::questionStop() {
    m_showReact = false;
    quint16 t = qRound(m_reactTime.elapsed() / 100);
    m_reactTimeLab->setText(QString("%1").arg((qreal) t /10, 0, 'f', 1));
    m_averTime = (m_averTime * (m_questNr-1) + t) / m_questNr;
//    m_averTimeLab->setText(QString("%1").arg((qreal)qRound(m_averTime)/10));
    m_averTimeLab->setText(formatReactTime(t));
    return t;
}

void TexamView::startExam(int passTimeInSec, int questNumber, int averTime, int mistakes, int halfMist) {
    m_questNr = questNumber;
    m_totElapsedTime = passTimeInSec;
    m_totalTime = QTime(0, 0);
    m_averTime = averTime;
    m_mistakes = mistakes;
    m_halfMistakes = halfMist;
    m_showReact = false;
    m_totalTime.start();
    m_totalTime.restart();
    m_timer->start(100);
    countTime();
    setAnswer();
    m_averTimeLab->setText(QString("%1").arg((qreal)qRound(m_averTime)/10));
}

void TexamView::setAnswer(TQAunit* answer) {
    if (answer) {
      if (!answer->isCorrect()) {
        if (answer->isWrong())
          m_mistakes++;
        else
          m_halfMistakes++;
      }
    }
    m_mistLab->setText(QString("%1").arg(m_mistakes));
    m_halfLab->setText(QString("%1").arg(m_halfMistakes));
    m_corrLab->setText(QString("%1").arg(m_questNr - m_mistakes - m_halfMistakes));
    // without halfMistakes - obsolete
//     m_effect = (((qreal)m_questNr - (qreal)m_mistakes) / (qreal)m_questNr) * 100;
    m_effect = Texam::effectiveness(m_questNr, m_mistakes, m_halfMistakes);
//    m_effLab->setText(QString("<b>%1 %</b>").arg(qRound(m_effect)));
    m_effLab->setText(formatReactTime(qRound(m_effect * 10)));
}

void TexamView::setFontSize(int s) {
    QFont f = m_reactTimeLab->font();
    f.setPointSize(s);
    m_reactTimeLab->setFont(f);
//     setMaximumHeight(m_reactTimeLab->height());
    m_averTimeLab->setFont(f);
    m_totalTimeLab->setFont(f);
    m_mistLab->setFont(f);
    m_corrLab->setFont(f);
    m_halfLab->setFont(f);
    m_effLab->setFont(f);
//    m_reactTimeLab->setFixedWidth(s * 3);
//    m_averTimeLab->setFixedWidth(s * 3);
    m_totalTimeLab->setFixedWidth(s * 5);
//     setMinimumHeight(okGr->height() + 5);
}

void TexamView::countTime() {
    if (m_showReact)
        m_reactTimeLab->setText(QString("%1").arg(formatReactTime(m_reactTime.elapsed() / 100)));
    m_totalTimeLab->setText(formatedTotalTime(m_totElapsedTime*1000 + m_totalTime.elapsed()));
}

QString TexamView::formatReactTime(quint16 timeX10) {
    QString hh = "", mm = "", ss = "";
    if (timeX10 / 36000)
        hh = QString("%1").arg(timeX10 / 36000, 2, 'i', 0, '0');
    if ((timeX10 % 36000) / 600)
        mm = QString("%1").arg((timeX10 % 36000) / 600, 2, 'i', 0, '0');
    ss =QString("%1").arg(((timeX10 % 36000) % 600) / 10, 2, 'i', 0, '0' );
    QString res = "";
    if (hh != "")
        res = hh + ":";
    if (mm != "")
        res += mm + ":";
    return res + ss + QString(".%1").arg(timeX10 % 10);
}

void TexamView::clearResults() {
    m_corrLab->setText("00");
    m_mistLab->setText("00");
    m_halfLab->setText("00");
    m_effLab->setText("<b>100%</b>");
    m_averTimeLab->setText("00.0");
    m_reactTimeLab->setText("00.0");
    m_totalTimeLab->setText("0:00:00");    
}
