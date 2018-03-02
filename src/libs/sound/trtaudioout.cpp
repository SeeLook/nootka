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
#include <unistd.h>

#include <QtCore/qdebug.h>


#if defined(Q_OS_WIN32)
  #include <windows.h>
  #define SLEEP(msecs) Sleep(msecs)
#else
  #define SLEEP(msecs) usleep(msecs * 1000)
#endif


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
              ao()->nextNoteStarted();
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
  if (instance->doEmit && !areStreamsSplit() && endState)
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
  setType(e_audio);
  setAudioOutParams();
  instance = this;
  forceUpdate = true;
  connect(ao(), &TaudioObject::paramsUpdated, this, &TaudioOUT::updateSlot);
  connect(ao(), &TaudioObject::playingFinished, this, &TaudioOUT::playingFinishedSlot);
  connect(ao(), &TaudioObject::nextNoteStarted, [=]{ emit nextNoteStarted(); });

  connect(oggScale, &ToggScale::noteDecoded, this, &TaudioOUT::decodeNext);
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
  playable = oggScale->loadAudioData(audioParams()->audioInstrNr);
  if (playable && streamParams()) {
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
      playable = false;
}


bool TaudioOUT::play(int noteNr) {
  if (!playable /*|| audioParams()->forwardInput*/) // if forwarding is enabled play() makes no sense
      return false;

  while (m_callBackIsBussy) {
      SLEEP(1);
//       qDebug() << "Oops! Call back method is in progress when a new note wants to be played!";
  }

  doEmit = true;

  playList().clear();
  playList() << TsingleSound(0, noteNr + static_cast<int>(audioParams()->a440diff), qRound(oggScale->sampleRate() * 1.5));
  p_playingNoteNr = 0;
//   p_playingNoteId = 0; // we have no any idea about current note id here
  p_decodingNoteNr = 0;
  p_posInNote = 0;
  p_posInOgg = 0;

  oggScale->decodeNote(playList().first().number);
  int loops = 0;
  while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
    SLEEP(1);
    loops++;
  }

  p_isPlaying = true;

//   if (loops) qDebug() << "latency:" << loops << "ms";
  if (areStreamsSplit() && state() != e_playing)
    openStream();
  return startStream();
}


void TaudioOUT::playMelody(const QList<Tnote>& notes, int tempo, int firstNote) {
  if (!playable)
    return;

  while (m_callBackIsBussy)
    SLEEP(1);

  if (firstNote < 0 || firstNote >= notes.count()) {
    qDebug() << "[TaudioOUT] Not such a note in a melody!";
    return;
  }

  preparePlayList(notes, tempo, firstNote, oggScale->sampleRate(), GLOB->transposition(), static_cast<int>(audioParams()->a440diff));
  if (playList().isEmpty()) // naughty user wants to play tied note at the score end
    return;

  p_playingNoteNr = 0;
  p_playingNoteId = playList().first().id;
  p_decodingNoteNr = 0;
  p_posInNote = 0;
  p_posInOgg = 0;
  doEmit = true;
  p_isPlaying = true;

  oggScale->decodeNote(playList().first().number);
  if (!oggScale->isReady()) {
      int loops = 0;
      while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
        SLEEP(1);
        loops++;
      }
//       if (loops) qDebug() << "latency:" << loops << "ms";
  }

  // in faster tempo wait for decoding of some notes
  QTimer::singleShot(tempo > 100 ? 50 : 1, [=]{
    if (areStreamsSplit() && state() != e_playing)
      openStream();
    startStream();
    ao()->emitNextNoteStarted();
  });
}


void TaudioOUT::playingFinishedSlot() {
  if (areStreamsSplit() && state() == e_playing)
    closeStream();

  p_isPlaying = false;

  if (doEmit) {
    emit noteFinished();
    doEmit = false;
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
 * This way all notes of melody are decoded in paralel just after melody starts playing
 */
void TaudioOUT::decodeNext() {
  p_decodingNoteNr++;
  if (p_decodingNoteNr < playList().size()) {
    int noteNr = playList()[p_decodingNoteNr].number;
    if (noteNr < REST_NR)
      oggScale->decodeNote(noteNr); // when it will finish this method will call again
    else
      decodeNext(); // so call itself for the next note
  }
}


#if defined(Q_OS_WIN)
void TaudioOUT::ASIORestartSlot() {
  if (!hasCallBackIn()) // Perform restart only when no input otherwise input does it
    restartASIO();
}
#endif

