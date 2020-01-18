/***************************************************************************
 *   Copyright (C) 2015-2020 by Tomasz Bojczuk                             *
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


#include "tqtaudioout.h"
#include "toggscale.h"
#include "taudiobuffer.h"
#include "taudioobject.h"
#include <taudioparams.h>
#include <tglobals.h>

#include <QtMultimedia/qaudiooutput.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <QtCore/qthread.h>


#define CROSS_SMP (2200) // 50ms
#define INVALID_NOTE_NR (-100)
#define SAMPLE_LEN (79380) // 1.8sec (70560) // 1.6 //(66150) 1.5 //61740 1.4


/*static*/
QStringList TaudioOUT::getAudioDevicesList() {
  QStringList devNamesList;
  QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
  for (int i = 0; i < devList.size(); ++i)
    devNamesList << devList[i].deviceName();
  return devNamesList;
}

QString                TaudioOUT::m_devName = QStringLiteral("anything");
TaudioOUT*             TaudioOUT::m_instance = nullptr;
/*end static*/



TaudioOUT::TaudioOUT(TaudioParams *_params, QObject *parent) :
  TabstractPlayer(parent),
  oggScale(new ToggScale()),
  ratioOfRate(1),
  m_bufferFrames(1024),
  m_sampleRate(44100),
  m_callBackIsBussy(false),
  m_audioParams(_params),
  m_audioOUT(nullptr)
{
  if (m_instance) {
    qDebug() << "Nothing of this kind... TaudioOUT already exist!";
    return;
  }
  p_oggScale = oggScale;
  p_audioParams = audioParams();
  m_instance = this;
  setType(e_audio);

  setAudioOutParams();

  connect(this, &TaudioOUT::finishSignal, this, &TaudioOUT::stop);
  connect(oggScale, &ToggScale::noteDecoded, this, &TaudioOUT::decodeNext, Qt::DirectConnection);
}


TaudioOUT::~TaudioOUT()
{
  delete oggScale;
  stop();
  m_audioParams->OUTdevName = m_devName;
  m_devName = QStringLiteral("anything");
  m_instance = 0;
  if (m_audioOUT) {
    delete m_audioOUT;
    delete m_buffer;
  }
}


void TaudioOUT::setAudioOutParams() {
  p_playable = oggScale->loadAudioData(audioParams()->audioInstrNr);
  if (m_audioParams->OUTdevName != m_devName)
    createOutputDevice();
  if (p_playable) {
      ratioOfRate = m_sampleRate / 44100;
      quint32 oggSR = m_sampleRate;
      if (ratioOfRate > 1) { //
        if (m_sampleRate == 88200 || m_sampleRate == 176400)
          oggSR = 44100;
        else if (m_sampleRate == 96000 || m_sampleRate == 192000)
          oggSR = 48000;
      }
      oggScale->setSampleRate(oggSR);
      // Shifts only float part of a440diff - integer part is shifted by play() method
      oggScale->setPitchOffset(audioParams()->a440diff - static_cast<qreal>(static_cast<int>(audioParams()->a440diff)));

  } else
      p_playable = false;
}


void TaudioOUT::createOutputDevice() {
  m_deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
  auto devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
  for (int i = 0; i < devList.size(); ++i) { // find device with name or keep default one
    if (devList[i].deviceName() == m_audioParams->OUTdevName) {
      m_deviceInfo = devList[i];
      break;
    }
  }
  m_devName = m_deviceInfo.deviceName();
  QAudioFormat format;
    format.setChannelCount(1); // Mono
    format.setSampleRate(m_sampleRate);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
  if (!m_deviceInfo.isFormatSupported(format)) {
    qDebug() << "Output Format 44100/16 stereo is not supported";
    format = m_deviceInfo.nearestFormat(format);
    qDebug() << "Format is" << format.sampleRate() << format.channelCount() << format.sampleSize();
  }
  m_sampleRate = format.sampleRate();

  if (m_audioOUT) {
    delete m_audioOUT;
    delete m_buffer;
  }
  m_audioOUT = new QAudioOutput(m_deviceInfo, format, this);
  m_audioOUT->setBufferSize(m_bufferFrames * 2);

  m_buffer = new TaudioBuffer(this);
  m_buffer->open(QIODevice::ReadOnly);
  m_buffer->setBufferSize(m_audioOUT->bufferSize());

  qDebug() << "OUT:" << m_deviceInfo.deviceName() << m_audioOUT->format().sampleRate();

  connect(m_buffer, &TaudioBuffer::feedAudio, this, &TaudioOUT::outCallBack, Qt::DirectConnection);
  connect(m_audioOUT, &QAudioOutput::stateChanged, this, &TaudioOUT::stateChangedSlot);
}


