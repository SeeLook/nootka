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

#include "taudioanalyzeitem.h"
#include <tsound.h>
#include <trtaudioin.h>


TaudioAnalyzeItem::TaudioAnalyzeItem(QQuickItem *parent) :
  QQuickItem(parent)
{
  connect(SOUND, &Tsound::initialized, this, &TaudioAnalyzeItem::soundInitSlot);
}


TaudioAnalyzeItem::~TaudioAnalyzeItem() {}

//#################################################################################################
//###################                PROTECTED         ############################################
//#################################################################################################

void TaudioAnalyzeItem::soundInitSlot() {
  if (SOUND->sniffer) {
    m_sniffer = SOUND->sniffer;
    connect(m_sniffer, &TcommonListener::volumeChanged, this, &TaudioAnalyzeItem::volumeSlot);
    connect(m_sniffer, &TcommonListener::noteFinished, this, &TaudioAnalyzeItem::noteFinishedSlot);
  }
}


void TaudioAnalyzeItem::volumeSlot() {
  emit dataReady(m_sniffer->pcmVolume(), m_sniffer->finder()->energy(), m_sniffer->finder()->isOnSet());
}


void TaudioAnalyzeItem::noteFinishedSlot(const TnoteStruct& n) {
  emit noteData(n.startChunk, n.endChunk, n.pitch.isValid() ? n.pitch.styledName() : "\ue4e5");
}
