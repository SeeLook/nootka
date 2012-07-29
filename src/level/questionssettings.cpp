/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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


#include "questionssettings.h"
#include <QtGui>

extern bool isNotSaved;

questionsSettings::questionsSettings(QWidget *parent) :
    QWidget(parent)
{
//     questAsToolBox = new QToolBox(this);
    QVBoxLayout *mainLay = new QVBoxLayout;    

    QGridLayout *qaLay = new QGridLayout();
    
    QLabel *qLab = new QLabel(TquestionAsWdg::questionsTxt().toUpper(), this);
    qaLay->addWidget(qLab, 0, 0);
    QLabel *aLab = new QLabel(TquestionAsWdg::answersTxt().toUpper(), this);
    qaLay->addWidget(aLab, 0, 1);
    
    
    TquestionAsWdg *asNoteWdg = new TquestionAsWdg(TquestionAsWdg::asNoteTxt(), qaLay, 1, this);
    
    TquestionAsWdg *asNameWdg = new TquestionAsWdg(TquestionAsWdg::asNameTxt(), qaLay, 2, this);
    
    TquestionAsWdg *asFretPosWdg = new TquestionAsWdg(TquestionAsWdg::asFretPosTxt(), qaLay, 3, this);
    
    TquestionAsWdg *asSoundWdg = new TquestionAsWdg(TquestionAsWdg::asSoundTxt(), qaLay, 4, this);
    
    mainLay->addLayout(qaLay);
    setLayout(mainLay);

//     asNoteWdg = new TasNoteWdg();
//     asNameWdg = new TasNameWdg();
//     asFretPosWdg = new TasFretPosWdg();
//     asPlayedSound = new TasPlayedSound();

//     questAsToolBox->addItem(asNoteWdg, TquestionAsWdg::asNoteTxt());
//     questAsToolBox->addItem(asNameWdg, TquestionAsWdg::asNameTxt());
//     questAsToolBox->addItem(asFretPosWdg, TquestionAsWdg::asFretPosTxt());
//     questAsToolBox->addItem(asPlayedSound, TquestionAsWdg::asSoundTxt());

//     connect(asNoteWdg, SIGNAL(asNoteChanged()), this, SLOT(whenParamsChanged()));
//     connect(asNameWdg, SIGNAL(asNameChanged()), this, SLOT(whenParamsChanged()));
//     connect(asFretPosWdg, SIGNAL(asFretPosChanged()), this, SLOT(whenParamsChanged()));
//     connect(asPlayedSound, SIGNAL(asPlayedSoundChanged()), this, SLOT(whenParamsChanged()));
//     
//     connect(asNameWdg->octaveRequiredChB, SIGNAL(clicked(bool)), asPlayedSound, SLOT(checkOctaveChB(bool)));
//     connect(asPlayedSound->octaveRequiredChB, SIGNAL(clicked(bool)), asNameWdg, SLOT(checkOctaveChB(bool)));

}

void questionsSettings::loadLevel(TexamLevel level) {
//     asNoteWdg->loadLevel(level);
//     asNameWdg->loadLevel(level);
//     asFretPosWdg->loadLevel(level);
//     asPlayedSound->loadLevel(level);
}

void questionsSettings::whenParamsChanged() {
    emit questSettChanged();
}

void questionsSettings::saveLevel(TexamLevel &level) {
//     asNoteWdg->saveLevel(level);
//     asNameWdg->saveLevel(level);
//     asFretPosWdg->saveLevel(level);
//     asPlayedSound->saveLevel(level);
}

