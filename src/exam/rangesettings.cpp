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
    scoreRang->setFixedWidth(160);
    QGroupBox *notesRangGr = new QGroupBox(tr("Notes' range:"),this);
    scoreLay->addWidget(scoreRang);
    notesRangGr->setLayout(scoreLay);

    allLay->addWidget(notesRangGr);

    QVBoxLayout *guitLay = new QVBoxLayout;
    QGroupBox *fretGr = new QGroupBox(tr("frets' range:"),this);
    QHBoxLayout *fretLay = new QHBoxLayout;
    QLabel *fromLab = new QLabel(tr("from"),this);
    fromSpinB = new QSpinBox(this);
    QLabel *toLab = new QLabel(tr("to"),this);
    toSpinB = new QSpinBox(this);
    fretLay->addWidget(fromLab);
    fretLay->addWidget(fromSpinB);
    fretLay->addStretch(1);
    fretLay->addWidget(toLab);
    fretLay->addWidget(toSpinB);
    fretGr->setLayout(fretLay);
    guitLay->addWidget(fretGr);
    guitLay->addStretch(1);

    QGroupBox *stringsGr = new QGroupBox(tr("avaiable strings:"),this);
    QGridLayout *strLay = new QGridLayout;
    for (int i=0; i<6; i++) {
        stringBut[i] = new QPushButton(QString("%1").arg(i+1),this);
        stringBut[i]->setCheckable(true);
        if (i<3)
            strLay->addWidget(stringBut[i],1,i+1,0);
        else
            strLay->addWidget(stringBut[i],2,i-2,0);
    }
    stringsGr->setLayout(strLay);
    guitLay->addWidget(stringsGr);
    guitLay->addStretch(1);

    allLay->addLayout(guitLay);

    mainLay->addLayout(allLay);
    mainLay->addStretch(1);

    hiStrOnlyChBox = new QCheckBox(tr("notes on the highest string only"),this);
    hiStrOnlyChBox->setStatusTip(tr("If checked, only simple possibility of a note are required<br>- without other variants of a note on another strings."));
    mainLay->addWidget(hiStrOnlyChBox);
    currKeySignChBox = new QCheckBox(tr("notes in current key signature only"),this);
    mainLay->addWidget(currKeySignChBox);

    setLayout(mainLay);
}
