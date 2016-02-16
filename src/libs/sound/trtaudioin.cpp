/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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

#include "trtaudioin.h"
#include "taudioparams.h"
#include <tinitcorelib.h>
#if defined(Q_OS_WIN)
  #include "tasioemitter.h"
#endif



/*static */
QStringList TaudioIN::getAudioDevicesList() {
	QStringList devList;
	createRtAudio();
  if (getCurrentApi() == RtAudio::LINUX_ALSA)
      closeStream(); // close ALSA stream to get full list of devices
	int devCnt = getDeviceCount();
	if (devCnt < 1)
			return devList;
	for (int i = 0; i < devCnt; i++) {
			RtAudio::DeviceInfo devInfo;
			if (!getDeviceInfo(devInfo, i))
				continue;
			if (devInfo.probed && devInfo.inputChannels > 0)
				devList << convDevName(devInfo);
	}
	if (getCurrentApi() == RtAudio::LINUX_ALSA && !devList.isEmpty())
			devList.prepend("ALSA default");
	return devList;
}


TaudioIN*        			TaudioIN::m_instance = 0;
bool                  TaudioIN::m_goingDelete = false;

//#################################################################################################
//###################              CONSTRUCTOR         ############################################
//#################################################################################################

TaudioIN::TaudioIN(TaudioParams* params, QObject* parent) :
  TcommonListener(params, parent),
  TrtAudio(params, e_input, inCallBack)
{
  if (m_instance) {
    qDebug() << "Nothing of this kind... TaudioIN already exist!";
    return;
  }
  m_instance = this;
  setAudioInParams();
	m_goingDelete = false;
	forceUpdate = true;
  
	connect(ao(), &TaudioObject::paramsUpdated, this, &TaudioIN::updateSlot);
  connect(ao(), &TaudioObject::playingFinished, this, &TaudioIN::playingFinishedSlot);
}

TaudioIN::~TaudioIN()
{
	m_goingDelete = true;
  closeStream();
	finder()->blockSignals(true);
  m_instance = 0;
  deleteInParams();
  resetCallBack();
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void TaudioIN::setAudioInParams() {
  TcommonListener::setAudioInParams();
  finder()->setSampleRate(inRate(), detectionRange()); // framesPerChunk is determined here
#if defined(Q_OS_WIN)
  if (getCurrentApi() == RtAudio::WINDOWS_ASIO)
    connect(rtDevice()->emitter(), &TASIOEmitter::resetASIO, this, &TaudioIN::ASIORestartSlot, Qt::UniqueConnection);
#endif
//   qDebug() << "setAudioInParams" << "\nrate:" << sampleRate() << "\nmethod:" << audioParams()->detectMethod
//            << "\nmin duration" << audioParams()->minDuration << "\nmin volume" << audioParams()->minimalVol
//            << "\nsplit volume" << (finder()->isSplitByVolume() ? finder()->minVolumeToSplit() * 100.0 : 0.0)
//            << "\nskip volume" << finder()->skipStillerValue() * 100.0
//            << "\nnoise filter:" << finder()->aGl()->equalLoudness << "\ndetection range:" << detectionRange();
}

//#################################################################################################
//###################         PUBLIC SLOTS             ############################################
//#################################################################################################

void TaudioIN::startListening() {
	if (!streamParams()) {
			qDebug() << "Can not start listening due to uninitialized input";
			return;
	}
	if (detectingState() != e_detecting) {
    resetVolume();
    if (!stoppedByUser()) {
      if (areStreamsSplit() && detectingState() != e_detecting)
        openStream();
      if (startStream())
        setState(e_detecting);
// 			qDebug() << "start listening";
		}
  }
}


void TaudioIN::stopListening() {
  if (detectingState() != e_stopped) {
//     qDebug() << "stop listening";
    resetVolume();
    resetChunkPitch();
		if (areStreamsSplit() || rtDevice()->getCurrentApi() != RtAudio::LINUX_PULSE)
			abortStream();
    setState(e_stopped);
    finder()->resetFinder();
  }
}

//#################################################################################################
//###################               PRIVATE SLOTS      ############################################
//#################################################################################################

void TaudioIN::playingFinishedSlot() {
  if (state() == e_listening) {
    openStream();
    startStream();
  }
}

#if defined(Q_OS_WIN)
void TaudioIN::ASIORestartSlot() {
  restartASIO();
}
#endif









