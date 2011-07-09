/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "texamview.h"
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;

TexamView::TexamView(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

    QHBoxLayout *mainLay = new QHBoxLayout;

    mainLay->addStretch(1);
    QHBoxLayout *okMistLay = new QHBoxLayout;
    m_corrLab = new QLabel(this);
    okMistLay->addWidget(m_corrLab, 0, Qt::AlignRight);
    okMistLay->addSpacing(10);
    m_mistLab = new QLabel(this);
    okMistLay->addWidget(m_mistLab, 0, Qt::AlignRight);
//     okMistLay->addStretch(1);
    QGroupBox *okGr = new QGroupBox(this);
    okGr->setLayout(okMistLay);
    mainLay->addWidget(okGr);
    mainLay->addStretch(1);

    m_effLab = new QLabel(this);
    QVBoxLayout *effLay = new QVBoxLayout;
    effLay->addWidget(m_effLab, 0, Qt::AlignCenter);
    QGroupBox *effGr = new QGroupBox(this);
    effGr->setLayout(effLay);
    mainLay->addWidget(effGr);
    mainLay->addStretch(1);

    m_averTimeLab = new QLabel(this);
    QVBoxLayout *averLay = new QVBoxLayout;
    averLay->addWidget(m_averTimeLab);
    QGroupBox *averGr = new QGroupBox(this);
    averGr->setLayout(averLay);
    mainLay->addWidget(averGr);
    mainLay->addStretch(1);

    m_reactTimeLab = new QLabel(this);
    QVBoxLayout *reactLay = new QVBoxLayout;
    reactLay->addWidget(m_reactTimeLab);
    QGroupBox *reactGr = new QGroupBox(this);
    reactGr->setLayout(reactLay);
    mainLay->addWidget(reactGr);
    mainLay->addStretch(1);

    m_totalTimeLab = new QLabel(this);
    QVBoxLayout *totalLay = new QVBoxLayout;
    totalLay->addWidget(m_totalTimeLab);
    QGroupBox *totalGr = new QGroupBox(this);
    totalGr->setLayout(totalLay);
    mainLay->addWidget(totalGr);
    mainLay->addStretch(1);

    setLayout(mainLay);

    clearResults();

    m_corrLab->setToolTip(tr("correct answers' count"));
    m_corrLab->setStyleSheet(gl->getBGcolorText(gl->EanswerColor));
    m_mistLab->setToolTip(tr("mistakes' count"));
    m_mistLab->setStyleSheet(gl->getBGcolorText(gl->EquestionColor));
    m_effLab->setToolTip(tr("effectiveness"));
    m_averTimeLab->setToolTip(tr("average reaction's time [in seconds]"));
    m_averTimeLab->setAlignment(Qt::AlignCenter);
    m_reactTimeLab->setToolTip(tr("reaction's time [in seconds]"));
    m_reactTimeLab->setAlignment(Qt::AlignCenter);
    m_totalTimeLab->setToolTip(tr("total exam's time"));
    m_totalTimeLab->setAlignment(Qt::AlignCenter);
    
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(countTime()));

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
    m_reactTimeLab->setText(QString("%1").arg((qreal)t/10, 0, 'f', 1));
    m_averTime = (m_averTime * (m_questNr-1) + t) / m_questNr;
    m_averTimeLab->setText(QString("%1").arg((qreal)qRound(m_averTime)/10));
    return t;
}

void TexamView::startExam(int passTimeInSec, int questNumber, int averTime, int mistakes) {
    m_questNr = questNumber;
    m_totElapsedTime = passTimeInSec;
    m_totalTime = QTime(0,0);
    m_averTime = averTime;
    m_mistakes = mistakes;
    m_showReact = false;
    m_totalTime.start();
    m_totalTime.restart();
    m_timer->start(1000);
    countTime();
    setAnswer(true);
    m_averTimeLab->setText(QString("%1").arg((qreal)qRound(m_averTime)/10));
}

void TexamView::setAnswer(bool wasCorrect) {
    if (!wasCorrect) {
        m_mistakes++;
    }
    m_mistLab->setText(QString("%1").arg(m_mistakes));
    m_corrLab->setText(QString("%1").arg(m_questNr - m_mistakes));
    qreal eff = (((qreal)m_questNr - (qreal)m_mistakes) / (qreal)m_questNr) * 100;
    m_effLab->setText(QString("<b>%1 %</b>").arg(qRound(eff)));
}

void TexamView::setFontSize(int s) {
    QFont f = m_reactTimeLab->font();
    f.setPixelSize(s);
    m_reactTimeLab->setFont(f);
    m_averTimeLab->setFont(f);
    m_totalTimeLab->setFont(f);
    m_mistLab->setFont(f);
    m_corrLab->setFont(f);
    m_effLab->setFont(f);
    m_reactTimeLab->setFixedWidth(s * 3);
    m_averTimeLab->setFixedWidth(s * 3);
    m_totalTimeLab->setFixedWidth(s * 4);
}

void TexamView::countTime() {
    if (m_showReact)
        m_reactTimeLab->setText(QString("%1").arg(m_reactTime.elapsed() / 1000, 0, 'f', 1, '0'));
    int t = m_totElapsedTime + m_totalTime.elapsed();
//    m_totalTimeLab->setText(m_totalTime.toString("hh:mm:ss"));
    m_totalTimeLab->setText(QString("%1:%2:%3").arg(t/3600000).arg((t%36000000)/60000, 2, 'f', 0, '0').arg((t%60000)/1000, 2, 'f', 0, '0'));
}

void TexamView::clearResults() {
    m_corrLab->setText("0");
    m_mistLab->setText("0");
    m_effLab->setText("<b>100%</b>");
    m_averTimeLab->setText("0.0");
    m_reactTimeLab->setText("0.0");
    m_totalTimeLab->setText("0:00:00");

}