#include <QtCore/qelapsedtimer.h>
QElapsedTimer pt;
void TaudioOUT::startPlaying() {
  pt.start();

  while (m_callBackIsBussy) {
    // TODO: seems like it never occurs in Qt Audio - remove it then
    QThread::currentThread()->usleep(500);
    qDebug() << "Oops! Call back method is in progress when a new note wants to be played!";
  }

  p_lastNotePlayed = false;
  oggScale->decodeNote(playList().first().number);
  if (!oggScale->isReady()) {
    int loops = 0;
    while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
      QThread::currentThread()->msleep(1);
      loops++;
    }
//     if (loops) qDebug() << "latency:" << loops << "ms";
  }

  if (p_prevNote > INVALID_NOTE_NR) {
    p_shiftOfPrev = 0;
    p_lastPosOfPrev = p_posInNote;
  }
  p_posInNote = 0;
  p_posInOgg = 0;
  if (playList().size() > 1 && p_tempo > 100) // in faster tempo wait for decoding more notes
    QThread::currentThread()->msleep(100);

  startPlayingSlot();
  if (playList().size() > 1)
    emit nextNoteStarted();
}


void TaudioOUT::startPlayingSlot() {
  if (m_audioOUT->state() != QAudio::ActiveState) {
    qDebug() << "preparation time" << pt.elapsed();
    m_audioOUT->start(m_buffer);
  }
}


