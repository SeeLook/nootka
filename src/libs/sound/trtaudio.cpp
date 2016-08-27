/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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


#include "trtaudio.h"
#include <taudioparams.h>
#include <QStringList>
#include <QDebug>
#include <QFileInfo>


/*static*/
RtAudio* 													TrtAudio::m_rtAduio = 0;
RtAudio::StreamParameters* 				TrtAudio::m_inParams = 0;
RtAudio::StreamParameters* 				TrtAudio::m_outParams = 0;
RtAudio::StreamOptions* 					TrtAudio::streamOptions = 0;
quint32 													TrtAudio::m_sampleRate = 44100;
quint32                           TrtAudio::m_inSR = 48000;
quint32                           TrtAudio::m_outSR = 44100;
unsigned int 											TrtAudio::m_bufferFrames = 1024;
bool 															TrtAudio::m_isAlsaDefault = false;
QString 													TrtAudio::m_inDevName = "anything";
QString 													TrtAudio::m_outDevName = "anything";
TrtAudio::callBackType				 		TrtAudio::m_cbIn = 0;
TrtAudio::callBackType 						TrtAudio::m_cbOut = 0;
TaudioObject*											TrtAudio::m_ao = 0;
RtAudioCallback										TrtAudio::m_callBack = TrtAudio::duplexCallBack;
bool 															TrtAudio::m_JACKorASIO = false;
bool 															TrtAudio::forceUpdate = false;
TrtAudio::EaudioState             TrtAudio::m_state = TrtAudio::e_iddle;
bool                              TrtAudio::m_areSplit = true;

int                               m_preferredBF = 512;
bool                              m_audioUpdated = false;


void TrtAudio::createRtAudio() {
	if (m_rtAduio == 0) { // Create RtAudio instance if doesn't exist
#if defined(Q_OS_WIN)
		RtAudio::Api rtAPI;
		if ((int)QSysInfo::windowsVersion() < (int)QSysInfo::WV_VISTA)
			rtAPI = RtAudio::WINDOWS_DS; // Direct sound for older versions
		else // WASAPI since Vista
			rtAPI = RtAudio::WINDOWS_WASAPI;
		if (m_JACKorASIO)
			rtAPI = RtAudio::UNSPECIFIED; // ASIO has a priority
#elif defined(Q_OS_LINUX)
		RtAudio::Api rtAPI;
		if (m_JACKorASIO)
      rtAPI = RtAudio::UNSPECIFIED; // check is JACK possible and allow it
    else
      rtAPI = RtAudio::LINUX_ALSA; // force ALSA
#else
    RtAudio::Api rtAPI = RtAudio::MACOSX_CORE;
#endif

#if defined(__LINUX_PULSE__)
      QFileInfo pulseBin("/usr/bin/pulseaudio");
      if (!m_JACKorASIO && pulseBin.exists()) // we check is PA possible to run - without check, it can hang over.
				rtAPI = RtAudio::LINUX_PULSE;
#endif
    try {
      m_rtAduio = new RtAudio(rtAPI);  
      m_rtAduio->showWarnings(false);
    } catch (RtAudioError& e) {
      qDebug() << "[TrtAudio] Cannot create RtAudio instance" << QString::fromStdString(e.getMessage());
      m_rtAduio = 0;
    }
	}
}


QString TrtAudio::currentRtAPI() {
	QString rtApiTxt;
	if (m_rtAduio) {
		switch (getCurrentApi()) {
			case RtAudio::WINDOWS_DS:
				rtApiTxt = "Direct Sound";
				break;
			case RtAudio::WINDOWS_WASAPI:
				rtApiTxt = "WAS API";
				break;
			case RtAudio::WINDOWS_ASIO:
				rtApiTxt = "ASIO";
				break;
			case RtAudio::LINUX_ALSA:
				rtApiTxt = "ALSA";
				break;
			case RtAudio::UNIX_JACK:
				rtApiTxt = "JACK";
				break;
			case RtAudio::LINUX_PULSE:
				rtApiTxt = "pulseaudio";
				break;
			case RtAudio::RtAudio::MACOSX_CORE:
				rtApiTxt = "CoreAudio";
				break;
			default:
				rtApiTxt = "Undefined";
				break;
		}
	} else
			rtApiTxt = "RtAudio API doesn't exist";
	return rtApiTxt;
}



