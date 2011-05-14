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
#include <QtGui>

TexamView::TexamView(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *mainLay = new QHBoxLayout;
//     QGroupBox *mainGr = new QGroupBox(this);

    QVBoxLayout *okMistLay = new QVBoxLayout;
    m_corrLab = new QLabel("12", this);
    m_corrLab->setToolTip(tr("correct answers' count"));
    okMistLay->addWidget(m_corrLab, 0, Qt::AlignRight);
    m_mistLab = new QLabel("12", this);
    m_mistLab->setToolTip(tr("mistakes' count"));
    okMistLay->addWidget(m_mistLab, 0, Qt::AlignRight);
    mainLay->addLayout(okMistLay);

    m_effLab = new QLabel("<b>100%</b>", this);
    m_effLab->setToolTip(tr("effectiveness"));
    mainLay->addWidget(m_effLab, 0, Qt::AlignCenter);
    m_averTime = new QLCDNumber(4, this);
    m_averTime->setFixedHeight(25);
    m_averTime->setToolTip(tr("average reaction's time"));
    mainLay->addWidget(m_averTime);
    m_averTime->display(0.1);
    m_averTime->setSmallDecimalPoint(true);
    m_reactTime = new QLCDNumber(4, this);
    m_reactTime->setFixedHeight(25);
    m_reactTime->setToolTip(tr("reaction's time"));
    mainLay->addWidget(m_reactTime);
    m_reactTime->display(12.5);
    m_reactTime->setSmallDecimalPoint(true);
    m_totalTime = new QLCDNumber(7, this);
    m_totalTime->setToolTip(tr("total exam's time"));
    m_totalTime->setFixedHeight(25);
    mainLay->addWidget(m_totalTime);
    m_totalTime->display("1:23:15");
    m_totalTime->setSmallDecimalPoint(true);

//     mainGr->setLayout(mainLay);
//     QHBoxLayout *lay = new QHBoxLayout;
//     lay->addWidget(mainGr);
//     setLayout(lay);
    setLayout(mainLay);
    

}
