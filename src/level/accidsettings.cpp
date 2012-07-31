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

accidSettings::accidSettings(QWidget* parent) :
  QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignHCenter);
    
    
    QVBoxLayout *accLay = new QVBoxLayout;
    m_sharpsChB = new QCheckBox(tr("# - sharps"),this);
    m_sharpsChB->setStatusTip(tr("Sharps will be uesd in exam's questions and answers.<br>It has to be checked, if keys with sharps are used."));
    m_flatsChB = new QCheckBox(tr("b - flats"),this);
    m_flatsChB->setStatusTip(tr("Flats will be uesd in exam's questions and answers.<br>It has to be checked, if keys with flats are used."));
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
    m_keySignGr = new QGroupBox(tr("use keys singature"),this);
    m_keySignGr->setCheckable(true);

    QVBoxLayout *rangeLay = new QVBoxLayout;
    rangeLay->setAlignment(Qt::AlignCenter);
    m_singleKeyRadio = new QRadioButton(tr("single key"),this);
    m_singleKeyRadio->setStatusTip(tr("only one, selected key signature<br>for whole exam."));
    m_rangeKeysRadio = new QRadioButton(tr("range of keys"),this);
    m_rangeKeysRadio->setStatusTip(tr("random key signature from selected range."));
    rangeButGr = new QButtonGroup(this);
    rangeButGr->addButton(m_singleKeyRadio);
    rangeButGr->addButton(m_rangeKeysRadio);
    rangeLay->addWidget(m_singleKeyRadio,0,Qt::AlignCenter);
    rangeLay->addWidget(m_rangeKeysRadio,0,Qt::AlignCenter);
    QHBoxLayout *comboLay = new QHBoxLayout;
    m_fromKeyCombo = new TkeySignComboBox(this);
    m_fromKeyCombo->setStatusTip(tr("Select a key signature.<br>Apropirate accidentals used in exam<br>will be automatically selected !"));
    m_toKeyCombo = new TkeySignComboBox(this);
    m_toKeyCombo->setStatusTip(m_fromKeyCombo->statusTip());
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
    mainLay->addWidget(m_keySignGr, 1, Qt::AlignCenter);

    setLayout(mainLay);    
    
    connect(rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));
    
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

void accidSettings::loadLevel ( TexamLevel& level ) {

}

void accidSettings::saveLevel ( TexamLevel& level ) {

}

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
        if (m_fromKeyCombo->getKeySignature().value() < 0 ||
            m_toKeyCombo->getKeySignature().value() < 0) {
            m_flatsChB->setChecked(true);
            m_flatsChB->setDisabled(true);
        }
        if (m_fromKeyCombo->getKeySignature().value() > 0 ||
            m_toKeyCombo->getKeySignature().value() > 0) {
            m_sharpsChB->setChecked(true);
            m_sharpsChB->setDisabled(true);
        }
        if (m_fromKeyCombo->getKeySignature().value() == 0 &&
            m_toKeyCombo->getKeySignature().value() == 0) {
            m_flatsChB->setDisabled(false);
            m_sharpsChB->setDisabled(false);
        }
      } else {
          if (m_fromKeyCombo->getKeySignature().value() < 0) {
            m_flatsChB->setChecked(true);
            m_flatsChB->setDisabled(true);
            m_sharpsChB->setDisabled(false);
        } else
            if (m_fromKeyCombo->getKeySignature().value() > 0) {
                m_sharpsChB->setChecked(true);
                m_sharpsChB->setDisabled(true);
                m_flatsChB->setDisabled(false);
            } else {
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

}




