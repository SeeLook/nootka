/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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

#include "trtaudioout.h"
#include "toggscale.h"
#include "tasioemitter.h"
#include <tglobals.h>
#include <taudioparams.h>
#include <music/tnote.h>
#include <QtCore/qtimer.h>
#include <QtCore/qthread.h>

#include <QtCore/qdebug.h>



/*static*/
QStringList TaudioOUT::getAudioDevicesList() {
    QStringList devList;
    createRtAudio();
    if (getCurrentApi() == RtAudio::LINUX_ALSA)
      closeStream(); // close ALSA stream to get full list of devices
    int devCnt = getDeviceCount();
    if (devCnt < 1)
        return devList;
    for (int i = 0; i < devCnt; i++) {
        RtAudio::DeviceInfo devInfo;
        if (!getDeviceInfo(devInfo, i))
          continue;
        if (devInfo.probed && devInfo.outputChannels > 0)
          devList << convDevName(devInfo);
    }
    if (getCurrentApi() == RtAudio::LINUX_ALSA && !devList.isEmpty())
        devList.prepend(QStringLiteral("ALSA default"));
    return devList;
}


TaudioOUT*              TaudioOUT::instance = nullptr;


#define CROSS_SMP (2200) // 50ms
#define INVALID_NOTE_NR (-100)
#define SAMPLE_LEN (79380) // 1.8sec (70560) // 1.6 //(66150) 1.5 //61740 1.4


bool TaudioOUT::outCallBack(void* outBuff, void* inBuff, unsigned int nBufferFrames) {
  instance->m_callBackIsBussy = true;

  bool endState = true;
  if (!instance->playList().isEmpty() && p_playingNoteNr < instance->playList().size() && p_ticksCountBefore == 0) {
      TsingleSound& playingSound = instance->playList()[p_playingNoteNr];
      auto out = static_cast<qint16*>(outBuff);
      bool unfinished = true;
      qint16 sample = 0;
      for (int i = 0; i < nBufferFrames / instance->ratioOfRate; i++) {
        if (p_posInNote >= playingSound.samplesCount) {
          p_prevNote = playingSound.number == REST_NR || p_posInOgg > SAMPLE_LEN ? INVALID_NOTE_NR : playingSound.number;
          p_shiftOfPrev = 0;
          p_lastPosOfPrev = p_posInOgg; // < SAMPLE_LEN ? p_posInNote : 0;
          p_playingNoteNr++;
          if (p_playingNoteNr < instance->playList().size()) {
              p_posInOgg = 0;
              p_posInNote = 0;
              playingSound = instance->playList()[p_playingNoteNr];
              p_playingNoteId = playingSound.id;
              ao()->emitNextNoteStarted();
          } else {
              unfinished = false;
              p_lastNotePlayed = true; // inform beating routine to stop
          }
        }
        if (unfinished && playingSound.number < REST_NR) {
          if (instance->oggScale->soundContinuous() && p_posInOgg > instance->oggScale->stopLoopSample(playingSound.number))
            p_posInOgg = instance->oggScale->startLoopSample(playingSound.number);

          if (p_posInOgg < SAMPLE_LEN) { // 1.4 sec of samples
              sample = instance->oggScale->getNoteSample(playingSound.number, p_posInOgg);
              if (p_posInOgg < 220) // fade in 5ms
                sample = static_cast<qint16>(sample * (1.0 - (static_cast<qreal>(220 - p_posInOgg) / 220.0)));
              if (p_playingNoteNr == instance->playList().size() - 1 && p_posInOgg > playingSound.samplesCount - 220) { // fade out 5ms the last one
                  qreal m = 1.0 - (static_cast<qreal>(p_posInOgg + 221 - playingSound.samplesCount) / 220.0);
                  sample = static_cast<qint16>(sample * (m < 0.0 ? 0.0 : m));
              }
              if (instance->oggScale->soundContinuous() && p_posInNote > 44100) { // fade out long continuous sound
                // slowly fade it out to minimize loop jump effect
                sample = static_cast<qint16>(sample * (1.0 - static_cast<qreal>(p_posInNote - 44100) / static_cast<qreal>(playingSound.samplesCount)));
                if (p_posInNote > playingSound.samplesCount - 220) // but fade it quickly (5ms) at the end of duration
                  sample = static_cast<qint16>(sample * static_cast<qreal>(playingSound.samplesCount - p_posInNote) / 220.0);
              }
          }
          p_posInOgg++;
        }
        if (unfinished && p_prevNote > INVALID_NOTE_NR && p_shiftOfPrev < CROSS_SMP) { // fade out previous note, and mix it with the current one
          qint16 sample2 = instance->oggScale->getNoteSample(p_prevNote, p_lastPosOfPrev + p_shiftOfPrev);
          sample2 = static_cast<qint16>(static_cast<qreal>(sample2) * (static_cast<qreal>(CROSS_SMP - p_shiftOfPrev) / 2200.0));
          sample = mix(sample, sample2);
          p_shiftOfPrev++;
          if (p_shiftOfPrev == CROSS_SMP)
            p_prevNote = INVALID_NOTE_NR;
        }
        qint16 beatSample = 0;
        if (instance->tickDuringPlay() && p_beatPeriod) {
          if (p_beatOffset < p_beatBytes)
            beatSample = instance->getBeatsample(p_beatOffset);
          p_beatOffset++;
          if (p_beatOffset >= p_beatPeriod)
            p_beatOffset = 0;
        }
        if (beatSample)
          sample = mix(sample, beatSample);
        for (int r = 0; r < instance->ratioOfRate; r++) {
          *out++ = sample; // left channel
          *out++ = sample; // right channel
        }
        p_posInNote++;
        sample = 0;
      }
      instance->m_callBackIsBussy = false;
      endState = p_playingNoteNr >= instance->playList().size();
  } else { // if no sound will be played: flush buffer with zeros or play a tick
      auto out = static_cast<qint16*>(outBuff);
      qint16 beatSample = 0;
      for (int i = 0; i < nBufferFrames / instance->ratioOfRate; i++) {
        beatSample = 0;
        if (p_beatPeriod && ((instance->tickBeforePlay() && p_ticksCountBefore > 0) || instance->tickDuringPlay())) {
          if (p_beatOffset < p_beatBytes)
            beatSample = instance->getBeatsample(p_beatOffset);
          p_beatOffset++;
          if (p_beatOffset >= p_beatPeriod) {
            p_beatOffset = 0;
            if (p_lastNotePlayed) { // play last beat when last note was finished, then stop metronome ticking
              p_lastNotePlayed = false;
              p_beatPeriod = 0;
            }
            if (p_ticksCountBefore > 0)
              p_ticksCountBefore--;
            else if (!instance->tickDuringPlay())
              p_beatPeriod = 0;
          }
        }
        if (instance->audioParams()->forwardInput)
          beatSample = mix(beatSample, *(static_cast<qint16*>(inBuff) + i));

        for (int r = 0; r < instance->ratioOfRate; r++) {
          *out++ = beatSample; // left channel
          *out++ = beatSample; // right channel
        }
      }
      instance->m_callBackIsBussy = false;
      endState = p_ticksCountBefore == 0 && (instance->playList().isEmpty() || p_playingNoteNr >= instance->playList().size());
  }
  if (instance->p_doEmit && endState) {
    ao()->emitPlayingFinished(); // emit in duplex mode
    instance->p_doEmit = false;
  }

  return endState;
}
/*end static*/

