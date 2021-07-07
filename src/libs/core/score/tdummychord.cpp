/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#include "tdummychord.h"
#include "score/tnoteitem.h"
#include "music/tmelody.h"

#include <QtCore/qdebug.h>


TdummyChord::TdummyChord(QQuickItem *parent) :
  QQuickItem(parent)
{
}


TdummyChord::~TdummyChord() {}


int TdummyChord::chordModel() const {
  return m_chord ? m_chord->length() : 0;
}


void TdummyChord::setChord(Tmelody* c) {
  m_parentNote = qobject_cast<TnoteItem*>(parentItem());
  m_chord = c;
  emit chordChanged();
}


QString TdummyChord::noteHead() {
  return m_parentNote ? m_parentNote->getHeadText(m_parentNote->note()->rtm) : QString();
}


qreal TdummyChord::headPos(int id) {
  return m_parentNote && m_chord ? m_parentNote->getHeadY(m_chord->note(id)->p()) : 0.0;
}


QString TdummyChord::alterText(int id) {
//   return m_parentNote ? m_parentNote->geta
  return QString();
}
