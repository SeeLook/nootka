/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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
  #include <QtAndroidExtras/qandroidfunctions.h>
  #include <QtAndroidExtras/qandroidjnienvironment.h>
#else
//   #include "tmidiout.h"
  #include "trtaudioout.h"
  #include "trtaudioin.h"
#endif
#include <tnootkaqml.h>
#include "ttickcolors.h"
#include <tglobals.h>
#include <taudioparams.h>
#include <music/tmelody.h>

#include <QtQml/qqmlengine.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <QtGui/qevent.h>
#include <QtGui/qguiapplication.h>


/* static */
Tsound* Tsound::m_instance = nullptr;

QString Tsound::soundTouchVersion() { return TabstractPlayer::soundTouchVersion(); }



#define INT_FACTOR (1.2)


Tsound::Tsound(QObject* parent) :
  QObject(parent),
  player(nullptr),
  sniffer(nullptr),
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

  setQuantization(GLOB->A->quantization);
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
      setDefaultAmbitus();
      if (sniffer)
        sniffer->startListening();
      emit initialized();
  });
#if defined (Q_OS_ANDROID)
  m_currVol = currentVol();
  m_maxVol = maxVolRange();
  qApp->installEventFilter(this);
  m_volKeyTimer.start();
#endif
}


//TODO: Clear dead code here
void Tsound::play(const Tnote& note) {
//   bool playing = true;
  if (player && note.isValid()) {
    m_stopSniffOnce = true;
    stopMetronome();
    /*playing = */player->playNote(note.chromatic());
  }
#if defined (Q_OS_ANDROID)
//  if (playing) {
    if (sniffer) { // stop sniffer
      if (!m_stopSniffOnce) { // stop listening just once
        sniffer->stopListening();
        m_stopSniffOnce = true;
      }
    }
//  }
#else
//   if (playing && player->type() == TabstractPlayer::e_midi) {
//     if (sniffer) { // stop sniffer if midi output was started
//       if (!m_stopSniffOnce) { // stop listening just once
//         sniffer->stopListening();
//         m_stopSniffOnce = true;
//       }
//     }
//   }
#endif
}


void Tsound::playMelody(Tmelody* mel, int transposition) {
  if (player && player->isPlayable()) {
    if (player->isPlaying()) {
        stopPlaying();
    } else {
        if (mel->length()) {
          m_stopSniffOnce = true;
          player->playMelody(mel, transposition, 0);
        }
    }
  }
}


