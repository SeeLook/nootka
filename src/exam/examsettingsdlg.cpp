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


#include "examsettingsdlg.h"
#include "tglobals.h"

extern Tglobals *gl;

examSettingsDlg::examSettingsDlg(QWidget *parent) :
    TsettingsDialogBase(parent)
{
    setWindowTitle("Nootka - "+tr("exam's levels' settings"));

    navList->addItem(tr("Levels"));
    navList->item(0)->setIcon(QIcon(gl->path+"picts/levelsSettings.png"));
    navList->item(0)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Questions"));
    navList->item(1)->setIcon(QIcon(gl->path+"picts/questionsSettings.png"));
    navList->item(1)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Range"));
    navList->item(2)->setIcon(QIcon(gl->path+"picts/rangeSettings.png"));
    navList->item(2)->setTextAlignment(Qt::AlignCenter);

    levelSett = new levelSettings();
    questSett = new questionsSettings();
    rangeSett = new rangeSettings();

    stackLayout->addWidget(levelSett);
    stackLayout->addWidget(questSett);
    stackLayout->addWidget(rangeSett);

    connect(navList, SIGNAL(currentRowChanged(int)), stackLayout, SLOT(setCurrentIndex(int)));

    navList->setCurrentRow(0);

    connect(levelSett, SIGNAL(levelChanged(TexamLevel)), this, SLOT(levelWasSelected(TexamLevel)));
}

void examSettingsDlg::levelWasSelected(TexamLevel level) {
    questSett->loadLevel(level);
}