#if !defined (Q_OS_MAC)
void TrtAudio::setJACKorASIO(bool jack) {
	if (jack != m_JACKorASIO) {
		abortStream();
    closeStream();
		delete m_rtAduio;
		m_rtAduio = 0;
		m_JACKorASIO = jack;
		createRtAudio();
	}
}


bool TrtAudio::switchAPI(RtAudio::Api rtApi) {
  return false;
}

#endif


void TrtAudio::printSupportedFormats(RtAudio::DeviceInfo& devInfo) {
  QString fmt;
  if (devInfo.nativeFormats & 0x1)
    fmt += " RTAUDIO_SINT8";
  if (devInfo.nativeFormats & 0x2)
    fmt += " RTAUDIO_SINT16";
  if (devInfo.nativeFormats & 0x4)
    fmt += " RTAUDIO_SINT24";
  if (devInfo.nativeFormats & 0x8)
    fmt += " RTAUDIO_SINT32";
  if (devInfo.nativeFormats & 0x10)
    fmt += " RTAUDIO_FLOAT32";
  if (devInfo.nativeFormats & 0x20)
    fmt += " RTAUDIO_FLOAT64";
  qDebug() << "[TrtAudio] supported sample formats:" << fmt;
}


void TrtAudio::printSupportedSampleRates(RtAudio::DeviceInfo& devInfo) {
  QString sRates;
  for (int i = 0; i < devInfo.sampleRates.size(); i++)
    sRates += QString("%1 ").arg(devInfo.sampleRates.at(i));
  qDebug() << "[TrtAudio] supported sample rates:"<< sRates;
}


/*----------------------------------------------------------------------------------------------*/


TrtAudio::TrtAudio(TaudioParams* audioP, TrtAudio::EdevType type, TrtAudio::callBackType cb) :
	m_audioParams(audioP),
	m_type(type)
{
	if (m_type == e_input) {
		m_cbIn = cb;
		if (!m_inParams)
			m_inParams = new RtAudio::StreamParameters;
	} else {
		m_cbOut = cb;
		if (!m_outParams)
			m_outParams = new RtAudio::StreamParameters;
	}
	if (!streamOptions) {
			streamOptions = new RtAudio::StreamOptions;
			streamOptions->streamName = "Nootka";
			m_ao = new TaudioObject();
	}
	ao()->blockSignals(true);
	createRtAudio();
	updateAudioParams();
	ao()->blockSignals(false);
}


TrtAudio::~TrtAudio()
{
// 	closeStream();
	if (m_outParams == 0 && m_inParams == 0) {
		delete m_rtAduio;
		m_rtAduio = 0;
		delete streamOptions;
		streamOptions = 0;
		delete m_ao;
		m_ao = 0;
	}
		
}


