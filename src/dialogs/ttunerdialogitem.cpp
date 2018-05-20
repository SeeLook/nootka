/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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

#include "ttunerdialogitem.h"
#include <music/tnotestruct.h>
#include <music/ttune.h>
#include <tsound.h>
#include <tglobals.h>
#include <taudioparams.h>

#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>


TtunerDialogItem::TtunerDialogItem(QQuickItem* parent) :
  QQuickItem(parent)
{
  m_timer = new QTimer(this);
  connect(m_timer, &QTimer::timeout, this, &TtunerDialogItem::timeoutSlot);
  m_stoppedByUserState = SOUND->stoppedByUser();
  QTimer::singleShot(350, [=]{ delayedInit(); });
  SOUND->setTunerMode(true);
  connect(SOUND, &Tsound::noteStartedEntire, this, &TtunerDialogItem::noteStartedSlot);
}


TtunerDialogItem::~TtunerDialogItem()
{
  SOUND->stopListen();
  SOUND->setStoppedByUser(m_stoppedByUserState);
  SOUND->setTunerMode(false);
  SOUND->startListen();
}


qreal TtunerDialogItem::deviation() const {
  return SOUND->pitchDeviation();
}


QString TtunerDialogItem::noteName() const {
  return SOUND->note().styledName();
}


QStringList TtunerDialogItem::tuningModel() {
  QStringList model;
  auto t = GLOB->Gtune();
  auto pitch440Offset = GLOB->A->a440diff;
  if (t->stringNr() > 2) { // guitar
      for (int i = 1; i <= t->stringNr(); i++) {
        float offPitch = TnoteStruct::pitchToFreq(t->str(i).toMidi() + pitch440Offset);
        model << QString("<span style=\"font-family: nootka;\">%1</span>%2 = %3 Hz").arg(i).arg((t->str(i)).toRichText()).arg(offPitch, 0, 'f', 1);
      }
  } else { // no guitar - C-major scale frequencies
      for (int i = 1; i < 8; i++) {
        Tnote n(i, 1, 0);
        float offPitch = TnoteStruct::pitchToFreq(n.toMidi() + pitch440Offset);
        model << QString("<b>%1</b> = %2 Hz").arg(n.toRichText()).arg(offPitch, 0, 'f', 1);
      }
  }
  return model;
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TtunerDialogItem::delayedInit() {
  m_timer->start(75);
  SOUND->setStoppedByUser(false);
  SOUND->startListen();
}


void TtunerDialogItem::timeoutSlot() {
  emit deviationChanged();
}


void TtunerDialogItem::noteStartedSlot(const TnoteStruct& note) {
  m_frequency = qCeil(note.freq * 100.0) / 100.0;
  emit frequencyChanged();
  if (note.pitch.isValid())
    emit noteNameChanged();
}




