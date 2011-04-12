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


#include "rangesettings.h"

rangeSettings::rangeSettings(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;

    QHBoxLayout *allLay = new QHBoxLayout;

    QVBoxLayout *scoreLay = new QVBoxLayout;
    scoreRang = new TscoreWidgetSimple(2, this);
    QLabel *notesRangLab = new QLabel(tr("Range of the notes:"),this);
    scoreLay->addWidget(notesRangLab);
    scoreLay->addWidget(scoreRang);

    allLay->addLayout(scoreLay);

    mainLay->addLayout(allLay);

    setLayout(mainLay);
}
