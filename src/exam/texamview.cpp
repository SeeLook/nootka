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
    m_corrLab = new QLabel("0", this);
    okMistLay->addWidget(m_corrLab, 0, Qt::AlignRight);
    okMistLay->addStretch(1);
    m_mistLab = new QLabel("0", this);
    okMistLay->addWidget(m_mistLab, 0, Qt::AlignRight);
//     okMistLay->addStretch(1);
    QGroupBox *okGr = new QGroupBox(this);
    okGr->setLayout(okMistLay);
    mainLay->addWidget(okGr);
    mainLay->addStretch(1);

    m_effLab = new QLabel("<b>100%</b>", this);
    QVBoxLayout *effLay = new QVBoxLayout;
    effLay->addWidget(m_effLab, 0, Qt::AlignCenter);
    QGroupBox *effGr = new QGroupBox(this);
    effGr->setLayout(effLay);
    mainLay->addWidget(effGr);
    mainLay->addStretch(1);

    m_averTimeLab = new QLabel("0.0", this);
    QVBoxLayout *averLay = new QVBoxLayout;
    averLay->addWidget(m_averTimeLab);
    QGroupBox *averGr = new QGroupBox(this);
    averGr->setLayout(averLay);
    mainLay->addWidget(averGr);
    mainLay->addStretch(1);

    m_reactTimeLab = new QLabel("0.0", this);
    QVBoxLayout *reactLay = new QVBoxLayout;
    reactLay->addWidget(m_reactTimeLab);
    QGroupBox *reactGr = new QGroupBox(this);
    reactGr->setLayout(reactLay);
    mainLay->addWidget(reactGr);
    mainLay->addStretch(1);

    m_totalTimeLab = new QLabel("0:00", this);
    QVBoxLayout *totalLay = new QVBoxLayout;
    totalLay->addWidget(m_totalTimeLab);
    QGroupBox *totalGr = new QGroupBox(this);
    totalGr->setLayout(totalLay);
    mainLay->addWidget(totalGr);
    mainLay->addStretch(1);

    setLayout(mainLay);

    m_corrLab->setToolTip(tr("correct answers' count"));
    m_corrLab->setStyleSheet(gl->getBGcolorText(gl->EanswerColor));
    m_mistLab->setToolTip(tr("mistakes' count"));
    m_mistLab->setStyleSheet(gl->getBGcolorText(gl->EquestionColor));
    m_effLab->setToolTip(tr("effectiveness"));
    m_averTimeLab->setToolTip(tr("average reaction's time [in seconds]"));
    m_reactTimeLab->setToolTip(tr("reaction's time [in seconds]"));
    m_totalTimeLab->setToolTip(tr("total exam's time"));

}

void TexamView::startQuestion() {
    m_reactTime.start();
}

void TexamView::stopQuestion() {
    int t = qRound(m_reactTime.elapsed() / 100);
    m_reactTimeLab->setText(QString("%1").arg((qreal)t/10));
}
