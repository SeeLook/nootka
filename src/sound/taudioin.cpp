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

#include "taudioin.h"
#include <QAudio>
#include <QDebug>
#include "tpitchfinder.h"
#include "taudioparams.h"
#include <QTimer>
#include <QThread>


/*static */

QAudioFormat TaudioIN::templAudioFormat = QAudioFormat();


void TaudioIN::prepTemplFormat() {
    templAudioFormat.setChannelCount(1);
    templAudioFormat.setSampleSize(16);
    templAudioFormat.setSampleRate(44100);
    templAudioFormat.setSampleType(QAudioFormat::SignedInt);
    templAudioFormat.setByteOrder(QAudioFormat::LittleEndian);
    templAudioFormat.setCodec("audio/pcm");
}


QStringList TaudioIN::getAudioDevicesList() {
    prepTemplFormat();
    QStringList devList;
    QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i<dL.size(); i++) {
        if (dL[i].isFormatSupported(templAudioFormat))
            devList << dL[i].deviceName();
    }
    return devList;
}


//------------------------------------------------------------------------------------
//------------          constructor     ----------------------------------------------
//------------------------------------------------------------------------------------

QThread *m_thread;

TaudioIN::TaudioIN(TaudioParams* params, QObject* parent) :
    QObject(parent),
    m_audioInput(0),
    m_IOaudioDevice(0),
    m_maxPeak(0),
    m_maxP(0),
    m_gotNote(false),
    m_floatBuff(0),
    m_noteStarted(false),
    m_deviceInfo(QAudioDeviceInfo::defaultInputDevice()),
    m_pitch(new TpitchFinder()),
    m_params(params), // points on gl->A or tmpParams in AudioInSettings
    m_devName("any"),
    m_paused(false)
{    
  m_thread = new QThread();
  m_pitch->moveToThread(m_thread);
  prepTemplFormat();
  setParameters(params);
  m_buffer.resize(8192*4); // calculated by hand, can be not enought
  m_buffer.fill(0);
  
  connect(m_pitch, SIGNAL(found(float,float)), this, SLOT(pitchFreqFound(float,float)));
  connect(m_pitch, SIGNAL(noteStoped()), this, SLOT(noteStopedSlot()));
}

TaudioIN::~TaudioIN()
{
  if (m_audioInput) {
    m_audioInput->stop();
    delete m_audioInput;
  }
  m_buffer.clear();
  m_thread->terminate();
  delete m_pitch;
  delete m_thread;
  if (m_floatBuff)
      delete[] (m_floatBuff);
}

//------------------------------------------------------------------------------------
//------------          methods         ----------------------------------------------
//------------------------------------------------------------------------------------


void TaudioIN::setParameters(TaudioParams* params) {
  setAudioDevice(params->INdevName);
  m_pitch->setIsVoice(params->isVoice);
//   m_params = params;
}

/** Device name is saved to globals and to config file only after changed the Nootka preferences.
* In other cases the default device is loaded. */
bool TaudioIN::setAudioDevice(const QString& devN) {
  if (devN == m_devName)
    return true;
	bool fnd = false;
  QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
  if (dL.size()) { // if there are any devices
    for(int i = 0; i < dL.size(); i++) {
        if (dL[i].deviceName() == devN) { // device found by name
            m_deviceInfo = dL[i];
            fnd = true; 
            break;
        }
    }
    if (!fnd) { // if no devices found by name, try default
      m_deviceInfo = QAudioDeviceInfo::defaultInputDevice();
      fnd = true;
    }
  }
  if (m_audioInput) {
	  delete m_audioInput;
	  m_audioInput = 0;
	}
	
  if (fnd) {
    if (m_deviceInfo.isFormatSupported(templAudioFormat)) {
      qDebug() << "in:" << m_deviceInfo.deviceName();
//       m_params->INdevName = m_deviceInfo.deviceName();
      m_devName = m_deviceInfo.deviceName();
      m_audioInput = new QAudioInput(m_deviceInfo, templAudioFormat, this);
      m_audioInput->setBufferSize(m_buffer.size());
    } else {
		  qDebug() << m_deviceInfo.deviceName() << "format unsupported !!";
      fnd = false;
    }	  
	}	else {
	  qDebug() << "no input devices found";
// 	  m_params->INdevName = "";
    m_devName = "any";
	}
	return fnd;
}


void TaudioIN::initInput() {
//    this->moveToThread(m_thread);
  m_floatsWriten = 0;
  m_maxPeak = 0;
  m_IOaudioDevice = m_audioInput->start();
//   qDebug() << "m_audioInput buffer size:" << m_audioInput->bufferSize();
}


void TaudioIN::startListening() {
//    qDebug("listening started");
  if (m_audioInput) {
    if (!m_floatBuff)
        m_floatBuff = new float[m_pitch->aGl()->framesPerChunk]; // 1024
    initInput();
    if (m_IOaudioDevice) {
      connect(m_IOaudioDevice, SIGNAL(readyRead()), this, SLOT(audioDataReady()));
      connect(m_audioInput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(deviceStateSlot(QAudio::State)));
    }
  }
}

