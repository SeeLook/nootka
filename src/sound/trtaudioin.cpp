/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include "tpitchfinder.h"
#include "taudioparams.h"
#include <QDebug>


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
          continue;
        }
        if (devInfo.probed && devInfo.inputChannels > 0)
          devList << QString::fromLocal8Bit(devInfo.name.data());
    }
    if (rta->getCurrentApi() == RtAudio::LINUX_ALSA && !devList.isEmpty())
				devList.prepend("ALSA default");
    delete rta;
    return devList;
}


int TaudioIN::inCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames,
												 double streamTime, RtAudioStreamStatus status, void* userData) {
    Q_UNUSED(outBuffer)
    Q_UNUSED(streamTime)
    Q_UNUSED(userData)
    if (status)
        qDebug() << "Stream over detected!";
    qint16 *in = (qint16*)inBuffer;
		float* inF = (float*)inBuffer;
		qint16 value;
    for (int i = 0; i < nBufferFrames; i++) {
				if (instance()->isFloat())
					*(instance()->m_floatBuff + instance()->m_floatsWriten) = *(inF + i);
				else {
					value = *(in + i);
					*(instance()->m_floatBuff + instance()->m_floatsWriten) = float(value) / 32768.0f;
				}
        if (instance()->m_floatsWriten == instance()->m_pitch->aGl()->framesPerChunk - 1) {
						instance()->m_pitch->searchIn(instance()->m_floatBuff);
						instance()->m_floatsWriten = -1;
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
    m_floatsWriten(0),
    m_lastPich(0.0f)
{
  m_instances << this;
  m_pitch = new TpitchFinder();
  m_thisInstance = m_instances.size() - 1;
  setParameters(params);
  
  connect(m_pitch, SIGNAL(found(float,float)), this, SLOT(pitchFreqFound(float,float)));
  connect(m_pitch, SIGNAL(pichInChunk(float)), this, SLOT(pitchInChunkSlot(float)));
  connect(m_pitch, SIGNAL(volume(float)), this, SLOT(volumeSlot(float)));
}

TaudioIN::~TaudioIN()
{
  disconnect(m_pitch, SIGNAL(found(float,float)), this, SLOT(pitchFreqFound(float,float)));
	closeStram();
  delete rtDevice;
  delete streamOptions;
  delete m_pitch;
  if (m_floatBuff)
    delete (m_floatBuff);
  m_instances.removeLast();
  m_thisInstance = m_instances.size() - 1;
}

//------------------------------------------------------------------------------------
//------------          methods         ----------------------------------------------
//------------------------------------------------------------------------------------

void TaudioIN::setParameters(TaudioParams* params) {
  m_pitch->setIsVoice(params->isVoice);
	setMinimalVolume(params->minimalVol);
	m_pitch->setMinimalDuration(params->minDuration);
  setAudioDevice(params->INdevName);
  audioParams = params;
}

/** Device name is saved to globals and to config file only after changed the Nootka preferences.
* In other cases the default device is loaded. */
bool TaudioIN::setAudioDevice(const QString& devN) {
  if (rtDevice)
    delete rtDevice;
	delete m_floatBuff;
	m_floatBuff = 0;
  rtDevice = getRtAudio();
  int devId = -1;
  int devCount = rtDevice->getDeviceCount();
	bool isAlsaDefault = false;
  streamOptions->flags = !RTAUDIO_ALSA_USE_DEFAULT; // reset options flags
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
				if (rtDevice->getCurrentApi() == RtAudio::LINUX_ALSA) {
						streamOptions->flags = RTAUDIO_ALSA_USE_DEFAULT;
						isAlsaDefault = true;
				} else {
						if (rtDevice->getDeviceInfo(devId).inputChannels <= 0) {
							// check has default input got channels
							qDebug("wrong default input device");
							return false;
						}
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
  m_pitch->setSampleRate(sampleRate, audioParams->range); // framesPerChunk is determined here
  m_bufferFrames = m_pitch->aGl()->framesPerChunk;
	RtAudioFormat dataFormat = determineDataFormat(devInfo);
	if (dataFormat == RTAUDIO_SINT8)
			return false;
  if (!openStream(NULL ,&streamParams, dataFormat, sampleRate, &m_bufferFrames, &inCallBack, 0, streamOptions))
    return false;
  if (rtDevice->isStreamOpen()) {
			if (isAlsaDefault)
					deviceName = "ALSA default";
			else
					deviceName = QString::fromLocal8Bit(rtDevice->getDeviceInfo(devId).name.data());
			if (checkBufferSize(m_bufferFrames)) {
					qDebug() << "IN:" << deviceName << "samplerate:" << sampleRate << ", buffer size:" << m_bufferFrames;
					return true;
      } else
					return false;
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
  if (rtDevice->getCurrentApi() == RtAudio::WINDOWS_DS/* || rtDevice->getCurrentApi() == RtAudio::LINUX_PULSE*/)
      stopStream();
  else
      closeStram();
	m_paused = false;
}

// void TaudioIN::wait() {
//   stopStream();
// }
// 
// void TaudioIN::go() {
//   startStream();
// }


void TaudioIN::setMinimalVolume(float minVol) {
		m_pitch->setMinimalVolume(minVol);
		audioParams->minimalVol = minVol;
}



void TaudioIN::setIsVoice(bool isV) {
  m_pitch->setIsVoice(isV);
}


/** Range of notes is increased one note down and up.
 * This 46 and 48 are its sign. 
 * Normally 47 is offset of midi note to Nootka Tnote. */
void TaudioIN::setAmbitus(Tnote loNote, Tnote hiNote) {
  m_pitch->setAmbitus(loNote.getChromaticNrOfNote() + 46, hiNote.getChromaticNrOfNote() + 48);
	m_loNote = loNote;
	m_hiNote = hiNote;
}

//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------

void TaudioIN::pitchInChunkSlot(float pitch) {
	if (m_paused)
			return;
  if (pitch == 0.0)
			m_LastChunkPitch = 0.0;
  else
			m_LastChunkPitch = pitch - audioParams->a440diff;
	emit chunkPitch(m_LastChunkPitch);
}


void TaudioIN::pitchFreqFound(float pitch, float freq) {
  if (!m_paused) {
			m_lastPich = pitch - audioParams->a440diff;
			if (pitch >= m_pitch->aGl()->loPitch && pitch <= m_pitch->aGl()->topPitch) {
// 					float into = qAbs((pitch - audioParams->a440diff) - (float)qRound(pitch - audioParams->a440diff));
// 					QString inText;
// 					if (into < 0.05)
// 						inText = "prof. of solfege";
// 					else if (into < 0.1)
// 						inText = "perfect";
// 					else if (into < 0.2)
// 						inText = "normal";
// 					else if (into < 0.3)
// 						inText = "sufficient";
// 					else 
// 						inText = "dog howl";
// 					qDebug() << inText;
// 					Tnote nnn = Tnote(qRound(pitch - audioParams->a440diff) - 47);
// 					qDebug() << "pitchFreqFound:" << nnn.toText();
					emit noteDetected(Tnote(qRound(pitch - audioParams->a440diff) - 47));
			}
			emit fundamentalFreq(freq);
  } else 
			m_lastPich = 0.0f;
}


void TaudioIN::volumeSlot(float vol) {
	m_maxPeak = vol;
}



