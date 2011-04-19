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


#include "questionssettings.h"

questionsSettings::questionsSettings(QWidget *parent) :
    QWidget(parent)
{
    questAsToolBox = new QToolBox(this);
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *headLab = new QLabel("<b>"+QString(TquestionAsWdg::questionsTxt).toUpper() + "</b>", this);
    mainLay->addWidget(headLab, 0, Qt::AlignCenter);
    mainLay->addWidget(questAsToolBox);
    setLayout(mainLay);

    TasNoteWdg *asNoteWdg = new TasNoteWdg();
    TasNameWdg *asNameWdg = new TasNameWdg();
    TasFretPosWdg *asFretPosWdg = new TasFretPosWdg();

    questAsToolBox->addItem(asNoteWdg, TquestionAsWdg::asNoteTxt);
    questAsToolBox->addItem(asNameWdg, TquestionAsWdg::asNameTxt);
    questAsToolBox->addItem(asFretPosWdg, TquestionAsWdg::asFretPosTxt);

}


//############################# AS NOTE IN SCORE ###################################

TasNoteWdg::TasNoteWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout *upperLay = new QHBoxLayout;
    asNoteGr = new TquestionAsWdg(this);
    upperLay->addWidget(asNoteGr);

    QVBoxLayout *accLay = new QVBoxLayout;
    sharpsChB = new QCheckBox(tr("sharps"),this);
    flatsChB = new QCheckBox(tr("flats"),this);
    doubleAccChB = new QCheckBox(tr("double accidentals"),this);
    accLay->addWidget(sharpsChB);
    accLay->addWidget(flatsChB);
    accLay->addWidget(doubleAccChB);
    accLay->addStretch(1);
    accidGr = new QGroupBox(tr("accidentals"),this);
    accidGr->setCheckable(true);
    accidGr->setLayout(accLay);
    upperLay->addWidget(accidGr);

    mainLay->addLayout(upperLay);

    QHBoxLayout *keyLay = new QHBoxLayout;
    keySignGr = new QGroupBox(tr("use keys singature"),this);
    keySignGr->setCheckable(true);

    QVBoxLayout *rangeLay = new QVBoxLayout;
    rangeLay->setAlignment(Qt::AlignCenter);
    singleKeyRadio = new QRadioButton(tr("single key"),this);
    singleKeyRadio->setStatusTip(tr("only one, selected key signature<br>for whole exam."));
    rangeKeysRadio = new QRadioButton(tr("range of keys"),this);
    rangeKeysRadio->setStatusTip(tr("random key signature from selecteed range."));
    rangeButGr = new QButtonGroup(this);
    rangeButGr->addButton(singleKeyRadio);
    rangeButGr->addButton(rangeKeysRadio);
    rangeLay->addWidget(singleKeyRadio,0,Qt::AlignCenter);
    rangeLay->addWidget(rangeKeysRadio,0,Qt::AlignCenter);
    QHBoxLayout *comboLay = new QHBoxLayout;
    fromKeyCombo = new TkeySignComboBox(this);
    toKeyCombo = new TkeySignComboBox(this);
    comboLay->addWidget(fromKeyCombo);
    QLabel *ll = new QLabel(" to ", this);
    comboLay->addWidget(ll);
    comboLay->addWidget(toKeyCombo);
    rangeLay->addLayout(comboLay);
    rangeLay->addStretch(1);
    keyInAnswerChB = new QCheckBox(tr("select a key signature manually"),this);
    keyInAnswerChB->setStatusTip(tr("if checked, user have to select a key signature,<br>otherwise it is shown by application."));
    rangeLay->addWidget(keyInAnswerChB,0,Qt::AlignCenter);
    keyLay->addLayout(rangeLay);
    keyLay->addStretch(1);


    keySignGr->setLayout(keyLay);

    mainLay->addWidget(keySignGr);

    forceAccChB = new QCheckBox(tr("force useing appropirate accidental"),this);
    forceAccChB->setStatusTip(tr("if checked, is possible to select a note<br>with given accidental only."));
    mainLay->addWidget(forceAccChB);

    setLayout(mainLay);

    connect(rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
}

void TasNoteWdg::keyRangeChanged() {
    if (singleKeyRadio->isChecked()) {
        toKeyCombo->setDisabled(true);
        keyInAnswerChB->setDisabled(true);
    }
    else {
        toKeyCombo->setDisabled(false);
        keyInAnswerChB->setDisabled(false);
    }
}


//############################# AS NOTE'S NAME  ###################################

TasNameWdg::TasNameWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    asNameGr = new TquestionAsWdg(this);
    mainLay->addWidget(asNameGr,1,Qt::AlignCenter);

    octaveRequiredChB = new QCheckBox(tr("require octave"),this);
    octaveRequiredChB->setStatusTip(tr("if checked, selecting of valid octave is required"));
    mainLay->addWidget(octaveRequiredChB,0,Qt::AlignCenter);
    styleRequiredChB = new QCheckBox(tr("use different nameing style"),this);
    styleRequiredChB->setStatusTip(tr("if checked, nameing style is switched between C D E and Do Re Mi.<br>It have to be checked if note's name is a question and an answer."));
    mainLay->addWidget(styleRequiredChB,0,Qt::AlignCenter);
    mainLay->addStretch(1);

    setLayout(mainLay);

    connect(asNameGr, SIGNAL(toggled(bool)), this, SLOT(disableStyleChBox()));
    connect(asNameGr, SIGNAL(answerStateChenged()), this, SLOT(disableStyleChBox()));

}

void TasNameWdg::disableStyleChBox() {
    if (asNameGr->answerAsName() && asNameGr->isChecked()) {
        styleRequiredChB->setChecked(true);
        styleRequiredChB->setDisabled(true);
    }
    else styleRequiredChB->setDisabled(false);
}

//############################# AS POSITION ON FINGEROARD ############################

TasFretPosWdg::TasFretPosWdg(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    asPosGr = new TquestionAsWdg(this);
    mainLay->addStretch(1);
    mainLay->addWidget(asPosGr,1,Qt::AlignCenter);
    mainLay->addStretch(1);

    setLayout(mainLay);
}
