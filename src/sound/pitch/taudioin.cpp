/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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

#include "taudioin.h"
#include <QAudio>
#include <QDebug>
#include "tpitchfinder.h"
#include <QTimer>
// #include <QBuffer>



/*static */
QStringList TaudioIN::getAudioDevicesList() {
	templAudioFormat.setChannelCount(1);
    templAudioFormat.setSampleSize(16);
    templAudioFormat.setSampleRate(44100);
    templAudioFormat.setSampleType(QAudioFormat::SignedInt);
    templAudioFormat.setCodec("audio/pcm");
    QStringList devList;
    QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i<dL.size(); i++) {
        if (dL[i].isFormatSupported(templAudioFormat))
            devList << dL[i].deviceName();
    }
    return devList;
}

QAudioFormat TaudioIN::templAudioFormat = QAudioFormat();


//************************************************************************************/

// float *tmpBuff;

TaudioIN::TaudioIN(QObject *parent) :
    QObject(parent),
    m_audioInput(0),
    m_maxPeak(0),
    m_floatBuff(0),
    m_noteStarted(false),
    m_deviceInfo(QAudioDeviceInfo::defaultInputDevice()),
    m_pitch(new TpitchFinder(this))
{    
  m_params.devName = "";
  m_params.noiseLevel = 70; // 0.2% of 32768 - smallest noise
  m_buffer.resize(8192*2); // samples count in mono signal
  m_buffer.fill(0);
}

TaudioIN::~TaudioIN()
{
  qDebug("TaudioIN deleteing");
  if (m_audioInput) {
	m_audioInput->stop();
	delete m_audioInput;
	qDebug("m_audioInput deleted");
  }
  m_buffer.clear();
  qDebug("buffer cleared");
  if (m_floatBuff) {
// 	delete[] m_floatBuff;
	qDebug("m_floatBuff deleted");
  }
//   delete m_pitch;
// 	delete[] tmpBuff;
}

QString TaudioIN::deviceName() {
  return m_params.devName;
}

void TaudioIN::setParameters(SaudioInParams& params) {
  m_params = params;
  m_pitch->setParameters(m_params);
  qDebug() << (int)m_params.analysisType << m_params.noiseLevel;
}


void TaudioIN::setAudioDevice(const QString &devN) {
    QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i<dL.size(); i++) {
        if (dL[i].deviceName() == devN) {
            m_deviceInfo = dL[i];
            break;
        }
    } //TODO: check when no  device with devN name
	qDebug() << m_deviceInfo.deviceName();    

    if (m_audioInput)	  
	  delete m_audioInput;
	m_params.devName = m_deviceInfo.deviceName();
	if (!m_deviceInfo.isFormatSupported(templAudioFormat))
	  qDebug() << m_deviceInfo.deviceName() << "format unsupported !!";
    m_audioInput = new QAudioInput(m_deviceInfo, templAudioFormat, this);
}


void TaudioIN::initInput() {
  m_floatsWriten = 0;
  m_maxPeak = 0;
  m_IOaudioDevice = m_audioInput->start();
}


void TaudioIN::startListening() {
  if (m_audioInput) {
	m_floatBuff = new float[m_pitch->aGl().framesPerChunk+16] + 16;
// 	tmpBuff = new  float[m_pitch->aGl().framesPerChunk+16] + 16;
	initInput();
	if (m_IOaudioDevice)
	  connect(m_IOaudioDevice, SIGNAL(readyRead()), this, SLOT(audioDataReady()));	
  }
}

void TaudioIN::stopListening() {
  m_audioInput->stop(); // TODO: maybe send something to m_pitch to clean it
  m_noteStarted = false;
}


void TaudioIN::calculateNoiseLevel()
{
  if (m_audioInput) {
	initInput();
	m_peakList.clear();
	if (m_IOaudioDevice) {
	  connect(m_IOaudioDevice, SIGNAL(readyRead()), this, SLOT(readToCalc()));
	  QTimer::singleShot(1000, this, SLOT(calc()));
	}
  }
}

void TaudioIN::calc() {
  m_audioInput->stop();
  disconnect(m_IOaudioDevice, SIGNAL(readyRead()), this, SLOT(readToCalc()));
  qint16 noise = 0;
  for (int i = 0; i < m_peakList.size(); i++ )
	noise = qMax(noise, m_peakList[i]);
  emit noiseLevel(noise);
}


void TaudioIN::audioDataReady() {
	
	if (m_audioInput->state() != QAudio::ActiveState && m_audioInput->state() != QAudio::IdleState)
	  qDebug() << "Device in state:" << (int)m_audioInput->state();
	qint64 bytesReady = m_audioInput->bytesReady();
	qint64 bSize = m_buffer.size();
	qint64 toRead = qMin(bytesReady, bSize);
// 	qDebug() << "period:" << bytesReady << m_audioInput->periodSize() << toRead 
// 	  << m_audioInput->periodSize()*2 << m_audioInput->periodSize()*2 + toRead*2;	
	
	qint64 dataRead = m_IOaudioDevice->read(m_buffer.data(), toRead) / 2;
	if (dataRead > bSize/2) {
		dataRead = bSize/2;
// 		m_buffer.resize(m_buffer.size()*2);
		qDebug() << dataRead << "Audio data was cut off. Buffer is too small !!!!";
	}
	
// 	qDebug() << "read data" << dataRead*2 ;
	qint16 maxP = 0;
	for (int i = 0; i < dataRead; i++) {
	  qint16 value = *reinterpret_cast<qint16*>(m_buffer.data()+i*2);
	  maxP = qMax(maxP, value);
	  *(m_floatBuff + m_floatsWriten) = float(value) / 32768.0f;

	  if (m_floatsWriten == m_pitch->aGl().framesPerChunk) {
		m_maxPeak = maxP;
		if (maxPeak() > m_params.noiseLevel) {
// 		  std::copy(tmpBuff, tmpBuff + m_pitch->aGl().framesPerChunk, m_floatBuff);
		  if (m_pitch->isBussy())
			qDebug() << "data ignored";
		  else {
			m_pitch->searchIn(m_floatBuff);
			if (!m_noteStarted) {
// 			  qDebug("note started");
			  m_noteStarted = true;
			}
		  }
		} else {
		  if (m_noteStarted) {
			m_pitch->searchIn(0);
// 			qDebug("note stoped");
			m_noteStarted = false;
		  }			
		}
		m_floatsWriten = -1;	/** FIXME: -1 */
		maxP = 0;
	  }
	  m_floatsWriten++;
	}
}
	
void TaudioIN::readToCalc() {
  qint64 bytesReady = m_audioInput->bytesReady();
  qint64 bSize = m_buffer.size();
  qint64 toRead = qMin(bytesReady, bSize);	
  qint64 dataRead = m_IOaudioDevice->read(m_buffer.data(), toRead) / 2;
  if (dataRead > bSize/2) {
	  dataRead = bSize/2;
	  // buffer size to resize
  }
  for (int i = 0; i < dataRead; i++) {
	qint16 value = *reinterpret_cast<qint16*>(m_buffer.data()+i*2);
	m_maxPeak = qMax(m_maxPeak, value);
	if (m_floatsWriten == m_pitch->aGl().framesPerChunk) {
	  m_floatsWriten = -1;
	  m_peakList << m_maxPeak;
	}
	m_floatsWriten++;
  }
}







