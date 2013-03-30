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
#include <QTimer>
#include <QThread>

#define SAMPLE_RATE (44100)

/*static*/
QStringList TaudioOUT::getAudioDevicesList() {
    QStringList devList;
    RtAudio *rta = getRtAudio();
    int devCnt = rta->getDeviceCount();
    if (devCnt < 1)
        return devList;
    for (int i = 0; i < devCnt; i++) {
        RtAudio::DeviceInfo devInfo;
        try {
          devInfo = rta->getDeviceInfo(i);
        }
        catch (RtError& e) {
          qDebug() << "error when probing output device" << i;
//           e.printMessage();
          continue;
        }
        if (devInfo.probed && devInfo.outputChannels > 0)
          devList << QString::fromStdString(devInfo.name);
    }
    delete rta;
    return devList;
}


int TaudioOUT::m_samplesCnt = 0;
unsigned int TaudioOUT::m_bufferFrames = 1024;
// int TaudioOUT::m_maxCBloops = SAMPLE_RATE / m_bufferFrames;
int TaudioOUT::m_maxCBloops = SAMPLE_RATE / m_bufferFrames * 2;
int TaudioOUT::m_noteOffset = 0;
TaudioOUT* TaudioOUT::instance = 0;


int TaudioOUT::outCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames, double streamTime, 
                           RtAudioStreamStatus status, void* userData) {
  if ( status )
    qDebug() << "Stream underflow detected!";
  m_samplesCnt++;
  if (m_samplesCnt == 0)
    qDebug("outCallBack");
  if (m_samplesCnt < m_maxCBloops - 10) {
      qint16 *out = (qint16*)outBuffer;
      int off = m_samplesCnt * nBufferFrames;
      qint16 sample;
      for (int i = 0; i < nBufferFrames; i++) {
          sample = instance->oggScale->getSample(off + i);
          *out++ = sample;
          *out++ = sample;
      }
      return 0;
  } 
  else
      return 1;
}
/*end static*/

//---------------------------------------------------------------------------------------
//                CONSTRUCTOR
//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams *_params, QString &path, QObject *parent) :
  TabstractPlayer(parent),
  TrtAudioAbstract(_params),
  oggScale(new ToggScale(path))
{
  setAudioOutParams(_params);
  instance = this;
  offTimer = new QTimer();
  connect(offTimer, SIGNAL(timeout()), this, SLOT(stopSlot()));
}


TaudioOUT::~TaudioOUT() 
{
  delete streamOptions;
  delete rtDevice;
  delete offTimer;
  delete oggScale;
}


void TaudioOUT::setAudioOutParams(TaudioParams* params) {
  if (deviceName != params->OUTdevName || !rtDevice) {
    playable = oggScale->loadAudioData();
    if (playable && setAudioDevice(params->OUTdevName))
        playable = true;
    else
        playable = false;
  }
}

bool TaudioOUT::setAudioDevice(QString &name) {
  if (!rtDevice)
    rtDevice = getRtAudio();
  int devId = -1;
  int devCount = rtDevice->getDeviceCount();
  if (devCount) {
    RtAudio::DeviceInfo devInfo;
    for(int i = 0; i < devCount; i++) { // Is there device on the list ??
        if (getDeviceInfo(devInfo , i)) {        
          if (devInfo.probed) {
            if (QString::fromStdString(devInfo.name) == name) { // Here it is !!
              devId = i;
              break;
            }
          }
        }
    }
    if (devId == -1) { // no device on the list - load default
        devId = rtDevice->getDefaultOutputDevice();
        if (rtDevice->getDeviceInfo(devId).outputChannels <= 0) {
          qDebug("wrong default output device");
          playable = false;
          return false;
        }
    }
  }
  RtAudio::DeviceInfo devInfo;
  if (!getDeviceInfo(devInfo, devId)) {
    playable = false;
    return false;
  }
  bool rateFound = false;
  for (int i = 0; i < devInfo.sampleRates.size(); i++) {
    if (devInfo.sampleRates.at(i) == SAMPLE_RATE) {
      rateFound = true;
      break;
    }
  }
  if (!rateFound) {
    qDebug() << "This device doesn't support 44100 sampling and resampling is not implemented yet.";
    playable = false;
    return false;
  }
  streamParams.deviceId = devId;
  streamParams.nChannels = 2;
  streamParams.firstChannel = 0;
  if (rtDevice->getCurrentApi() == RtAudio::UNIX_JACK) {
    if (!streamOptions)
      streamOptions = new RtAudio::StreamOptions;
    streamOptions->streamName = "nootkaOUT";
  }
  if (!openStream(&streamParams, NULL, RTAUDIO_SINT16, SAMPLE_RATE, &m_bufferFrames, &outCallBack, 0, streamOptions)) {
      playable = false;
      return false;
  }
  if (rtDevice->isStreamOpen()) {
      m_maxCBloops = SAMPLE_RATE / (m_bufferFrames / 2);
      qDebug() << "RtOUT:" << QString::fromStdString(rtDevice->getDeviceInfo(devId).name) << "buffer:" << m_bufferFrames;
      deviceName = QString::fromStdString(rtDevice->getDeviceInfo(devId).name);
      return true;
  } else
    return false;
}


bool TaudioOUT::play(int noteNr) {
  if (!playable)
      return false;
  openStream(&streamParams, NULL, RTAUDIO_SINT16, SAMPLE_RATE, &m_bufferFrames, &outCallBack, 0, streamOptions);
  
  noteNr = noteNr + qRound(audioParams->a440diff);
  if (noteNr < -11 || noteNr > 41)
      return false;
  
  doEmit = true;
  m_samplesCnt = -1;
  double fasterOffset = 1000;
  if (noteNr + 11 == 0)
    fasterOffset = 0;
  m_noteOffset = (noteNr + 11) * SAMPLE_RATE * 2 - fasterOffset;
    
  oggScale->setPos(m_noteOffset);
  qDebug() << "ok";
//   setPos((noteNr + 11) * 2 - fasterOffset);
  
  if (offTimer->isActive())
      offTimer->stop();
  offTimer->start(1600);
  return startStream();
}


void TaudioOUT::stop() {
  if (offTimer->isActive()) {
    offTimer->stop();
    doEmit = false;
    stopSlot();
  }
}


void TaudioOUT::stopSlot() {
  offTimer->stop();
  closeStram();
  if (doEmit)
    emit noteFinished();
}

