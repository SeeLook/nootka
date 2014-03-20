/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>


levelSettings::levelSettings(QString &imgPath, QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;

    levelSelector = new TlevelSelector();
    mainLay->addWidget(levelSelector);
    mainLay->addStretch(1);

    QHBoxLayout *butLay = new QHBoxLayout;
    saveBut = new QPushButton(QIcon(imgPath + "picts/notSaved.png"), tr("Save"),this);
    saveBut->setIconSize(QSize(48, 48));
    saveBut->setStatusTip(tr("Save level settings to file"));
    butLay->addWidget(saveBut);
    // Start an exam button
    startExamBut = new QPushButton(QIcon(imgPath + "picts/startExam.png"), tr("Start exam"), this);
    startExamBut->setIconSize(QSize(48, 48));
    startExamBut->setStatusTip(tr("Start an exam on selected level"));
    startExamBut->setDisabled(true);
    butLay->addWidget(startExamBut);

    mainLay->addLayout(butLay);
    mainLay->addStretch(1);
    
    QLabel *moreLab = new QLabel(moreLevelLinkTxt(), this);
    moreLab->setOpenExternalLinks(true);
    mainLay->addWidget(moreLab, 0, Qt::AlignCenter);
    mainLay->addStretch(1);

    setLayout(mainLay);
}
