/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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


bool TaudioOUT::outCallBack(void* outBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status) {
  Q_UNUSED(status)
  instance->m_callBackIsBussy = true;
//   if (status) // It doesn't harm if occurs
//       qDebug() << "Stream underflow detected!";

  bool endState = true;
  if (!instance->playList().isEmpty() && p_playingNoteNr < instance->playList().size()) {
      TsingleSound& playingSound = instance->playList()[p_playingNoteNr];
      auto out = static_cast<qint16*>(outBuff);
      bool unfinished = true;
      qint16 sample = 0;
      for (int i = 0; i < nBufferFrames / instance->ratioOfRate; i++) {
        if (p_posInNote >= playingSound.samplesCount) {
          p_playingNoteNr++;
          if (p_playingNoteNr < instance->playList().size()) {
              p_posInOgg = 0;
              p_posInNote = 0;
              playingSound = instance->playList()[p_playingNoteNr];
              p_playingNoteId = playingSound.id;
              ao()->emitNextNoteStarted();
          } else
              unfinished = false;
        }
        if (unfinished && playingSound.number < REST_NR) {
          if (instance->oggScale->soundContinuous() && p_posInOgg > instance->oggScale->stopLoopSample(playingSound.number))
            p_posInOgg = instance->oggScale->startLoopSample(playingSound.number);

          if (p_posInOgg < 61740) { // 1.4 sec of samples
              sample = instance->oggScale->getNoteSample(playingSound.number, p_posInOgg);
              if (p_posInOgg > playingSound.samplesCount - 220) { // fade out 5ms
                  qreal m = 1.0 - (static_cast<qreal>(p_posInOgg + 221 - playingSound.samplesCount) / 220.0);
                  sample = static_cast<qint16>(sample * (m < 0.0 ? 0.0 : m));
              } else if (p_posInOgg < 220) // fade in 5ms
                  sample = static_cast<qint16>(sample * (1.0 - (static_cast<qreal>(220 - p_posInOgg) / 220.0)));
              if (instance->oggScale->soundContinuous() && p_posInNote > 44100) { // fade out long continuous sound
                // slowly fade it out to minimize loop jump effect
                sample = static_cast<qint16>(sample * (1.0 - static_cast<qreal>(p_posInNote - 44100) / static_cast<qreal>(playingSound.samplesCount)));
                if (p_posInNote > playingSound.samplesCount - 220) // but fade it quickly (5ms) at the end of duration
                  sample = static_cast<qint16>(sample * static_cast<qreal>(playingSound.samplesCount - p_posInNote) / 220.0);
              }
          }
          p_posInOgg++;
        }
        for (int r = 0; r < instance->ratioOfRate; r++) {
          *out++ = sample; // left channel
          *out++ = sample; // right channel
        }
        p_posInNote++;
        sample = 0;
      }
      instance->m_callBackIsBussy = false;
      endState = p_playingNoteNr >= instance->playList().size();
  } else { // flush buffer with zeros if no sound will be played
      auto out = static_cast<qint32*>(outBuff); // 4 bytes for both channels at once
      for (int i = 0; i < nBufferFrames / instance->ratioOfRate; i++) {
        for (int r = 0; r < instance->ratioOfRate; r++)
          *out++ = 0; // both channels at once channel
      }
      instance->m_callBackIsBussy = false;
      endState = true;
  }
  if (instance->p_doEmit && !areStreamsSplit() && endState)
    ao()->emitPlayingFinished(); // emit in duplex mode

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
  connect(ao(), &TaudioObject::playingFinished, this, &TaudioOUT::playingFinishedSlot);
  connect(ao(), &TaudioObject::nextNoteStarted, [=]{ emit nextNoteStarted(); });

  connect(oggScale, &ToggScale::noteDecoded, this, &TaudioOUT::decodeNextSlot, Qt::DirectConnection);
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
//   qDebug() << "setAudioOutParams";
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


void TaudioOUT::startPlaying() {
  while (m_callBackIsBussy) {
    playThread()->usleep(500);
    qDebug() << "[TaudioOUT] Oops! Call back method is in progress when a new note wants to be played!";
  }

  oggScale->decodeNote(playList().first().number);
  if (!oggScale->isReady()) {
    int loops = 0;
    while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
      playThread()->msleep(1);
      loops++;
    }
    //  if (loops) qDebug() << "latency:" << loops << "ms";
  }

  p_isPlaying = true;
  if (playList().size() > 1 && p_tempo > 100) // in faster tempo wait for decoding more notes
    playThread()->msleep(100);
  if (areStreamsSplit() && state() != e_playing)
    openStream();
  startStream();
  if (playList().size() > 1)
    ao()->emitNextNoteStarted();
}


void TaudioOUT::playingFinishedSlot() {
  if (areStreamsSplit() && state() == e_playing)
    closeStream();

  p_isPlaying = false;

  if (p_doEmit) {
    emit noteFinished();
    p_doEmit = false;
  }
}


void TaudioOUT::stop() {
  p_isPlaying = false;
  if (areStreamsSplit() || getCurrentApi() == RtAudio::LINUX_PULSE)
    closeStream();
  else
    abortStream();
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

