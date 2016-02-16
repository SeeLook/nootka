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

#include "tqtaudioin.h"
#include "tpitchfinder.h"
#include <taudioparams.h>
#include <QtMultimedia/qaudioinput.h>
#include <QtCore/qiodevice.h>
#include <QtCore/qthread.h>
#include <QtCore/qdebug.h>


/*static */
QStringList TaudioIN::getAudioDevicesList() {
  QStringList devNamesList;
  QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  for (int i = 0; i < devList.size(); ++i)
    devNamesList << devList[i].deviceName();
  return devNamesList;
}


TaudioIN*        			TaudioIN::m_instance = 0;
QString          			TaudioIN::m_deviceName = QStringLiteral("anything");


TaudioIN::TaudioIN(TaudioParams* params, QObject *parent) :
  TcommonListener(params, parent),
  m_audioParams(params),
  m_audioIN(0),
  m_inDevice(0),
  m_buffer(0),
  m_thread(new QThread)
{
  if (m_instance) {
    qDebug() << "Nothing of this kind... TaudioIN already exist!";
    return;
  }
  m_instance = this;
  finder()->setCopyInThread(false);
//   finder()->setNrChunksToReset(500); // Less memory usage

  createInputDevice();

  moveToThread(m_thread);
  connect(m_thread, &QThread::started, this, &TaudioIN::startThread);
  connect(m_thread, &QThread::finished, this, &TaudioIN::stopThread);
}


TaudioIN::~TaudioIN() {
  stopListening();
  m_audioIN->stop();
  finder()->blockSignals(true);
  if (m_thread->isRunning()) { // In fact, it never goes here
      qDebug() << "Terminating audio input thread";
      m_thread->terminate();
      m_thread->quit();
      stopThread();
  }
  m_audioParams->INdevName = m_deviceName; // store device name at the app exit
  m_deviceName = QStringLiteral("anything");
  delete m_thread;
  m_instance = 0;
  if (m_audioIN)
    delete m_audioIN;
}


void TaudioIN::updateAudioParams() {
  if (m_audioIN && m_audioParams->INdevName != m_deviceName) // device changed
    createInputDevice();
  TcommonListener::setAudioInParams();
}


void TaudioIN::startListening() {
  if (!stoppedByUser() && detectingState() != e_detecting) {
    m_thread->start();
    setState(e_detecting);
  }
}


void TaudioIN::stopListening() {
  m_thread->quit();
  setState(e_stopped);
}

//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TaudioIN::createInputDevice() {
  m_deviceInfo = QAudioDeviceInfo::defaultInputDevice();
  QList<QAudioDeviceInfo> devList = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  for (int i = 0; i < devList.size(); ++i) { // find device with name or keep default one
    if (devList[i].deviceName() == m_audioParams->INdevName) {
      m_deviceInfo = devList[i];
      break;
    }
  }
  m_deviceName = m_deviceInfo.deviceName();
  qDebug() << "IN:" << m_deviceName;
  QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(48000);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
  if (!m_deviceInfo.isFormatSupported(format)) {
    qDebug() << "Format 48000/16 mono is not supported";
    format = m_deviceInfo.nearestFormat(format);
    qDebug() << "Format is" << format.sampleRate() << format.channelCount() << format.sampleSize();
  }

  if (m_audioIN)
    delete m_audioIN;
  m_audioIN = new QAudioInput(m_deviceInfo, format, this);
  m_audioIN->setBufferSize(2048);
  finder()->setSampleRate(m_audioIN->format().sampleRate()); // framesPerChunk is determined here

//   qDebug() << "setAudioInParams" << "\nrate:" << finder()->aGl()->rate << m_audioIN->format().sampleRate() << "\nmethod:" << params->detectMethod
//           << "\nmin duration" << params->minDuration << "\nmin volume" << params->minimalVol
//           << "\nsplit volume" << (finder()->isSplitByVolume() ? finder()->minVolumeToSplit() * 100.0 : 0.0)
//           << "\nskip volume" << finder()->skipStillerValue() * 100.0
//           << "\nnoise filter:" << finder()->aGl()->equalLoudness << "\ndetection range:" << detectionRange();
  
}


void TaudioIN::startThread() {
  m_inDevice = m_audioIN->start();
  if (m_inDevice) {
    m_buffer = new qint16[m_audioIN->bufferSize()];
    connect(m_inDevice, &QIODevice::readyRead, this, &TaudioIN::dataReady);
    //       qDebug() << "started with buffer" << m_audioIN->bufferSize();
  }
}


void TaudioIN::stopThread() {
  m_audioIN->stop();
  if (m_buffer) {
    delete m_buffer;
    m_buffer = 0;
  }
  resetVolume();
  resetChunkPitch();
  finder()->resetFinder();
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