//#################################################################################################
//###################              CONSTRUCTOR         ############################################
//#################################################################################################
TaudioOUT::TaudioOUT(TaudioParams *_params, QObject *parent) :
  TabstractPlayer(parent),
  TrtAudio(_params, e_output, &outCallBack),
  oggScale(new ToggScale()),
  ratioOfRate(1),
  m_callBackIsBussy(false)
{
  if (instance) {
    qDebug() << "Nothing of this kind... TaudioOUT already exist!";
    return;
  }
  p_oggScale = oggScale;
  p_audioParams = audioParams();
  setType(e_audio);
  setAudioOutParams();
  instance = this;
  forceUpdate = true;
  connect(ao(), &TaudioObject::paramsUpdated, this, &TaudioOUT::updateSlot);

  connect(ao(), &TaudioObject::playingStarted, this, &TaudioOUT::playingStarted);
  connect(ao(), &TaudioObject::nextNoteStarted, this, &TaudioOUT::nextNoteStarted, Qt::DirectConnection);
  connect(ao(), &TaudioObject::playingFinished, this, &TaudioOUT::playingFinishedDelay);

  connect(oggScale, &ToggScale::noteDecoded, this, &TaudioOUT::decodeNextSlot, Qt::DirectConnection);

  // open stream if has not opened yet to avoid delay (Pulse Audio) when playing is already wanted
  QTimer::singleShot(500, this, [=]{
    if (!isOpened()) {
      openStream();
      if (getCurrentApi() == RtAudio::LINUX_PULSE)
        closeStream(); // HACK: Fixes strange delay of input data
    }
    setAudioOutParams();
  });
}


TaudioOUT::~TaudioOUT()
{
  closeStream();
  deleteOutParams();
  resetCallBack();
  delete oggScale;
  instance = nullptr;
}