void TaudioIN::stopListening() {
//    qDebug("listening stoped");
  if (m_audioInput) {
      m_audioInput->stop();
      m_pitch->resetFinder();
      m_noteStarted = false;
  }
}

void TaudioIN::wait() {
//    qDebug("listening suspended");
  if (isAvailable()) {
    m_audioInput->suspend();
    m_pitch->resetFinder();
  }  
}

void TaudioIN::go() {
  if (isAvailable() && m_audioInput->state() == QAudio::SuspendedState)
    m_audioInput->resume();
}

void TaudioIN::setIsVoice(bool isV) {
  m_pitch->setIsVoice(isV);
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

/** Range of notes is increased one note down and up.
 * This 46 and 48 are its sign. 
 * Normaly 47 is offset of midi note to Nootka Tnote. */
void TaudioIN::setAmbitus(Tnote loNote, Tnote hiNote) {
  m_pitch->setAmbitus(loNote.getChromaticNrOfNote()+46, hiNote.getChromaticNrOfNote()+48);
}


//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------

//int cntr = 0;

void TaudioIN::audioDataReady() {
//    qDebug() << cntr << ": " << m_audioInput->bytesReady(); cntr++;
//	if (m_audioInput->state() != QAudio::ActiveState && m_audioInput->state() != QAudio::IdleState)
//	  qDebug() << "Input device in state:" << (int)m_audioInput->state();
  
	qint64 bytesReady = m_audioInput->bytesReady();
	qint64 bSize = m_buffer.size();
	qint64 toRead = qMin(bytesReady, bSize);	
	qint64 dataRead = m_IOaudioDevice->read(m_buffer.data(), toRead) / 2;
	if (dataRead > bSize / 2) {
		dataRead = bSize / 2;
		qDebug() << dataRead << "Audio data was cut off. Buffer is too small !!!!";
	}
//    qDebug() << "read data" << dataRead*2 ;
	if (!dataRead) {
//     qDebug() << "no data. TaudioIN in state:" << (int)m_audioInput->state();
		return;
  }
	for (int i = 0; i < dataRead; i++) {
	  qint16 value = *reinterpret_cast<qint16*>(m_buffer.data()+i*2);
	  m_maxP = qMax(m_maxP, value);
	  *(m_floatBuff + m_floatsWriten) = float(value) / 32768.0f;

	  if (m_floatsWriten == m_pitch->aGl()->framesPerChunk-1) {
		m_maxPeak = m_maxP;
//        if (m_maxPeak > m_params->noiseLevel) {
    if (m_pitch->isBussy())
        qDebug() << "data ignored";
    else {
        m_pitch->searchIn(m_floatBuff);
        if (!m_noteStarted) {
          m_noteStarted = true;
        }
//		  }
//        } else {
//          if (m_noteStarted) {
//        m_pitch->searchIn(0);
//        m_noteStarted = false;
//        m_gotNote = false;
//          }
    }
    m_floatsWriten = -1;
    m_maxP = 0;
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
	if (m_floatsWriten == m_pitch->aGl()->framesPerChunk) {
	  m_floatsWriten = -1;
	  m_peakList << m_maxPeak;
	}
	m_floatsWriten++;
  }
}


void TaudioIN::pitchFreqFound(float pitch, float freq) {
//  if(!m_gotNote) {
//     qDebug() << QString::fromStdString(Tnote(qRound(pitch - m_params->a440diff)-47).getName());
  if (!m_paused) {
//       qDebug() << QString::fromStdString(Tnote(qRound(pitch - m_params->a440diff)-47).getName());
       emit noteDetected(Tnote(qRound(pitch - m_params->a440diff)-47));
       emit fundamentalFreq(freq);
  } else {
//      qDebug("pitch found skipped");
  }
//       m_gotNote = true;
//  }
}

void TaudioIN::noteStopedSlot() {
  m_gotNote = false;
}


void TaudioIN::resetSlot() {
    m_audioInput->suspend();
//    wait();
    go();
}

void TaudioIN::deviceStateSlot(QAudio::State auStat) {
//   QString statTxt = "innput state: ";
   switch (auStat) {
//    case QAudio::ActiveState : statTxt += "active"; break;
    /** Iddle state occurs mostly under Mac and it shouldn't. 
     * This is why it should be reseted. 
     * On old machines it can occurs as well so let it be. */
    case QAudio::IdleState :
#if !defined(Q_OS_MAC)
//       statTxt += "iddle";
//       qDebug() << statTxt;
#endif
       resetSlot();
       break;
//    case QAudio::SuspendedState : statTxt += "suspended"; break;
//    case QAudio::StoppedState : statTxt += "stoped"; break;
   }
//   qDebug() << statTxt;
}

