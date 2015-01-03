/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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
#include "taudioobject.h"
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
unsigned int 											TrtAudio::m_bufferFrames = 1024;
bool 															TrtAudio::m_isAlsaDefault = false;
QString 													TrtAudio::m_inDevName = "anything";
QString 													TrtAudio::m_outDevName = "anything";
TrtAudio::callBackType				 		TrtAudio::m_cbIn = 0;
TrtAudio::callBackType 						TrtAudio::m_cbOut = 0;
TaudioObject*											TrtAudio::m_ao = 0;
RtAudioCallback										TrtAudio::m_callBack = TrtAudio::duplexCallBack;
bool 															TrtAudio::m_JACKorASIO = false;


void TrtAudio::createRtAudio() {
	if (m_rtAduio == 0) { // Create RtAudio instance if doesn't exist
#if defined(Q_OS_WIN)
		RtAudio::Api rtAPI;
		if ((int)QSysInfo::windowsVersion() < (int)QSysInfo::WV_VISTA)
			rtAPI = RtAudio::WINDOWS_DS; // Direct sound for older versions
		else // WASAPI since Vista
			rtAPI = RtAudio::WINDOWS_WASAPI;
		if (m_JACKorASIO)
			rtAPI = RtAudio::WINDOWS_ASIO;
		m_rtAduio = new RtAudio(rtAPI);
#else
		RtAudio::Api rtAPI;
		if (m_JACKorASIO)
      rtAPI = RtAudio::UNSPECIFIED; // check is JACK possible and allow it
    else
      rtAPI = RtAudio::LINUX_ALSA; // force ALSA
#endif
#if defined(__LINUX_PULSE__)
      QFileInfo pulseBin("/usr/bin/pulseaudio");
      if (pulseBin.exists()) // we check is PA possible to run - without check, it can hang over.
				rtAPI = RtAudio::LINUX_PULSE;
#endif
		m_rtAduio = new RtAudio(rtAPI);
    m_rtAduio->showWarnings(false);
	}
}


QString TrtAudio::currentRtAPI() {
	QString rtApiTxt;
	if (m_rtAduio) {
		switch (m_rtAduio->getCurrentApi()) {
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
		delete m_rtAduio;
		m_rtAduio = 0;
		m_JACKorASIO = jack;
		createRtAudio();
	}
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
  qDebug() << "supported sample formats:" << fmt;
}


void TrtAudio::printSupportedSampleRates(RtAudio::DeviceInfo& devInfo) {
  QString sRates;
  for (int i = 0; i < devInfo.sampleRates.size(); i++)
    sRates += QString("%1 ").arg(devInfo.sampleRates.at(i));
  qDebug() << "supported sample rates:"<< sRates;
}


int TrtAudio::duplexCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames, 
																		 double streamTime, RtAudioStreamStatus status, void* userData) {
	Q_UNUSED (streamTime)
	Q_UNUSED (userData)
	if (m_cbOut) {
		if (m_cbOut(outBuffer, nBufferFrames, status))
			if (m_cbIn)
				m_cbIn(inBuffer, nBufferFrames, status);
	} else 
			if (m_cbIn)
				m_cbIn(inBuffer, nBufferFrames, status);
	return 0;
}


