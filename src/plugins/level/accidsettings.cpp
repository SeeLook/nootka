/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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
#include <exam/tlevel.h>
#include <QtWidgets/QtWidgets>


accidSettings::accidSettings(TlevelCreatorDlg* creator) :
  TabstractLevelPage(creator)
{
  QVBoxLayout *mainLay = new QVBoxLayout;
  mainLay->setAlignment(Qt::AlignHCenter);

  QVBoxLayout *accLay = new QVBoxLayout;
  m_sharpsChB = new QCheckBox(tr("# - sharps"),this);
  m_sharpsChB->setStatusTip(tr("Sharps will be used in questions and answers.<br>It has to be checked, if keys with sharps are used."));
  m_sharpsChB->setChecked(true);
  m_flatsChB = new QCheckBox(tr("b - flats"),this);
  m_flatsChB->setStatusTip(tr("Flats will be used in questions and answers.<br>It has to be checked, if keys with flats are used."));
  m_flatsChB->setChecked(true);
  m_doubleAccChB = new QCheckBox(tr("x, bb - double accidentals"),this);
  accLay->addWidget(m_sharpsChB);
#if defined (Q_OS_ANDROID)
  accLay->addWidget(getLabelFromStatus(m_sharpsChB, true, true));
#endif
  accLay->addWidget(m_flatsChB);
#if defined (Q_OS_ANDROID)
  accLay->addWidget(getLabelFromStatus(m_flatsChB, true, true));
#endif
  accLay->addWidget(m_doubleAccChB);
  m_accidGr = new QGroupBox(accidsText(), this);
  m_accidGr->setLayout(accLay);
  m_forceAccChB = new QCheckBox(tr("force using appropriate accidental"),this);
  m_forceAccChB->setStatusTip(tr("if checked, it is possible to select a note<br>with given accidental only."));

  mainLay->addStretch();
  mainLay->addWidget(m_accidGr);
  mainLay->addWidget(m_forceAccChB);
#if defined (Q_OS_ANDROID)
  mainLay->addWidget(getLabelFromStatus(m_forceAccChB, true, true));
#endif
  mainLay->addStretch();

  QHBoxLayout *keyLay = new QHBoxLayout;
  m_keySignGr = new QGroupBox(tr("use key signatures"),this);
  m_keySignGr->setCheckable(true);

  QVBoxLayout *rangeLay = new QVBoxLayout;
  rangeLay->setAlignment(Qt::AlignCenter);
  m_singleKeyRadio = new QRadioButton(tr("single key"),this);
  m_singleKeyRadio->setStatusTip(tr("only one, selected key signature for whole exam."));
  m_rangeKeysRadio = new QRadioButton(tr("range of keys"),this);
  m_rangeKeysRadio->setStatusTip(tr("random key signature from selected range."));
  m_rangeButGr = new QButtonGroup(this);
  m_rangeButGr->addButton(m_singleKeyRadio);
  m_rangeButGr->addButton(m_rangeKeysRadio);
  m_rangeKeysRadio->setChecked(true);
  rangeLay->addWidget(m_singleKeyRadio,0,Qt::AlignCenter);
#if defined (Q_OS_ANDROID)
  rangeLay->addWidget(getLabelFromStatus(m_singleKeyRadio, true, true));
#endif
  rangeLay->addWidget(m_rangeKeysRadio,0,Qt::AlignCenter);
#if defined (Q_OS_ANDROID)
  rangeLay->addWidget(getLabelFromStatus(m_rangeKeysRadio, true, true));
#endif
  QHBoxLayout *comboLay = new QHBoxLayout;
  m_fromKeyCombo = new TkeySignComboBox(this);
  m_fromKeyCombo->setStatusTip(tr("Select a key signature. Appropriate accidentals used in exam will be automatically selected!"));
  m_fromKeyCombo->setKeySignature(TkeySignature(0));
  m_toKeyCombo = new TkeySignComboBox(this);
  m_toKeyCombo->setStatusTip(m_fromKeyCombo->statusTip());
  m_fromKeyCombo->setKeySignature(TkeySignature(0));
  comboLay->addWidget(m_fromKeyCombo);
  QLabel *ll = new QLabel(" - ", this);
  comboLay->addWidget(ll);
  comboLay->addWidget(m_toKeyCombo);
  rangeLay->addLayout(comboLay);
#if defined (Q_OS_ANDROID)
  rangeLay->addWidget(getLabelFromStatus(m_fromKeyCombo, true, true));
  m_toKeyCombo->setStatusTip(QString());
#endif

  m_keyInAnswerChB = new QCheckBox(tr("select a key signature manually"),this);
  m_keyInAnswerChB->setStatusTip(tr("if checked, in exam user have to select a key signature, otherwise it is shown by application."));
  rangeLay->addWidget(m_keyInAnswerChB,0,Qt::AlignCenter);
#if defined (Q_OS_ANDROID)
  rangeLay->addWidget(getLabelFromStatus(m_keyInAnswerChB, true, true));
#endif
  keyLay->addLayout(rangeLay);
  keyLay->addStretch(1);

  m_keySignGr->setLayout(keyLay);
  m_keySignGr->setChecked(false);
  mainLay->addWidget(m_keySignGr, 0, Qt::AlignCenter);

  m_currKeySignChBox = new QCheckBox(tr("notes in current key signature only"),this);
  m_currKeySignChBox->setStatusTip(tr("Only notes from current key signature are taken. If key signature is disabled accidentals are not used."));
  mainLay->addWidget(m_currKeySignChBox, 0, Qt::AlignCenter);
#if defined (Q_OS_ANDROID)
  mainLay->addWidget(getLabelFromStatus(m_currKeySignChBox, true, true));
  mainLay->addSpacing(20);
#endif

  mainLay->addStretch();
  setLayout(mainLay);

  connect(m_rangeButGr, SIGNAL(buttonClicked(int)), this, SLOT(keyRangeChanged()));

  connect(m_fromKeyCombo, SIGNAL(activated(int)), this, SLOT(keySignChanged()));
  connect(m_toKeyCombo, SIGNAL(activated(int)), this, SLOT(keySignChanged()));
  connect(m_sharpsChB, SIGNAL(clicked()), this, SLOT(keySignChanged()));
  connect(m_flatsChB, SIGNAL(clicked()), this, SLOT(keySignChanged()));
  connect(m_keySignGr, SIGNAL(clicked()), this, SLOT(keySignChanged()));

  connect(m_forceAccChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
  connect(m_sharpsChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
  connect(m_flatsChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
  connect(m_doubleAccChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
  connect(m_keySignGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
  connect(m_singleKeyRadio, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
  connect(m_rangeKeysRadio, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
  connect(m_fromKeyCombo, SIGNAL(activated(int)), this, SLOT(whenParamsChanged()));
  connect(m_toKeyCombo, SIGNAL(activated(int)), this, SLOT(whenParamsChanged()));
  connect(m_keyInAnswerChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
  connect(m_currKeySignChBox, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
}


//#################### PUBLIC METHODS ######################


void accidSettings::loadLevel (Tlevel* level) {
  const QSignalBlocker blocker(this);
  m_sharpsChB->setChecked(level->withSharps);
  m_flatsChB->setChecked(level->withFlats);
  m_doubleAccChB->setChecked(level->withDblAcc);
  m_forceAccChB->setChecked(level->forceAccids);
  m_keySignGr->setChecked(level->useKeySign);
  if (level->isSingleKey)
      m_singleKeyRadio->setChecked(true);
  else
      m_rangeKeysRadio->setChecked(true);
  m_fromKeyCombo->setKeySignature(level->loKey);
  m_toKeyCombo->setKeySignature(level->hiKey);
  m_keyInAnswerChB->setChecked(level->manualKey);
  m_currKeySignChBox->setChecked(level->onlyCurrKey);
  keyRangeChanged();
  changed();
  saveLevel(workLevel());
}


void accidSettings::saveLevel (Tlevel* level) {
  if (m_accidGr->isEnabled()) {
    level->withSharps = m_sharpsChB->isChecked();
    level->withFlats = m_flatsChB->isChecked();
    level->withDblAcc = m_doubleAccChB->isChecked();
  } else { // ignore checking when group box is disabled
    level->withSharps = false;
    level->withFlats = false;
    level->withDblAcc = false;
  }
  level->forceAccids = m_forceAccChB->isEnabled() && m_forceAccChB->isChecked();
  if (m_keySignGr->isEnabled()) {
    level->useKeySign = m_keySignGr->isChecked();
    if (m_singleKeyRadio->isChecked()) {
        level->isSingleKey = true;
        level->loKey = m_fromKeyCombo->getKeySignature();
        level->hiKey = m_toKeyCombo->getKeySignature();
    }
    else { // range of keys
        level->isSingleKey = false;
        if (m_fromKeyCombo->getKeySignature().value() < m_toKeyCombo->getKeySignature().value()) {
            level->loKey = m_fromKeyCombo->getKeySignature();
            level->hiKey = m_toKeyCombo->getKeySignature();
        } else 
          if (m_fromKeyCombo->getKeySignature().value() > m_toKeyCombo->getKeySignature().value()) {
            level->loKey = m_toKeyCombo->getKeySignature();
            level->hiKey = m_fromKeyCombo->getKeySignature();
          } else { // == means only one key is selected
            level->isSingleKey = true;
            level->loKey = m_fromKeyCombo->getKeySignature();
            level->hiKey = m_toKeyCombo->getKeySignature();
            }
    }
    level->manualKey = m_keyInAnswerChB->isEnabled() && m_keyInAnswerChB->isChecked();
  } else {
    level->useKeySign = false;
    level->manualKey = false;
  }
  level->onlyCurrKey = m_currKeySignChBox->isChecked();
}

//#################### PUBLIC SLOTS #####################

void accidSettings::changed() {
  const QSignalBlocker blocker(this);
  if (workLevel()->canBeScore())
			enableKeys(true);
  else
			enableKeys(false);
  if ((workLevel()->answerIsNote() || workLevel()->answerIsName()) &&
				(workLevel()->withDblAcc || workLevel()->withFlats || workLevel()->withSharps))
			m_forceAccChB->setDisabled(false);
	else
			m_forceAccChB->setDisabled(true);
  m_keyInAnswerChB->setDisabled(!workLevel()->answerIsNote()); // disable manual key check box if no score as an answer
}



void accidSettings::enableAccids(bool enable) {
  if (enable) { // score and note names are enabled in the level
      m_accidGr->setDisabled(false);
      setStatusTip(QString());
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
  if (m_keySignGr->isChecked()) { // key signatures enabled
      if (m_singleKeyRadio->isChecked()) { // single key signature
          m_toKeyCombo->setKeySignature(TkeySignature(0)); // reset and disable hiKey of range
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
  const QSignalBlocker blocker(this);
	if (sender() == m_keySignGr) {
		if (m_keySignGr->isChecked()) {
			if (m_rangeKeysRadio->isChecked())
				m_toKeyCombo->setDisabled(false);
			else {
				m_toKeyCombo->setDisabled(true);
				m_toKeyCombo->setKeySignature(0);
			}
		}
	}
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
            } else { // no key - C-major
                m_flatsChB->setDisabled(false);
                m_sharpsChB->setDisabled(false);
            }
        }
    } else {
        m_flatsChB->setDisabled(false);
        m_sharpsChB->setDisabled(false);
    }
  if (!m_flatsChB->isChecked() && !m_sharpsChB->isChecked() && !m_doubleAccChB->isChecked()) { 
		// without accidentals those options makes no sense
			m_currKeySignChBox->setDisabled(true);
			m_forceAccChB->setDisabled(true);
			m_currKeySignChBox->setChecked(false);
			m_forceAccChB->setChecked(false);
	} else {
			m_currKeySignChBox->setDisabled(false);
			m_forceAccChB->setDisabled(false);
	}
  changedLocal();
}


void accidSettings::whenParamsChanged() {
	changedLocal();
}


void accidSettings::updateStatusTip() {
  setStatusTip("<b>" + tr("Elements are disabled because appropriate types of questions or answers are not selected.") + "</b>");
}