void TrtAudio::updateAudioParams() {
// reopen audio device only when necessary
  if (forceUpdate || (m_inParams && m_inDevName != audioParams()->INdevName) || (m_outParams && m_outDevName != m_audioParams->OUTdevName) ||
      (audioParams()->forwardInput && m_callBack == &duplexCallBack) || (!audioParams()->forwardInput && m_callBack == &passInputCallBack) ) {
    closeStream();
#if defined (Q_OS_LINUX) || defined (Q_OS_WIN)
    setJACKorASIO(audioParams()->JACKorASIO);
#endif
		forceUpdate = false; // no more
		m_audioUpdated = true;
		if (audioParams()->forwardInput || getCurrentApi() == RtAudio::WINDOWS_ASIO) {
      m_areSplit = false;
    } else {
      m_areSplit = true;
    }
  // preparing devices
    int inDevId = -1, outDevId = -1;
    int devCount = getDeviceCount();
    m_isAlsaDefault = false;
    streamOptions->flags = !RTAUDIO_ALSA_USE_DEFAULT; // reset options flags
    if (devCount) {
      RtAudio::DeviceInfo devInfo;
      for(int i = 0; i < devCount; i++) { // Is there device on the list ??
          if (getDeviceInfo(devInfo, i)) {
            if (devInfo.probed) {
              if (m_inParams && devInfo.inputChannels > 0 && convDevName(devInfo) == audioParams()->INdevName) {
                inDevId = i;
                m_inDevName = convDevName(devInfo);
              }
              if (m_outParams && devInfo.outputChannels > 0 && convDevName(devInfo) == audioParams()->OUTdevName) {
                outDevId = i;
                m_outDevName = convDevName(devInfo);
              }
            }
          }
      }
      if (inDevId == -1) { // no device on the list - load default
          if (getCurrentApi() != RtAudio::LINUX_ALSA) {
            inDevId = getDefaultIn();
            if (inDevId > -1) {
              RtAudio::DeviceInfo inputInfo;
              getDeviceInfo(inputInfo, inDevId);
              if (inputInfo.inputChannels <= 0) {
                qDebug("[TrtAudio] wrong default input device");
                deleteInParams();
              }
            }
          }
      }
      if (outDevId == -1) {
        if (getCurrentApi() != RtAudio::LINUX_ALSA) {
          outDevId = getDefaultOut();
          RtAudio::DeviceInfo outInfo;
          getDeviceInfo(outInfo, outDevId);
          if (outDevId > -1) {
            if (outInfo.outputChannels <= 0) {
              qDebug("[TrtAudio] wrong default output device");
              deleteOutParams();
            }
          }
        }
      }
      // Default ALSA device can be set only when both devices are undeclared
      if (inDevId == -1 && outDevId == -1 && getCurrentApi() == RtAudio::LINUX_ALSA) {
        streamOptions->flags = RTAUDIO_ALSA_USE_DEFAULT;
        m_isAlsaDefault = true;
        if (m_inParams) // As long as ALSA ignores device id for its default the id has to be correct number (0 - devCount - 1)
          inDevId = 0;
        if (m_outParams)
          outDevId = 0;
      }
    } else {
      qDebug() << "[TrtAudio] No audio devices!";
      return;
    }
  // setting device parameters
    if (m_inParams) {
      m_inParams->deviceId = inDevId;
      m_inParams->nChannels = 1;
      m_inParams->firstChannel = 0;
    }
    if (m_outParams) {
      m_outParams->deviceId = outDevId;
      m_outParams->nChannels = 2;
      m_outParams->firstChannel = 0;
    }
    RtAudio::DeviceInfo inDevInfo, outDevInfo;
    if (m_inParams && !getDeviceInfo(inDevInfo, inDevId))
      deleteInParams();
    else {
      if (areStreamsSplit())
        m_inSR = inDevInfo.preferredSampleRate; // Usually it is 48000
      else
        m_inSR = determineSampleRate(inDevInfo);
    }
    if (m_outParams && !getDeviceInfo(outDevInfo, outDevId))
      deleteOutParams();
    else
      m_outSR = determineSampleRate(outDevInfo); // 44100 is preferred

    if (!areStreamsSplit()) {
      m_sampleRate = qMax(m_inSR, m_outSR);
      m_inSR = m_sampleRate;
      m_outSR = m_sampleRate;
    }
#if !defined (Q_OS_MAC) // Mac has reaction for this flag - it open streams with 15 buffer frames
    streamOptions->flags |= RTAUDIO_MINIMIZE_LATENCY;
#endif
    if (audioParams()->forwardInput && m_inParams && m_outParams)
        m_callBack = &passInputCallBack;
    else
        m_callBack = &duplexCallBack;
  }
	ao()->emitParamsUpdated();
}


bool TrtAudio::listen() {
  if (rtDevice()->isStreamOpen()) {
    if (m_state == e_listening)
      return true;
    if (m_state == e_playing) {
      abortStream();
      closeStream();
    }
  }
  m_state = e_listening;
  rtDevice()->openStream(0, m_inParams, RTAUDIO_SINT16, m_inSR, &m_bufferFrames, &listenCallBack, 0, streamOptions);
//   qDebug() << "[TrtAudio] stream is listening";
  return true;
}