int TrtAudio::passInputCallBack(void* outBuffer, void* inBuffer, unsigned int nBufferFrames,
																double streamTime, RtAudioStreamStatus status, void* userData) {
	Q_UNUSED (streamTime)
	Q_UNUSED (userData)
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
	abortStream();
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
	m_isOpened = false;
	closeStream();
  if (!m_inParams)
      m_inParams = new RtAudio::StreamParameters;
  if (!m_outParams)
    m_outParams = new RtAudio::StreamParameters;
	setJACKorASIO(audioParams()->JACKorASIO);
// preparing devices
	int inDevId = -1, outDevId = -1;
  int devCount = rtDevice()->getDeviceCount();
	m_isAlsaDefault = false;
  streamOptions->flags = !RTAUDIO_ALSA_USE_DEFAULT; // reset options flags
  if (devCount) {
    RtAudio::DeviceInfo devInfo;
    for(int i = 0; i < devCount; i++) { // Is there device on the list ??
        if (getDeviceInfo(devInfo, i)) {
          if (devInfo.probed) {
						if (m_inParams && QString::fromLocal8Bit(devInfo.name.data()) == audioParams()->INdevName) {
							inDevId = i;
              m_inDevName = QString::fromLocal8Bit(devInfo.name.data());
						}
						if (m_outParams && QString::fromLocal8Bit(devInfo.name.data()) == audioParams()->OUTdevName) {
							outDevId = i;
              m_outDevName = QString::fromLocal8Bit(devInfo.name.data());
						}
          }
        }
    }
    if (inDevId == -1) { // no device on the list - load default
				if (rtDevice()->getCurrentApi() != RtAudio::LINUX_ALSA) {
					inDevId = rtDevice()->getDefaultInputDevice();
					if (rtDevice()->getDeviceInfo(inDevId).inputChannels <= 0) {
							qDebug("wrong default input device");
							deleteInParams();
						}
				}
    }
    if (outDevId == -1) {
			if (rtDevice()->getCurrentApi() != RtAudio::LINUX_ALSA) {
				outDevId = rtDevice()->getDefaultOutputDevice();
				if (rtDevice()->getDeviceInfo(outDevId).outputChannels <= 0) {
						qDebug("wrong default output device");
						deleteOutParams();
				}
			}
		}
		// Default ALSA device can be set only when both devices are undeclared
		if (inDevId == -1 && outDevId == -1 && rtDevice()->getCurrentApi() == RtAudio::LINUX_ALSA) {
			streamOptions->flags = RTAUDIO_ALSA_USE_DEFAULT;
			m_isAlsaDefault = true;
			if (m_inParams) // As long as ALSA ignores device id for its default the id has to be correct number (0 - devCount - 1)
				inDevId = 0;
			if (m_outParams)
				outDevId = 0;
		}
		if (inDevId == -1)
        inDevId = 0;
    if (outDevId == -1)
        outDevId = 0;
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
	quint32 inSR = 0, outSR = 0;
	if (m_inParams && !getDeviceInfo(inDevInfo, inDevId))
		deleteInParams();
	else
		inSR = determineSampleRate(inDevInfo);
	if (m_outParams && !getDeviceInfo(outDevInfo, outDevId))
		deleteOutParams();
	else
		outSR = determineSampleRate(outDevInfo);
// 	if (inSR != outSR)
	m_sampleRate = qMax(inSR, outSR);
	if (audioParams()->forwardInput && m_inParams && m_outParams)
			m_callBack = &passInputCallBack;
	else
			m_callBack = &duplexCallBack;
	ao()->emitParamsUpdated();
}


bool TrtAudio::getDeviceInfo(RtAudio::DeviceInfo& devInfo, int id) {
  try {
			devInfo = rtDevice()->getDeviceInfo(id);
  }
  catch (RtAudioError& e) {
    qDebug() << "error when probing audio device" << id;
    return false;
  }
  return true;
}


bool TrtAudio::openStream() {
	try {
    if (rtDevice() && !rtDevice()->isStreamOpen()) {
				m_bufferFrames = 1024; // reset when it was overridden by another rt API
				rtDevice()->openStream(m_outParams, m_inParams, RTAUDIO_SINT16, sampleRate(), &m_bufferFrames, m_callBack, 0, streamOptions);
// 				qDebug() << "openStream";
				if (rtDevice()->isStreamOpen()) {
					ao()->emitStreamOpened();
          if (m_isAlsaDefault) {
             if (m_inParams)
               m_inDevName = "ALSA default";
             if (m_outParams)
               m_outDevName = "ALSA default";
          } else {
             if (m_inParams)
               m_inDevName = QString::fromLocal8Bit(rtDevice()->getDeviceInfo(m_inParams->deviceId).name.data());
             if (m_outParams)
               m_outDevName = QString::fromLocal8Bit(rtDevice()->getDeviceInfo(m_outParams->deviceId).name.data());
          }
					if (!m_isOpened) { // print info once per new params set
						if (m_inParams)
							qDebug() << currentRtAPI() << "IN:" << m_inDevName << "samplerate:" << sampleRate() << ", buffer size:" << m_bufferFrames;
						if (m_outParams)
							qDebug() << currentRtAPI() << "OUT:" << m_outDevName << "samplerate:" << sampleRate() << ", buffer size:" << m_bufferFrames;
						m_isOpened = true;
					}
					return true;
      } else
					return false;
    }
  }
  catch (RtAudioError& e) {
    qDebug() << "can't open stream" << QString::fromStdString(e.getMessage());
    return false;
  }
  return true;
}


bool TrtAudio::startStream() {
	if (rtDevice() && !rtDevice()->isStreamOpen())
		if (!openStream())
			return false;
  try {
    if (rtDevice() && !rtDevice()->isStreamRunning()) {
      rtDevice()->startStream();
// 			qDebug("stream started");
		}
  }
  catch (RtAudioError& e) {
    qDebug() << "can't start stream";
    return false;
  }
  return true;
}


void TrtAudio::stopStream() {
  try {
    if (rtDevice() && rtDevice()->isStreamRunning())
      rtDevice()->stopStream();
// 		qDebug("stream stoped");
  }
  catch (RtAudioError& e) {
    qDebug() << "can't stop stream";
  }
}


void TrtAudio::closeStream() {
  try {
    stopStream();
    if (rtDevice() && rtDevice()->isStreamOpen())
      rtDevice()->closeStream();
// 		qDebug("stream closed");
  }
  catch (RtAudioError& e) {
    qDebug() << "can't close stream";
  }
}


void TrtAudio::abortStream() {
	try {
    if (rtDevice() && rtDevice()->isStreamOpen())
      rtDevice()->abortStream();
  }
  catch (RtAudioError& e) {
    qDebug() << "can't abort stream";
  }
}


quint32 TrtAudio::determineSampleRate(RtAudio::DeviceInfo& devInfo) {
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









