/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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

#include "trtaudioin.h"
#include <QDebug>
#include "tpitchfinder.h"
#include "taudioparams.h"
// #include <QThread>


/*static */
QStringList TaudioIN::getAudioDevicesList() {
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
          qDebug() << "error when probing input device" << i;
//           e.printMessage();
          continue;
        }
        if (devInfo.probed && devInfo.inputChannels > 0)
          devList << QString::fromStdString(devInfo.name);
    }
    delete rta;
    return devList;
}

int TaudioIN::inCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void* userData) {
    if ( status )
        qDebug() << "Stream over detected!";
    qint16 *in = (qint16*)inBuffer;
    for (int i = 0; i < nBufferFrames; i++) {
        qint16 value = *(in + i);
        instance()->m_maxP = qMax(instance()->m_maxP, value);
        *(instance()->m_floatBuff + instance()->m_floatsWriten) = float(value) / 32768.0f;
        if (instance()->m_floatsWriten == instance()->m_pitch->aGl()->framesPerChunk-1) {
//           instance()->m_maxPeak = instance()->m_maxP;
          if (instance()->m_pitch->isBussy())
              qDebug() << "data ignored";
          else
              instance()->m_pitch->searchIn(instance()->m_floatBuff);
          instance()->m_floatsWriten = -1;
        instance()->m_maxP = 0;
      }
      instance()->m_floatsWriten++;
    }
    return 0;
}

QList<TaudioIN*> TaudioIN::m_instances = QList<TaudioIN*>();
int TaudioIN::m_thisInstance = -1;

//------------------------------------------------------------------------------------
//------------          constructor     ----------------------------------------------
//------------------------------------------------------------------------------------


TaudioIN::TaudioIN(TaudioParams* params, QObject* parent) :
    QObject(parent),
    TrtAudioAbstract(params),
    m_paused(false),
    m_floatBuff(0),
    m_pitch(0),
    m_bufferFrames(1024),
    m_maxPeak(0),
    m_maxP(0),
    m_floatsWriten(0)
{
  m_instances << this;
  m_pitch = new TpitchFinder();
  m_thisInstance = m_instances.size() - 1;
//  m_thread = new QThread();
//  m_pitch->moveToThread(m_thread);
  setParameters(params);
  
  connect(m_pitch, SIGNAL(found(float,float)), this, SLOT(pitchFreqFound(float,float)));
  connect(m_pitch, SIGNAL(pichInChunk(float)), this, SLOT(pitchInChunkSlot(float)));
  connect(m_pitch, SIGNAL(volume(float)), this, SLOT(volumeSlot(float)));
}

TaudioIN::~TaudioIN()
{
  disconnect(m_pitch, SIGNAL(found(float,float)), this, SLOT(pitchFreqFound(float,float)));
//  m_thread->terminate();
  delete rtDevice;
  delete streamOptions;
  delete m_pitch;
  if (m_floatBuff)
    delete (m_floatBuff);
  m_instances.removeLast();
  m_thisInstance = m_instances.size() - 1;
  
//  delete m_thread;
}

//------------------------------------------------------------------------------------
//------------          methods         ----------------------------------------------
//------------------------------------------------------------------------------------


void TaudioIN::setParameters(TaudioParams* params) {
  m_pitch->setIsVoice(params->isVoice);
  m_pitch->setMinimalVolume(params->noiseLevel);
  setAudioDevice(params->INdevName);
  audioParams = params;
}

/** Device name is saved to globals and to config file only after changed the Nootka preferences.
* In other cases the default device is loaded. */
bool TaudioIN::setAudioDevice(const QString& devN) {
  if (devN == deviceName) {
    return true;
  }
  if (!rtDevice)
    rtDevice = getRtAudio();
  int devId = -1;
  int devCount = rtDevice->getDeviceCount();
  if (devCount) {
    RtAudio::DeviceInfo devInfo;
    for(int i = 0; i < devCount; i++) { // Is there device on the list ??
        if (getDeviceInfo(devInfo, i)) {
          if (devInfo.probed) {
              if (QString::fromStdString(devInfo.name) == devN) { // Here it is !!
                devId = i;
                break;
              }
          }
        }
    }
    if (devId == -1) { // no device on the list - load default
        devId = rtDevice->getDefaultInputDevice();
        if (rtDevice->getDeviceInfo(devId).inputChannels <= 0) {
          // check has default input got channels
          qDebug("wrong default input device");
          return false;
        }
    }
  } else 
    return false;
  streamParams.deviceId = devId;
  streamParams.nChannels = 1;
  streamParams.firstChannel = 0;
  RtAudio::DeviceInfo devInfo;
  getDeviceInfo(devInfo, devId);
  determineSampleRate(devInfo);
  m_pitch->setSampleRate(sampleRate);
  m_bufferFrames = m_pitch->aGl()->framesPerChunk;
  if (rtDevice->getCurrentApi() == RtAudio::UNIX_JACK) {
    if (!streamOptions)
      streamOptions = new RtAudio::StreamOptions;
    streamOptions->streamName = "nootkaIN";
  }
//   printSupportedFormats(devInfo);
//   printSupportedSampleRates(devInfo);
  if (!openStream(NULL ,&streamParams, RTAUDIO_SINT16, sampleRate, &m_bufferFrames, &inCallBack, 0, streamOptions))
    return false;
  if (rtDevice->isStreamOpen()) {
      qDebug() << "RtIN:" << QString::fromStdString(rtDevice->getDeviceInfo(devId).name) << "samplerate:" << sampleRate << "buffer:" << m_bufferFrames;
      deviceName = QString::fromStdString(rtDevice->getDeviceInfo(devId).name);
      return true;
  } else
      return false;
}


void TaudioIN::initInput() {
  m_floatsWriten = 0;
  m_maxPeak = 0;
}


void TaudioIN::startListening() {
  if (rtDevice) {
    if (!m_floatBuff)
        m_floatBuff = new float[m_pitch->aGl()->framesPerChunk]; // 1024
    initInput();
    if (openStream(NULL, &streamParams, RTAUDIO_SINT16, sampleRate, &m_bufferFrames, &inCallBack, NULL, streamOptions))
      startStream();
  }
}

void TaudioIN::stopListening() {
  closeStram();
}

// void TaudioIN::wait() {
//   stopStream();
// }
// 
// void TaudioIN::go() {
//   startStream();
// }

void TaudioIN::setIsVoice(bool isV) {
  m_pitch->setIsVoice(isV);
}



/** Range of notes is increased one note down and up.
 * This 46 and 48 are its sign. 
 * Normaly 47 is offset of midi note to Nootka Tnote. */
void TaudioIN::setAmbitus(Tnote loNote, Tnote hiNote) {
  m_pitch->setAmbitus(loNote.getChromaticNrOfNote()+46, hiNote.getChromaticNrOfNote()+48);
}


//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------




void TaudioIN::pitchFreqFound(float pitch, float freq) {
  if (!m_paused) {
//       qDebug() << QString::fromStdString(Tnote(qRound(pitch - m_params->a440diff)-47).getName());
       emit noteDetected(Tnote(qRound(pitch - audioParams->a440diff)-47));
       emit fundamentalFreq(freq);
  }
}

