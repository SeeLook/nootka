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

#include "tnotesbaritem.h"


TnotesBarItem::TnotesBarItem(QQuickItem *parent) :
  QQuickItem(parent)
{
}


void TnotesBarItem::setLowestNote(int lowestNote)
{
  if (m_lowestNote == lowestNote)
    return;

  m_lowestNote = lowestNote;
  emit lowestNoteChanged();
}


void TnotesBarItem::setHighestNote(int highestNote)
{
  if (m_highestNote == highestNote)
    return;

  m_highestNote = highestNote;
  emit highestNoteChanged();
}


void TnotesBarItem::setDetectedNote(const Tnote &detectedNote)
{
  if (m_detectedNote == detectedNote)
    return;

  m_detectedNote = detectedNote;
  emit detectedNoteChanged();
}


void TnotesBarItem::setExpectedNote(const Tnote &expectedNote)
{
  if (m_expectedNote == expectedNote)
    return;

  m_expectedNote = expectedNote;
  emit expectedNoteChanged();
}
