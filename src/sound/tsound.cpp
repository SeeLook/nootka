/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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
#include "tsound.h"
#include "tglobals.h"
#include "taudioin.h"
#include "taudioparams.h"
#include "tpitchview.h"
#include "tmidiout.h"
#include <QPushButton>
#include <QThread>
#include <QTimer>
#include <QDebug>

#if defined (Q_OS_LINUX)
//   #include "pulseprober.h"
  #include "trtaudioout.h"
#else
  #include "taudioout.h"
#endif




extern Tglobals *gl;

Tsound::Tsound(QObject* parent) :
  QObject(parent),
  m_thread(new QThread()),
  audioPlayer(0),
  midiPlayer(0),
  sniffer(0),
  m_examMode(false)
{
// #if defined (Q_OS_LINUX)
//   if (!checkForPulse()) { // checks is PulseAudio needed and works
//     gl->A->midiEnabled = true;
//     gl->A->INenabled = false;
//   }
// #endif
  if (gl->A->OUTenabled)
      createPlayer();
  else {
      audioPlayer = 0;
      midiPlayer = 0;
  }
  if (gl->A->INenabled) {
    createSniffer();
  } else {
    sniffer = 0;
  }
}

Tsound::~Tsound()
{ //They have not a parent
  deleteSniffer();
  deletePlayer();
  m_thread->quit();
  m_thread->deleteLater();
}

//------------------------------------------------------------------------------------
//------------  public  methods     --------------------------------------------------
//------------------------------------------------------------------------------------

void Tsound::play(Tnote note) {
  bool plaing;
  if(audioPlayer)
    plaing = audioPlayer->play(note.getChromaticNrOfNote());
  else if (midiPlayer)
    plaing = midiPlayer->play(note.getChromaticNrOfNote());
  if (plaing) { // true if plaing was started
    if (sniffer) { // pause sniffer if note was started
      sniffer->wait();
      m_pitchView->stopVolume();
    }
  }
}

void Tsound::acceptSettings() {
  // for output
  if (gl->A->OUTenabled) {
    if (!audioPlayer && !midiPlayer)
        createPlayer();
    else {
        deletePlayer();
        createPlayer();
        if (audioPlayer)
          if (!audioPlayer->isPlayable())
            deletePlayer();
        if (midiPlayer)
          if (!midiPlayer->isPlayable())
            deletePlayer();
    }
  } else {
    if (audioPlayer || midiPlayer)
      deletePlayer();
  }
  // for input
  if (gl->A->INenabled) {
    m_pitchView->setEnabled(true);
    if (!sniffer) {
      createSniffer();
      m_pitchView->setAudioInput(sniffer);
      m_pitchView->setIsVoice(gl->A->isVoice);
      if (m_pitchView->isPaused()) {
        sniffer->stopListening();
      }
      else {
        m_pitchView->startVolume();
      }
    }
    else {
      sniffer->setParameters(gl->A);
      sniffer->setAmbitus(gl->loString(), Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber));
//       m_pitchView->setAudioInput(sniffer);
      m_pitchView->setIsVoice(gl->A->isVoice);
      if (!m_pitchView->isPaused()) { // and pause button
        sniffer->startListening();
        m_pitchView->startVolume();
      }
    }
  } else {
    m_pitchView->setDisabled(true);
    if (sniffer)
      deleteSniffer();
  }
}

void Tsound::setPitchView(TpitchView* pView) {
  m_pitchView = pView;
  m_pitchView->setPitchColor(gl->EanswerColor);
  m_pitchView->setIsVoice(gl->A->isVoice);
  if (sniffer)
      m_pitchView->startVolume();
  else
    m_pitchView->setDisabled(true);
}

void Tsound::prepareToConf() {
  stopPlaying();
  if (midiPlayer)
    midiPlayer->deleteMidi();
  if (sniffer) {
    sniffer->stopListening();
    m_pitchView->stopVolume();
  }
}

void Tsound::restoreAfterConf() {
  if (midiPlayer) {
    if (gl->A->midiEnabled)
      midiPlayer->setMidiParams();
  }
  if (sniffer) {
    if (!m_pitchView->isPaused()) {
      sniffer->startListening();
      m_pitchView->startVolume();
    }
  }
}

