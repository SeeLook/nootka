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
  // /* HORIZONTAL LAYOUT
    QHBoxLayout *mainLay = new QHBoxLayout;
//     QGroupBox *mainGr = new QGroupBox(this);


//     QVBoxLayout *okMistLay = new QVBoxLayout;
    QHBoxLayout *okMistLay = new QHBoxLayout;
//     okMistLay->addStretch(1);
    m_corrLab = new QLabel("100", this);
    okMistLay->addWidget(m_corrLab, 0, Qt::AlignRight);
    m_mistLab = new QLabel("100", this);
    okMistLay->addWidget(m_mistLab, 0, Qt::AlignRight);
//     okMistLay->addStretch(1);
    mainLay->addLayout(okMistLay);

    mainLay->addStretch(1);
    m_effLab = new QLabel("<b>100%</b>", this);
    mainLay->addWidget(m_effLab, 0, Qt::AlignCenter);
    mainLay->addStretch(1);
    m_averTime = new QLCDNumber(4, this);
    m_averTime->setFixedHeight(20);
    mainLay->addWidget(m_averTime);
    m_reactTimeLCD = new QLCDNumber(4, this);
    m_reactTimeLCD->setFixedHeight(20);
    mainLay->addWidget(m_reactTimeLCD);
    m_totalTime = new QLCDNumber(7, this);
    m_totalTime->setFixedHeight(20);
    mainLay->addWidget(m_totalTime);


  /* VERTICAL LAYOUT
    QVBoxLayout *mainLay = new QVBoxLayout;
//    QGroupBox *mainGr = new QGroupBox(this);

    m_totalTime = new QLCDNumber(7, this);
//    m_totalTime->setFixedHeight(20);
    mainLay->addWidget(m_totalTime);

    m_reactTimeLCD = new QLCDNumber(4, this);
//    m_reactTimeLCD->setFixedHeight(20);
    mainLay->addWidget(m_reactTimeLCD);

    m_averTime = new QLCDNumber(4, this);
//    m_averTime->setFixedHeight(20);
    mainLay->addWidget(m_averTime);

    m_effLab = new QLabel("<b>100%</b>", this);
    mainLay->addWidget(m_effLab, 0, Qt::AlignCenter);

    QHBoxLayout *okMistLay = new QHBoxLayout;
    m_corrLab = new QLabel("12", this);
    okMistLay->addWidget(m_corrLab, 0, Qt::AlignCenter);
    m_mistLab = new QLabel("12", this);
    okMistLay->addWidget(m_mistLab, 0, Qt::AlignCenter);
    mainLay->addLayout(okMistLay);
*/

//    mainGr->setLayout(mainLay);
//    QHBoxLayout *lay = new QHBoxLayout;
//    lay->addWidget(mainGr);
//    setLayout(lay);
    setLayout(mainLay);

    m_corrLab->setToolTip(tr("correct answers' count"));
    m_corrLab->setStyleSheet(gl->getBGcolorText(gl->EanswerColor));
    m_mistLab->setToolTip(tr("mistakes' count"));
    m_mistLab->setStyleSheet(gl->getBGcolorText(gl->EquestionColor));
    m_effLab->setToolTip(tr("effectiveness"));
    m_averTime->setToolTip(tr("average reaction's time"));
    m_averTime->display(0.1);
    m_averTime->setSmallDecimalPoint(true);
    m_reactTimeLCD->setToolTip(tr("reaction's time"));
    m_reactTimeLCD->display("0'0");
    m_reactTimeLCD->setSmallDecimalPoint(true);
    m_totalTime->setToolTip(tr("total exam's time"));
    m_totalTime->display("1:23:15");
    m_totalTime->setSmallDecimalPoint(true);
}

void TexamView::startQuestion() {
    m_reactTime.start();
}

void TexamView::stopQuestion() {
    int t = m_reactTime.elapsed();
    m_reactTimeLCD->display(QString("%1'%2").arg(t/1000).arg((t/100)%10));
}
