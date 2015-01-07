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


#include "trtaudioout.h"
#include "taudioobject.h"
#include "toggscale.h"
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
    int devCnt = rtDevice()->getDeviceCount();
    if (devCnt < 1)
        return devList;
    for (int i = 0; i < devCnt; i++) {
        RtAudio::DeviceInfo devInfo;
        try {
          devInfo = rtDevice()->getDeviceInfo(i);
        }
        catch (RtAudioError& e) {
          qDebug() << "error when probing output device" << i;
          continue;
        }
        if (devInfo.probed && devInfo.outputChannels > 0)
          devList << QString::fromLocal8Bit(devInfo.name.data());
    }
    if (getCurrentApi() == RtAudio::LINUX_ALSA && !devList.isEmpty())
				devList.prepend("ALSA default");
    return devList;
}


int 					TaudioOUT::m_samplesCnt = 0;
int 					TaudioOUT::m_maxCBloops = 44100 / TrtAudio::bufferFrames() * 2;
TaudioOUT* 		TaudioOUT::instance = 0;
qint16* 			TaudioOUT::m_crossBuffer = 0;
bool 					TaudioOUT::m_doCrossFade = false;
float 				TaudioOUT::m_cross = 0.0f;


bool TaudioOUT::outCallBack(void* outBuff, unsigned int nBufferFrames, const RtAudioStreamStatus& status) {
	instance->m_callBackIsBussy = true;
  if ( status )
			qDebug() << "Stream underflow detected!";
	if (m_doCrossFade) { // Cross-fading avoids cracking during transition of notes.
			m_doCrossFade = false;
			m_cross = 1.0f;
			instance->m_crossCount = 0;
	}
  if (m_samplesCnt < m_maxCBloops - 10) {
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
  setType(e_audio);
  setAudioOutParams();
	m_samplesCnt = 10000;
  instance = this;
	m_crossBuffer = new qint16[1000];
	connect(ao(), SIGNAL(streamOpened()), this, SLOT(streamOpenedSlot()));
	connect(ao(), SIGNAL(paramsUpdated()), this, SLOT(updateSlot()));
}


TaudioOUT::~TaudioOUT() 
{
//   abortStream();
  closeStream();
  deleteOutParams();
  resetCallBack();
  delete oggScale;
	if (m_crossBuffer)
		delete m_crossBuffer;
}


void TaudioOUT::setAudioOutParams() {
// 	qDebug() << "setAudioOutParams";
	playable = oggScale->loadAudioData(audioParams()->audioInstrNr);
	if (playable && streamParams()) {
			ratioOfRate = sampleRate() / 44100;
			quint32 oggSR = sampleRate();
			if (ratioOfRate > 1) { //
				if (sampleRate() == 88200 || sampleRate() == 176400)
					oggSR = 44100;
				else if (sampleRate() == 96000 || sampleRate() == 192000)
					oggSR = 48000;
			}
			oggScale->setSampleRate(oggSR);
			// Shifts only float part of a440diff - integer part is shifted by play() method
			oggScale->setPitchOffset(audioParams()->a440diff - (float)int(audioParams()->a440diff));
	} else
        playable = false;
}

void TaudioOUT::streamOpenedSlot() {
	m_maxCBloops = (88200 * ratioOfRate) / bufferFrames();
}


bool TaudioOUT::play(int noteNr) {
  if (!playable /*|| audioParams()->forwardInput*/) // if forwarding is enabled play() makes no sense
      return false;
  
	while (m_callBackIsBussy) {
		  SLEEP(1);
// 			qDebug() << "Oops! Call back method is in progress when a new note wants to be played!";
	}
	
  if (m_samplesCnt < m_maxCBloops - 10) {
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
  return startStream();
}


void TaudioOUT::stop() {
  abortStream();
  stopStream();
// 	closeStream();
}



