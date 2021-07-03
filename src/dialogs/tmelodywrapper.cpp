/***************************************************************************
 *   Copyright (C) 2020-2021 by Tomasz Bojczuk                             *
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
#include <tglobals.h>

#include <QtCore/qtimer.h>


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
  m_melody = qvariant_cast<Tmelody*>(m_melodyView->getMelody(m_nr));
  if (m_melody != oldMelody) {
    m_score->setMelody(m_melody, false, MELODY_LENGHT);
    checkOutOfScale();
    emit melodyChanged();
    /**
     * HACK: Trigger mouse hover with delay to avoid highlighting every wrapper.
     * When mouse is above and delegates are created one by one every of them gets hover enter
     * but not hover exit somehow, so all behaves like hovered - black texts, preview button highlighted.
     * With this delay it works as intended.
     */
    QTimer::singleShot(500, this, [=]{ parent()->setProperty("hoverEnabled", QVariant::fromValue(true)); });
  }
}


void TmelodyWrapper::reload() {
  if (m_melody && m_score)
    m_score->setMelody(m_melody, false, MELODY_LENGHT);
  checkOutOfScale();
  emit melodyChanged();
}


int TmelodyWrapper::key() const {
  return m_melody ? static_cast<int>(m_melody->key().value()) : 0;
}


void TmelodyWrapper::setKey(int k) {
  if (m_melody)
    m_melody->setKey(TkeySignature(static_cast<char>(k)));
}


//#################################################################################################
//###################                PROTECTED         ############################################
//#################################################################################################

void TmelodyWrapper::checkOutOfScale() {
  m_outOfScale = false;
  auto hi = GLOB->hiNote().chromatic(), lo = GLOB->loNote().chromatic();
  for (int n = 0; n < m_melody->length(); ++n) {
    auto chrom = m_melody->note(n)->p().chromatic();
    if (chrom < lo || chrom > hi) {
      m_outOfScale = true;
      break;
    }
  }
}