void TaudioOUT::outCallBack(char *data, qint64 maxLen, qint64 &wasRead) {
//   qDebug() << "outCallBack" << m_samplesCnt << maxLen << m_audioOUT->bytesFree() << m_audioOUT->bufferSize();
  m_callBackIsBussy = true;

  bool endState = true;
  if (!playList().isEmpty() && p_playingNoteNr < playList().size() && p_ticksCountBefore == 0) {
      TsingleSound& playingSound = playList()[p_playingNoteNr];
      auto out = reinterpret_cast<qint16*>(data);
      bool unfinished = true;
      qint16 sample = 0;
      for (int i = 0; i < (maxLen / 2) / ratioOfRate; i++) {
        if (p_posInNote >= playingSound.samplesCount) {
          p_prevNote = playingSound.number == REST_NR || p_posInOgg > SAMPLE_LEN ? INVALID_NOTE_NR : playingSound.number;
          p_shiftOfPrev = 0;
          p_lastPosOfPrev = p_posInOgg;
          p_playingNoteNr++;
          if (p_playingNoteNr < playList().size()) {
              p_posInOgg = 0;
              p_posInNote = 0;
              playingSound = playList()[p_playingNoteNr];
              p_playingNoteId = playingSound.id;
              emit nextNoteStarted();
          } else {
              unfinished = false;
              p_lastNotePlayed = true; // inform beating routine to stop
          }
        }
        if (unfinished && playingSound.number < REST_NR) {
          if (oggScale->soundContinuous() && p_posInOgg > oggScale->stopLoopSample(playingSound.number))
            p_posInOgg = oggScale->startLoopSample(playingSound.number);

          if (p_posInOgg < SAMPLE_LEN) { // 1.4 sec of samples
            sample = oggScale->getNoteSample(playingSound.number, p_posInOgg);
            if (p_posInOgg < 220) // fade in 5ms
              sample = static_cast<qint16>(sample * (1.0 - (static_cast<qreal>(220 - p_posInOgg) / 220.0)));
            if (p_playingNoteNr == playList().size() - 1 && p_posInOgg > playingSound.samplesCount - 220) { // fade out 5ms the last one
              qreal m = 1.0 - (static_cast<qreal>(p_posInOgg + 221 - playingSound.samplesCount) / 220.0);
              sample = static_cast<qint16>(sample * (m < 0.0 ? 0.0 : m));
            }
            if (oggScale->soundContinuous() && p_posInNote > 44100) { // fade out long continuous sound
              // slowly fade it out to minimize loop jump effect
              sample = static_cast<qint16>(sample * (1.0 - static_cast<qreal>(p_posInNote - 44100) / static_cast<qreal>(playingSound.samplesCount)));
              if (p_posInNote > playingSound.samplesCount - 220) // but fade it quickly (5ms) at the end of duration
                sample = static_cast<qint16>(sample * static_cast<qreal>(playingSound.samplesCount - p_posInNote) / 220.0);
            }
          }
          p_posInOgg++;
        }
        if (unfinished && p_prevNote > INVALID_NOTE_NR && p_shiftOfPrev < CROSS_SMP) { // fade out previous note, and mix it with the current one
          qint16 sample2 = oggScale->getNoteSample(p_prevNote, p_lastPosOfPrev + p_shiftOfPrev);
          sample2 = static_cast<qint16>(static_cast<qreal>(sample2) * (static_cast<qreal>(CROSS_SMP - p_shiftOfPrev) / 2200.0));
          sample = mix(sample, sample2);
          p_shiftOfPrev++;
          if (p_shiftOfPrev == CROSS_SMP)
            p_prevNote = INVALID_NOTE_NR;
        }
        qint16 beatSample = 0;
        if (tickDuringPlay() && p_beatPeriod) {
          if (p_beatOffset < p_beatBytes)
            beatSample = getBeatsample(p_beatOffset);
          p_beatOffset++;
          if (p_beatOffset >= p_beatPeriod)
            p_beatOffset = 0;
        }
        if (beatSample)
          sample = mix(sample, beatSample);
        for (int r = 0; r < ratioOfRate; r++)
          *out++ = sample; // left channel
        p_posInNote++;
        sample = 0;
      }
      m_callBackIsBussy = false;
      endState = p_playingNoteNr >= playList().size();
  } else { // flush buffer with zeros if no sound will be played
      auto out = reinterpret_cast<qint16*>(data);
      qint16 beatSample = 0;
      for (int i = 0; i < (maxLen / 2) / ratioOfRate; i++) {
        if (p_beatPeriod && ((tickBeforePlay() && p_ticksCountBefore > 0) || tickDuringPlay())) {
          if (p_beatOffset < p_beatBytes)
            beatSample = getBeatsample(p_beatOffset);
          p_beatOffset++;
          if (p_beatOffset >= p_beatPeriod) {
            p_beatOffset = 0;
            if (p_lastNotePlayed) { // play last beat when last note was finished, then stop metronome ticking
              p_lastNotePlayed = false;
              p_beatPeriod = 0;
            }
            if (p_ticksCountBefore > 0)
              p_ticksCountBefore--;
            else if (!tickDuringPlay())
              p_beatPeriod = 0;
          }
        }

        for (int r = 0; r < ratioOfRate; r++)
          *out++ = beatSample;
    }
    m_callBackIsBussy = false;
    endState = p_ticksCountBefore == 0 && (playList().isEmpty() || p_playingNoteNr >= playList().size());
  }
  wasRead = maxLen;
  if (p_doEmit && endState) {
    emit finishSignal();
  }
}


void TaudioOUT::stateChangedSlot(QAudio::State state) {
//   qDebug() << "[TaudioOUT]" << state;
  p_isPlaying = (state == QAudio::ActiveState);
  if (state == QAudio::ActiveState)
    emit playingStarted();
  else if (state == QAudio::StoppedState)
    emit playingFinished();
  else if (state == QAudio::IdleState)
    stop();
//     playingFinishedSlot();
}


void TaudioOUT::playingFinishedSlot() {
//   qDebug() << "stop playing";
  m_audioOUT->stop();
//   if (p_doEmit)
//     emit playingFinished();
  p_doEmit = false; // emit once per play/stop cycle
}


void TaudioOUT::stop() {
  // TODO fade out latest played note to avoid cracks: Simply uncomment code below
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
  p_ticksCountBefore = 0;
  p_doEmit = false;
  playingFinishedSlot();
}


void TaudioOUT::decodeNext() {
  p_decodingNoteNr++;
//   qDebug() << "decodeNext" << p_decodingNoteNr << "of" << playList().size() << pt.elapsed() << "[ms]";
//   pt.restart();
  if (p_decodingNoteNr < playList().size()) {
    int noteNr = playList()[p_decodingNoteNr].number;
    if (noteNr < REST_NR)
      oggScale->decodeNote(noteNr);
    else
      decodeNext();
  }
}

