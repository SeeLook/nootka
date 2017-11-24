/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#include "tlevelcreatoritem.h"
#include "tlevelselector.h"
#include "tlevelpreviewitem.h"
#include <exam/tlevel.h>
#include <tglobals.h>

#include <QtCore/qdebug.h>
#include "checktime.h"


TlevelCreatorItem::TlevelCreatorItem(QQuickItem* parent) :
  QQuickItem(parent)
{
  m_level = new Tlevel();
  m_level->name.clear();
  m_level->desc.clear();
  m_title = tr("Level creator");
  m_answersList << m_level->answersAs[0].value() << m_level->answersAs[1].value() << m_level->answersAs[2].value() << m_level->answersAs[3].value();
}


TlevelCreatorItem::~TlevelCreatorItem()
{
  delete m_level;
}


void TlevelCreatorItem::setSelector(TlevelSelector* sel) {
  if (sel != m_selector) {
    m_selector = sel;
    if (m_selector)
      connect(m_selector, &TlevelSelector::levelChanged, this, &TlevelCreatorItem::whenLevelChanged);
  }
}


QString TlevelCreatorItem::title() const { return m_title + m_titleExtension; }

bool TlevelCreatorItem::notSaved() const { return !m_titleExtension.isEmpty(); }

int TlevelCreatorItem::questionAs() const { return m_level->questionAs.value(); }
void TlevelCreatorItem::setQuestionAs(int qAs) {
CHECKTIME (
  bool prevState = m_level->questionAs.isNote();
  bool doEmit = false;
  m_level->questionAs.setAsNote(qAs & 1);
  if (m_level->questionAs.isNote() != prevState) {
    doEmit = true;
    if (m_level->questionAs.isNote()) { // question checkbox was set, so select all answers
        m_level->answersAs[0].setValue(15);
        m_answersList[0] = 15;
    } else { // question was un-set, switch off all answers
        m_level->answersAs[0].setValue(0);
        m_answersList[0] = 0;
    }
  }
  prevState = m_level->questionAs.isName();
  m_level->questionAs.setAsName(qAs & 2);
  if (m_level->questionAs.isName() != prevState) {
    doEmit = true;
    if (m_level->questionAs.isName()) { // question checkbox was set, so select all answers
        m_level->answersAs[1].setValue(15);
        m_answersList[1] = 15;
    } else { // question was un-set, switch off all answers
        m_level->answersAs[1].setValue(0);
        m_answersList[1] = 0;
    }
  }
  prevState = m_level->questionAs.isFret();
  m_level->questionAs.setAsFret(qAs & 4);
  if (m_level->questionAs.isFret() != prevState) {
    doEmit = true;
    if (m_level->questionAs.isFret()) { // question checkbox was set, so select all answers
        m_level->answersAs[2].setValue(15);
        m_answersList[2] = 15;
    } else { // question was un-set, switch off all answers
        m_level->answersAs[2].setValue(0);
        m_answersList[2] = 0;
    }
  }
  prevState = m_level->questionAs.isSound();
  m_level->questionAs.setAsSound(qAs & 8);
  if (m_level->questionAs.isSound() != prevState) {
    doEmit = true;
    if (m_level->questionAs.isSound()) { // question checkbox was set, so select all answers
        m_level->answersAs[3].setValue(15);
        m_answersList[3] = 15;
    } else { // question was un-set, switch off all answers
        m_level->answersAs[3].setValue(0);
        m_answersList[3] = 0;
    }
  }
  levelParamChanged();
  if (doEmit)
    emit updateLevel();
)
}


void TlevelCreatorItem::setAnswersAs(QList<int> aAs) {
  for (int a = 0; a < 4; ++a) {
    m_level->answersAs[a].setValue(aAs[a]);
    switch (a) {
      case 0: m_level->questionAs.setAsNote(aAs[a] != 0); break;
      case 1: m_level->questionAs.setAsName(aAs[a] != 0); break;
      case 2: m_level->questionAs.setAsFret(aAs[a] != 0); break;
      case 3: m_level->questionAs.setAsSound(aAs[a] != 0); break;
    }
  }
  emit updateLevel();
  levelParamChanged();
}


void TlevelCreatorItem::setAnswers(int questionType, int answersValue) {
CHECKTIME (
  if (questionType >= 0 && questionType < 4) {
    m_level->answersAs[questionType].setValue(answersValue);
    m_answersList[questionType] = answersValue;
    switch (questionType) {
      case 0: m_level->questionAs.setAsNote(answersValue != 0); break;
      case 1: m_level->questionAs.setAsName(answersValue != 0); break;
      case 2: m_level->questionAs.setAsFret(answersValue != 0); break;
      case 3: m_level->questionAs.setAsSound(answersValue != 0); break;
    }
    levelParamChanged();
    updateLevel();
  }
)
}



int TlevelCreatorItem::loFret() const { return static_cast<int>(m_level->loFret); }
void TlevelCreatorItem::setLoFret(int lf) {
  m_level->loFret = static_cast<char>(lf);
  levelParamChanged();
}

