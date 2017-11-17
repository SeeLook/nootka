/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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
#if defined (Q_OS_ANDROID)
  #include "tqtaudioin.h"
  #include "tqtaudioout.h"
#else
  #include "tmidiout.h"
  #include "trtaudioout.h"
  #include "trtaudioin.h"
#endif
#include "tnootkaqml.h"
#include "ttickcolors.h"
#include <tinitcorelib.h>
#include <taudioparams.h>
#include "music/tmelody.h"
#include "music/tchunk.h"

#include <QtQml/qqmlengine.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>


/* static */
Tsound* Tsound::m_instance = nullptr;


#define INT_FACTOR (1.2)


Tsound::Tsound(QObject* parent) :
  QObject(parent),
  player(nullptr),
  sniffer(nullptr),
  m_examMode(false),
  m_melodyNoteIndex(-1),
  m_tempo(60),
  m_quantVal(6)
{
  if (m_instance) {
    qDebug() << "Tsound instance already exists!";
    return;
  }

  m_instance = this;
  qRegisterMetaType<Tchunk>("Tchunk");
  qRegisterMetaType<TnoteStruct>("TnoteStruct");
  qmlRegisterType<TtickColors>("Nootka", 1, 0, "TtickColors");
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

void Tsound::init() {
  QTimer::singleShot(500, [=]{
#if !defined (Q_OS_ANDROID) && (defined (Q_OS_LINUX) || defined (Q_OS_WIN))
      TrtAudio::initJACKorASIO(GLOB->A->JACKorASIO);
#endif
      if (GLOB->A->OUTenabled)
          createPlayer();
      if (GLOB->A->INenabled)
          createSniffer();

      connect(NOO, &TnootkaQML::playNote, [=](const Tnote& n){ play(n); });
      if (sniffer)
        sniffer->startListening();
      emit initialized();
  });
}


void Tsound::play(const Tnote& note) {
  bool playing = false;
  if (player && note.note)
      playing = player->play(note.chromatic());
#if defined (Q_OS_ANDROID)
  if (playing) {
    if (sniffer) { // stop sniffer
      if (!m_stopSniffOnce) { // stop listening just once
        sniffer->stopListening();
        m_stopSniffOnce = true;
      }
    }
  }
#else
  if (playing && !GLOB->A->playDetected && player->type() == TabstractPlayer::e_midi) {
    if (sniffer) { // stop sniffer if midi output was started
      if (!m_stopSniffOnce) { // stop listening just once
        sniffer->stopListening();
        m_stopSniffOnce = true;
      }
    }
  }
#endif
  emit playingChanged();
}


void Tsound::playMelody(Tmelody* mel) {
//   if (m_melodyNoteIndex > -1)
//     m_melodyNoteIndex = m_playedMelody->length();
//   else {
//     m_melodyNoteIndex = 0;
//     m_playedMelody = mel;
//   }
}


void Tsound::playScoreNotes(const QList<Tnote>& notes, int firstNote) {
  if (player && player->type() == TabstractPlayer::e_audio)
    static_cast<TaudioOUT*>(player)->playMelody(notes, m_tempo, firstNote);
}


void Tsound::playScore() {
  if (!player->isPlaying()) {
      playScoreNotes(NOO->scoreNoteList(), NOO->selectedNoteId() > -1 ? NOO->selectedNoteId() : 0);
      if (player->isPlaying()) {
        if (sniffer) { // stop sniffer if midi output was started
          if (!m_stopSniffOnce) { // stop listening just once
            sniffer->stopListening();
            m_stopSniffOnce = true;
          }
        }
        emit playingChanged();
      }
  } else {
      stopPlaying();
      playingFinishedSlot();
  }
}


qreal Tsound::inputVol() {
  return sniffer ? sniffer->volume() : 0.0;
}


qreal Tsound::pitchDeviation() {
  if (sniffer)
    return static_cast<qreal>(qBound(-0.49, (sniffer->lastChunkPitch() - static_cast<float>(qRound(sniffer->lastChunkPitch()))) * INT_FACTOR, 0.49));
  else
    return 0.0;
}


void Tsound::acceptSettings() {
  bool doParamsUpdated = false;
  // for output
  if (GLOB->A->OUTenabled) {
      if (!player)
          createPlayer();
      else {
        #if !defined (Q_OS_ANDROID)
          if (GLOB->A->midiEnabled) {
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
  } else
        deletePlayer();

  // for input
  if (GLOB->A->INenabled) {
      if (!sniffer) {
          createSniffer();
      } else {
//       m_userState = sniffer->stoppedByUser();
          setDefaultAmbitus();
          doParamsUpdated = true;
      }
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
      if (player && player->type() == TabstractPlayer::e_audio)
          static_cast<TaudioOUT*>(player)->updateAudioParams();
      else if (sniffer)
          sniffer->updateAudioParams();
  }
#endif
//   if (sniffer)
//     restoreSniffer();
}


QStringList Tsound::inputDevices() const { return TaudioIN::getAudioDevicesList(); }

QStringList Tsound::outputDevices() const { return TaudioOUT::getAudioDevicesList(); }

QString Tsound::currentInDevName() const { return TaudioIN::inputName(); }

QString Tsound::currentOutDevName() const { return TaudioOUT::outputName(); }


void Tsound::prepareToConf() {
  if (player) {
//     player->stop();
// #if !defined (Q_OS_ANDROID)
//     player->deleteMidi();
// #endif
  }
  if (sniffer) {
    m_userState = sniffer->stoppedByUser();
//     sniffer->stopListening();
//     blockSignals(true);
    sniffer->setStoppedByUser(false);
  }
}


void Tsound::restoreAfterConf() {
// #if !defined (Q_OS_ANDROID)
//   if (GLOB->A->midiEnabled) {
//     if (player)
//       player->setMidiParams();
//   }
// #endif
  if (sniffer)
    restoreSniffer();
}


float Tsound::pitch() {
  if (sniffer)
    return sniffer->lastNotePitch();
  else
    return 0.0f;
}


void Tsound::setTempo(int t) {
  if (t != m_tempo && t > 39 && t < 181) {
    m_tempo = qRound(15000.0 / (qRound(15000.0 / static_cast<qreal>(t) / sniffer->chunkTime()) * sniffer->chunkTime()));
    emit tempoChanged();
  }
}


/**
 * @p m_quantVal is expressed in @p Trhythm duration of: Sixteenth triplet -> 4 or just Sixteenth -> 6 or Eighth -> 12
 */
void Tsound::setQuantization(int q) {
  if ((q == 4 || q == 6 || q == 12) && q != m_quantVal) {
    m_quantVal = q;
  }
}


bool Tsound::stoppedByUser() const {
  return sniffer ? sniffer->stoppedByUser() : false;
}


void Tsound::setStoppedByUser(bool sbu) {
  if (sniffer && sniffer->stoppedByUser() != sbu) {
    sniffer->setStoppedByUser(sbu);
    emit stoppedByUserChanged();
  }
}


bool Tsound::listening() const {
  return sniffer ? sniffer->detectingState() == TcommonListener::e_detecting : false;
}


bool Tsound::playing() const {
  return player ? player->isPlaying() : false;
}


void Tsound::stopListen() {
  if (sniffer)
    sniffer->stopListening();
}


void Tsound::startListen() {
  if (sniffer)
    sniffer->startListening();
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
  return sniffer ? sniffer->isPaused() : false;
}


bool Tsound::isSniferStopped() {
  return sniffer ? sniffer->isStoped() : true;
}


void Tsound::prepareToExam(Tnote loNote, Tnote hiNote) {
  m_examMode = true;
  if (sniffer) {
//      m_pitchView->setDisabled(true);
     m_prevLoNote = sniffer->loNote();
     m_prevHiNote = sniffer->hiNote();
     sniffer->setAmbitus(loNote, hiNote);
  }
}


void Tsound::restoreAfterExam() {
  m_examMode = false;
  if (sniffer) {
//     sniffer->setAmbitus(m_prevLoNote, m_prevHiNote); // acceptSettings() has already invoked setDefaultAmbitus()
//     m_pitchView->setDisabled(false);
    unPauseSniffing();
    startListen();
  }
}


void Tsound::stopPlaying() {
  if (player)
    player->stop();
//   m_melodyNoteIndex = -1;
}


bool Tsound::isPlayable() {
  return player ? player->isPlayable() : false;
}


void Tsound::setDefaultAmbitus() {
  if (sniffer)
    sniffer->setAmbitus(Tnote(GLOB->loString().chromatic() - 5), // range extended about 4th up and down
                  Tnote(GLOB->hiString().chromatic() + GLOB->GfretsNumber + 5));
}


#if !defined (Q_OS_ANDROID)
void Tsound::setDumpFileName(const QString& fName) {
  if (sniffer && !GLOB->A->dumpPath.isEmpty())
    sniffer->setDumpFileName(fName);
}
#endif

//#################################################################################################
//###################                PRIVATE           ############################################
//#################################################################################################



void Tsound::createPlayer() {
  player = new TaudioOUT(GLOB->A);
  connect(player, &TaudioOUT::nextNoteStarted, this, &Tsound::selectNextNote);
  connect(player, &TaudioOUT::noteFinished, this, &Tsound::playingFinishedSlot);
  m_stopSniffOnce = false;
}


void Tsound::createSniffer() {
#if !defined (Q_OS_ANDROID)
  if (TaudioIN::instance())
    sniffer = TaudioIN::instance();
  else
#endif
  sniffer = new TaudioIN(GLOB->A);
  setDefaultAmbitus();
//   sniffer->setAmbitus(Tnote(-31), Tnote(82)); // fixed ambitus bounded Tartini capacities
  connect(sniffer, &TaudioIN::noteStarted, this, &Tsound::noteStartedSlot);
  connect(sniffer, &TaudioIN::noteFinished, this, &Tsound::noteFinishedSlot);
  connect(sniffer, &TaudioIN::stateChanged, [=]{ emit listeningChanged(); });
  m_userState = false; // user didn't stop sniffing yet
}


void Tsound::deletePlayer() {
  if (player) {
    player->stop();
    delete player;
    player = nullptr;
  }
}


void Tsound::deleteSniffer() {
  delete sniffer;
  sniffer = nullptr;
}


void Tsound::restoreSniffer() {
  sniffer->setStoppedByUser(m_userState);
//   blockSignals(false);
//   sniffer->startListening();
}

//#################################################################################################
//###################            PRIVATE SLOTS         ############################################
//#################################################################################################


void Tsound::playingFinishedSlot() {
  if (!m_examMode && sniffer) {
    if (m_stopSniffOnce)
      sniffer->startListening();
    m_stopSniffOnce = false;
  }
  emit plaingFinished();
  emit playingChanged();
}


void Tsound::noteStartedSlot(const TnoteStruct& note) {
  m_detectedNote = note.pitch;
  m_detectedNote.setRhythm(Trhythm::Sixteenth, !m_detectedNote.isValid());
  if (!m_examMode)
    NOO->noteStarted(m_detectedNote);
  emit noteStarted(m_detectedNote);
  emit noteStartedEntire(note);
  if (player && GLOB->instrument().type() != Tinstrument::NoInstrument && GLOB->A->playDetected)
    play(m_detectedNote);
}


// qreal durationBalance = 0.0;
void Tsound::noteFinishedSlot(const TnoteStruct& note) {
  if (note.pitch.isValid())
    m_detectedNote = note.pitch;
  qreal rFactor = 2500.0 / m_tempo;
  qreal dur = (note.duration * 1000.0/* + durationBalance*/) / rFactor;
  int normDur = qRound(dur / static_cast<qreal>(m_quantVal)) * m_quantVal;
//   durationBalance = note.duration * 1000.0 - static_cast<qreal>(normDur) * rFactor;
//   qDebug() << "[Tsound] rest of duration average" << durationBalance << "of" << note.duration * 1000.0 << normDur * rFactor;
  Trhythm r(normDur, m_detectedNote.isRest());
  if (r.isValid()) {
      m_detectedNote.setRhythm(r);
      qDebug() << "Detected" << note.duration << normDur << note.pitchF << m_detectedNote.rtm.string();
      emit noteFinished();
      if (!m_examMode)
        NOO->noteFinished(m_detectedNote);
  } else {
      TrhythmList notes;
      Trhythm::resolve(normDur, notes);
      for (int n = 0; n < notes.count(); ++n) {
        Trhythm& rr = notes[n];
        if (!m_detectedNote.isRest()) {
          if (n == 0)
              rr.setTie(Trhythm::e_tieStart);
          else if (n == notes.count() - 1)
              rr.setTie(Trhythm::e_tieEnd);
          else
            rr.setTie(Trhythm::e_tieCont);
        }
        m_detectedNote.setRhythm(rr.rhythm(), m_detectedNote.isRest(), rr.hasDot(), rr.isTriplet());
        qDebug() << "Detected" << note.duration << normDur << n << note.pitchF << m_detectedNote.rtm.string();
        emit noteFinished();
        if (!m_examMode) {
          if (n == 0) // update rhythm of the last note
            NOO->noteFinished(m_detectedNote);
          else // but create others
            NOO->noteStarted(m_detectedNote);
        }
      }
  }
  emit noteFinishedEntire(note);
  if (player && GLOB->instrument().type() == Tinstrument::NoInstrument && GLOB->A->playDetected)
    play(m_detectedNote);
}


void Tsound::selectNextNote() {
  if (player->playingNoteId() != NOO->selectedNoteId())
    NOO->selectPlayingNote(player->playingNoteId());
}
