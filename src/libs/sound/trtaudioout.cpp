/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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


#include "trtaudioout.h"
#include "toggscale.h"
#include "tasioemitter.h"
#include <taudioparams.h>
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
        if (devInfo.probed && devInfo.outputChannels > 0)
          devList << convDevName(devInfo);
    }
    if (getCurrentApi() == RtAudio::LINUX_ALSA && !devList.isEmpty())
				devList.prepend("ALSA default");
    return devList;
}


int 					TaudioOUT::m_samplesCnt = 0;
int 					TaudioOUT::m_maxCBloops = 44100 / TrtAudio::bufferFrames() * 2 - 10;
TaudioOUT* 		TaudioOUT::instance = 0;
qint16* 			TaudioOUT::m_crossBuffer = 0;
bool 					TaudioOUT::m_doCrossFade = false;
float 				TaudioOUT::m_cross = 0.0f;


bool TaudioOUT::outCallBack(void* outBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status) {
	instance->m_callBackIsBussy = true;
//   if (status) // It doesn't harm if occurs
// 			qDebug() << "Stream underflow detected!";
	if (m_doCrossFade) { // Cross-fading avoids cracking during transition of notes.
			m_doCrossFade = false;
			m_cross = 1.0f;
			instance->m_crossCount = 0;
	}
  if (m_samplesCnt < m_maxCBloops) {
			m_samplesCnt++;
      qint16 *out = (qint16*)outBuff;
      int off = m_samplesCnt * (nBufferFrames / instance->ratioOfRate);
      qint16 sample;
      for (int i = 0; i < nBufferFrames / instance->ratioOfRate; i++) {
				if (m_cross > 0.0 && instance->m_crossCount < 1000) { // mix current and previous samples when cross-fading
							sample =  (qint16)qRound((1.0 - m_cross) * (float)instance->oggScale->getSample(off + i) + 
											m_cross * (float)m_crossBuffer[instance->m_crossCount]);
							m_cross -= CROSS_STEP;
							instance->m_crossCount++;
				} else {
							sample = instance->oggScale->getSample(off + i);
				}
				for (int r = 0; r < instance->ratioOfRate; r++) {
						*out++ = sample; // left channel
						*out++ = sample; // right channel
				}
      }
      instance->m_callBackIsBussy = false;
      if (m_samplesCnt == m_maxCBloops)
        return true;
      else
        return false;
  } else {
			instance->m_callBackIsBussy = false;
      return true;
	}			
}
/*end static*/

//---------------------------------------------------------------------------------------
//                CONSTRUCTOR
//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams *_params, QObject *parent) :
  TabstractPlayer(parent),
  TrtAudio(_params, e_output, &outCallBack),
  oggScale(new ToggScale()),
  ratioOfRate(1),
  m_crossCount(0),
  m_callBackIsBussy(false)
{
  if (instance) {
    qDebug() << "Nothing of this kind... TaudioOUT already exist!";
    return;
  }
  setType(e_audio);
  setAudioOutParams();
	m_samplesCnt = 10000;
  instance = this;
  forceUpdate = true;
	m_crossBuffer = new qint16[1000];
  connect(ao(), &TaudioObject::streamOpened, this, &TaudioOUT::streamOpenedSlot);
	connect(ao(), &TaudioObject::paramsUpdated, this, &TaudioOUT::updateSlot);
  connect(ao(), &TaudioObject::playingFinished, this, &TaudioOUT::playingFinishedSlot);
}


TaudioOUT::~TaudioOUT() 
{
  closeStream();
  deleteOutParams();
  resetCallBack();
  delete oggScale;
	if (m_crossBuffer)
		delete m_crossBuffer;
  instance = 0;
}


void TaudioOUT::setAudioOutParams() {
// 	qDebug() << "setAudioOutParams";
	playable = oggScale->loadAudioData(audioParams()->audioInstrNr);
	if (playable && streamParams()) {
			ratioOfRate = outRate() / 44100;
			quint32 oggSR = outRate();
			if (ratioOfRate > 1) { //
				if (outRate() == 88200 || outRate() == 176400)
					oggSR = 44100;
				else if (outRate() == 96000 || outRate() == 192000)
					oggSR = 48000;
			}
			oggScale->setSampleRate(oggSR);
			// Shifts only float part of a440diff - integer part is shifted by play() method
			oggScale->setPitchOffset(audioParams()->a440diff - (float)int(audioParams()->a440diff));
#if defined(Q_OS_WIN)
      if (getCurrentApi() == RtAudio::WINDOWS_ASIO)
        connect(rtDevice()->emitter(), &TASIOEmitter::resetASIO, this, &TaudioOUT::ASIORestartSlot, Qt::UniqueConnection);
#endif
	} else
        playable = false;
}


void TaudioOUT::streamOpenedSlot() {
  m_maxCBloops = (outRate() * 1.6) / bufferFrames();
}


bool TaudioOUT::play(int noteNr) {
  if (!playable /*|| audioParams()->forwardInput*/) // if forwarding is enabled play() makes no sense
      return false;
  
	while (m_callBackIsBussy) {
		  SLEEP(1);
// 			qDebug() << "Oops! Call back method is in progress when a new note wants to be played!";
	}
	
  if (m_samplesCnt < m_maxCBloops) {
			int off = (m_samplesCnt + 1) * (bufferFrames() / ratioOfRate); // next chunk of playing sound
			for (int i = 0; i < 1000; i++) // copy data of current sound to perform crrosfading
				m_crossBuffer[i] = oggScale->getSample(off + i);
			m_doCrossFade = true;
	} else
			m_doCrossFade = false;
  
  noteNr = noteNr + int(audioParams()->a440diff);
	
  doEmit = true;
  oggScale->setNote(noteNr);
  int loops = 0;
  while (!oggScale->isReady() && loops < 40) { // 40ms - max latency
      SLEEP(1);
      loops++;
  }
  m_samplesCnt = -1;
//   if (loops) qDebug() << "latency:" << loops << "ms";
  if (areStreamsSplit() && state() != e_playing)
    openStream();
  return startStream();
}


void TaudioOUT::playingFinishedSlot() {
  if (areStreamsSplit() && state() == e_playing) {
    closeStream();
  }
}


void TaudioOUT::stop() {
	if (areStreamsSplit() || getCurrentApi() == RtAudio::LINUX_PULSE)
		closeStream();
  else
    abortStream();
}


#if defined(Q_OS_WIN)
void TaudioOUT::ASIORestartSlot() {
  if (!hasCallBackIn()) // Perform restart only when no input otherwise input does it
    restartASIO();
}
#endif