bool TrtAudio::play() {
  if (rtDevice()->isStreamOpen()) {
    if (m_state == e_playing)
      return true;
    if (m_state == e_listening) {
      abortStream();
      closeStream();
    }
  }
  m_state = e_playing;
  rtDevice()->openStream(m_outParams, 0, RTAUDIO_SINT16, m_outSR, &m_bufferFrames, &playCallBack, 0, streamOptions);
//   qDebug() << "[TrtAudio] stream is playing";
  return true;
}


bool TrtAudio::openStream() {
	try {
    if (rtDevice()) {
      m_bufferFrames = m_preferredBF; // reset when it was overridden by another rt API
      if (m_areSplit) {
        bool splitTry = false;
        if (m_type == e_input)
          splitTry = listen();
        else
          splitTry = play();
        if (!splitTry) {
          qDebug() << "[TrtAudio] Cannot open split stream";
          return false;
        }
      } else if (!rtDevice()->isStreamOpen()) {
          rtDevice()->openStream(m_outParams, m_inParams, RTAUDIO_SINT16, sampleRate(), &m_bufferFrames, m_callBack, 0, streamOptions);
          qDebug() << "[TrtAudio] audio opened in duplex mode";
      }

      if (rtDevice()->isStreamOpen()) {
        ao()->emitStreamOpened();
        if (m_isAlsaDefault) {
            if (m_inParams)
              m_inDevName = "ALSA default";
            if (m_outParams)
              m_outDevName = "ALSA default";
        } else {
            RtAudio::DeviceInfo di;
            if (m_inParams && getDeviceInfo(di, m_inParams->deviceId))
              m_inDevName = convDevName(di);
            if (m_outParams && getDeviceInfo(di, m_outParams->deviceId))
              m_outDevName = convDevName(di);
        }
        if (m_audioUpdated) { // print params once
          if (m_inParams)
            qDebug() << currentRtAPI() << "IN:" << m_inDevName << "samplerate:" << inRate() << ", buffer size:" << m_bufferFrames;
          if (m_outParams)
            qDebug() << currentRtAPI() << "OUT:" << m_outDevName << "samplerate:" << outRate() << ", buffer size:" << m_bufferFrames;
          m_audioUpdated = false;
        }
        return true;
    } else
        return false;
    }
  } catch (RtAudioError& e) {
      qDebug() << "[TrtAudio] can't open stream" << m_inDevName << m_outDevName << "\n" << QString::fromStdString(e.getMessage());
      return false;
  }
  return true;
}


void TrtAudio::apiStopOrClose() {
  if (getCurrentApi() == RtAudio::LINUX_ALSA)
      closeStream();
  else
      stopStream();
}


bool TrtAudio::startStream() {
	if (!isOpened()) {
		if (!openStream())
			return false;
  }
  try {
    if (rtDevice() && !rtDevice()->isStreamRunning()) {
      rtDevice()->startStream();
		}
  } catch (RtAudioError& e) {
      qDebug() << "[TrtAudio] can't start stream";
      return false;
  }
//   qDebug("[TrtAudio] stream started");
  return true;
}


void TrtAudio::stopStream() {
  try {
    if (rtDevice() && rtDevice()->isStreamRunning()) {
      rtDevice()->stopStream();
//       qDebug("[TrtAudio] stream stopped");
    }
  } catch (RtAudioError& e) {
    qDebug() << "[TrtAudio] can't stop stream";
  }
}


void TrtAudio::closeStream() {
  try {
    stopStream();
    if (rtDevice() && rtDevice()->isStreamOpen()) {
      rtDevice()->closeStream();
      m_state = e_iddle;
//       qDebug("[TrtAudio] stream closed");
    }
  } catch (RtAudioError& e) {
      qDebug() << "[TrtAudio] can't close stream";
  }
}


void TrtAudio::abortStream() {
	try {
    if (rtDevice() && rtDevice()->isStreamRunning()) {
      rtDevice()->abortStream();
//       qDebug("[TrtAudio] stream aborted");
    }
  }
  catch (RtAudioError& e) {
    qDebug() << "[TrtAudio] can't abort stream";
  }
  if (areStreamsSplit())
    closeStream();
}


bool TrtAudio::isOpened() {
  if (rtDevice()) {
    try {
      return rtDevice()->isStreamOpen();
    }
    catch (RtAudioError& e) {
      return false;
    }
  }
  return false;
}


