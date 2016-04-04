/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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
#include <QtMultimedia/qaudiooutput.h>
#include <QtCore/qdebug.h>
#include <unistd.h>


#if defined(Q_OS_WIN32)
  #include <windows.h>
  #define SLEEP(msecs) Sleep(msecs)
#else
  #define SLEEP(msecs) usleep(msecs * 1000)
#endif

/** Value of CROSS_STEP = 0.00023 (100ms) cases reverb effect with sound
* Shorter cross-fade duration works better. */
#define CROSS_STEP (0.001f) // cross-fade will take about 23 [ms] - 1000 samples (it determines m_crossBuffer size)


/*static*/
QStringList TaudioOUT::getAudioDevicesList() {
  QStringList devNamesList;
  QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
  for (int i = 0; i < devList.size(); ++i)
    devNamesList << devList[i].deviceName();
  return devNamesList;
}

QString                TaudioOUT::m_devName = QStringLiteral("anything");
TaudioOUT*             TaudioOUT::m_instance = 0;
/*end static*/



TaudioOUT::TaudioOUT(TaudioParams *_params, QObject *parent) :
  TabstractPlayer(parent),
  m_audioParams(_params),
  oggScale(new ToggScale()),
  ratioOfRate(1),
  m_samplesCnt(100000),
  m_bufferFrames(1024),
  m_sampleRate(44100),
  m_doCrossFade(false),
  m_cross(0.0f),
  m_crossCount(0),
  m_callBackIsBussy(false),
  m_audioOUT(0)
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
    format.setChannelCount(1);
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
       qDebug() << "Oops! Call back method is in progress when a new note wants to be played!";
  }

  if (m_samplesCnt < m_maxSamples) {
      int off = m_samplesCnt + 1; // next chunk of playing sound
      for (int i = 0; i < 1000; i++) // copy data of current sound to perform crrosfading
        m_crossBuffer[i] = oggScale->getSample(off + i);
      m_doCrossFade = true;
  } else
      m_doCrossFade = false;

  noteNr = noteNr + int(audioParams()->a440diff);

  doEmit = true;
  oggScale->setNote(noteNr); // oggScale will emit oggReady() and invoke startPlayingSlot()

  return true;
}


void TaudioOUT::startPlayingSlot() {
  qDebug() << "preparation time" << pt.elapsed();
  m_samplesCnt = -1;
  if (m_audioOUT->state() != QAudio::ActiveState)
    m_audioOUT->start(m_buffer);
}


void TaudioOUT::outCallBack(char *data, qint64 maxLen, qint64 &wasRead) {
//   qDebug() << "outCallBack" << m_samplesCnt << maxLen << m_audioOUT->bytesFree() << m_audioOUT->bufferSize();
  if (maxLen && m_samplesCnt < m_maxSamples) {
    m_callBackIsBussy = true;
    if (m_doCrossFade) { // Cross-fading avoids cracking during transition of notes.
        m_doCrossFade = false;
        m_cross = 1.0f;
        m_crossCount = 0;
    }
//    if (m_samplesCnt > oggScale->alreadyDecoded()) {
//      qDebug() << "Uh-uh! Ogg data not decoded yet!" << m_samplesCnt << oggScale->alreadyDecoded();
//      while (m_samplesCnt > oggScale->alreadyDecoded()) {
//        SLEEP(1);
//        qDebug() << "waiting";
//      }
//    }
    qint16 *outPtr = (qint16*)data;
    for (int i = 0; i < (maxLen / 2) / ratioOfRate; i++) {
      m_samplesCnt++;
      qint16 sample;
        if (m_cross > 0.0 && m_crossCount < 1000) { // mix current and previous samples when cross-fading
              sample =  (qint16)qRound((1.0 - m_cross) * (float)oggScale->getSample(m_samplesCnt) +
                      m_cross * (float)m_crossBuffer[m_crossCount]);
              m_cross -= CROSS_STEP;
              m_crossCount++;
        } else {
              sample = oggScale->getSample(m_samplesCnt);
        }
        for (int r = 0; r < ratioOfRate; r++)
            *outPtr++ = sample; // left channel
    }
    m_callBackIsBussy = false;
//    if (m_samplesCnt < m_maxSamples)
      wasRead = maxLen;
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
  m_samplesCnt = 100000;
  if (doEmit)
    emit noteFinished();
  doEmit = false; // emit once per play/stop cycle
}


void TaudioOUT::stop() {
  doEmit = false;
  playingFinishedSlot();
}



