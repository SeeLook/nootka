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

/*static*/
QString levelSettings::saveLevelTxt = tr("Save level's settings to file");
QString levelSettings::loadLevelTxt = tr("Load exam's level from file");

levelSettings::levelSettings(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;

    levelSelector = new TlevelSelector(this);
    mainLay->addWidget(levelSelector);

    QHBoxLayout *butLay = new QHBoxLayout;
    saveBut = new QPushButton(tr("Save"),this);
    saveBut->setStatusTip(saveLevelTxt);
    butLay->addWidget(saveBut);
    loadBut = new QPushButton(tr("Load"),this);
    loadBut->setStatusTip(loadLevelTxt);
    butLay->addWidget(loadBut);

    mainLay->addLayout(butLay);

    setLayout(mainLay);

}
