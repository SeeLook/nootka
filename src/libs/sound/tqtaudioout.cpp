/***************************************************************************
 *   Copyright (C) 2015-2017 by Tomasz Bojczuk                             *
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
#include <taudioparams.h>
#include <tglobals.h>

#include <QtMultimedia/qaudiooutput.h>
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include <unistd.h>


#if defined(Q_OS_WIN32)
  #include <windows.h>
  #define SLEEP(msecs) Sleep(msecs)
#else
  #define SLEEP(msecs) usleep(msecs * 1000)
#endif


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
  m_doCrossFade(false),
  m_cross(0.0f),
  m_crossCount(0),
  m_callBackIsBussy(false),
  m_audioParams(_params),
  m_audioOUT(nullptr)
{
  if (m_instance) {
    qDebug() << "Nothing of this kind... TaudioOUT already exist!";
    return;
  }
  m_instance = this;
  setType(e_audio);
  m_crossBuffer = new qint16[1000];

  setAudioOutParams();

  connect(this, &TaudioOUT::finishSignal, this, &TaudioOUT::playingFinishedSlot);
  connect(oggScale, &ToggScale::oggReady, this, &TaudioOUT::startPlayingSlot, Qt::DirectConnection);
  // With direct connection playing is called immediately after minimal audio data amount was decoded
  // without event loop delay like for default queued connection
  connect(oggScale, &ToggScale::noteDecoded, this, &TaudioOUT::decodeNext);
}


TaudioOUT::~TaudioOUT()
{
  delete oggScale;
  delete m_crossBuffer;
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
  playable = oggScale->loadAudioData(audioParams()->audioInstrNr);
  if (m_audioParams->OUTdevName != m_devName)
    createOutputDevice();
  if (playable) {
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
      oggScale->setPitchOffset(audioParams()->a440diff - (float)int(audioParams()->a440diff));

  } else
      playable = false;
}


void TaudioOUT::createOutputDevice() {
  m_deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
  QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
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

  m_maxSamples = m_sampleRate * 1.5;

  qDebug() << "OUT:" << m_deviceInfo.deviceName() << m_audioOUT->format().sampleRate() << m_maxSamples;

  connect(m_buffer, &TaudioBuffer::feedAudio, this, &TaudioOUT::outCallBack, Qt::DirectConnection);
  connect(m_audioOUT, &QAudioOutput::stateChanged, this, &TaudioOUT::stateChangedSlot);
}

#include <QtCore/qelapsedtimer.h>
QElapsedTimer pt;
bool TaudioOUT::play(int noteNr) {
  if (!playable)
      return false;

  pt.start();
  while (m_callBackIsBussy) {
      SLEEP(1); // TODO: seems like it never occurs in Qt Audio - remove it then
//        qDebug() << "Oops! Call back method is in progress when a new note wants to be played!";
  }

  playList().clear();
  playList() << TsingleSound(0, noteNr + static_cast<int>(audioParams()->a440diff), qRound(oggScale->sampleRate() * 1.5));


  doEmit = true;
  p_playingNoteNr = 0;
  //   p_playingNoteId = 0; // we have no any idea about current note id here
  p_decodingNoteNr = 0;
  p_posInNote = 0;
  p_posInOgg = 0;

  oggScale->decodeNote(playList().first().number);
  p_isPlaying = true;

  return true;
}


void TaudioOUT::playMelody(const QList<Tnote>& notes, int tempo, int firstNote) {
  if (!playable)
    return;

  pt.start();
  while (m_callBackIsBussy)
    SLEEP(1);

  if (firstNote < 0 || firstNote >= notes.count())
    return;

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
  QTimer::singleShot(tempo > 100 ? 150 : 10, [=]{
    emit nextNoteStarted();
  });
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

  if (p_playingNoteNr < playList().size()) {
    TsingleSound& playingSound = playList()[p_playingNoteNr];
    auto out = reinterpret_cast<qint16*>(data);
    bool unfinished = true;
    qint16 sample = 0;
    for (int i = 0; i < (maxLen / 2) / ratioOfRate; i++) {
      if (p_posInNote >= playingSound.samplesCount) {
        p_playingNoteNr++;
        if (p_playingNoteNr < playList().size()) {
            p_posInOgg = 0;
            p_posInNote = 0;
            playingSound = playList()[p_playingNoteNr];
            p_playingNoteId = playingSound.id;
            emit nextNoteStarted();
        } else
            unfinished = false;
      }
      if (unfinished && playingSound.number < REST_NR) {
        if (oggScale->soundContinuous() && p_posInOgg > oggScale->stopLoopSample(playingSound.number))
          p_posInOgg = oggScale->startLoopSample(playingSound.number);

        if (p_posInOgg < 61740) { // 1.4 sec of samples
          sample = oggScale->getNoteSample(playingSound.number, p_posInOgg);
          if (p_posInOgg > playingSound.samplesCount - 220) { // fade out 5ms
            qreal m = 1.0 - (static_cast<qreal>(p_posInOgg + 221 - playingSound.samplesCount) / 220.0);
            sample = static_cast<qint16>(sample * (m < 0.0 ? 0.0 : m));
          } else if (p_posInOgg < 220) // fade in 5ms
            sample = static_cast<qint16>(sample * (1.0 - (static_cast<qreal>(220 - p_posInOgg) / 220.0)));
          if (oggScale->soundContinuous() && p_posInNote > 44100) { // fade out long continuous sound
            // slowly fade it out to minimize loop jump effect
            sample = static_cast<qint16>(sample * (1.0 - static_cast<qreal>(p_posInNote - 44100) / static_cast<qreal>(playingSound.samplesCount)));
            if (p_posInNote > playingSound.samplesCount - 220) // but fade it quickly (5ms) at the end of duration
              sample = static_cast<qint16>(sample * static_cast<qreal>(playingSound.samplesCount - p_posInNote) / 220.0);
          }
        }
        p_posInOgg++;
      }
      for (int r = 0; r < ratioOfRate; r++)
        *out++ = sample; // left channel
      p_posInNote++;
      sample = 0;
      wasRead = maxLen;
    }
    m_callBackIsBussy = false;
  } else { // flush buffer with zeros if no sound will be played
      auto out = reinterpret_cast<qint16*>(data);
      for (int i = 0; i < (maxLen / 2) / ratioOfRate; i++) {
        for (int r = 0; r < ratioOfRate; r++)
        *out++ = 0;
    }
    m_callBackIsBussy = false;
  }
}


void TaudioOUT::stateChangedSlot(QAudio::State state) {
//   qDebug() << state;
  if (state == QAudio::IdleState)
    playingFinishedSlot();
}


void TaudioOUT::playingFinishedSlot() {
//   qDebug() << "stop playing";
  m_audioOUT->stop();
  p_isPlaying = false;
  if (doEmit)
    emit noteFinished();
  doEmit = false; // emit once per play/stop cycle
}


void TaudioOUT::stop() {
  doEmit = false;
  playingFinishedSlot();
}


void TaudioOUT::decodeNext() {
  p_decodingNoteNr++;
  qDebug() << "decodeNext" << p_decodingNoteNr << "of" << playList().size() << pt.elapsed() << "[ms]";
  pt.restart();
  if (p_decodingNoteNr < playList().size()) {
    int noteNr = playList()[p_decodingNoteNr].number;
    if (noteNr < REST_NR)
      oggScale->decodeNote(noteNr);
    else
      decodeNext();
  }
}

