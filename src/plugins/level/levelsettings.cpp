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


#include "levelsettings.h"
#include <level/tlevelselector.h>
#include <exam/textrans.h>
#include <tpath.h>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>


levelSettings::levelSettings(QWidget* parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;

    m_levelSelector = new TlevelSelector();
    mainLay->addWidget(m_levelSelector);
#if !defined (Q_OS_ANDROID)
    mainLay->addStretch(1);
#endif

    m_saveBut = new QPushButton(QIcon(Tpath::img("notSaved")), tr("Save"),this);
    m_saveBut->setStatusTip(tr("Save level settings to file"));
	// Start an exam button
    m_startExamBut = new QPushButton(QIcon(Tpath::img("exam")), tr("Start exam"), this);
    m_startExamBut->setStatusTip(tr("Start an exam on selected level"));
    m_startExamBut->setDisabled(true);
	// Start an exercise button
    m_startExerBut = new QPushButton(QIcon(Tpath::img("practice")), tr("Start exercise"), this);
    m_startExerBut->setStatusTip(tr("Start an exercise on selected level"));
    m_startExerBut->setDisabled(true);
#if !defined (Q_OS_ANDROID)
    m_saveBut->setIconSize(QSize(48, 48));
    m_startExamBut->setIconSize(QSize(48, 48));
    m_startExerBut->setIconSize(QSize(48, 48));
#endif
		
		QHBoxLayout *butLay = new QHBoxLayout;
			butLay->addWidget(m_saveBut);
			butLay->addWidget(m_startExamBut);
			butLay->addWidget(m_startExerBut);

    mainLay->addLayout(butLay);
#if !defined (Q_OS_ANDROID)
    mainLay->addStretch(1);
#endif
    QLabel *moreLab = new QLabel(TexTrans::moreLevelLinkTxt(), this);
    moreLab->setOpenExternalLinks(true);
    mainLay->addWidget(moreLab, 0, Qt::AlignCenter);
    mainLay->addStretch(1);

    setLayout(mainLay);
}
