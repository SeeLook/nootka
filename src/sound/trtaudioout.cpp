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
          continue;
        }
        if (devInfo.probed && devInfo.outputChannels > 0)
          devList << QString::fromLocal8Bit(devInfo.name.data());
    }
    delete rta;
    return devList;
}


int TaudioOUT::m_samplesCnt = 0;
unsigned int TaudioOUT::m_bufferFrames = 1024;
int TaudioOUT::m_maxCBloops = 44100 / m_bufferFrames * 2;
TaudioOUT* TaudioOUT::instance = 0;
qint16* TaudioOUT::m_crossBuffer = 0;
bool TaudioOUT::m_doCrossFade = false;
float TaudioOUT::m_cross = 0.0f;
int m_crossCount = 0; // counts samples of crossing buffer
bool m_callBackIsBussy = false;


int TaudioOUT::outCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames, double streamTime, 
                           RtAudioStreamStatus status, void* userData) {
  Q_UNUSED(inBuffer)
  Q_UNUSED(streamTime)
  Q_UNUSED(userData)
	m_callBackIsBussy = true;
  if ( status )
			qDebug() << "Stream underflow detected!";
	if (m_doCrossFade) { // Cross-fading avoids cracking during transition of notes.
			m_doCrossFade = false;
			m_cross = 1.0f;
			m_crossCount = 0;
	}
  m_samplesCnt++;
  if (m_samplesCnt < m_maxCBloops - 10) {
      qint16 *out = (qint16*)outBuffer;
      int off = m_samplesCnt * (nBufferFrames / instance->ratioOfRate);
      qint16 sample;
      for (int i = 0; i < nBufferFrames / instance->ratioOfRate; i++) {
				if (m_cross > 0.0 && m_crossCount < 1000) { // mix current and previous samples when cross-fading
							sample =  (qint16)qRound((1.0 - m_cross) * (float)instance->oggScale->getSample(off + i) + 
											m_cross * (float)m_crossBuffer[m_crossCount]);
							m_cross -= CROSS_STEP;
							m_crossCount++;
				} else {
							sample = instance->oggScale->getSample(off + i);
				}
				for (int r = 0; r < instance->ratioOfRate; r++) {
						*out++ = sample; // left channel
						*out++ = sample; // right channel
				}
      }
      m_callBackIsBussy = false;
      return 0;
  } else {
			m_callBackIsBussy = false;
      return 1;
	}			
}
/*end static*/

//---------------------------------------------------------------------------------------
//                CONSTRUCTOR
//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams *_params, QString &path, QObject *parent) :
  TabstractPlayer(parent),
  TrtAudioAbstract(_params),
  oggScale(new ToggScale(path)),
  ratioOfRate(1)
{
  setType(e_audio);
  setAudioOutParams(_params);
  instance = this;
  offTimer = new QTimer();
	m_crossBuffer = new qint16[1000];
  connect(offTimer, SIGNAL(timeout()), this, SLOT(stopSlot()));
}


TaudioOUT::~TaudioOUT() 
{
  delete streamOptions;
  delete rtDevice;
  delete offTimer;
  delete oggScale;
	if (m_crossBuffer)
		delete m_crossBuffer;
}


void TaudioOUT::setAudioOutParams(TaudioParams* params) {
	playable = oggScale->loadAudioData(params->audioInstrNr);
  if (deviceName != params->OUTdevName || !rtDevice) {
    if (playable && setAudioDevice(params->OUTdevName))
        playable = true;
    else
        playable = false;
  }
  // Shifts only float part of a440diff - integer part is shifted by play() method
  oggScale->setPitchOffset(audioParams->a440diff - (float)int(audioParams->a440diff));
}


