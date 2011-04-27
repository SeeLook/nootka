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

extern bool isNotSaved;

questionsSettings::questionsSettings(QWidget *parent) :
    QWidget(parent)
{
    questAsToolBox = new QToolBox(this);
    QVBoxLayout *mainLay = new QVBoxLayout;
    QLabel *headLab = new QLabel("<b>"+QString(TquestionAsWdg::questionsTxt).toUpper() + "</b>", this);
    mainLay->addWidget(headLab, 0, Qt::AlignCenter);
    mainLay->addWidget(questAsToolBox);
    setLayout(mainLay);

    asNoteWdg = new TasNoteWdg();
    asNameWdg = new TasNameWdg();
    asFretPosWdg = new TasFretPosWdg();

    questAsToolBox->addItem(asNoteWdg, TquestionAsWdg::asNoteTxt);
    questAsToolBox->addItem(asNameWdg, TquestionAsWdg::asNameTxt);
    questAsToolBox->addItem(asFretPosWdg, TquestionAsWdg::asFretPosTxt);

    connect(asNoteWdg, SIGNAL(asNoteChanged()), this, SLOT(whenParamsChanged()));
    connect(asNameWdg, SIGNAL(asNameChanged()), this, SLOT(whenParamsChanged()));
    connect(asFretPosWdg, SIGNAL(asFretPosChanged()), this, SLOT(whenParamsChanged()));

}

void questionsSettings::loadLevel(TexamLevel level) {
    asNoteWdg->loadLevel(level);
    asNameWdg->loadLevel(level);
    asFretPosWdg->loadLevel(level);
}

void questionsSettings::whenParamsChanged() {
    emit questSettChanged();
}

void questionsSettings::saveLevel(TexamLevel &level) {
    asNoteWdg->saveLevel(level);
    asNameWdg->saveLevel(level);
    asFretPosWdg->saveLevel(level);
}

//############################# AS NOTE IN A SCORE ###################################

