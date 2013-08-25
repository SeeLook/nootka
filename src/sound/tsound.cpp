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
#include "taudioparams.h"
#include "tpitchview.h"
#include "tmidiout.h"
#include "trtaudioout.h"
#include "trtaudioin.h"
#include <QPushButton>
#include <QTimer>
#include <QDebug>



extern Tglobals *gl;

Tsound::Tsound(QObject* parent) :
  QObject(parent),
  sniffer(0),
  player(0),
  m_examMode(false)
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
{ //They have not a parent
  deleteSniffer();
  deletePlayer();
}

//------------------------------------------------------------------------------------
//------------  public  methods     --------------------------------------------------
//------------------------------------------------------------------------------------

void Tsound::play(Tnote note) {
  bool playing = false;
  if (player)
			playing = player->play(note.getChromaticNrOfNote());
  if (playing) { // true if playing was started
    if (sniffer) { // pause sniffer if note was started
      sniffer->wait();
      m_pitchView->stopVolume();
    }
  }
}


void Tsound::acceptSettings() {
  // for output
  if (gl->A->OUTenabled) {
    if (!player)
        createPlayer();
    else {
        if (gl->A->midiEnabled) {
          deletePlayer(); // it is safe to delete midi
          createPlayer(); // and create it again
        } else { // avoids deleting TaudioOUT instance and loading ogg file every acceptSettings call
          if (player->type() == TabstractPlayer::e_midi) {
              deletePlayer(); // player was midi so delete
              createPlayer();
          } else { // just set new params to TaudioOUT
              TaudioOUT *aOut = static_cast<TaudioOUT*>(player);
              aOut->setAudioOutParams(gl->A);
          }
        }
        if (player)
          if (!player->isPlayable())
            deletePlayer();
    }
  } else {
      deletePlayer();
  }
  // for input
  if (gl->A->INenabled) {
    m_pitchView->setEnabled(true);
    if (!sniffer) {
      createSniffer();
      m_pitchView->setAudioInput(sniffer);
      m_pitchView->setIsVoice(gl->A->isVoice);
      m_pitchView->setMinimalVolume(gl->A->minimalVol);
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
      m_pitchView->setIsVoice(gl->A->isVoice);
      m_pitchView->setMinimalVolume(gl->A->minimalVol);
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
  m_pitchView->setMinimalVolume(gl->A->minimalVol);
  if (sniffer)
      m_pitchView->startVolume();
  else
    m_pitchView->setDisabled(true);
}


void Tsound::prepareToConf() {
  stopPlaying();
  if (player)
    player->deleteMidi();
  if (sniffer) {
    sniffer->stopListening();
    m_pitchView->stopVolume();
  }
}


void Tsound::restoreAfterConf() {
  if (gl->A->midiEnabled) {
    if (player)
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
//     qDebug("wait");
  if (sniffer) {
    sniffer->stopListening();
    m_pitchView->stopVolume();
  }
}


void Tsound::go() {
//     qDebug("go");
  if (sniffer && !m_pitchView->isPaused()) {
    sniffer->startListening();
    m_pitchView->startVolume();
  }
}


void Tsound::prepareAnswer() {
  m_pitchView->setBgColor(gl->EanswerColor);
  m_pitchView->setDisabled(false);
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
}


void Tsound::prepareToExam(Tnote loNote, Tnote hiNote) {
  m_examMode = true;
  if (sniffer) {
     m_pitchView->setDisabled(true);
		 m_prevLoNote = sniffer->loNote();
		 m_prevHiNote = sniffer->hiNote();
		 sniffer->setAmbitus(loNote, hiNote);
	}
}


void Tsound::restoreAfterExam() {
  m_examMode = false;
  if (sniffer) {
		sniffer->setAmbitus(m_prevLoNote, m_prevHiNote);
		m_pitchView->setDisabled(false);
		unPauseSniffing();
		go();
  }
}


void Tsound::stopPlaying() {
  if (player)
    player->stop();
}


bool Tsound::isPlayable() {
  if (player)
   return true;
  else
    return false;
}



//------------------------------------------------------------------------------------
//------------  private  methods     --------------------------------------------------
//------------------------------------------------------------------------------------

  

void Tsound::createPlayer() {
  if (gl->A->midiEnabled)
    player = new TmidiOut(gl->A);
  else
    player = new TaudioOUT(gl->A, gl->path);
  connect(player, SIGNAL(noteFinished()), this, SLOT(playingFinishedSlot()));
}


void Tsound::createSniffer() {
  sniffer = new TaudioIN(gl->A);
  sniffer->setAmbitus(gl->loString(), Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber));
  sniffer->startListening();
  connect(sniffer, SIGNAL(noteDetected(Tnote)), this, SLOT(noteDetectedSlot(Tnote)));
}

void Tsound::deletePlayer() {
  if (player)
    player->stop();
  if (player) {
    player->deleteLater();
    player = 0;
  }
}


void Tsound::deleteSniffer() {
 delete sniffer;
  sniffer = 0;
}


//------------------------------------------------------------------------------------
//------------  slots               --------------------------------------------------
//------------------------------------------------------------------------------------
void Tsound::playingFinishedSlot() {
//   qDebug("playingFinished");
  if (!m_examMode && sniffer) {
    if (!m_pitchView->isPaused()) {
        sniffer->go();
        m_pitchView->startVolume();
    }
  }
  emit plaingFinished();
//   go();
}

void Tsound::noteDetectedSlot(Tnote note) {
  m_detNote = note;
//  qDebug() << "Tsound: got note" << note.toText();
  emit detectedNote(note);
}

