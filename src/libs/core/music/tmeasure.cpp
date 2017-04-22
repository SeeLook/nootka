/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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

#include "tmeasure.h"
#include "tchunk.h"


Tmeasure::Tmeasure(int nr, Tmeter::Emeter m) :
  m_number(nr),
  m_meter(m),
  m_duration(0)
{
}


void Tmeasure::addNote(const Tchunk& n) {
  m_notes << n;
  if (m_meter.meter() != Tmeter::NoMeter)
    m_duration += n.duration();
}


void Tmeasure::removeLastNote() {
  m_duration -= lastNote().duration();
  m_notes.removeLast();
}


bool Tmeasure::isFull() {
  return m_duration == m_meter.duration();
}

