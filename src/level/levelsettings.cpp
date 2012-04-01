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


levelSettings::levelSettings(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;

    levelSelector = new TlevelSelector();
    mainLay->addWidget(levelSelector);
    mainLay->addStretch(1);

    QHBoxLayout *butLay = new QHBoxLayout;
    saveBut = new QPushButton(tr("Save"),this);
    saveBut->setStatusTip(tr("Save level's settings to file"));
    butLay->addWidget(saveBut);

    mainLay->addLayout(butLay);
    mainLay->addStretch(1);
    
    QLabel *moreLab = new QLabel(moreLevelLinkTxt(), this);
    moreLab->setOpenExternalLinks(true);
    mainLay->addWidget(moreLab, 0, Qt::AlignCenter);
    mainLay->addStretch(1);

    setLayout(mainLay);
}