bool TaudioOUT::setAudioDevice(QString &name) {
  if (rtDevice)
    delete rtDevice;
  rtDevice = getRtAudio();
  int devId = -1;
  int devCount = rtDevice->getDeviceCount();
  if (devCount) {
    RtAudio::DeviceInfo devInfo;
    for(int i = 0; i < devCount; i++) { // Is there device on the list ??
        if (getDeviceInfo(devInfo, i)) {        
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
				if (rtDevice->getCurrentApi() == RtAudio::LINUX_ALSA)
						streamOptions->flags = RTAUDIO_ALSA_USE_DEFAULT;
				else {
						if (rtDevice->getDeviceInfo(devId).outputChannels <= 0) {
							qDebug("wrong default output device");
							playable = false;
							return false;
						}
				}
    }
  }
  RtAudio::DeviceInfo devInfo;
  if (!getDeviceInfo(devInfo, devId)) {
    playable = false;
    return false;
  }
  determineSampleRate(devInfo);
  streamParams.deviceId = devId;
  streamParams.nChannels = 2;
  streamParams.firstChannel = 0;
	RtAudioFormat dataFormat = determineDataFormat(devInfo);
	if (dataFormat == RTAUDIO_SINT8) {
			playable = false;
			return false;
	}
  ratioOfRate = sampleRate / 44100;
  if (ratioOfRate > 1) { // from here sample rate is sampleRate * ratioOfRate
    if (sampleRate == 88200 || sampleRate == 176400)
      sampleRate = 44100;
    else if (sampleRate == 96000 || sampleRate == 192000)
      sampleRate = 48000;
    m_bufferFrames = m_bufferFrames * ratioOfRate; // increase buffer to give time for resampling in oggScale
  }
  oggScale->setSampleRate(sampleRate);
  if (!openStream(&streamParams, NULL, dataFormat, sampleRate * ratioOfRate, &m_bufferFrames, &outCallBack, 0, streamOptions)) {
      playable = false;
      return false;
  }
  if (rtDevice->isStreamOpen() && checkBufferSize(m_bufferFrames)) {
      m_maxCBloops = (88200 * ratioOfRate) / m_bufferFrames;
      deviceName = QString::fromLocal8Bit(rtDevice->getDeviceInfo(devId).name.data());
      qDebug() << "OUT:" << deviceName << "samplerate:" << sampleRate * ratioOfRate << ", buffer size:" << m_bufferFrames;
      if (rtDevice->getCurrentApi() != RtAudio::LINUX_PULSE)
          closeStram(); // otherwise devices are blocked (not appears in settings dialog)
      return true;
  } else {
			playable = false;
			return false;
	}
}


bool TaudioOUT::play(int noteNr) {
  if (!playable)
      return false;
  
	while (m_callBackIsBussy) {
		  SLEEP(1);
			qDebug() << "Oops! Call back method is in progress when a new note wants to be played!";
	}
	
  if (offTimer->isActive()) {
      offTimer->stop();
			int off = (m_samplesCnt + 1) * (m_bufferFrames / ratioOfRate); // next chunk of plaing sound
			for (int i = 0; i < 1000; i++) // copy data of current sound to perform crrosfading
				m_crossBuffer[i] = oggScale->getSample(off + i);
			m_doCrossFade = true;
	} else
			m_doCrossFade = false;
  
  noteNr = noteNr + int(audioParams->a440diff);
	
  openStream(&streamParams, NULL, RTAUDIO_SINT16, sampleRate * ratioOfRate, &m_bufferFrames, &outCallBack, 0, streamOptions);
  
  doEmit = true;
  m_samplesCnt = -1;
  oggScale->setNote(noteNr);
  int loops = 0;
  while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
      SLEEP(1);
      loops++;
  }
//   if (loops)
//        qDebug() << "latency:" << loops << "ms";  
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
  if (rtDevice->getCurrentApi() == RtAudio::LINUX_PULSE || rtDevice->getCurrentApi() == RtAudio::WINDOWS_DS) 
			stopStream();
  else
			closeStram();
  if (doEmit)
    emit noteFinished();
}