TasNoteWdg::TasNoteWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout *upperLay = new QHBoxLayout;
    asNoteGr = new TquestionAsWdg(this);
    asNoteGr->setTitle(TquestionAsWdg::questionTxt + " - " + TquestionAsWdg::asNoteTxt);
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
    QLabel *ll = new QLabel(" - ", this);
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

    connect(asNoteGr, SIGNAL(answerStateChenged()), this, SLOT(whenParamsChanged()));
    connect(asNoteGr, SIGNAL(toggled(bool)), this, SLOT(whenParamsChanged()));
    connect(accidGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(sharpsChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(flatsChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(doubleAccChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(keySignGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(singleKeyRadio, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(rangeKeysRadio, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(fromKeyCombo, SIGNAL(activated(int)), this, SLOT(whenParamsChanged()));
    connect(toKeyCombo, SIGNAL(activated(int)), this, SLOT(whenParamsChanged()));
    connect(keyInAnswerChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(forceAccChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));

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

void TasNoteWdg::loadLevel(TexamLevel level) {
    disconnect(rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
    asNoteGr->setChecked(level.questionAs.isNote());
    asNoteGr->setAnswers(level.answersAs[TQAtype::e_asNote]);
    if (level.withSharps || level.withFlats || level.withDblAcc)
        accidGr->setChecked(true);
    else
        accidGr->setChecked(false);
    sharpsChB->setChecked(level.withSharps);
    flatsChB->setChecked(level.withFlats);
    doubleAccChB->setChecked(level.withDblAcc);
    keySignGr->setChecked(level.useKeySign);
    if (level.isSingleKey)
        singleKeyRadio->setChecked(true);
    else
        rangeKeysRadio->setChecked(true);
    fromKeyCombo->setKeySignature(level.loKey);
    toKeyCombo->setKeySignature(level.hiKey);
    keyInAnswerChB->setChecked(level.manualKey);
    forceAccChB->setChecked(level.forceAccids);
    keyRangeChanged();
    connect(rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
}

void TasNoteWdg::whenParamsChanged() {
    if (!isNotSaved) {
        isNotSaved = true;
        emit asNoteChanged();
    }
}

void TasNoteWdg::saveLevel(TexamLevel &level) {
    level.questionAs.setAsNote(asNoteGr->isChecked());
    level.answersAs[TQAtype::e_asNote] = asNoteGr->getAnswers();
    level.withSharps = sharpsChB->isChecked();
    level.withFlats = flatsChB->isChecked();
    level.withDblAcc = doubleAccChB->isChecked();
    level.useKeySign = keySignGr->isChecked();
    if (singleKeyRadio->isChecked())
        level.isSingleKey = true;
    else
        level.isSingleKey = false;
    if (fromKeyCombo->getKeySignature().getKey() < toKeyCombo->getKeySignature().getKey()) {
        level.loKey = fromKeyCombo->getKeySignature();
        level.hiKey = toKeyCombo->getKeySignature();
    } else
        if (fromKeyCombo->getKeySignature().getKey() > toKeyCombo->getKeySignature().getKey()) {
            level.loKey = toKeyCombo->getKeySignature();
            level.hiKey = fromKeyCombo->getKeySignature();
        } else { // == means only one key is selected
            level.isSingleKey = true;
            level.loKey = fromKeyCombo->getKeySignature();
            level.hiKey = toKeyCombo->getKeySignature();
        }
    level.manualKey = keyInAnswerChB->isChecked();
    level.forceAccids = forceAccChB->isChecked();
}


//############################# AS NOTE'S NAME  ###################################

TasNameWdg::TasNameWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    asNameGr = new TquestionAsWdg(this);
    asNameGr->setTitle(TquestionAsWdg::questionTxt + " - " + TquestionAsWdg::asNameTxt);
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

    connect(asNameGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(asNameGr, SIGNAL(answerStateChenged()), this, SLOT(whenParamsChanged()));
    connect(octaveRequiredChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(styleRequiredChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));

}

void TasNameWdg::disableStyleChBox() {
    if (asNameGr->answerAsName() && asNameGr->isChecked()) {
        styleRequiredChB->setChecked(true);
        styleRequiredChB->setDisabled(true);
    }
    else styleRequiredChB->setDisabled(false);
}

void TasNameWdg::loadLevel(TexamLevel level) {
    asNameGr->setChecked(level.questionAs.isName());
    asNameGr->setAnswers(level.answersAs[TQAtype::e_asName]);
    octaveRequiredChB->setChecked(level.requireOctave);
    styleRequiredChB->setChecked(level.requireStyle);
    disableStyleChBox();
}

void TasNameWdg::whenParamsChanged() {
    if (!isNotSaved) {
        isNotSaved = true;
        emit asNameChanged();
    }
}

void TasNameWdg::saveLevel(TexamLevel &level) {
    level.questionAs.setAsName(asNameGr->isChecked());
    level.answersAs[TQAtype::e_asName] = asNameGr->getAnswers();
    level.requireOctave = octaveRequiredChB->isChecked();
    level.requireStyle = styleRequiredChB->isChecked();
}



//############################# AS POSITION ON FINGEROARD ############################

TasFretPosWdg::TasFretPosWdg(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    asPosGr = new TquestionAsWdg(this);
    asPosGr->setTitle(TquestionAsWdg::questionTxt + " - " + TquestionAsWdg::asFretPosTxt);
    mainLay->addStretch(1);
    mainLay->addWidget(asPosGr,1,Qt::AlignCenter);
    mainLay->addStretch(1);

    setLayout(mainLay);

    connect(asPosGr, SIGNAL(answerStateChenged()), this, SLOT(whenParamsChanged()));
    connect(asPosGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
}

void TasFretPosWdg::loadLevel(TexamLevel level) {
    asPosGr->setChecked(level.questionAs.isFret());
    asPosGr->setAnswers(level.answersAs[TQAtype::e_asFretPos]);
}

void TasFretPosWdg::whenParamsChanged() {
    if (!isNotSaved) {
        isNotSaved = true;
        emit asFretPosChanged();
    }
}

void TasFretPosWdg::saveLevel(TexamLevel &level) {
    level.questionAs.setAsFret(asPosGr->isChecked());
    level.answersAs[TQAtype::e_asFretPos] = asPosGr->getAnswers();
}
