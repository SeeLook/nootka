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
    QHBoxLayout *nLay = new QHBoxLayout;
    nameStyleGr = new TnotationRadioGroup(gl->NnameStyleInNoteName, this);
    nLay->addWidget(nameStyleGr);
    nLay->addSpacing(5);

    QGroupBox *bGr = new QGroupBox(tr("7-th note is:"),this);
    QVBoxLayout *bLay = new QVBoxLayout;
    bLay->setAlignment(Qt::AlignCenter);
    QButtonGroup *bButtGr = new QButtonGroup(this);
    isBRadio = new QRadioButton(tr("B"),this);
    isBRadio->setStatusTip(tr("7-th note is <b>B</b> and with flat is <b>Bb</b> or bes or <b>bs</b>"));
    bLay->addWidget(isBRadio);
    bButtGr->addButton(isBRadio);
    isHRadio = new QRadioButton(tr("H"),this);
    isHRadio->setStatusTip(tr("7-th note is <b>H</b> and with flat is <b>Hb</b> or <b>B</b>"));
    bLay->addWidget(isHRadio);
    bLay->addStretch(1);
    bButtGr->addButton(isHRadio);
    bGr->setLayout(bLay);
    nLay->addWidget(bGr);

    if (gl->seventhIs_B) isBRadio->setChecked(true);
    else isHRadio->setChecked(true);

    mainLay->addLayout(nLay);
    mainLay->addStretch(1);

    /** @todo example label with all scale*/

    octInNameCh = new QCheckBox(tr("show octave in the note's name"),this);
    mainLay->addWidget(octInNameCh);
    octInNameCh->setStatusTip(tr("Shows formated note's name. For small octave - the name is small letter,<br>for great octave - the name starts with a capital letter,<br>for one-line, digit <sup>1</sup> is added, and so on." ));
    octInNameCh->setChecked(gl->NoctaveInNoteNameFormat);
    mainLay->addStretch(1);
    setLayout(mainLay);

    connect(bButtGr, SIGNAL(buttonClicked(int)), this, SLOT(seventhNoteWasChanged()));
}

void NameSettings::saveSettings() {
    gl->NnameStyleInNoteName = nameStyleGr->getNameStyle();
    gl->NoctaveInNoteNameFormat = octInNameCh->isChecked();
    if (isBRadio->isChecked()) gl->seventhIs_B = true;
    else gl->seventhIs_B = false;
}

void NameSettings::seventhNoteWasChanged() {
    if (isBRadio->isChecked()) {
        nameStyleGr->seventhNoteWasChanged(true);
        emit seventhIsBChanged(true);
    } else {
        nameStyleGr->seventhNoteWasChanged(false);
        emit seventhIsBChanged(false);
    }

}
