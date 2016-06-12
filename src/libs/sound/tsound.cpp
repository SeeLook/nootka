/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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

#include "tsound.h"
#include "widgets/tpitchview.h"
#if defined (Q_OS_ANDROID)
  #include "tqtaudioin.h"
  #include "tqtaudioout.h"
#else
  #include "tmidiout.h"
  #include "trtaudioout.h"
  #include "trtaudioin.h"
#endif
#include <tprecisetimer.h>
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <music/tmelody.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qaction.h>
#include <QtCore/qdebug.h>

/* static */
Tsound* Tsound::m_instance = nullptr;


Tsound::Tsound(QObject* parent) :
  QObject(parent),
  player(0),
  sniffer(0),
  m_examMode(false),
  m_melodyNoteIndex(-1)
{
  if (m_instance) {
    qDebug() << "Tsound instance already exists!";
    return;
  }

  m_instance = this;
  qRegisterMetaType<Tchunk>("Tchunk");
  qRegisterMetaType<TnoteStruct>("TnoteStruct");
#if !defined (Q_OS_ANDROID) && (defined (Q_OS_LINUX) || defined (Q_OS_WIN))
	TrtAudio::initJACKorASIO(Tcore::gl()->A->JACKorASIO);
#endif
  if (Tcore::gl()->A->OUTenabled)
      createPlayer();
  else
      player = 0;
  if (Tcore::gl()->A->INenabled) {
			createSniffer();
  } else {
			sniffer = 0;
  }
}

Tsound::~Tsound()
{ //They have not a parent
  deleteSniffer();
  deletePlayer();
  m_instance = nullptr;
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void Tsound::play(Tnote& note) {
  bool playing = false;
  if (player && note.note)
			playing = player->play(note.chromatic());
#if defined (Q_OS_ANDROID)
  if (playing) {
    if (sniffer) { // stop sniffer if midi output was started
      if (!m_stopSniffOnce) { // stop listening just once
        sniffer->stopListening();
        m_stopSniffOnce = true;
      }
    }
  }
#else
  if (playing && !Tcore::gl()->A->playDetected && player->type() == TabstractPlayer::e_midi) {
    if (sniffer) { // stop sniffer if midi output was started
			if (!m_stopSniffOnce) { // stop listening just once
				sniffer->stopListening();
				m_stopSniffOnce = true;
			}
    }
  }
#endif
}


void Tsound::playMelody(Tmelody* mel) {
	if (m_melodyNoteIndex > -1)
    m_melodyNoteIndex = m_playedMelody->length();
	else {
    m_melodyNoteIndex = 0;
    m_playedMelody = mel;
  }
  playMelodySlot();
}


void Tsound::acceptSettings() {
	bool doParamsUpdated = false;
  // for output
  if (Tcore::gl()->A->OUTenabled) {
    if (!player)
        createPlayer();
    else {
      #if !defined (Q_OS_ANDROID)
        if (Tcore::gl()->A->midiEnabled) {
          deletePlayer(); // it is safe to delete midi
          createPlayer(); // and create it again
        } else
      #endif
        { // avoids deleting TaudioOUT instance and loading ogg file every acceptSettings call
          if (player->type() == TabstractPlayer::e_midi) {
              deletePlayer(); // player was midi so delete
              createPlayer();
          } else { // just set new params to TaudioOUT
              doParamsUpdated = true;
          }
        }
        if (player) {
          if (!player->isPlayable())
            deletePlayer();
				}
    }
  } else {
      deletePlayer();
  }
  // for input
  if (Tcore::gl()->A->INenabled) {
    if (!sniffer) {
      createSniffer();
      m_pitchView->setAudioInput(sniffer);
    } else {
//       m_userState = sniffer->stoppedByUser();
      setDefaultAmbitus();
      doParamsUpdated = true;
    }
		m_pitchView->setMinimalVolume(Tcore::gl()->A->minimalVol);
		m_pitchView->setIntonationAccuracy(Tcore::gl()->A->intonation);
  } else {
    if (sniffer)
      deleteSniffer();
  }
#if defined (Q_OS_ANDROID)
  if (player)
    static_cast<TaudioOUT*>(player)->setAudioOutParams();
  if (sniffer)
    sniffer->updateAudioParams();
#else
  if (doParamsUpdated) {
			if (player && player->type() == TabstractPlayer::e_audio) {
					static_cast<TaudioOUT*>(player)->updateAudioParams();
      } else if (sniffer)
					sniffer->updateAudioParams();
	}
#endif
	if (sniffer) {
    restoreSniffer();
	}
}


void Tsound::setPitchView(TpitchView* pView) {
  m_pitchView = pView;
  m_pitchView->setPitchColor(Tcore::gl()->EanswerColor);
  m_pitchView->setMinimalVolume(Tcore::gl()->A->minimalVol);
	m_pitchView->setIntonationAccuracy(Tcore::gl()->A->intonation);
	m_pitchView->setAudioInput(sniffer);

  if (sniffer) {
#if defined (Q_OS_ANDROID)
    QTimer::singleShot(750, [=]{ m_pitchView->pauseAction()->trigger(); });
#else
    QTimer::singleShot(750, [=]{ sniffer->startListening(); });
#endif
  }
}


void Tsound::prepareToConf() {
  if (player) {
		player->stop();
#if !defined (Q_OS_ANDROID)
    player->deleteMidi();
#endif
	}
  if (sniffer) {
    m_userState = sniffer->stoppedByUser(); // m_pitchView->isPaused();
    sniffer->stopListening();
    m_pitchView->setDisabled(true);
    blockSignals(true);
    sniffer->setStoppedByUser(false);
  }
}


void Tsound::restoreAfterConf() {
#if !defined (Q_OS_ANDROID)
  if (Tcore::gl()->A->midiEnabled) {
    if (player)
      player->setMidiParams();
  }
#endif
  if (sniffer)
    restoreSniffer();
}


float Tsound::pitch() {
	if (sniffer)
		return sniffer->lastNotePitch();
	else
		return 0.0f;
}



void Tsound::wait() {
//     qDebug("wait");
  if (sniffer) {
    sniffer->stopListening();
  }
}


void Tsound::go() {
//     qDebug("go");
  if (sniffer /*&& !m_pitchView->isPaused()*/) {
    sniffer->startListening();
  }
}


void Tsound::prepareAnswer() {
  m_pitchView->setBgColor(Tcore::gl()->EanswerColor);
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
// 		sniffer->setAmbitus(m_prevLoNote, m_prevHiNote); // acceptSettings() has already invoked setDefaultAmbitus()
		m_pitchView->setDisabled(false);
		unPauseSniffing();
		go();
  }
}