int TlevelCreatorItem::hiFret() const { return static_cast<int>(m_level->hiFret); }
void TlevelCreatorItem::setHiFret(int hf) {
  m_level->hiFret = static_cast<char>(hf);
  levelParamChanged();
}

Tnote TlevelCreatorItem::loNote() const { return m_level->loNote; }
void TlevelCreatorItem::setLoNote(const Tnote& n) {
  m_level->loNote = n;
  levelParamChanged();
}

Tnote TlevelCreatorItem::hiNote() const { return m_level->hiNote; }
void TlevelCreatorItem::setHiNote(const Tnote& n) {
  m_level->hiNote = n;
  levelParamChanged();
}

int TlevelCreatorItem::clef() const { return static_cast<int>(m_level->clef.type()); }
void TlevelCreatorItem::setClef(int c) {
  m_level->clef.setClef(static_cast<Tclef::EclefType>(c));
  levelParamChanged();
}

int TlevelCreatorItem::usedStrings() const {
  return (m_level->usedStrings[0] ? 1 : 0) + (m_level->usedStrings[1] ? 2 : 0) + (m_level->usedStrings[2] ? 4 : 0)
          + (m_level->usedStrings[3] ? 8 : 0) + (m_level->usedStrings[4] ? 16 : 0) + (m_level->usedStrings[5] ? 32 : 0);
}


void TlevelCreatorItem::setUsedStrings(int uStr) {
  for (int s = 0; s < 6; ++s)
    m_level->usedStrings[s] = (uStr & qRound(qPow(2.0, static_cast<qreal>(s)))) > 0;
  levelParamChanged();
}


bool TlevelCreatorItem::withSharps() const { return m_level->withSharps; }
void TlevelCreatorItem::setWithSharps(bool sharps) {
  m_level->withSharps = sharps;
  levelParamChanged();
}

bool TlevelCreatorItem::withFlats() const { return m_level->withFlats; }
void TlevelCreatorItem::setWithFlats(bool flats) {
  m_level->withFlats = flats;
  levelParamChanged();
}

bool TlevelCreatorItem::withDblAccids() const { return m_level->withDblAcc; }
void TlevelCreatorItem::setWithDblAccids(bool dblAccids) {
  m_level->withDblAcc = dblAccids;
  levelParamChanged();
}

bool TlevelCreatorItem::forceAccids() const { return m_level->forceAccids; }
void TlevelCreatorItem::setForceAccids(bool fa) {
  m_level->forceAccids = fa;
  levelParamChanged();
}

bool TlevelCreatorItem::useKeySign() const { return m_level->useKeySign; }
void TlevelCreatorItem::setUseKeySign(bool useKeys) {
  m_level->useKeySign = useKeys;
  levelParamChanged();
}

bool TlevelCreatorItem::isSingleKey() const { return m_level->isSingleKey; }
void TlevelCreatorItem::setIsSingleKey(bool isSingle) {
  m_level->isSingleKey = isSingle;
  levelParamChanged();
}

int TlevelCreatorItem::loKey() const { return static_cast<int>(m_level->loKey.value()); }
void TlevelCreatorItem::setLoKey(int k) {
  m_level->loKey = TkeySignature(static_cast<char>(k));
  levelParamChanged();
}

int TlevelCreatorItem::hiKey() const { return static_cast<int>(m_level->hiKey.value()); }
void TlevelCreatorItem::setHiKey(int k) {
  m_level->hiKey = TkeySignature(static_cast<char>(k));
  levelParamChanged();
}

bool TlevelCreatorItem::onlyCurrKey() const { return m_level->onlyCurrKey; }
void TlevelCreatorItem::setOnlyCurrKey(bool only) {
  m_level->onlyCurrKey = only;
  levelParamChanged();
}

bool TlevelCreatorItem::manualKey() const { return m_level->manualKey; }
void TlevelCreatorItem::setManualKey(bool manual) {
  m_level->manualKey = manual;
  levelParamChanged();
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TlevelCreatorItem::whenLevelChanged() {
  qDebug() << "[TlevelCreatorItem] level was changed";
  *m_level = *m_selector->currentLevel();
  m_answersList.clear();
  m_answersList << m_level->answersAs[0].value() << m_level->answersAs[1].value() << m_level->answersAs[2].value() << m_level->answersAs[3].value();
  emit updateLevel();
  if (!m_titleExtension.isEmpty()) {
    m_titleExtension.clear();
    emit saveStateChanged();
    // TODO: Warn about unsaved changes
  }
}

//#################################################################################################
//#####################              PRIVATE           ############################################
//#################################################################################################

void TlevelCreatorItem::levelParamChanged() {
  if (m_titleExtension.isEmpty()) {
    m_titleExtension = QLatin1String("  (") + tr("level not saved!") + QLatin1String(")");
    emit saveStateChanged();
  }
  m_selector->levelPreview()->setLevel(m_level);
}

