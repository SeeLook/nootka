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

TlevelCreatorItem::TlevelCreatorItem(QQuickItem* parent) :
  QQuickItem(parent)
{
  m_level = new Tlevel();
  m_level->name.clear();
  m_level->desc.clear();
  m_title = tr("Level creator");
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

