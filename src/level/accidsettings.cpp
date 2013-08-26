/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#include "accidsettings.h"
#include "tkeysigncombobox.h"
#include "texamlevel.h"
#include "QtGui"

extern bool isNotSaved;

accidSettings::accidSettings(QWidget* parent) :
  QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignHCenter);
    
    
    QVBoxLayout *accLay = new QVBoxLayout;
    m_sharpsChB = new QCheckBox(tr("# - sharps"),this);
    m_sharpsChB->setStatusTip(tr("Sharps will be used in exam's questions and answers.<br>It has to be checked, if keys with sharps are used."));
    m_sharpsChB->setChecked(true);
    m_flatsChB = new QCheckBox(tr("b - flats"),this);
    m_flatsChB->setStatusTip(tr("Flats will be used in exam's questions and answers.<br>It has to be checked, if keys with flats are used."));
    m_flatsChB->setChecked(true);
    m_doubleAccChB = new QCheckBox(tr("x, bb - double accidentals"),this);
    accLay->addWidget(m_sharpsChB);
    accLay->addWidget(m_flatsChB);
    accLay->addWidget(m_doubleAccChB);
    accLay->addStretch(1);
    m_accidGr = new QGroupBox(accidsText(), this);
    m_accidGr->setStatusTip(tr("Accidentals used in exam."));
    m_accidGr->setLayout(accLay);
    mainLay->addWidget(m_accidGr, 1, Qt::AlignCenter);
    
    QHBoxLayout *keyLay = new QHBoxLayout;
    m_keySignGr = new QGroupBox(tr("use keys signature"),this);
    m_keySignGr->setCheckable(true);

    QVBoxLayout *rangeLay = new QVBoxLayout;
    rangeLay->setAlignment(Qt::AlignCenter);
    m_singleKeyRadio = new QRadioButton(tr("single key"),this);
    m_singleKeyRadio->setStatusTip(tr("only one, selected key signature<br>for whole exam."));
    m_rangeKeysRadio = new QRadioButton(tr("range of keys"),this);
    m_rangeKeysRadio->setStatusTip(tr("random key signature from selected range."));
    m_rangeButGr = new QButtonGroup(this);
    m_rangeButGr->addButton(m_singleKeyRadio);
    m_rangeButGr->addButton(m_rangeKeysRadio);
    m_rangeKeysRadio->setChecked(true);
    rangeLay->addWidget(m_singleKeyRadio,0,Qt::AlignCenter);
    rangeLay->addWidget(m_rangeKeysRadio,0,Qt::AlignCenter);
    QHBoxLayout *comboLay = new QHBoxLayout;
    m_fromKeyCombo = new TkeySignComboBox(this);
    m_fromKeyCombo->setStatusTip(tr("Select a key signature.<br>Appropriate accidentals used in exam<br>will be automatically selected !"));
    m_fromKeyCombo->setKeySignature(TkeySignature(0));
    m_toKeyCombo = new TkeySignComboBox(this);
    m_toKeyCombo->setStatusTip(m_fromKeyCombo->statusTip());
    m_fromKeyCombo->setKeySignature(TkeySignature(0));
    comboLay->addWidget(m_fromKeyCombo);
    QLabel *ll = new QLabel(" - ", this);
    comboLay->addWidget(ll);
    comboLay->addWidget(m_toKeyCombo);
    rangeLay->addLayout(comboLay);
    rangeLay->addStretch(1);
    m_keyInAnswerChB = new QCheckBox(tr("select a key signature manually"),this);
    m_keyInAnswerChB->setStatusTip(tr("if checked, in exam user have to select a key signature,<br>otherwise it is shown by application."));
    rangeLay->addWidget(m_keyInAnswerChB,0,Qt::AlignCenter);
    keyLay->addLayout(rangeLay);
    keyLay->addStretch(1);

    m_keySignGr->setLayout(keyLay);
    m_keySignGr->setChecked(false);
    mainLay->addWidget(m_keySignGr, 1, Qt::AlignCenter);

    setLayout(mainLay);    
    
    connect(m_rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged())); 
    
    connect(m_fromKeyCombo, SIGNAL(activated(int)), this, SLOT(keySignChanged()));
    connect(m_toKeyCombo, SIGNAL(activated(int)), this, SLOT(keySignChanged()));
    connect(m_sharpsChB, SIGNAL(clicked()), this, SLOT(keySignChanged()));
    connect(m_flatsChB, SIGNAL(clicked()), this, SLOT(keySignChanged()));
    connect(m_keySignGr, SIGNAL(clicked()), this, SLOT(keySignChanged()));

    connect(m_sharpsChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(m_flatsChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(m_doubleAccChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(m_keySignGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(m_singleKeyRadio, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(m_rangeKeysRadio, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(m_fromKeyCombo, SIGNAL(activated(int)), this, SLOT(whenParamsChanged()));
    connect(m_toKeyCombo, SIGNAL(activated(int)), this, SLOT(whenParamsChanged()));
    connect(m_keyInAnswerChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));

}


//#################### PUBLIC METHODS ######################


void accidSettings::loadLevel ( TexamLevel& level ) {
    disconnect(m_rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
    
    m_sharpsChB->setChecked(level.withSharps);
    m_flatsChB->setChecked(level.withFlats);
    m_doubleAccChB->setChecked(level.withDblAcc);
    m_keySignGr->setChecked(level.useKeySign);
    if (level.isSingleKey)
        m_singleKeyRadio->setChecked(true);
    else
        m_rangeKeysRadio->setChecked(true);
    m_fromKeyCombo->setKeySignature(level.loKey);
    m_toKeyCombo->setKeySignature(level.hiKey);
    m_keyInAnswerChB->setChecked(level.manualKey);
    keyRangeChanged();

    connect(m_rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
}

void accidSettings::saveLevel ( TexamLevel& level ) {
  if (m_accidGr->isEnabled()) {
    level.withSharps = m_sharpsChB->isChecked();
    level.withFlats = m_flatsChB->isChecked();
    level.withDblAcc = m_doubleAccChB->isChecked();
  } else { // ignore checking when groupbox is disabled
    level.withSharps = false;
    level.withFlats = false;
    level.withDblAcc = false;
  }
  if (m_keySignGr->isEnabled()) {
    level.useKeySign = m_keySignGr->isChecked();
    if (m_singleKeyRadio->isChecked()) {
        level.isSingleKey = true;
        level.loKey = m_fromKeyCombo->getKeySignature();
        level.hiKey = m_toKeyCombo->getKeySignature();
    }
    else { // range of keys
        level.isSingleKey = false;
        if (m_fromKeyCombo->getKeySignature().value() < m_toKeyCombo->getKeySignature().value()) {
            level.loKey = m_fromKeyCombo->getKeySignature();
            level.hiKey = m_toKeyCombo->getKeySignature();
        } else 
          if (m_fromKeyCombo->getKeySignature().value() > m_toKeyCombo->getKeySignature().value()) {
            level.loKey = m_toKeyCombo->getKeySignature();
            level.hiKey = m_fromKeyCombo->getKeySignature();
          } else { // == means only one key is selected
            level.isSingleKey = true;
            level.loKey = m_fromKeyCombo->getKeySignature();
            level.hiKey = m_toKeyCombo->getKeySignature();
            }
        }
    level.manualKey = m_keyInAnswerChB->isChecked();
  } else {
    level.useKeySign = false;
    level.manualKey = false;
  }
}

//#################### PUBLIC SLOTS #####################

void accidSettings::enableAccids(bool enable) {
    if (enable) { // score and note names are enabled in the level
       m_accidGr->setDisabled(false);
       setStatusTip("");
    } else {
      m_accidGr->setDisabled(true);
      updateStatusTip();
    }
}

void accidSettings::enableKeys(bool enable) {
    if (enable) { // score is enabled in the level
       m_keySignGr->setDisabled(false);
       if (m_accidGr->isEnabled())
          setStatusTip("");
    } else {
      m_keySignGr->setDisabled(true);
      updateStatusTip();
    }
}


//#################### PRIVATE METHODS #####################


void accidSettings::keyRangeChanged() {
    if (m_keySignGr->isChecked()) {
        if (m_singleKeyRadio->isChecked()) {
            m_toKeyCombo->setKeySignature(TkeySignature(0));
            m_toKeyCombo->setDisabled(true);
            m_keyInAnswerChB->setDisabled(true);
            m_keyInAnswerChB->setChecked(false);
        }
        else {
            m_toKeyCombo->setDisabled(false);
            m_keyInAnswerChB->setDisabled(false);
        }
    }
    keySignChanged();
}

void accidSettings::keySignChanged() {
    if (m_keySignGr->isChecked()) {
      if (m_rangeKeysRadio->isChecked()) {
        if (m_fromKeyCombo->getKeySignature().value() < 0 || // keys with flats ?
            m_toKeyCombo->getKeySignature().value() < 0) {
            m_flatsChB->setChecked(true);
            m_flatsChB->setDisabled(true);
            if (m_fromKeyCombo->getKeySignature().value() > 0 || // keys with sharps also ?
            m_toKeyCombo->getKeySignature().value() > 0) {
              m_sharpsChB->setChecked(true);
              m_sharpsChB->setDisabled(true);
            } else { // no sharps so unlock
              m_sharpsChB->setDisabled(false);
            }
        }
        if (m_fromKeyCombo->getKeySignature().value() > 0 || // keys with sharps
            m_toKeyCombo->getKeySignature().value() > 0) {
            m_sharpsChB->setChecked(true);
            m_sharpsChB->setDisabled(true);
            if (m_fromKeyCombo->getKeySignature().value() < 0 || // keys with flats also ?
            m_toKeyCombo->getKeySignature().value() < 0) {
              m_flatsChB->setChecked(true);
              m_flatsChB->setDisabled(true);
            } else {
              m_flatsChB->setDisabled(false);
            }
        }
        if (m_fromKeyCombo->getKeySignature().value() == 0 &&
            m_toKeyCombo->getKeySignature().value() == 0) {
            m_flatsChB->setDisabled(false);
            m_sharpsChB->setDisabled(false);
        }
      } else { // single key, so ignore m_toKeyCombo value
          if (m_fromKeyCombo->getKeySignature().value() < 0) { // flat key
            m_flatsChB->setChecked(true);
            m_flatsChB->setDisabled(true);
            m_sharpsChB->setDisabled(false);
        } else
            if (m_fromKeyCombo->getKeySignature().value() > 0) { // sharp key
                m_sharpsChB->setChecked(true);
                m_sharpsChB->setDisabled(true);
                m_flatsChB->setDisabled(false);
            } else { // no key - C-majpr
                m_flatsChB->setDisabled(false);
                m_sharpsChB->setDisabled(false);
            }
        }
    } else {
        m_flatsChB->setDisabled(false);
        m_sharpsChB->setDisabled(false);
    }

}

void accidSettings::whenParamsChanged() {
    if (!isNotSaved) {
        isNotSaved = true;
        emit accidsChanged();
    }
}

void accidSettings::updateStatusTip() {
		setStatusTip("<b>" + tr("Elements are disabled because appropriate types of questions or answers are not selected.") 
        + "</b>");
}



