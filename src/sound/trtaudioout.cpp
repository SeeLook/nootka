/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "trtaudioout.h"
#include "taudioparams.h"
#include <QDebug>

#define SAMPLE_RATE (44100)

/*static*/
QStringList TaudioOUT::getAudioDevicesList() {
    QStringList devList;
    RtAudio rta;
//     rta.showWarnings(false);
    int devCnt = rta.getDeviceCount();
    if (devCnt < 1)
        return devList;
    for (int i = 0; i < devCnt; i++) {
        RtAudio::DeviceInfo devInfo;
        devInfo = rta.getDeviceInfo(i);
        if (devInfo.probed && devInfo.outputChannels > 0)
          devList << QString::fromStdString(devInfo.name);
    }
    return devList;
}


int TaudioOUT::m_samplesCnt = 0;
unsigned int TaudioOUT::m_bufferFrames = 1024;
int TaudioOUT::m_maxCBloops = SAMPLE_RATE / m_bufferFrames;
int TaudioOUT::m_noteOffset = 0;
TaudioOUT* TaudioOUT::m_this = 0;


int TaudioOUT::outCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames, double streamTime, 
                           RtAudioStreamStatus status, void* userData) {
  if ( status )
    qDebug() << "Stream underflow detected!";
  m_samplesCnt++;
  if (m_samplesCnt < m_maxCBloops - 10) {
//       qint16 *data = (qint16*)userData;
      qint16 *out = (qint16*)outBuffer;
      int off = m_samplesCnt * nBufferFrames / 2;
      qint16 sample;
      for (int i = 0; i < nBufferFrames / 2; i++) {
          sample = m_this->getSample(m_noteOffset + i + off);
//           sample = data[m_noteOffset + i + off];
          *out++ = sample;
          *out++ = sample;
          *out++ = sample;
          *out++ = sample;
      }
      return 0;
  } else {
      m_this->emitNoteFinished();
      return 1;
  }
}
/*end static*/

//---------------------------------------------------------------------------------------
//                CONSTRUCTOR
//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams *params, QString &path, QObject *parent) :
  TabstractPlayer(parent),
  TscaleFile(path),
  m_devName("anything"),
  m_rtAudio(0),
  m_params(params),
  m_streamOptions(0)
{
  setAudioOutParams(params);
  m_this = this;
}


TaudioOUT::~TaudioOUT() 
{
  delete m_streamOptions;
  delete m_rtAudio;
}

void TaudioOUT::emitNoteFinished() {
  if (m_rtAudio->isStreamRunning())
    m_rtAudio->stopStream();
  emit noteFinished();
}


void TaudioOUT::setAudioOutParams(TaudioParams* params) {
  if (m_devName != params->OUTdevName || !m_rtAudio) {
    playable = loadAudioData();
    if (playable && setAudioDevice(params->OUTdevName))
        playable = true;
    else
        playable = false;
  }
}

bool TaudioOUT::setAudioDevice(QString &name) {
  if (!m_rtAudio)
    m_rtAudio = new RtAudio;
  int devId = -1;
  int devCount = m_rtAudio->getDeviceCount();
  if (devCount) {
    RtAudio::DeviceInfo devInfo;
    for(int i = 0; i < devCount; i++) { // Is there device on the list ??
        devInfo = m_rtAudio->getDeviceInfo(i);
//         qDebug() << i << "got";
        if (devInfo.probed) {
          if (QString::fromStdString(devInfo.name) == name) { // Here it is !!
            devId = i;
            break;
          }
        }
    }
    if (devId == -1) { // no device on the list - load default
        devId = m_rtAudio->getDefaultOutputDevice();
    }
  }
  RtAudio::DeviceInfo devInfo = m_rtAudio->getDeviceInfo(devId);
//   qDebug() << "samples" << "got";
  bool rateFound = false;
  for (int i = 0; i < devInfo.sampleRates.size(); i++) {
    if (devInfo.sampleRates.at(i) == SAMPLE_RATE) {
      rateFound = true;
      break;
    }
  }
  if (!rateFound) {
    qDebug() << "This device doesn't support 44100 sampling and resampling is not implemented yet.";
    return false;
  }
  RtAudio::StreamParameters outParams;
  outParams.deviceId = devId;
  outParams.nChannels = 2;
  outParams.firstChannel = 0;
  if (m_rtAudio->getCurrentApi() == RtAudio::UNIX_JACK) {
    if (!m_streamOptions)
      m_streamOptions = new RtAudio::StreamOptions;
    m_streamOptions->streamName = "nootkaOUT";
  }
  try {
    m_rtAudio->openStream(&outParams, NULL, RTAUDIO_SINT16, SAMPLE_RATE, &m_bufferFrames, &outCallBack, 0, m_streamOptions);
  }
  catch ( RtError& e ) {
    e.printMessage();
    return false;
  }
  if (m_rtAudio->isStreamOpen()) {
      m_maxCBloops = SAMPLE_RATE / (m_bufferFrames / 2);
      qDebug() << "RtOUT:" << QString::fromStdString(m_rtAudio->getDeviceInfo(devId).name);
      return true;
  } else
    return false;
}

bool TaudioOUT::play(int noteNr) {
  if (!playable)
      return false;
  
  noteNr = noteNr + qRound(m_params->a440diff);
  if (noteNr < -11 || noteNr > 41)
      return false;
  
  m_samplesCnt = -1;
  int fasterOffset = 1000;
  if (noteNr + 11 == 0)
    fasterOffset = 0;
  m_noteOffset = (noteNr + 11) * SAMPLE_RATE - fasterOffset;
  if (!m_rtAudio->isStreamRunning()) {
      try {
        m_rtAudio->startStream();
      }
      catch ( RtError& e ) {
        e.printMessage();
        return false;
      }
  }
  return true;   
}


void TaudioOUT::stop() {
  if (m_rtAudio->isStreamOpen())
      m_rtAudio->closeStream();
}


