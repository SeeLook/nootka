/***************************************************************************
 *   Copyright (C) 2020 by Tomasz Bojczuk                                  *
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

#include "tmelodywrapper.h"
#include "tmelodylistview.h"
#include "music/tmelody.h"
#include "score/tscoreobject.h"


TmelodyWrapper::TmelodyWrapper(QQuickItem *parent) :
  QQuickItem(parent)
{
}

TmelodyWrapper::~TmelodyWrapper() {}


void TmelodyWrapper::setNr(int n) {
  if (m_nr != n) {
    m_nr = n;
    emit nrChanged();
  }
}


QString TmelodyWrapper::title() const {
  return m_melody ? m_melody->title() : QString();
}


QString TmelodyWrapper::composer() const {
  return m_melody ? m_melody->composer() : QString();
}


void TmelodyWrapper::updateMelody() {
  auto oldMelody = m_melody;
  m_melody = m_melodyView->getMelody(m_nr);
  if (m_melody != oldMelody) {
    m_score->setMelody(m_melody, false, MELODY_LENGHT);
    emit melodyChanged();
  }
}
