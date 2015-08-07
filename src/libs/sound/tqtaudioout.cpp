/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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


#include "tqtaudioout.h"
#include "toggscale.h"
#include <taudioparams.h>
#include <QtMultimedia/QAudioOutput>
#include <QtCore/QTimer>
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

    return devList;
}
/*end static*/

//---------------------------------------------------------------------------------------
//                CONSTRUCTOR
//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams *_params, QObject *parent) :
  TabstractPlayer(parent),
  m_audioParams(_params),
  oggScale(new ToggScale()),
  ratioOfRate(1),
  m_samplesCnt(0),
  m_bufferFrames(1024),
  m_sampleRate(44100),
  m_doCrossFade(false),
  m_cross(0.0f),
  m_crossCount(0),
  m_callBackIsBussy(false),
  m_outDevice(0)
{
  setType(e_audio);
	m_samplesCnt = 10000;
	m_crossBuffer = new qint16[1000];

  QAudioDeviceInfo defaultOut = QAudioDeviceInfo::defaultOutputDevice();
  qDebug() << "OUT:" << defaultOut.deviceName();
  QAudioFormat format;
    format.setChannelCount(2);
    format.setSampleRate(m_sampleRate);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
  if (!defaultOut.isFormatSupported(format)) {
    qDebug() << "Output Format 44100/16 stereo is not supported";
    format = defaultOut.nearestFormat(format);
    qDebug() << "Format is" << format.sampleRate() << format.channelCount() << format.sampleSize();
  }

  m_sampleRate = format.sampleRate();

  m_audioOUT = new QAudioOutput(defaultOut, format, this);
  m_audioOUT->setBufferSize(m_bufferFrames * 2);

  setAudioOutParams();

  m_maxSamples = m_sampleRate * 1.6;

}


TaudioOUT::~TaudioOUT() 
{
  delete oggScale;
  delete m_crossBuffer;
}


void TaudioOUT::setAudioOutParams() {
// 	qDebug() << "setAudioOutParams";
	playable = oggScale->loadAudioData(audioParams()->audioInstrNr);
  qDebug() << "setAudioOutParams" << playable;
	if (playable /*&& streamParams()*/) {
			ratioOfRate = m_sampleRate / 44100;
			quint32 oggSR = m_sampleRate;
			if (ratioOfRate > 1) { //
				if (m_sampleRate == 88200 || m_sampleRate == 176400)
					oggSR = 44100;
				else if (m_sampleRate == 96000 || m_sampleRate == 192000)
					oggSR = 48000;
			}
			oggScale->setSampleRate(oggSR);
			// Shifts only float part of a440diff - integer part is shifted by play() method
			oggScale->setPitchOffset(audioParams()->a440diff - (float)int(audioParams()->a440diff));

	} else
      playable = false;
}




bool TaudioOUT::play(int noteNr) {
  if (!playable)
      return false;
  
	while (m_callBackIsBussy) {
		  SLEEP(1);
			qDebug() << "Oops! Call back method is in progress when a new note wants to be played!";
	}
	
  if (m_samplesCnt < m_maxSamples) {
			int off = m_samplesCnt + 1; // next chunk of playing sound
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
  if (loops)
    qDebug() << "latency:" << loops << "ms";
  if (m_audioOUT->state() == QAudio::StoppedState) {
    m_outDevice = m_audioOUT->start();
    if (m_outDevice) {
      QTimer::singleShot(1, this, SLOT(outCallBack()));
      qDebug() <<  m_audioOUT->error();
//       connect(m_outDevice, &QIODevice::bytesWritten, this, &TaudioOUT::outCallBack);
//       connect(m_outDevice, &QIODevice::readyRead, this, &TaudioOUT::outCallBack);
      return true;
    }
  } else
      return true;

  return false;
}


void TaudioOUT::outCallBack() {
  if (m_outDevice && m_audioOUT->bytesFree()) {
    int bytesWritten = m_audioOUT->bytesFree();
//     qDebug() << "outCallBack" << bytesWritten << m_audioOUT->bufferSize() << m_outDevice->bytesToWrite() << m_audioOUT->state();
    m_callBackIsBussy = true;
    if (m_doCrossFade) { // Cross-fading avoids cracking during transition of notes.
        m_doCrossFade = false;
        m_cross = 1.0f;
        m_crossCount = 0;
    }
    for (int i = 0; i < (bytesWritten / 2) / ratioOfRate; i++) {
      m_samplesCnt++;
      qint16 sample;
      char* outSample = (char*)&sample;
        if (m_cross > 0.0 && m_crossCount < 1000) { // mix current and previous samples when cross-fading
              sample =  (qint16)qRound((1.0 - m_cross) * (float)oggScale->getSample(m_samplesCnt) +
                      m_cross * (float)m_crossBuffer[m_crossCount]);
              m_cross -= CROSS_STEP;
              m_crossCount++;
        } else {
              sample = oggScale->getSample(m_samplesCnt);
        }
        for (int r = 0; r < ratioOfRate; r++) {
            m_outDevice->write(outSample, 2); // left channel
            m_outDevice->write(outSample, 2); // right channel
        }
    }
    m_callBackIsBussy = false;
    if (m_samplesCnt < m_maxSamples)
      QTimer::singleShot(20, this, SLOT(outCallBack()));
    else
      playingFinishedSlot();
  }
}


void TaudioOUT::playingFinishedSlot() {
  m_audioOUT->stop();
  m_outDevice = 0;
  if (doEmit)
    emit noteFinished();
}


void TaudioOUT::stop() {
  doEmit = false;
  playingFinishedSlot();
}