void Tsound::wait() {
//     qDebug("wait");
  if (sniffer) {
    sniffer->stopListening();
//     sniffer->wait();
    m_pitchView->stopVolume();
  }
}

void Tsound::go() {
//     qDebug("go");
  if (sniffer && !m_pitchView->isPaused()) {
    sniffer->startListening();
//     sniffer->go();
    m_pitchView->startVolume();
  }
}

void Tsound::prepareAnswer() {
  m_pitchView->setBgColor(gl->EanswerColor);
  m_pitchView->setDisabled(false);
//   m_pitchView->update();
}

void Tsound::pauseSinffing() {
    if (sniffer)
        sniffer->pause();
}

void Tsound::unPauseSniffing() {
    if (sniffer)
        sniffer->unPause();
}

bool Tsound::isSnifferPaused() {
    if (sniffer)
        return sniffer->isPaused();
    else
        return false;
}

void Tsound::restoreAfterAnswer() {
  m_pitchView->setBgColor(Qt::transparent);
  m_pitchView->setDisabled(true);
//   m_pitchView->update();
}

void Tsound::prepareToExam() {
  m_examMode = true;
  if (sniffer)
     m_pitchView->setDisabled(true);
}

void Tsound::restoreAfterExam() {
  m_examMode = false;
  if (sniffer) {
     m_pitchView->setDisabled(false);
     unPauseSniffing();
     go();
  }
}

void Tsound::stopPlaying() {
    if (audioPlayer)
      audioPlayer->stop();
    else if (midiPlayer)
      midiPlayer->stop();
}

bool Tsound::isPlayable() {
  if (audioPlayer || midiPlayer)
   return true;
  else
    return false;
}



//------------------------------------------------------------------------------------
//------------  private  methods     --------------------------------------------------
//------------------------------------------------------------------------------------

  

void Tsound::createPlayer() {
  if (gl->A->midiEnabled)
    midiPlayer = new TmidiOut(gl->A);
  else
    audioPlayer = new TaudioOUT(gl->A, gl->path);
#if defined(Q_OS_WIN32)
    // Windows has problems with playing it in separate thread - cuted sound
    // so we skip
#else
  if (audioPlayer)
//     audioPlayer->moveToThread(m_thread);
//   else 
    if (midiPlayer)
    midiPlayer->moveToThread(m_thread);
  m_thread->start(QThread::HighPriority);
#endif
  if (audioPlayer)
    connect(audioPlayer, SIGNAL(noteFinished()), this, SLOT(playingFinishedSlot()));
  else if (midiPlayer)
    connect(midiPlayer, SIGNAL(noteFinished()), this, SLOT(playingFinishedSlot()));
}


void Tsound::createSniffer() {
  sniffer = new TaudioIN(gl->A);
//   sniffer->moveToThread(m_thread);
//   m_thread->start(QThread::HighPriority);
  sniffer->setAmbitus(gl->loString(), Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber));

  sniffer->startListening();
  connect(sniffer, SIGNAL(noteDetected(Tnote)), this, SLOT(noteDetectedSlot(Tnote)), Qt::QueuedConnection);
}

void Tsound::deletePlayer() {
  if (audioPlayer)
    audioPlayer->stop();
  else if (midiPlayer)
    midiPlayer->stop();
  m_thread->quit();
  if (audioPlayer) {
    audioPlayer->deleteLater();
    audioPlayer = 0;
  } else if (midiPlayer) {
    midiPlayer->deleteLater();
    midiPlayer = 0;
  }
}


void Tsound::deleteSniffer() {
//    if (m_thread) {
//     m_thread->quit();
//    m_thread->terminate();
//     delete m_thread;
//   }
//  delete sniffer;
  sniffer->deleteLater();
  sniffer = 0;
}


//------------------------------------------------------------------------------------
//------------  slots               --------------------------------------------------
//------------------------------------------------------------------------------------
void Tsound::playingFinishedSlot() {
//  qDebug("playingFinished");
  if (!m_examMode && sniffer) {
    sniffer->go();
    m_pitchView->startVolume();
  }
  emit plaingFinished();
//   go();
}

void Tsound::noteDetectedSlot(Tnote note) {
  m_detNote = note;
//  qDebug() << "Tsound: got note" << note.toText();
  emit detectedNote(note);
}