bool TrtAudio::isRunning() {
  if (rtDevice()) {
    try {
      return rtDevice()->isStreamRunning();
    }
    catch (RtAudioError& e) {
      return false;
    }
  }
  return false;
}



bool TrtAudio::getDeviceInfo(RtAudio::DeviceInfo& devInfo, int id) {
  try {
      devInfo = rtDevice()->getDeviceInfo(id);
  }
  catch (RtAudioError& e) {
    qDebug() << "[TrtAudio] error when probing audio device" << id;
    return false;
  }
  return true;
}


RtAudio::Api TrtAudio::getCurrentApi() {
  RtAudio::Api api = RtAudio::UNSPECIFIED;
  if (rtDevice()) {
    try {
        api = rtDevice()->getCurrentApi();
    }
    catch (RtAudioError& e) {
      qDebug() << "[TrtAudio] Cannot determine current API";;
    }
  }
  return api;
}


unsigned int TrtAudio::getDeviceCount() {
  unsigned int cnt = 0;
  if (rtDevice()) {
    try {
        cnt = rtDevice()->getDeviceCount();
    }
    catch (RtAudioError& e) {
      qDebug() << "[TrtAudio] Cannot obtain devices number";
    }
  }
  return cnt;
}


int TrtAudio::getDefaultIn() {
  int inNr = -1;
  if (rtDevice()) {
    try {
      inNr = rtDevice()->getDefaultInputDevice();
    }
    catch (RtAudioError& e) {
      qDebug() << "[TrtAudio] Cannot get default input device";
    }
  }
  return inNr;
}


int TrtAudio::getDefaultOut() {
  int inNr = -1;
  if (rtDevice()) {
    try {
      inNr = rtDevice()->getDefaultOutputDevice();
    }
    catch (RtAudioError& e) {
      qDebug() << "[TrtAudio] Cannot get default output device";
    }
  }
  return inNr;
}


quint32 TrtAudio::determineSampleRate(RtAudio::DeviceInfo& devInfo) {
//   return devInfo.preferredSampleRate;
  for (int i = 0; i < devInfo.sampleRates.size(); i++) {
    unsigned int &sr = devInfo.sampleRates.at(i);
    if ( sr == 44100 || sr == 48000 || sr == 88200 || sr == 96000 || sr == 176400 || sr == 192000)
      return sr;
  }
	if (devInfo.sampleRates.size() > 0)
		return devInfo.sampleRates.at(devInfo.sampleRates.size() - 1);
	else
		return 44100;
}

#if defined(Q_OS_WIN)
void TrtAudio::restartASIO() {
  closeStream();
  startStream();
}
#endif

//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################
int TrtAudio::duplexCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames, double, RtAudioStreamStatus status, void*) {
  if (m_cbOut) {
    if (m_cbOut(outBuffer, nBufferFrames, status))
      if (m_cbIn)
        m_cbIn(inBuffer, nBufferFrames, status);
  } else
      if (m_cbIn)
        m_cbIn(inBuffer, nBufferFrames, status);
  return 0;
}


int TrtAudio::passInputCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames, double, RtAudioStreamStatus status, void*) {
  qint16 *in = (qint16*)inBuffer;
  qint16 *out = (qint16*)outBuffer;
  if (m_cbOut(outBuffer, nBufferFrames, status)) // none playing is performed
      for (int i = 0; i < nBufferFrames; i++) { // then forward input
          *out++ = *(in + i); // left channel
          *out++ = *(in + i); // right channel
      }
  m_cbIn(inBuffer, nBufferFrames, status);
  return 0;
}


int TrtAudio::playCallBack(void* outBuffer, void*, unsigned int nBufferFrames, double, RtAudioStreamStatus status, void*) {
  if (m_cbOut(outBuffer, nBufferFrames, status))
    ao()->emitPlayingFinished();
  return 0;
}


int TrtAudio::listenCallBack(void*, void* inBuffer, unsigned int nBufferFrames, double, RtAudioStreamStatus status, void*) {
  m_cbIn(inBuffer, nBufferFrames, status);
  return 0;
}


