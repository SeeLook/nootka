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
  #include "tmidiout.h"
  #include "trtaudioout.h"
  #include "trtaudioin.h"
  #include <QtCore/qfileinfo.h>
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
#if !defined (Q_OS_ANDROID)
  if (!m_dumpPath.isEmpty())
    GLOB->A->dumpPath.clear();
#endif
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void Tsound::init() {
  QTimer::singleShot(500, this, [=]{
#if !defined (Q_OS_ANDROID) && (defined (Q_OS_LINUX) || defined (Q_OS_WIN))
      TrtAudio::initJACKorASIO(GLOB->A->JACKorASIO);
#endif
      if (GLOB->outputType())
        createPlayer();
      if (GLOB->inputType())
        createSniffer();

      connect(NOO, &TnootkaQML::playNote, this, &Tsound::play);
      setDefaultAmbitus();
      if ( m_sniffer )
            m_sniffer->startListening();
      emit initialized();
  });
#if defined (Q_OS_ANDROID)
  m_currVol = currentVol();
  m_maxVol = maxVolRange();
  qApp->installEventFilter(this);
  m_volKeyTimer.start();
  connect(qApp, &QGuiApplication::applicationStateChanged, this, [=]{
    if (qApp->applicationState() == Qt::ApplicationActive)
        startListen();
    else if (qApp->applicationState() == Qt::ApplicationInactive)
        stop();
    else {
        stop();
        auto qtSniff = qobject_cast<TaudioIN*>(sniffer);
        if (qtSniff)
          qtSniff->stopDevice();
    }
  });
#endif
}


void Tsound::play(const Tnote& note) {
  if ( m_player && note.isValid()) {
    m_stopSniffOnce = true;
    stopMetronome();
        m_player->playNote(note.chromatic());
  }

#if defined (Q_OS_ANDROID)
  if (sniffer) { // stop sniffer
    if (!m_stopSniffOnce) { // stop listening just once
      sniffer->stopListening();
      m_stopSniffOnce = true;
    }
  }
#endif
}


void Tsound::playMelody(Tmelody* mel, int transposition) {
  if ( m_player && m_player->isPlayable()) {
    if ( m_player->isPlaying()) {
        stopPlaying();
    } else {
        if (mel->length()) {
          m_stopSniffOnce = true;
                m_player->playMelody(mel, transposition, 0);
        }
    }
  }
}