void TaudioOUT::setAudioOutParams() {
//   qDebug() << "[TaudioOUT] setAudioOutParams";
  p_playable = oggScale->loadAudioData(audioParams()->audioInstrNr);
  if (p_playable && streamParams()) {
      ratioOfRate = outRate() / 44100;
      quint32 oggSR = outRate();
      if (ratioOfRate > 1) { //
        if (outRate() == 88200 || outRate() == 176400)
          oggSR = 44100;
        else if (outRate() == 96000 || outRate() == 192000)
          oggSR = 48000;
      }
      oggScale->setSampleRate(oggSR);
      // Shifts only float part of a440diff - integer part is shifted by play() method
      oggScale->setPitchOffset(audioParams()->a440diff - static_cast<qreal>(static_cast<int>(audioParams()->a440diff)));
#if defined(Q_OS_WIN)
      if (getCurrentApi() == RtAudio::WINDOWS_ASIO)
        connect(rtDevice()->emitter(), &TASIOEmitter::resetASIO, this, &TaudioOUT::ASIORestartSlot, Qt::UniqueConnection);
#endif
  } else
      p_playable = false;
}


/**
 * This method is invoked in other thread by direct connection,
 * so sleeps doesn't freeze main thread
 */
void TaudioOUT::startPlaying() {
  while (m_callBackIsBussy) {
    qDebug() << "[TaudioOUT] Oops! Call back method is in progress when a new note wants to be played!";
    QThread::currentThread()->usleep(500);
  }

  p_isPlaying = true;
  ao()->emitPlayingStarted();
  setPlayCallbackInvolved(true);
  p_lastNotePlayed = false;
  oggScale->decodeNote(playList().first().number);
  if (!oggScale->isReady()) {
    int loops = 0;
    while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
      QThread::currentThread()->msleep(1);
      loops++;
    }
    //  if (loops) qDebug() << "latency:" << loops << "ms";
  }

  if (p_prevNote > INVALID_NOTE_NR) {
    p_shiftOfPrev = 0;
    p_lastPosOfPrev = p_posInNote;
  }
  p_posInNote = 0;
  p_posInOgg = 0;
  if (playList().size() > 1 && p_tempo > 100) // in faster tempo wait for decoding more notes
    QThread::currentThread()->msleep(100);

  startStream();
  if (playList().size() > 1)
    ao()->emitNextNoteStarted();
}


/**
 * Invoke @p playingFinished with delay to give output audio device consume data was sent.
 * Due to ao()->emitPlayingFinished() is sent from another thread, we don't want to use lambda connection.
 * Below slot is performed in main thread already, timers are happy
 */
void TaudioOUT::playingFinishedDelay() {
  QTimer::singleShot(500, this, [=]{ playingFinishedSlot(); });
}


void TaudioOUT::playingFinishedSlot() {
  p_lastNotePlayed = false;
  p_isPlaying = false;
  setPlayCallbackInvolved(false);
  emit playingFinished();
}


void TaudioOUT::stop() {
  if (m_callBackIsBussy) {
    qDebug() << "[TrtAudioOUT] Stopping when outCallBack is running. Wait 2ms!";
    QTimer::singleShot(2, this, [=]{ this->stop(); });
  }

  /**
   * To avoid cracking at the end of last played note
   * when it is still playing, cheat the system by:
   * - setting samplesCount (duration) to the actual position in played note (pos in ogg) plus 220 (5ms of fade out)
   * - remove from the list remaining notes (if any),
   * so p_playingNoteNr will be the last one and outCallBack will fade it out
   */
  if (!playList().isEmpty() && p_playingNoteNr <= playList().size() - 1 && p_posInNote < playList()[p_playingNoteNr].samplesCount) {
    playList()[p_playingNoteNr].samplesCount = p_posInOgg + 219;
    int toRemove = playList().size() - p_playingNoteNr - 1;
    for (int n = 0; n < toRemove; ++n)
      playList().removeLast();
    QTimer::singleShot(50, [=]{ this->stop(); });
    return;
  }
  p_prevNote = INVALID_NOTE_NR;
  p_shiftOfPrev = 0;
  p_lastPosOfPrev = 0;
  p_isPlaying = false;
  p_ticksCountBefore = 0;
}


/**
 * Invokes decoding note from ogg when melody is playing.
 * It is connected to @p ToggScale::noteDecoded emitted when decoding of a note is finished.
 * This way all notes of melody are decoded in parallel just after melody starts playing
 */
void TaudioOUT::decodeNextSlot() {
  p_decodingNoteNr++;
  if (p_decodingNoteNr < playList().size()) {
    int noteNr = playList()[p_decodingNoteNr].number;
    if (noteNr < REST_NR)
      oggScale->decodeNote(noteNr); // when it will finish decodeNextSlot() method will be called again
    else
      decodeNextSlot(); // so call itself for the next note
  }
}


#if defined(Q_OS_WIN)
void TaudioOUT::ASIORestartSlot() {
  if (!hasCallBackIn()) // Perform restart only when no input otherwise input does it
    restartASIO();
}
#endif

