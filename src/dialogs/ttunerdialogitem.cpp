/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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
#if defined (Q_OS_ANDROID)
  #include <QtAndroidExtras/qandroidfunctions.h>
  #include <QtAndroidExtras/qandroidjnienvironment.h>
#endif


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
  GLOB->setMidAfreq(m_workFreq);
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
  if (GLOB->instrument().isGuitar() && t->stringNr() > 2) { // guitar
      for (int i = 1; i <= t->stringNr(); i++) {
        float offPitch = TnoteStruct::pitchToFreq(t->str(i).toMidi() + pitch440Offset + static_cast<qreal>(GLOB->A->transposition));
        model << QString("%1;%2 Hz").arg((t->str(i)).styledName()).arg(offPitch, 0, 'f', 1);
      }
  } else { // no guitar - C-major scale frequencies
      for (int i = 1; i < 8; i++) {
        Tnote n(i, 1, 0);
        float offPitch = TnoteStruct::pitchToFreq(n.toMidi() + pitch440Offset + static_cast<qreal>(GLOB->A->transposition));
        model << QString("%1;%2 Hz").arg(n.styledName()).arg(offPitch, 0, 'f', 1);
      }
  }
  return model;
}


/**
 * By setting @p a440diff parameter directly, we are cheating the system...
 * This parameter has direct influence on calculating detected pitch of a listened sound,
 * so change the middle A frequency by user has immediate effect on displayed pitches on tuner window.
 * But "the system" (Nootka) is not aware of that (i.e. for recalculating audio data of changed note)
 * Global change will be approve only after tuner dialog is closed (destroyed)
 */
void TtunerDialogItem::setWorkFreq(int wFreq) {
  if (wFreq != m_workFreq) {
    GLOB->A->a440diff = Tglobals::pitchOfFreq(static_cast<qreal>(wFreq)) - Tglobals::pitchOfFreq(440.0);
    m_workFreq = wFreq;
    emit workFreqChanged();
    emit tuningModelChanged();
  }
}


int TtunerDialogItem::pitch() const {
  return SOUND->note().isValid() ? static_cast<int>(SOUND->note().chromatic()) : 0;
}


int TtunerDialogItem::lowestNote() const {
  return static_cast<int>(GLOB->loNote().chromatic()) - 5;
}


int TtunerDialogItem::highestNote() const {
  return static_cast<int>(GLOB->hiString().chromatic()) + 3;
}


QString TtunerDialogItem::styledName(int chroma) const {
  return Tnote(chroma).styledName();
}


bool TtunerDialogItem::isOpenString(int chroma) const {
  for (int s = 1; s <= GLOB->stringNumber(); ++s) {
    if (GLOB->Gtune()->strChromatic(s) == chroma)
      return true;
  }
  return false;
}


int TtunerDialogItem::whichString(int chroma) const {
  for (int s = 1; s <= GLOB->stringNumber(); ++s) {
    if (GLOB->Gtune()->strChromatic(s) == chroma)
      return GLOB->strOrder(s - 1) + 1;
  }
  return 0;
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




