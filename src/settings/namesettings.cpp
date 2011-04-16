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


#include "namesettings.h"
#include "tglobals.h"

extern Tglobals *gl;

NameSettings::NameSettings(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);
    nameStyleGr = new TnotationRadioGroup(gl->NnameStyleInNoteName, this);
    mainLay->addWidget(nameStyleGr);
    sevenNoteChB = new QCheckBox(tr("seventh note is B"),this);
    mainLay->addWidget(sevenNoteChB);
    sevenNoteChB->setChecked(gl->seventhIs_B);
    octInNameCh = new QCheckBox(tr("show octave in the note's name"),this);
    mainLay->addWidget(octInNameCh);
    octInNameCh->setStatusTip(tr("Shows formated note's name. For small octave - the name is small letter,<br>for great octave - the name starts with a capital letter,<br>for one-line, digit <sup>1</sup> is added, and so on." ));
    octInNameCh->setChecked(gl->NoctaveInNoteNameFormat);
    mainLay->addStretch(1);
    setLayout(mainLay);
}

void NameSettings::saveSettings() {
    gl->NnameStyleInNoteName = nameStyleGr->getNameStyle();
    gl->NoctaveInNoteNameFormat = octInNameCh->isChecked();
    gl->seventhIs_B = sevenNoteChB->isChecked();
}