void Tsound::stopPlaying() {
  if (player)
    player->stop();
	m_melodyNoteIndex = -1;		
}


bool Tsound::isPlayable() {
  if (player)
   return true;
  else
    return false;
}


void Tsound::setDefaultAmbitus() {
	if (sniffer)
		sniffer->setAmbitus(Tnote(Tcore::gl()->loString().chromatic() - 5), // range extended about 4th up and down
									Tnote(Tcore::gl()->hiString().chromatic() + Tcore::gl()->GfretsNumber + 5));
}


//#################################################################################################
//###################                PRIVATE           ############################################
//#################################################################################################



void Tsound::createPlayer() {
#if defined (Q_OS_ANDROID)
  player = new TaudioOUT(Tcore::gl()->A);
  connect(player, SIGNAL(noteFinished()), this, SLOT(playingFinishedSlot()));
#else
  if (Tcore::gl()->A->midiEnabled) {
      player = new TmidiOut(Tcore::gl()->A);
      connect(player, SIGNAL(noteFinished()), this, SLOT(playingFinishedSlot()));
	} else
      player = new TaudioOUT(Tcore::gl()->A);
#endif
  m_stopSniffOnce = false;
}


void Tsound::createSniffer() {
#if !defined (Q_OS_ANDROID)
  if (TaudioIN::instance())
    sniffer = TaudioIN::instance();
  else
#endif
    sniffer = new TaudioIN(Tcore::gl()->A);
  setDefaultAmbitus();
// 	sniffer->setAmbitus(Tnote(-31), Tnote(82)); // fixed ambitus bounded Tartini capacities
	connect(sniffer, &TaudioIN::noteStarted, this, &Tsound::noteStartedSlot);
	connect(sniffer, &TaudioIN::noteFinished, this, &Tsound::noteFinishedSlot);
  m_userState = false; // user didn't stop sniffing yet
}


void Tsound::deletePlayer() {
  if (player) {
    player->stop();
		delete player;
    player = 0;
  }
}


void Tsound::deleteSniffer() {
	delete sniffer;
  sniffer = 0;
}


void Tsound::restoreSniffer() {
  sniffer->setStoppedByUser(m_userState);
  m_pitchView->setDisabled(false);
  blockSignals(false);
  sniffer->startListening();
}



//#################################################################################################
//###################            PRIVATE SLOTS         ############################################
//#################################################################################################


void Tsound::playingFinishedSlot() {
//   qDebug("playingFinished");
  if (!m_examMode && sniffer) {
    if (m_stopSniffOnce) {
      sniffer->startListening();
    }
    m_stopSniffOnce = false;
  }
  emit plaingFinished();
}


void Tsound::playMelodySlot() {
	if (m_melodyNoteIndex > -1 && m_melodyNoteIndex < m_playedMelody->length()) {
		play(m_playedMelody->note(m_melodyNoteIndex)->p());
		TpreciseTimer::singleShot(60000 / m_playedMelody->tempo(), this, SLOT(playMelodySlot()));
		m_melodyNoteIndex++;
	} else {
		m_melodyNoteIndex = -1;
		playingFinishedSlot();
	}
}


void Tsound::noteStartedSlot(const TnoteStruct& note) {
	m_detectedPitch = note.pitch;
	emit noteStarted(m_detectedPitch);
  emit noteStartedEntire(note);
	if (player && Tcore::gl()->instrument != e_noInstrument && Tcore::gl()->A->playDetected)
		play(m_detectedPitch);
}


Tchunk m_lastChunk;
void Tsound::noteFinishedSlot(const TnoteStruct& note) {
	m_detectedPitch = note.pitch;
// 	Tchunk noteChunk(m_detectedPitch, Trhythm());
  m_lastChunk.p() = m_detectedPitch;
//   m_lastChunk.r() = ; // TODO not supported yet
	emit noteFinished(&m_lastChunk);
  emit noteFinishedEntire(note);
	if (player && Tcore::gl()->instrument == e_noInstrument && Tcore::gl()->A->playDetected)
		play(m_detectedPitch);
}








