/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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

#include "tqtaudioin.h"
#include "tpitchfinder.h"
#include <taudioparams.h>
#include <QAudioInput>
#include <QIODevice>
#include <QThread>
#include <QDebug>


TaudioIN::TaudioIN(TaudioParams* params, QObject *parent) :
  TcommonListener(params, parent),
  m_inDevice(0),
  m_buffer(0),
  m_thread(new QThread)
{
  QAudioDeviceInfo defaultIn = QAudioDeviceInfo::defaultInputDevice();
  QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  for (int i = 0; i < devList.size(); ++i) {
    if (devList[i].deviceName() == "camcorder") { // camcorder voicerecognition
      defaultIn = devList[i];
      break;
    }
  }

  qDebug() << "IN:" << defaultIn.deviceName();
  QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(48000);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
  if (!defaultIn.isFormatSupported(format)) {
    qDebug() << "Format 48000/16 mono is not supported";
    format = defaultIn.nearestFormat(format);
    qDebug() << "Format is" << format.sampleRate() << format.channelCount() << format.sampleSize();
  }

  m_audioIN = new QAudioInput(defaultIn, format, this);
  m_audioIN->setBufferSize(2048);
  finder()->setCopyInThread(false);
  finder()->setNrChunksToReset(500); // Less memory usage
  finder()->setSampleRate(m_audioIN->format().sampleRate()); // framesPerChunk is determined here

//   qDebug() << "setAudioInParams" << "\nrate:" << finder()->aGl()->rate << m_audioIN->format().sampleRate() << "\nmethod:" << params->detectMethod
//           << "\nmin duration" << params->minDuration << "\nmin volume" << params->minimalVol
//           << "\nsplit volume" << (finder()->isSplitByVolume() ? finder()->minVolumeToSplit() * 100.0 : 0.0)
//           << "\nskip volume" << finder()->skipStillerValue() * 100.0
//           << "\nnoise filter:" << finder()->aGl()->equalLoudness << "\ndetection range:" << detectionRange();

  moveToThread(m_thread);
  connect(m_thread, &QThread::started, this, &TaudioIN::startThread);
  connect(m_thread, &QThread::finished, this, &TaudioIN::stopThread);
}


TaudioIN::~TaudioIN() {
  stopListening();
  if (m_thread->isRunning()) {
      m_thread->terminate();
      m_thread->quit();
  }
  m_audioIN->stop();
  m_thread->deleteLater();
}


void TaudioIN::startListening() {
  if (!stoppedByUser() && detectingState() != e_detecting)
    m_thread->start();
}


void TaudioIN::stopListening() {
  m_thread->quit();
}


void TaudioIN::startThread() {
//   if (detectingState() != e_detecting) {
    m_inDevice = m_audioIN->start();
    if (m_inDevice) {
      m_buffer = new qint16[m_audioIN->bufferSize()];
      connect(m_inDevice, &QIODevice::readyRead, this, &TaudioIN::dataReady);
      setState(e_detecting);
//       qDebug() << "started with buffer" << m_audioIN->bufferSize();
    }
//   }
}


void TaudioIN::stopThread() {
  if (detectingState() != e_stopped) {
    m_audioIN->stop();
    delete m_buffer;
    m_buffer = 0;
    resetVolume();
    resetChunkPitch();
    setState(e_stopped);
    finder()->resetFinder();
  }
}


void TaudioIN::dataReady() {
  int bytesReady = m_audioIN->bytesReady();
//   qDebug() << "latency" << latencyGap.elapsed() << bytesReady << m_audioIN->error();
  while (bytesReady > 0) {
    int dataToRead = m_inDevice->read((char*)m_buffer, m_audioIN->bufferSize()) / 2;
    for (int i = 0; i < dataToRead; i++)
      finder()->fillBuffer(float(*(m_buffer + i)) / 32760.0f);
    bytesReady = bytesReady - dataToRead * 2;
  }
}





