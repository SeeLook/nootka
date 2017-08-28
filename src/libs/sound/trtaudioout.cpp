/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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
#include <taudioparams.h>
#include <music/tnote.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <unistd.h>


#if defined(Q_OS_WIN32)
  #include <windows.h>
  #define SLEEP(msecs) Sleep(msecs)
#else
  #define SLEEP(msecs) usleep(msecs * 1000)
#endif

/**
 * Value of CROSS_STEP = 0.0023 (100ms) cases reverb effect with sound
 * Shorter cross-fade duration works better.
 */
#define CROSS_STEP (0.001f) // cross-fade will take about 23 [ms] - 1000 samples (it determines m_crossBuffer size)

#define REST_NR (127)


class TsingleSound {
  public:
    TsingleSound(int nId = -1, qint16 nr = REST_NR, quint32 samples = 0) : id(nId), number(nr), samplesCount(samples) {}
    int id; /** Note id in the melody */
    qint8 number; /**< number of note (chromatic) - none (rest) by default */
    quint32 samplesCount; /**< Number of samples the note takes */
};


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
unsigned int            TaudioOUT::m_posInNote = 0;
unsigned int            TaudioOUT::m_posInOgg = 0;
int                     TaudioOUT::m_playingNoteNr = -1;
int                     TaudioOUT::m_decodingNoteNr = -1;
int                     TaudioOUT::m_playingNoteId = -1;


bool TaudioOUT::outCallBack(void* outBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status) {
  Q_UNUSED(status)
  instance->m_callBackIsBussy = true;
//   if (status) // It doesn't harm if occurs
//       qDebug() << "Stream underflow detected!";

  if (m_playingNoteNr < instance->m_playList.size()) {
      auto playingSound = instance->m_playList[m_playingNoteNr];
      qint16 sample;
        qint16 *out = static_cast<qint16*>(outBuff);
        for (int i = 0; i < nBufferFrames / instance->ratioOfRate; i++) {
          if (m_posInNote >= playingSound.samplesCount) {
            m_playingNoteNr++;
            if (m_playingNoteNr < instance->m_playList.size()) {
                m_posInOgg = 0;
                m_posInNote = 0;
                playingSound = instance->m_playList[m_playingNoteNr];
                m_playingNoteId = playingSound.id;
            } else {
                instance->m_callBackIsBussy = false;
                return true; // playlist finished
            }
          }
          if (playingSound.number < REST_NR && m_posInOgg < 66150) {
              sample = instance->oggScale->getNoteSample(playingSound.number, m_posInOgg);
              m_posInOgg++;
          } else
              sample = 0;
          for (int r = 0; r < instance->ratioOfRate; r++) {
            *out++ = sample; // left channel
            *out++ = sample; // right channel
          }
          m_posInNote++;
        }
      instance->m_callBackIsBussy = false;
      return false;
  } else {
      instance->m_callBackIsBussy = false;
      return true;
  }


/*  if (m_doCrossFade) { // Cross-fading avoids cracking during transition of notes.
      m_doCrossFade = false;
      m_cross = 1.0f;
      p_instance->m_crossCount = 0;
  }
  if (m_samplesCnt < m_maxCBloops) {
      m_samplesCnt++;
      qint16 *out = (qint16*)outBuff;
      int off = m_samplesCnt * (nBufferFrames / p_instance->p_ratioOfRate);
      qint16 sample;
      for (int i = 0; i < nBufferFrames / p_instance->p_ratioOfRate; i++) {
        if (m_cross > 0.0 && p_instance->m_crossCount < 1000) { // mix current and previous samples when cross-fading
              sample = (qint16)qRound((1.0 - m_cross) * (float)p_instance->p_oggScale->getSample(off + i) +
                      m_cross * (float)m_crossBuffer[p_instance->m_crossCount]);
              m_cross -= CROSS_STEP;
              p_instance->m_crossCount++;
        } else {
              sample = p_instance->p_oggScale->getSample(off + i);
        }
        for (int r = 0; r < p_instance->p_ratioOfRate; r++) {
            *out++ = sample; // left channel
            *out++ = sample; // right channel
        }
      }
      p_instance->m_callBackIsBussy = false;
      if (m_samplesCnt == m_maxCBloops)
        return true;
      else
        return false;
  } else {
      p_instance->m_callBackIsBussy = false;
      return true;
  }*/
}
/*end static*/

//---------------------------------------------------------------------------------------
//                CONSTRUCTOR
//---------------------------------------------------------------------------------------
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
      oggScale->setPitchOffset(audioParams()->a440diff - static_cast<float>(static_cast<int>(audioParams()->a440diff)));
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

//   if (m_samplesCnt < m_maxCBloops) {
//       int off = (m_samplesCnt + 1) * (bufferFrames() / p_ratioOfRate); // next chunk of playing sound
//       for (int i = 0; i < 1000; i++) // copy data of current sound to perform crrosfading
//         m_crossBuffer[i] = p_oggScale->getSample(off + i);
//       m_doCrossFade = true;
//   } else
//       m_doCrossFade = false;
// 
//   noteNr = noteNr + int(audioParams()->a440diff);

  doEmit = true;

  m_playList.clear();
  m_playList << TsingleSound(0, noteNr, qRound(oggScale->sampleRate() * 1.5));
  m_playingNoteNr = 0;
  m_playingNoteId = 0;
  m_decodingNoteNr = 0;
  m_posInNote = 0;
  m_posInOgg = 0;

  oggScale->decodeNote(m_playList.first().number + static_cast<int>(audioParams()->a440diff));
  int loops = 0;
  while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
    SLEEP(1);
    loops++;
  }

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

  m_playList.clear();
  for (int n = firstNote; n < notes.count(); ++n) {
    const Tnote& tmpN = notes.at(n);
    int samplesDuration = qRound(((tmpN.duration() > 0 ? tmpN.duration() / 24.0 : 1.0) * (60000.0 / tempo)) * (oggScale->sampleRate() / 1000.0));
    if (tmpN.rtm.tie() > Trhythm::e_tieStart) // append duration if tie is continued or at end
      m_playList.last().samplesCount += samplesDuration;
    else
      m_playList << TsingleSound(n, tmpN.isValid() ? tmpN.chromatic() + static_cast<int>(audioParams()->a440diff) : REST_NR, samplesDuration);
  }
  m_playingNoteNr = 0;
  m_playingNoteId = 0;
  m_decodingNoteNr = 0;
  m_posInNote = 0;
  m_posInOgg = 0;

  oggScale->decodeNote(m_playList.first().number);
  if (oggScale->isReady())
      QTimer::singleShot(10, [=]{ decodeNext(); });
  else {
    int loops = 0;
    while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
      SLEEP(1);
      loops++;
    }
  }

  if (areStreamsSplit() && state() != e_playing)
    openStream();
  startStream();
}



void TaudioOUT::playingFinishedSlot() {
  if (areStreamsSplit() && state() == e_playing) {
    closeStream();
  }
}


void TaudioOUT::stop() {
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
  m_decodingNoteNr++;
  if (m_decodingNoteNr < m_playList.size()) {
    int noteNr = m_playList[m_decodingNoteNr].number;
    if (noteNr < REST_NR)
      oggScale->decodeNote(noteNr);
    if (oggScale->isReady())
      decodeNext();
  }
}


#if defined(Q_OS_WIN)
void TaudioOUT::ASIORestartSlot() {
  if (!hasCallBackIn()) // Perform restart only when no input otherwise input does it
    restartASIO();
}
#endif

