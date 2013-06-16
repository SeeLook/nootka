/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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

#include "tguitarsettings.h"
#include "tcolorbutton.h"
#include "ttune.h"
#include "tglobals.h"
#include "tsimplescore.h"
#include <QtGui>



extern Tglobals *gl;

TguitarSettings::TguitarSettings(QWidget *parent) :
        QWidget(parent)
{
    
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout *upLay = new QHBoxLayout;
    QGroupBox *tuneGr = new QGroupBox(tr("tune of the guitar"));
    tuneGr->setStatusTip(tr("Select appropirate tune from the list or prepare your own."));
    QVBoxLayout *tuneLay = new QVBoxLayout;
    tuneLay->setAlignment(Qt::AlignCenter);
    tuneCombo = new QComboBox(this);
    tuneLay->addWidget(tuneCombo);
    tuneView = new TsimpleScore(6, this);
    tuneLay->addWidget(tuneView);
    tuneView->setClef(Tclef(Tclef::e_treble_G)); // TODO global clef !!!!
    
//     tuneView->setFixedWidth(280);
//     tuneView->setAmbitus(Tnote(6,-2,0),Tnote(6,1,0));
    setTune(gl->Gtune());
    tuneCombo->addItem(Ttune::stdTune.name);
    if (gl->Gtune() == Ttune::stdTune)
        tuneCombo->setCurrentIndex(0);
    for (int i=0; i<4; i++) {
        tuneCombo->addItem(Ttune::tunes[i].name);
        if (gl->Gtune() == Ttune::tunes[i])
            tuneCombo->setCurrentIndex(i+1);
    }
    QString S = tr("Custom tune");
    tuneCombo->addItem(S);
    if (gl->Gtune().name == S)
        tuneCombo->setCurrentIndex(5);
    tuneGr->setLayout(tuneLay);
    upLay->addWidget(tuneGr);

    QVBoxLayout *hfLay = new QVBoxLayout;
    QGroupBox *hfGr = new QGroupBox;
    hfLay->addStretch(1);
    righthandCh = new QCheckBox(tr("guitar for right-handed"),this);
    righthandCh->setChecked(gl->GisRightHanded);
    righthandCh->setStatusTip(tr("Uncheck this if you are lefthanded<br>and your gitar has changed strings' order"));
    hfLay->addWidget(righthandCh);
    hfLay->addStretch(1);
    QLabel *fretLab = new QLabel(tr("number of frets:"),this);
    hfLay->addWidget(fretLab);
    fretsNrSpin = new QSpinBox(this);
    fretsNrSpin->setValue(gl->GfretsNumber);
    fretsNrSpin->setMaximum(24);
    fretsNrSpin->setMinimum(15);
    hfLay->addWidget(fretsNrSpin);
    hfLay->addStretch(1);
    upLay->addSpacing(3);
    hfGr->setLayout(hfLay);
    upLay->addWidget(hfGr);

    mainLay->addLayout(upLay);

    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *prefLay = new QVBoxLayout;
    QGroupBox *prefBox = new QGroupBox(tr("prefered accidentals:"),this);
    prefBox->setStatusTip(tr("Choose which accidentals will be shown in the score."));
    prefSharpBut = new QRadioButton(tr("# - sharps"),this);
    prefFlatBut = new  QRadioButton(tr("b - flats"),this);
    QButtonGroup *prefGr = new QButtonGroup(this);
    prefGr->addButton(prefSharpBut);
    prefGr->addButton(prefFlatBut);
    prefLay->addWidget(prefSharpBut);
    prefLay->addWidget(prefFlatBut);
    prefBox->setLayout(prefLay);
    if (gl->GpreferFlats) prefFlatBut->setChecked(true);
    else
        prefSharpBut->setChecked(true);
    downLay->addWidget(prefBox);

    morePosCh = new QCheckBox(tr("show all possibilities of a note"),this);
    morePosCh->setStatusTip(tr("As you know, the same note can be played in few places on a fingerboard.<br>If checked, all of them are showed."));
    downLay->addWidget(morePosCh);
    morePosCh->setChecked(gl->GshowOtherPos);

    mainLay->addLayout(downLay);
    QGridLayout *colorLay = new QGridLayout;
    QLabel *pointLab = new QLabel(tr("color of string/fret pointer"), this);
    pointColorBut = new TcolorButton(gl->GfingerColor, this);
    colorLay->addWidget(pointLab, 0, 0);
    colorLay->addWidget(pointColorBut, 0 ,1);
    QLabel *selLab = new QLabel(tr("color of selected string/fret"), this);
    selColorBut = new TcolorButton(gl->GselectedColor, this);
    colorLay->addWidget(selLab, 1, 0);
    colorLay->addWidget(selColorBut, 1, 1);
    mainLay->addLayout(colorLay);

    setLayout(mainLay);

    connect(tuneCombo, SIGNAL(activated(int)), this, SLOT(tuneSelected(int)));
    connect(tuneView, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(userTune(int,Tnote)));
    
}

void TguitarSettings::setTune(Ttune tune) {
    for (int i = 0; i < 6; i++) {
        tuneView->setNote(i, tune[6-i]);
        tuneView->setStringNumber(i, 6 - i);
    }
}

void TguitarSettings::tuneSelected(int tuneId) {
    if (tuneId == 0)
        setTune(Ttune::stdTune);
    else
        if (tuneId != tuneCombo->count() - 1) //the last is custom
        setTune(Ttune::tunes[tuneId - 1]);
}

void TguitarSettings::userTune(int, Tnote) {
    tuneCombo->setCurrentIndex(tuneCombo->count() - 1);
}

void TguitarSettings::saveSettings() {
    gl->GisRightHanded = righthandCh->isChecked();
    gl->GfretsNumber = fretsNrSpin->value();
    gl->setTune(Ttune(tuneCombo->currentText(), tuneView->getNote(5), tuneView->getNote(4),          tuneView->getNote(3), tuneView->getNote(2), tuneView->getNote(1), tuneView->getNote(0)));
    gl->GshowOtherPos = morePosCh->isChecked();
    if (prefFlatBut->isChecked()) gl->GpreferFlats = true;
    else gl->GpreferFlats = false;
    gl->GfingerColor = pointColorBut->getColor();
    gl->GfingerColor.setAlpha(200);
    gl->GselectedColor = selColorBut->getColor();
}
