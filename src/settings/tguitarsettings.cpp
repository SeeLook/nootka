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
    m_tuneCombo = new QComboBox(this);
    tuneLay->addWidget(m_tuneCombo);
    m_tuneView = new TsimpleScore(6, this);
    tuneLay->addWidget(m_tuneView);
    m_tuneView->setClef(Tclef(Tclef::e_treble_G_8down)); // TODO global clef !!!!
    
//     tuneView->setFixedWidth(280);
//     tuneView->setAmbitus(Tnote(6,-2,0),Tnote(6,1,0));
    setTune(gl->Gtune());
    m_tuneCombo->addItem(Ttune::stdTune.name);
    if (gl->Gtune() == Ttune::stdTune)
        m_tuneCombo->setCurrentIndex(0);
    for (int i=0; i<4; i++) {
        m_tuneCombo->addItem(Ttune::tunes[i].name);
        if (gl->Gtune() == Ttune::tunes[i])
            m_tuneCombo->setCurrentIndex(i+1);
    }
    QString S = tr("Custom tune");
    m_tuneCombo->addItem(S);
    if (gl->Gtune().name == S)
        m_tuneCombo->setCurrentIndex(5);
    tuneGr->setLayout(tuneLay);
    upLay->addWidget(tuneGr);

    QVBoxLayout *hfLay = new QVBoxLayout;
    QGroupBox *hfGr = new QGroupBox;
    hfLay->addStretch(1);
    m_righthandCh = new QCheckBox(tr("guitar for right-handed"),this);
    m_righthandCh->setChecked(gl->GisRightHanded);
    m_righthandCh->setStatusTip(tr("Uncheck this if you are lefthanded<br>and your gitar has changed strings' order"));
    hfLay->addWidget(m_righthandCh);
    hfLay->addStretch(1);
    QLabel *fretLab = new QLabel(tr("number of frets:"),this);
    hfLay->addWidget(fretLab);
    m_fretsNrSpin = new QSpinBox(this);
    m_fretsNrSpin->setValue(gl->GfretsNumber);
    m_fretsNrSpin->setMaximum(24);
    m_fretsNrSpin->setMinimum(15);
    hfLay->addWidget(m_fretsNrSpin);
    hfLay->addStretch(1);
    upLay->addSpacing(3);
    hfGr->setLayout(hfLay);
    upLay->addWidget(hfGr);

    mainLay->addLayout(upLay);

    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *prefLay = new QVBoxLayout;
    QGroupBox *prefBox = new QGroupBox(tr("prefered accidentals:"),this);
    prefBox->setStatusTip(tr("Choose which accidentals will be shown in the score."));
    m_prefSharpBut = new QRadioButton(tr("# - sharps"),this);
    m_prefFlatBut = new  QRadioButton(tr("b - flats"),this);
    QButtonGroup *prefGr = new QButtonGroup(this);
    prefGr->addButton(m_prefSharpBut);
    prefGr->addButton(m_prefFlatBut);
    prefLay->addWidget(m_prefSharpBut);
    prefLay->addWidget(m_prefFlatBut);
    prefBox->setLayout(prefLay);
    if (gl->GpreferFlats) m_prefFlatBut->setChecked(true);
    else
        m_prefSharpBut->setChecked(true);
    downLay->addWidget(prefBox);

    m_morePosCh = new QCheckBox(tr("show all possibilities of a note"),this);
    m_morePosCh->setStatusTip(tr("As you know, the same note can be played in few places on a fingerboard.<br>If checked, all of them are showed."));
    downLay->addWidget(m_morePosCh);
    m_morePosCh->setChecked(gl->GshowOtherPos);

    mainLay->addLayout(downLay);
    QGridLayout *colorLay = new QGridLayout;
    QLabel *pointLab = new QLabel(tr("color of string/fret pointer"), this);
    m_pointColorBut = new TcolorButton(gl->GfingerColor, this);
    colorLay->addWidget(pointLab, 0, 0);
    colorLay->addWidget(m_pointColorBut, 0 ,1);
    QLabel *selLab = new QLabel(tr("color of selected string/fret"), this);
    m_selColorBut = new TcolorButton(gl->GselectedColor, this);
    colorLay->addWidget(selLab, 1, 0);
    colorLay->addWidget(m_selColorBut, 1, 1);
    mainLay->addLayout(colorLay);

    setLayout(mainLay);

    connect(m_tuneCombo, SIGNAL(activated(int)), this, SLOT(tuneSelected(int)));
    connect(m_tuneView, SIGNAL(noteHasChanged(int,Tnote)), this, SLOT(userTune(int,Tnote)));
    
}

void TguitarSettings::setTune(Ttune tune) {
    for (int i = 0; i < 6; i++) {
        m_tuneView->setNote(i, tune[6-i]);
        m_tuneView->setStringNumber(i, 6 - i);
    }
}

void TguitarSettings::tuneSelected(int tuneId) {
    if (tuneId == 0)
        setTune(Ttune::stdTune);
    else
        if (tuneId != m_tuneCombo->count() - 1) //the last is custom
        setTune(Ttune::tunes[tuneId - 1]);
}

void TguitarSettings::userTune(int, Tnote) {
    m_tuneCombo->setCurrentIndex(m_tuneCombo->count() - 1);
}

void TguitarSettings::saveSettings() {
    gl->GisRightHanded = m_righthandCh->isChecked();
    gl->GfretsNumber = m_fretsNrSpin->value();
    gl->setTune(Ttune(m_tuneCombo->currentText(), m_tuneView->getNote(5), m_tuneView->getNote(4),          m_tuneView->getNote(3), m_tuneView->getNote(2), m_tuneView->getNote(1), m_tuneView->getNote(0)));
    gl->GshowOtherPos = m_morePosCh->isChecked();
    if (m_prefFlatBut->isChecked()) gl->GpreferFlats = true;
    else gl->GpreferFlats = false;
    gl->GfingerColor = m_pointColorBut->getColor();
    gl->GfingerColor.setAlpha(200);
    gl->GselectedColor = m_selColorBut->getColor();
}
