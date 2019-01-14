/***************************************************************************
 *   Copyright (C) 2017-2019 by Tomasz Bojczuk                             *
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

#include "tpianobg.h"
#include "tpath.h"
#include "tglobals.h"

#include <QtGui/qpainter.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmath.h>
#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>
#include "checktime.h"


TpianoBg::TpianoBg(QQuickItem* parent) :
  TcommonInstrument(parent),
  m_keyWidth(32.0),
  m_firstOctave(-3)
{
}


TpianoBg::~TpianoBg() {}


void TpianoBg::setKeyWidth(qreal kw) {
  if (m_keyWidth != kw) {
    m_keyWidth = kw;
    calculateMetrics(width());
  }
}


void TpianoBg::setNote(const Tnote& n, quint32 noteDataValue) {
  Q_UNUSED(noteDataValue)
  if (!p_note.compareNotes(n)) {
    int keyNr = -1;
    bool isWhite = true;
    if (n.isValid()) {
      Tnote sharpNote = Tnote(n.chromatic());
      int o = sharpNote.octave() - m_firstOctave;
      isWhite = sharpNote.alter() == 0;
      keyNr = o * 7 + sharpNote.note() - (isWhite ? 1 : 0);
    }
    p_note = n;
    emit wantKeyToSelect(keyNr, isWhite);
  }
}


void TpianoBg::askQuestion(const Tnote& n, quint32 noteDataValue) {
  Q_UNUSED(noteDataValue)
  setNote(n);
}


void TpianoBg::setFirstOctave(int firstO) {
  auto fo = static_cast<char>(firstO);
  if (fo != m_firstOctave) {
    m_firstOctave = fo;
    emit firstOctaveChanged();
  }
}


void TpianoBg::setReadOnly(bool ro) {
  if (ro != m_readOnly) {
    m_readOnly = ro;
    emit readOnlyChanged();
  }
}


void TpianoBg::markSelected(const QColor& markColor) {
  int borderWidth = qRound(m_keyWidth / (markColor.alpha() ? 8.0 : 16.0));
  markBorder(m_keyHighlight, borderWidth, markColor);
}


void TpianoBg::applyCorrect() {
  int keyNr = -1;
  bool isWhite = true;
  Tnote sharpNote = Tnote(p_note.chromatic());
  int o = sharpNote.octave() - m_firstOctave;
  isWhite = sharpNote.alter() == 0;
  keyNr = o * 7 + sharpNote.note() - (isWhite ? 1 : 0);
  emit wantKeyToSelect(keyNr, isWhite);
  markSelected(GLOB->correctColor());
}


void TpianoBg::showNoteName() {
}


void TpianoBg::correct(const Tnote& n, quint32 noteData) {
  Q_UNUSED(noteData)
  p_wrongItem = m_keyHighlight;
  p_note = n;
  p_goodItem = m_keyHighlight;
  emit correctInstrument();
}


QString TpianoBg::octaveName(int oNr) const {
  return Tnote::shortOctaveName(oNr);
}



void TpianoBg::selectKey(QQuickItem* keyItem) {
  if (keyItem != m_selectedKey) {
    m_selectedKey = keyItem;
    emit selectedKeyChanged();
  }
}


void TpianoBg::setSelectedKey(QQuickItem* it) {
  selectKey(it);
  if (!m_readOnly && m_selectedKey) {
    int keyNr = m_selectedKey->property("nr").toInt();
    Tnote newNote(static_cast<char>(keyNr % 7 + 1), m_firstOctave + static_cast<char>(keyNr / 7), 0);
    if (m_selectedKey->z() > 0) { // black key
      newNote.setNote(newNote.note() - 1);
      newNote.setAlter(Tnote::e_Sharp);
    }
    p_note = newNote;
    emit noteChanged();
  }
}


void TpianoBg::setKeyHighlight(QQuickItem* hi) {
  m_keyHighlight = hi;
  qDebug() << "[TpianoBg] m_keyHighlight" << m_keyHighlight;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TpianoBg::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (oldGeometry.width() != newGeometry.width() || oldGeometry.height() != newGeometry.height()) {
    calculateMetrics(qFloor(newGeometry.width()));
    emit widthChanged();
    emit heightChanged();
  }
}


void TpianoBg::calculateMetrics(int newWidth) {
  int oldKeysNr = m_keysNumber;
  m_keysNumber = newWidth / qRound(m_keyWidth);
  if (m_keysNumber > 56) {
    m_keysNumber = 56;
    int oldKeyWidth = m_keyWidth;
    m_keyWidth = newWidth / m_keysNumber;
    if (oldKeyWidth != m_keyWidth)
      emit keyWidthChanged();
  }
  m_margin = (newWidth - m_keysNumber * qFloor(m_keyWidth)) / 2;
  if (oldKeysNr != m_keysNumber)
    emit keysNumberChanged();
}