void Tsound::playNoteList(QList<Tnote>& notes, int firstNote, int countdownDuration) {
  if ( m_player ) {
    if (!m_player->isPlaying()) {
        if (!notes.isEmpty()) {
          runMetronome(firstNote == 0 && tickBeforePlay() ? Tmeter(static_cast<Tmeter::Emeter>(m_currentMeter)).countTo() : 0);
          m_stopSniffOnce = true;
                m_player->playNotes(std::addressof(notes),
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
  return m_sniffer ? m_sniffer->volume() : 0.0;
}


qreal Tsound::pitchDeviation() {
  if ( m_sniffer )
    return static_cast<qreal>(qBound(-0.49, ( m_sniffer->lastChunkPitch() - static_cast<float>(qRound( m_sniffer->lastChunkPitch()))) * INT_FACTOR, 0.49));
  else
    return 0.0;
}


void Tsound::acceptSettings() {
  bool doParamsUpdated = false;
  // for output
  if (GLOB->outputType()) {
      if (!m_player )
          createPlayer();
      else {
        #if !defined (Q_OS_ANDROID)
          if (GLOB->A->outType == TaudioParams::e_midiSound) {
            deletePlayer(); // it is safe to delete midi
            createPlayer(); // and create it again
          } else
        #endif
          { // avoids deleting TaudioOUT instance and loading ogg file every acceptSettings call
            if ( m_player->type() == TabstractPlayer::e_midi) {
                deletePlayer(); // player was midi so delete
                createPlayer();
            } else { // just set new params to TaudioOUT
                doParamsUpdated = true;
            }
          }
          if ( m_player ) {
            if (!m_player->isPlayable())
              deletePlayer();
          }
      }
  } else
        deletePlayer();

  // for input
  if (GLOB->inputType()) {
      if (!m_sniffer ) {
          createSniffer();
      } else {
//       m_userState = sniffer->stoppedByUser();
          setDefaultAmbitus();
          doParamsUpdated = true;
      }
  } else {
      if ( m_sniffer )
        deleteSniffer();
  }
#if defined (Q_OS_ANDROID)
  if (player)
    static_cast<TaudioOUT*>(player)->setAudioOutParams();
  if (sniffer)
    sniffer->updateAudioParams();
#else
  if (doParamsUpdated) {
      if ( m_player && m_player->type() == TabstractPlayer::e_audio) {
          static_cast<TaudioOUT*>( m_player )->updateAudioParams();
          static_cast<TaudioOUT*>( m_player )->setAudioOutParams();
      } else if ( m_sniffer )
            m_sniffer->updateAudioParams();
  }
#endif
//   if (sniffer)
//     restoreSniffer();
}


QStringList Tsound::inputDevices() const { return TaudioIN::getAudioDevicesList(); }

QStringList Tsound::outputDevices() const { return TaudioOUT::getAudioDevicesList(); }

QStringList Tsound::midiPorts() const {
#if defined (Q_OS_ANDROID)
  return QStringList();
#else
  return TmidiOut::getMidiPortsList();
#endif
}


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
  if ( m_sniffer )
    return m_sniffer->lastNotePitch();
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
      m_tempo = qMin(240,
                     qRound(static_cast<qreal>(m_tempo) *
                        Tmeter::beatTempoFactor(static_cast<Tmeter::EbeatUnit>(m_beatUnit)) / Tmeter::beatTempoFactor(static_cast<Tmeter::EbeatUnit>(oldBu)))
                     );
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
  if (!GLOB->isSingleNote() && m_player && !m_metronomeIsRun && m_player->doTicking()) {
        m_player->setMetronome(m_tempo);
    if ( m_player->tickBeforePlay() && preTicksNr) {
      qreal preTicksSeconds = static_cast<qreal>(preTicksNr) * (60.0 / static_cast<qreal>(m_tempo));
      while (preTicksSeconds < 2.0) { // Multiple number of countdown ticks if it is to short (less than 2 sec) - to give user time to catch up
        preTicksNr += preTicksNr;
        preTicksSeconds += preTicksSeconds;
      }
            m_player->setTicksCountBefore(preTicksNr);
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
  return m_sniffer ? m_sniffer->stoppedByUser() : false;
}


void Tsound::setStoppedByUser(bool sbu) {
  if ( m_sniffer && m_sniffer->stoppedByUser() != sbu) {
        m_sniffer->setStoppedByUser(sbu);
    emit stoppedByUserChanged();
  }
}


bool Tsound::listening() const {
  return m_sniffer ? m_sniffer->detectingState() == TcommonListener::e_detecting : false;
}


bool Tsound::playing() const {
  return m_player && m_player->isPlaying();
}


void Tsound::stopListen() {
  if ( m_sniffer )
        m_sniffer->stopListening();
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
  if ( m_sniffer ) {
    if (!m_sniffer->stoppedByUser())
      runMetronome(skipPreTicking ? 0 : Tmeter(static_cast<Tmeter::Emeter>(m_currentMeter)).countTo());
        m_sniffer->startListening();
  }
}


void Tsound::pauseSinffing() {
  if ( m_sniffer )
        m_sniffer->pause();
}


void Tsound::unPauseSniffing() {
  if ( m_sniffer )
        m_sniffer->unPause();
}


bool Tsound::isSnifferPaused() {
  return m_sniffer ? m_sniffer->isPaused() : false;
}


bool Tsound::isSniferStopped() {
  return m_sniffer ? m_sniffer->isStoped() : true;
}


bool Tsound::tickBeforePlay() const { return m_player && m_player->tickBeforePlay(); }
void Tsound::setTickBeforePlay(bool tbp) {
  if ( m_player && tbp != m_player->tickBeforePlay()) {
        m_player->setTickBeforePlay(tbp);
    emit tickStateChanged();
  }
}


bool Tsound::tickDuringPlay() const { return m_player && m_player->tickDuringPlay(); }
void Tsound::setTickDuringPlay(bool tdp) {
  if ( m_player && tdp != m_player->tickDuringPlay()) {
        m_player->setTickDuringPlay(tdp);
    emit tickStateChanged();
  }
}


int Tsound::playingNoteId() const {
  return m_player && m_player->playingNoteId();
}


void Tsound::prepareToExam(Tnote loNote, Tnote hiNote) {
  m_examMode = true;
  if ( m_sniffer ) {
     m_prevLoNote = m_sniffer->loNote();
     m_prevHiNote = m_sniffer->hiNote();
        m_sniffer->setAmbitus(loNote, hiNote);
  }
  if ( m_player )
    disconnect( m_player, &TaudioOUT::nextNoteStarted, this, &Tsound::selectNextNote);
}


void Tsound::restoreAfterExam() {
  m_examMode = false;
  if ( m_sniffer ) {
//     sniffer->setAmbitus(m_prevLoNote, m_prevHiNote); // acceptSettings() has already invoked setDefaultAmbitus()
    unPauseSniffing();
    startListen();
  }
  if ( m_player )
    connect( m_player, &TaudioOUT::nextNoteStarted, this, &Tsound::selectNextNote);
}


void Tsound::stopPlaying() {
  if ( m_player ) {
    stopMetronome();
        m_player->stop();
  }
}


void Tsound::stop() {
  stopPlaying();
  stopListen();
}


bool Tsound::isPlayable() {
  return m_player ? m_player->isPlayable() : false;
}


bool Tsound::melodyIsPlaying() {
  return m_player && m_player->isPlaying();
}


void Tsound::setDefaultAmbitus() {
  if ( m_sniffer )
        m_sniffer->setAmbitus(Tnote(GLOB->loString().chromatic() - 5), // range extended about 4th up and down
                  Tnote(GLOB->hiString().chromatic() + GLOB->GfretsNumber + 5));
}


void Tsound::setTunerMode(bool isTuner) {
  if (isTuner != m_tunerMode) {
    m_tunerMode = isTuner;
    emit tunerModeChanged();
    if (!m_tunerMode && m_player ) // approve changed middle A frequency (if any)
            m_player->setPitchOffset(GLOB->A->a440diff - static_cast<qreal>(static_cast<int>(GLOB->A->a440diff)));
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
void Tsound::changeDumpPath(const QString& path) {
  if (QFileInfo(path).exists()) {
      m_dumpPath = path;
      GLOB->A->dumpPath = path;
  } else
      qDebug() << "[Tsound] dump path" << path << "does not exist!";
}


void Tsound::setDumpFileName(const QString& fName) {
  if ( m_sniffer && !GLOB->A->dumpPath.isEmpty())
        m_sniffer->setDumpFileName(fName);
}
#endif


//#################################################################################################
//###################                PRIVATE           ############################################
//#################################################################################################

void Tsound::createPlayer() {
  if (GLOB->A->outType == TaudioParams::e_realSound)
        m_player = new TaudioOUT(GLOB->A);
  else
        m_player = new TmidiOut(GLOB->A);
  connect( m_player, &TaudioOUT::playingStarted, this, &Tsound::playingStartedSlot);
  connect( m_player, &TaudioOUT::nextNoteStarted, this, &Tsound::selectNextNote);
  connect( m_player, &TaudioOUT::playingFinished, this, &Tsound::playingFinishedSlot);
  m_stopSniffOnce = false;
}


void Tsound::createSniffer() {
#if !defined (Q_OS_ANDROID)
  if (TaudioIN::instance())
        m_sniffer = TaudioIN::instance();
  else
#endif
        m_sniffer = new TaudioIN(GLOB->A);
  setDefaultAmbitus();
//   sniffer->setAmbitus(Tnote(-31), Tnote(82)); // fixed ambitus bounded Tartini capacities
  connect( m_sniffer, &TaudioIN::noteStarted, this, &Tsound::noteStartedSlot);
  connect( m_sniffer, &TaudioIN::noteFinished, this, &Tsound::noteFinishedSlot);
  connect( m_sniffer, &TaudioIN::stateChanged, this, &Tsound::listeningChanged);
  m_userState = false; // user didn't stop sniffing yet
}


void Tsound::deletePlayer() {
  if ( m_player ) {
        m_player->stop();
    delete m_player;
        m_player = nullptr;
  }
}


void Tsound::deleteSniffer() {
  delete m_sniffer;
    m_sniffer = nullptr;
}


void Tsound::restoreSniffer() {
    m_sniffer->setStoppedByUser(m_userState);
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
                } else if (!GLOB->isExam())
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
  if ( m_sniffer )
        m_sniffer->stopListening();
}


void Tsound::playingFinishedSlot() {
  if (!m_examMode && m_sniffer ) {
    if (m_stopSniffOnce) {
            m_sniffer->startListening();
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
  if ( m_player->playingNoteId() > -1 && m_player->playingNoteId() != NOO->selectedNoteId())
      NOO->selectPlayingNote( m_player->playingNoteId());
  emit playingNoteIdChanged();
}


void Tsound::stopMetronome() {
  if (m_metronomeIsRun) {
    if ( m_player )
            m_player->stopMetronome();
    m_metronomeIsRun = false;
    emit metroRunningChanged();
  }
}