void Tsound::playNoteList(QList<Tnote>& notes, int firstNote, int countdownDuration) {
  if (player) {
    if (!player->isPlaying()) {
        if (!notes.isEmpty()) {
          runMetronome(firstNote == 0 && tickBeforePlay() ? Tmeter(static_cast<Tmeter::Emeter>(m_currentMeter)).countTo() : 0);
          m_stopSniffOnce = true;
          player->playNotes(std::addressof(notes),
                            Tmeter::quarterTempo(m_tempo, m_beatUnit),
                            firstNote,
                            countdownDuration);
        }
//         if (player->isPlaying()) {
//           if (sniffer) { // stop sniffer if midi output was started
//             if (!m_stopSniffOnce) { // stop listening just once
// //               sniffer->stopListening();
//               m_stopSniffOnce = true;
//             }
//           }
// //           emit playingChanged();
//         }
    } else {
        stopPlaying();
    }
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
      if (player && player->type() == TabstractPlayer::e_audio) {
          static_cast<TaudioOUT*>(player)->updateAudioParams();
          static_cast<TaudioOUT*>(player)->setAudioOutParams();
      } else if (sniffer)
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


void Tsound::setJACKorASIO(bool setOn) {
#if !defined (Q_OS_MAC) && !defined(Q_OS_ANDROID)
  TaudioIN::setJACKorASIO(setOn);
#else
  Q_UNUSED(setOn)
#endif
}


float Tsound::pitch() {
  if (sniffer)
    return sniffer->lastNotePitch();
  else
    return 0.0f;
}


void Tsound::setTempo(int t) {
  if (t != m_tempo && t > 39 && t < qMin(240, qRound(181.0 * Tmeter::beatTempoFactor(static_cast<Tmeter::EbeatUnit>(m_beatUnit))))) {
    m_tempo = t;
    emit tempoChanged();
  }
}


void Tsound::setBeatUnit(int bu) {
  if (bu > -1 && bu < 4) {
    if (bu != m_beatUnit) {
      int oldBu = m_beatUnit;
      m_beatUnit = bu;
      m_tempo = qMin(240, qRound(static_cast<qreal>(m_tempo) * 
                        Tmeter::beatTempoFactor(static_cast<Tmeter::EbeatUnit>(m_beatUnit)) / Tmeter::beatTempoFactor(static_cast<Tmeter::EbeatUnit>(oldBu))));
      emit tempoChanged();
    }
  }
}


void Tsound::setCurrentMeter(int curMet) {
  if (curMet != m_currentMeter) {
    m_currentMeter = curMet;
    setBeatUnit(static_cast<int>(Tmeter(static_cast<Tmeter::Emeter>(m_currentMeter)).optimalBeat()));
  }
}


void Tsound::setMetronome(int metronomeTempo, int metronomeBeat) {
  if (metronomeBeat != m_beatUnit || metronomeTempo != m_tempo) {
    int quarterTempo = Tmeter::quarterTempo(metronomeTempo, metronomeBeat);
    if (quarterTempo >= 40 && quarterTempo <= 180) {
        m_tempo = metronomeTempo;
        m_beatUnit = metronomeBeat;
        emit tempoChanged();
    } else
        qDebug() << "[Tsound] Can't set tempo" << metronomeTempo << "with" << static_cast<Tmeter::EbeatUnit>(metronomeBeat);
  }
}


void Tsound::runMetronome(int preTicksNr) {
  if (!GLOB->isSingleNote() && player && !m_metronomeIsRun && player->doTicking()) {
    player->setMetronome(m_tempo);
    if (player->tickBeforePlay() && preTicksNr) {
      qreal preTicksSeconds = static_cast<qreal>(preTicksNr) * (60.0 / static_cast<qreal>(m_tempo));
      while (preTicksSeconds < 2.0) { // Multiple number of countdown ticks if it is to short (less than 2 sec) - to give user time to catch up
        preTicksNr += preTicksNr;
        preTicksSeconds += preTicksSeconds;
      }
      player->setTicksCountBefore(preTicksNr);
      emit countdownPrepare(preTicksNr);
    }
    m_metronomeIsRun = true;
    emit metroRunningChanged();
  }
}


/**
 * @p m_quantVal is expressed in @p Trhythm duration of: Sixteenth triplet -> 4 or just Sixteenth -> 6 or Eighth -> 12
 * TODO: Triplets are not yet supported, so enable the code when they will be...
 */
void Tsound::setQuantization(int q) {
  if ((/*q == 4 || */q == 6 || q == 12) && q != m_quantVal) {
    m_quantVal = q;
    GLOB->A->quantization = m_quantVal;
    emit quantizationChanged();
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
  return player && player->isPlaying();
}


void Tsound::stopListen() {
  if (sniffer)
    sniffer->stopListening();
  stopMetronome();
}


/**
 * Starts pitch detection.
 * But in fact, only when user didn't stop it before.
 * By default skips initial count down (@p skipPreTicking)
 * - it avoids counting down after dialog windows are closed,
 * so exams executor should call it with @p FALSE
 */
void Tsound::startListen(bool skipPreTicking) {
  if (sniffer) {
    if (!sniffer->stoppedByUser())
      runMetronome(skipPreTicking ? 0 : Tmeter(static_cast<Tmeter::Emeter>(m_currentMeter)).countTo());
    sniffer->startListening();
  }
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


bool Tsound::tickBeforePlay() const { return player && player->tickBeforePlay(); }
void Tsound::setTickBeforePlay(bool tbp) {
  if (player && tbp != player->tickBeforePlay()) {
    player->setTickBeforePlay(tbp);
    emit tickStateChanged();
  }
}


bool Tsound::tickDuringPlay() const { return player && player->tickDuringPlay(); }
void Tsound::setTickDuringPlay(bool tdp) {
  if (player && tdp != player->tickDuringPlay()) {
    player->setTickDuringPlay(tdp);
    emit tickStateChanged();
  }
}


int Tsound::playingNoteId() const {
  return player && player->playingNoteId();
}


void Tsound::prepareToExam(Tnote loNote, Tnote hiNote) {
  m_examMode = true;
  if (sniffer) {
     m_prevLoNote = sniffer->loNote();
     m_prevHiNote = sniffer->hiNote();
     sniffer->setAmbitus(loNote, hiNote);
  }
  if (player)
    disconnect(player, &TaudioOUT::nextNoteStarted, this, &Tsound::selectNextNote);
}


void Tsound::restoreAfterExam() {
  m_examMode = false;
  if (sniffer) {
//     sniffer->setAmbitus(m_prevLoNote, m_prevHiNote); // acceptSettings() has already invoked setDefaultAmbitus()
    unPauseSniffing();
    startListen();
  }
  if (player)
    connect(player, &TaudioOUT::nextNoteStarted, this, &Tsound::selectNextNote);
}


void Tsound::stopPlaying() {
  if (player) {
    stopMetronome();
    player->stop();
  }
}


void Tsound::stop() {
  stopPlaying();
  stopListen();
}


bool Tsound::isPlayable() {
  return player ? player->isPlayable() : false;
}


bool Tsound::melodyIsPlaying() {
  return player && player->isPlaying();
}


void Tsound::setDefaultAmbitus() {
  if (sniffer)
    sniffer->setAmbitus(Tnote(GLOB->loString().chromatic() - 5), // range extended about 4th up and down
                  Tnote(GLOB->hiString().chromatic() + GLOB->GfretsNumber + 5));
}


void Tsound::setTunerMode(bool isTuner) {
  if (isTuner != m_tunerMode) {
    m_tunerMode = isTuner;
    emit tunerModeChanged();
    if (!m_tunerMode && player) // approve changed middle A frequency (if any)
      player->setPitchOffset(GLOB->A->a440diff - static_cast<qreal>(static_cast<int>(GLOB->A->a440diff)));
  }
}


#if defined (Q_OS_ANDROID)
int Tsound::maxVolRange() const {
  return QAndroidJniObject::callStaticMethod<jint>("net/sf/nootka/ToutVolume", "maxStreamVolume");
}


int Tsound::currentVol() const {
  return QAndroidJniObject::callStaticMethod<jint>("net/sf/nootka/ToutVolume", "streamVolume");
}


void Tsound::setVol(int v) {
  QAndroidJniObject::callStaticMethod<void>("net/sf/nootka/ToutVolume", "setStreamVolume", "(I)V", v);
}


void Tsound::setTouchHandling(bool th) {
  if (sniffer) {
    auto audioIn = qobject_cast<TaudioIN*>(sniffer);
    if (audioIn) {
      GLOB->setTouchStopsSniff(th);
      if (th)
        audioIn->startTouchHandle();
      else
        audioIn->stopTouchHandle();
    }
  }
}
#endif


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
  connect(player, &TaudioOUT::playingStarted, this, &Tsound::playingStartedSlot);
  connect(player, &TaudioOUT::nextNoteStarted, this, &Tsound::selectNextNote);
  connect(player, &TaudioOUT::playingFinished, this, &Tsound::playingFinishedSlot);
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
  connect(sniffer, &TaudioIN::stateChanged, this, &Tsound::listeningChanged);
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
//###################                PROTECTED         ############################################
//#################################################################################################
#if defined (Q_OS_ANDROID)
bool Tsound::eventFilter(QObject* watched, QEvent* event) {
  if (event->type() == QEvent::KeyPress) {
      auto ke = static_cast<QKeyEvent*>(event);
      if (ke->key() == Qt::Key_VolumeDown || ke->key() == Qt::Key_VolumeUp) {
        if (m_volKeyTimer.elapsed() > 100) {
            if (!m_tunerMode) {
                if (playing()) {
                    QAndroidJniObject::callStaticMethod<void>("net/sf/nootka/ToutVolume", "show");
                    if (ke->key() == Qt::Key_VolumeDown)
                      m_currVol--;
                    else
                      m_currVol++;
                    m_currVol = qBound(0, m_currVol, m_maxVol);
                    setVol(m_currVol);
                    m_currVol = QAndroidJniObject::callStaticMethod<jint>("net/sf/nootka/ToutVolume", "streamVolume");
                } else
                    QTimer::singleShot(10, this, &Tsound::volumeKeyPressed);
            } else {
                if (ke->key() == Qt::Key_VolumeDown)
                    QTimer::singleShot(10, this, &Tsound::volumeDownPressed);
                else
                    QTimer::singleShot(10, this, &Tsound::volumeUpPressed);
            }
            m_volKeyTimer.start();
        }
      }
  }
  return QObject::eventFilter(watched, event);
}
#endif

//#################################################################################################
//###################            PRIVATE SLOTS         ############################################
//#################################################################################################

void Tsound::playingStartedSlot() {
  emit playingChanged();
  if (sniffer)
    sniffer->stopListening();
}


void Tsound::playingFinishedSlot() {
  if (!m_examMode && sniffer) {
    if (m_stopSniffOnce) {
      sniffer->startListening();
      m_stopSniffOnce = false;
    }
  }
  emit plaingFinished();
  emit playingChanged();
  stopMetronome();
}


void Tsound::noteStartedSlot(const TnoteStruct& note) {
  m_detectedNote = note.pitch;
  m_detectedNote.setRhythm(GLOB->rhythmsEnabled() ? Trhythm::Sixteenth : Trhythm::NoRhythm, !m_detectedNote.isValid());
  if (!m_examMode && !m_tunerMode)
    NOO->noteStarted(m_detectedNote);
  emit noteStarted(m_detectedNote);
  emit noteStartedEntire(note);
}


void Tsound::noteFinishedSlot(const TnoteStruct& note) {
  if (note.pitch.isValid())
    m_detectedNote = note.pitch;
  if (GLOB->rhythmsEnabled()) {
      qreal rFactor = 2500.0 / Tmeter::quarterTempo(m_tempo, m_beatUnit);
      qreal dur = (note.duration * 1000.0) / rFactor;
      int quant = dur > 20.0 ? 12 : m_quantVal; // avoid sixteenth dots
      int normDur = qRound(dur / static_cast<qreal>(quant)) * quant;
      Trhythm r(normDur, m_detectedNote.isRest());
      if (r.isValid()) {
          m_detectedNote.setRhythm(r);
          emit noteFinished();
          if (!m_examMode && !m_tunerMode)
            NOO->noteFinished(m_detectedNote);
//          int realTempo = qRound(60.0 / ((24.0 / static_cast<qreal>(m_detectedNote.duration())) * static_cast<qreal>(note.duration)));
//           qDebug() << "Detected" << note.duration << normDur << note.pitchF << m_detectedNote.rtm.string() << "tempo" << realTempo << "\n";
      } else {
          int rtmRest = 0;
          TrhythmList notes = Trhythm::resolve(normDur, &rtmRest);
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
            m_detectedNote.rtm.setTie(rr.tie());
//             qDebug() << "Split note detected" << note.duration << normDur << n << note.pitchF << m_detectedNote.rtm.string();
            emit noteFinished();
            if (!m_examMode && !m_tunerMode) {
              if (n == 0) // update rhythm of the last note
                NOO->noteFinished(m_detectedNote);
              else { // but create others
                NOO->noteStarted(m_detectedNote);
                NOO->noteFinished(m_detectedNote);
              }
            }
          }
      }
  } else {
      if (!m_examMode && !m_tunerMode)
        emit noteFinished();
  }
  emit noteFinishedEntire(note);
}


void Tsound::selectNextNote() {
  if (player->playingNoteId() > -1 && player->playingNoteId() != NOO->selectedNoteId())
      NOO->selectPlayingNote(player->playingNoteId());
  emit playingNoteIdChanged();
}


void Tsound::stopMetronome() {
  if (m_metronomeIsRun) {
    if (player)
      player->stopMetronome();
    m_metronomeIsRun = false;
    emit metroRunningChanged();
  }
}