//############################# AS NOTE IN A SCORE ###################################
/*
TasNoteWdg::TasNoteWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout *upperLay = new QHBoxLayout;
    asNoteGr = new TquestionAsWdg(this);
    asNoteGr->setTitle(TquestionAsWdg::questionTxt() + " - " + TquestionAsWdg::asNoteTxt());
    upperLay->addWidget(asNoteGr);

    QVBoxLayout *accLay = new QVBoxLayout;
    sharpsChB = new QCheckBox(tr("# - sharps"),this);
    sharpsChB->setStatusTip(tr("Sharps will be uesd in exam's questions and answers.<br>It has to be checked, if keys with sharps are used."));
    flatsChB = new QCheckBox(tr("b - flats"),this);
    flatsChB->setStatusTip(tr("Flats will be uesd in exam's questions and answers.<br>It has to be checked, if keys with flats are used."));
    doubleAccChB = new QCheckBox(tr("x, bb - double accidentals"),this);
    accLay->addWidget(sharpsChB);
    accLay->addWidget(flatsChB);
    accLay->addWidget(doubleAccChB);
    accLay->addStretch(1);
    accidGr = new QGroupBox(tr("accidentals"),this);
    accidGr->setStatusTip(tr("Accidentals used in exam."));
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
    rangeKeysRadio->setStatusTip(tr("random key signature from selected range."));
    rangeButGr = new QButtonGroup(this);
    rangeButGr->addButton(singleKeyRadio);
    rangeButGr->addButton(rangeKeysRadio);
    rangeLay->addWidget(singleKeyRadio,0,Qt::AlignCenter);
    rangeLay->addWidget(rangeKeysRadio,0,Qt::AlignCenter);
    QHBoxLayout *comboLay = new QHBoxLayout;
    fromKeyCombo = new TkeySignComboBox(this);
    fromKeyCombo->setStatusTip(tr("Select a key signature.<br>Apropirate accidentals used in exam<br>will be automatically selected !"));
    toKeyCombo = new TkeySignComboBox(this);
    toKeyCombo->setStatusTip(fromKeyCombo->statusTip());
    comboLay->addWidget(fromKeyCombo);
    QLabel *ll = new QLabel(" - ", this);
    comboLay->addWidget(ll);
    comboLay->addWidget(toKeyCombo);
    rangeLay->addLayout(comboLay);
    rangeLay->addStretch(1);
    keyInAnswerChB = new QCheckBox(tr("select a key signature manually"),this);
    keyInAnswerChB->setStatusTip(tr("if checked, in exam user have to select a key signature,<br>otherwise it is shown by application."));
    rangeLay->addWidget(keyInAnswerChB,0,Qt::AlignCenter);
    keyLay->addLayout(rangeLay);
    keyLay->addStretch(1);


    keySignGr->setLayout(keyLay);

    mainLay->addWidget(keySignGr);

    setLayout(mainLay);

    connect(rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
    
    connect(fromKeyCombo, SIGNAL(activated(int)), this, SLOT(keySignChanged()));
    connect(toKeyCombo, SIGNAL(activated(int)), this, SLOT(keySignChanged()));
    connect(sharpsChB, SIGNAL(clicked()), this, SLOT(keySignChanged()));
    connect(flatsChB, SIGNAL(clicked()), this, SLOT(keySignChanged()));
    connect(keySignGr, SIGNAL(clicked()), this, SLOT(keySignChanged()));

    connect(asNoteGr, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
    connect(asNoteGr, SIGNAL(toggled(bool)), this, SLOT(whenParamsChanged()));
    connect(sharpsChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(flatsChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(doubleAccChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(keySignGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(singleKeyRadio, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(rangeKeysRadio, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(fromKeyCombo, SIGNAL(activated(int)), this, SLOT(whenParamsChanged()));
    connect(toKeyCombo, SIGNAL(activated(int)), this, SLOT(whenParamsChanged()));
    connect(keyInAnswerChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));

}

void TasNoteWdg::keyRangeChanged() {
    if (keySignGr->isChecked()) {
        if (singleKeyRadio->isChecked()) {
            toKeyCombo->setKeySignature(TkeySignature(0));
            toKeyCombo->setDisabled(true);
            keyInAnswerChB->setDisabled(true);
            keyInAnswerChB->setChecked(false);
        }
        else {
            toKeyCombo->setDisabled(false);
            keyInAnswerChB->setDisabled(false);
        }
    }
    keySignChanged();
}

void TasNoteWdg::loadLevel(TexamLevel level) {
    disconnect(rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
    disconnect(asNoteGr, SIGNAL(toggled(bool)), this, SLOT(whenParamsChanged()));
    asNoteGr->setChecked(level.questionAs.isNote());
    asNoteGr->setAnswers(level.answersAs[TQAtype::e_asNote]);
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
    keyRangeChanged();
//    keySignChanged();
    connect(rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
    connect(asNoteGr, SIGNAL(toggled(bool)), this, SLOT(whenParamsChanged()));
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
    if (singleKeyRadio->isChecked()) {
        level.isSingleKey = true;
        level.loKey = fromKeyCombo->getKeySignature();
        level.hiKey = toKeyCombo->getKeySignature();
    }
    else {
        level.isSingleKey = false;
        if (fromKeyCombo->getKeySignature().value() < toKeyCombo->getKeySignature().value()) {
            level.loKey = fromKeyCombo->getKeySignature();
            level.hiKey = toKeyCombo->getKeySignature();
        } else
          if (fromKeyCombo->getKeySignature().value() > toKeyCombo->getKeySignature().value()) {
            level.loKey = toKeyCombo->getKeySignature();
            level.hiKey = fromKeyCombo->getKeySignature();
          } else { // == means only one key is selected
            level.isSingleKey = true;
            level.loKey = fromKeyCombo->getKeySignature();
            level.hiKey = toKeyCombo->getKeySignature();
            }
        }
    level.manualKey = keyInAnswerChB->isChecked();
}

void TasNoteWdg::keySignChanged() {
    if (keySignGr->isChecked()) {
      if (rangeKeysRadio->isChecked()) {
        if (fromKeyCombo->getKeySignature().value() < 0 ||
            toKeyCombo->getKeySignature().value() < 0) {
            flatsChB->setChecked(true);
            flatsChB->setDisabled(true);
        }
        if (fromKeyCombo->getKeySignature().value() > 0 ||
            toKeyCombo->getKeySignature().value() > 0) {
            sharpsChB->setChecked(true);
            sharpsChB->setDisabled(true);
        }
        if (fromKeyCombo->getKeySignature().value() == 0 &&
            toKeyCombo->getKeySignature().value() == 0) {
            flatsChB->setDisabled(false);
            sharpsChB->setDisabled(false);
        }
      } else {
          if (fromKeyCombo->getKeySignature().value() < 0) {
            flatsChB->setChecked(true);
            flatsChB->setDisabled(true);
            sharpsChB->setDisabled(false);
        } else
            if (fromKeyCombo->getKeySignature().value() > 0) {
                sharpsChB->setChecked(true);
                sharpsChB->setDisabled(true);
                flatsChB->setDisabled(false);
            } else {
                flatsChB->setDisabled(false);
                sharpsChB->setDisabled(false);
            }
        }
    } else {
        flatsChB->setDisabled(false);
        sharpsChB->setDisabled(false);
    }

}
/*
//############################# AS NOTE'S NAME  ###################################

TasNameWdg::TasNameWdg(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    asNameGr = new TquestionAsWdg(this);
    asNameGr->setTitle(TquestionAsWdg::questionTxt() + " - " + TquestionAsWdg::asNameTxt());
    mainLay->addWidget(asNameGr,1,Qt::AlignCenter);

    octaveRequiredChB = new QCheckBox(tr("require octave"),this);
    octaveRequiredChB->setStatusTip(tr("if checked, selecting of valid octave is required"));
    mainLay->addWidget(octaveRequiredChB,0,Qt::AlignCenter);
    styleRequiredChB = new QCheckBox(tr("use different nameing styles"),this);
    styleRequiredChB->setStatusTip(tr("if checked, nameing style is switched between letters and solfge.<br>It has to be checked if note's name is a question and an answer."));
    mainLay->addWidget(styleRequiredChB,0,Qt::AlignCenter);
    mainLay->addStretch(1);

    setLayout(mainLay);

    connect(asNameGr, SIGNAL(clicked()), this, SLOT(disableStyleChBox()));
    connect(asNameGr, SIGNAL(answerStateChanged()), this, SLOT(disableStyleChBox()));

    connect(asNameGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(asNameGr, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
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

/** @param octaveRequiredChB QCheckBox is connected with the same checkBox
 * in TasPlayedSound widget. It may be confused for user but I doesn't invent 
 * nothing better. So ignoring octeve can work only:
 * when answer is note name or answer is played sound. 
void TasNameWdg::checkOctaveChB(bool check) {
    octaveRequiredChB->setChecked(check);
}


//############################# AS POSITION ON FINGEROARD ############################

TasFretPosWdg::TasFretPosWdg(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    asPosGr = new TquestionAsWdg(this);
    asPosGr->setTitle(TquestionAsWdg::questionTxt() + " - " + TquestionAsWdg::asFretPosTxt());
    mainLay->addStretch(1);
    mainLay->addWidget(asPosGr,1,Qt::AlignCenter);
    mainLay->addStretch(1);
    asPosGr->asFretPosChB->setDisabled(true);
    asPosGr->asFretPosChB->setStatusTip(asPosGr->asSoundChB->statusTip());// not implemented

    forceAccChB = new QCheckBox(tr("force useing appropirate accidental"),this);
    forceAccChB->setStatusTip(tr("if checked, is possible to select a note<br>with given accidental only."));
    mainLay->addWidget(forceAccChB);

    mainLay->addStretch(1);
    showStrNrChB = new QCheckBox(tr("show string number in questions"), this);
    showStrNrChB->setStatusTip(tr("Shows on which string an answer has to be given.<br>Be careful, when it is needed and when it has no sense"));
    mainLay->addWidget(showStrNrChB);
    mainLay->addStretch(1);

    setLayout(mainLay);

    connect(asPosGr, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
    connect(asPosGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(forceAccChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(showStrNrChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
}

void TasFretPosWdg::loadLevel(TexamLevel level) {
    asPosGr->setChecked(level.questionAs.isFret());
    asPosGr->setAnswers(level.answersAs[TQAtype::e_asFretPos]);
    forceAccChB->setChecked(level.forceAccids);
    showStrNrChB->setChecked(level.showStrNr);
}

void TasFretPosWdg::whenParamsChanged() {
    asPosGr->asFretPosChB->setChecked(false);
    /** @todo remove this when asfretPos as answer & question
                                               will be implement 
    if (!isNotSaved) {
        isNotSaved = true;
        emit asFretPosChanged();
    }
}

void TasFretPosWdg::saveLevel(TexamLevel &level) {
    level.questionAs.setAsFret(asPosGr->isChecked());
    level.answersAs[TQAtype::e_asFretPos] = asPosGr->getAnswers();
    level.forceAccids = forceAccChB->isChecked();
    level.showStrNr = showStrNrChB->isChecked();
}


//############################# AS PLAYED SOUND ############################

TasPlayedSound::TasPlayedSound(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    asSoundGr = new TquestionAsWdg(this);
    asSoundGr->setTitle(TquestionAsWdg::questionTxt() + " - " + TquestionAsWdg::asSoundTxt());
    mainLay->addStretch(1);
    mainLay->addWidget(asSoundGr, 1, Qt::AlignCenter);
    octaveRequiredChB = new QCheckBox(tr("require octave"), this);
    octaveRequiredChB->setStatusTip(tr("if checked, playing or singing in valid octave is required"));
    mainLay->addWidget(octaveRequiredChB,0,Qt::AlignCenter);
    mainLay->addStretch(1);

    setLayout(mainLay);

    connect(octaveRequiredChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(asSoundGr, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
    connect(asSoundGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
}

void TasPlayedSound::loadLevel(TexamLevel level) {
    asSoundGr->setChecked(level.questionAs.isSound());
    asSoundGr->setAnswers(level.answersAs[TQAtype::e_asSound]);
    octaveRequiredChB->setChecked(level.requireOctave);
}

void TasPlayedSound::whenParamsChanged() {
    if (!isNotSaved) {
        isNotSaved = true;
        emit asPlayedSoundChanged();;
    }
}

void TasPlayedSound::saveLevel(TexamLevel &level) {
    level.questionAs.setAsSound(asSoundGr->isChecked());
    level.answersAs[TQAtype::e_asSound] = asSoundGr->getAnswers();
}

void TasPlayedSound::checkOctaveChB(bool check) {
    octaveRequiredChB->setChecked(check);
}
*/
