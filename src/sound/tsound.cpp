/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
#include "taudioout.h"
#include "taudioin.h"
#include "taudioparams.h"
#include "tpitchview.h"
#include <QPushButton>
// #include <QApplication>
#include <QThread>
#include <QTimer>
#include <QAudioInput>


extern Tglobals *gl;

Tsound::Tsound(QObject* parent) :
  QObject(parent),
  m_thread(new QThread())
{
  if (gl->A->OUTenabled)
      createPlayer();
  else
      player = 0;
  if (gl->A->INenabled) {
    createSniffer();
  } else {
    sniffer = 0;
  }
}

Tsound::~Tsound()
{ //They have not a prent
  deleteSniffer();
  deletePlayer();
  m_thread->quit();
  m_thread->deleteLater();
}

//------------------------------------------------------------------------------------
//------------  public  methods     --------------------------------------------------
//------------------------------------------------------------------------------------

void Tsound::play(Tnote note) {
  if(player) {
    if (player->play(note.getChromaticNrOfNote())) { // true if plaing was started
      if (sniffer) { // pause sniffer if note was started
        sniffer->wait();
        m_pitchView->stopVolume();
      }
    }
//     player->play(note.getChromaticNrOfNote());
  }
}

void Tsound::acceptSettings() {
  // for output
  if (gl->A->OUTenabled) {
    if (!player)
        createPlayer();
    else {
//         player->setAudioOutParams(gl->A);
        deletePlayer();
        createPlayer();
        if (!player->isPlayable())
          deletePlayer();
    }
  } else {
    if (player)
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
  if (player) {
    player->deleteMidi();
    stopPlaying();
  }
  if (sniffer) {
    sniffer->stopListening();
    m_pitchView->stopVolume();
//     deleteSniffer();
  }
}

void Tsound::restoreAfterConf() {
  if (player) {
    if (gl->A->midiEnabled)
      player->setMidiParams();
  }
  if (sniffer) {
    if (!m_pitchView->isPaused()) {
      sniffer->startListening();
      m_pitchView->startVolume();
    }
  }
}

void Tsound::wait() {
  if (sniffer) {
    sniffer->stopListening();
//     sniffer->wait();
    m_pitchView->stopVolume();
  }
}

void Tsound::go() {
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

void Tsound::restoreAfterAnswer() {
  m_pitchView->setBgColor(Qt::transparent);
  m_pitchView->setDisabled(true);
//   m_pitchView->update();
}

void Tsound::prepareToExam() {
  if (player) // to avoid activate sniffing after play() during an exam
    disconnect(player, SIGNAL(noteFinished()), this, SLOT(playingFinished()));
  if (sniffer)
     m_pitchView->setDisabled(true);
}

void Tsound::restoreAfterExam() {
  if (player)
    connect(player, SIGNAL(noteFinished()), this, SLOT(playingFinished()));
  if (sniffer) {
     m_pitchView->setDisabled(false);
     go();
  }
}

void Tsound::stopPlaying() {
    if (player)
      player->stop();
}


//------------------------------------------------------------------------------------
//------------  private  methods     --------------------------------------------------
//------------------------------------------------------------------------------------

  

void Tsound::createPlayer() {
    player = new TaudioOUT(gl->A, gl->path);
    player->moveToThread(m_thread);
    m_thread->start(QThread::HighPriority);
    connect(player, SIGNAL(noteFinished()), this, SLOT(playingFinished()));
}

void Tsound::createSniffer() {
//  m_thread = new QThread();
  sniffer = new TaudioIN(gl->A);
//   sniffer->moveToThread(m_thread);
//   m_thread->start(QThread::HighPriority);
  sniffer->setAmbitus(gl->loString(), Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber));

  sniffer->startListening();
  connect(sniffer, SIGNAL(noteDetected(Tnote)), this, SLOT(noteDetectedSlot(Tnote)), Qt::QueuedConnection);
}

void Tsound::deletePlayer() {
  player->stop();
  m_thread->quit();
//   delete player;
  player->deleteLater();
  player = 0;
}


void Tsound::deleteSniffer() {
//    if (m_thread) {
//     m_thread->quit();
//    m_thread->terminate();
//     delete m_thread;
//   }
  delete sniffer;
  sniffer = 0;
}


//------------------------------------------------------------------------------------
//------------  slots               --------------------------------------------------
//------------------------------------------------------------------------------------
void Tsound::playingFinished() {
//  qDebug("playingFinished");
  if (sniffer) {
    sniffer->go();
    m_pitchView->startVolume();
    
  }
//   go();
}

void Tsound::noteDetectedSlot(Tnote note) {
  m_detNote = note;
//   qDebug("Tsound: got note");
  emit detectedNote(note);
}

