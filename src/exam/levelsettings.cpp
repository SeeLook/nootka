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


#include "levelsettings.h"

levelSettings::levelSettings(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *mainLay = new QHBoxLayout;

    QVBoxLayout *levLay = new QVBoxLayout;
    QLabel *levLab = new QLabel(tr("Levels:"),this);
    levLay->addWidget(levLab);
    levelsList = new QListWidget(this);
    levelsList->setMouseTracking(true);
    levelsList->setFixedWidth(200);
    levLay->addWidget(levelsList);
    levLay->addStretch(1);

    QHBoxLayout *butLay = new QHBoxLayout;
    saveBut = new QPushButton(tr("Save"),this);
    butLay->addWidget(saveBut);
    loadBut = new QPushButton(tr("Load"),this);
    loadBut->setStatusTip(tr("Load exam's level from file"));
    butLay->addWidget(loadBut);
    levLay->addLayout(butLay);
    levLay->addStretch(1);


    mainLay->addLayout(levLay);

    summWdg = new TlevelSummaryWdg(this);
    mainLay->addWidget(summWdg);

    setLayout(mainLay);

    findLevels();

    connect(levelsList, SIGNAL(currentRowChanged(int)), this, SLOT(levelSelected(int)));


}

void levelSettings::findLevels() {
    TexamLevel lev = TexamLevel();
    // from constructor
    levelsList->addItem(lev.name);
    levelsList->item(0)->setStatusTip(lev.desc);
    levList << lev;
}

void levelSettings::levelSelected(int id) {
    summWdg->setLevel(levList[id]);
    emit levelChanged(levList[id]);
}
