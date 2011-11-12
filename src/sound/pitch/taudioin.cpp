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



TaudioIN::TaudioIN(QObject *parent) :
    QObject(parent),
    m_audioInput(0),
    m_maxPeak(0),
    m_deviceInfo(QAudioDeviceInfo::defaultInputDevice()),
    m_pitch(new TpitchFinder())
{    
	setAudioDevice(m_deviceInfo.deviceName());
// 	setAudioDevice("plughw:CARD=default,DEV=0");


}

TaudioIN::~TaudioIN()
{
	m_buffer.clear();
	delete[] m_floatBuff;
}



void TaudioIN::setAudioDevice(const QString &devN) {

    QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioInput);
    for(int i = 0; i<dL.size(); i++) {
        if (dL[i].deviceName() == devN) {
            m_deviceInfo = dL[i];
            break;
        }
    }
	qDebug() << m_deviceInfo.deviceName();    

    if (m_audioInput)
        delete m_audioInput;
	if (!m_deviceInfo.isFormatSupported(templAudioFormat))
	  qDebug() << m_deviceInfo.deviceName() << "format unsupported !!";
    m_audioInput = new QAudioInput(m_deviceInfo, templAudioFormat, this);
}

void TaudioIN::startListening() {
	
	m_buffer.resize(2048*2); // 2048 samples, 16 bits each
	m_buffer.fill(0);
	m_floatBuff = new float[m_pitch->aGl().framesPerChunk+16] + 16;
	m_floatsWriten = 0;
	m_IOaudioDevice = m_audioInput->start();
	connect(m_IOaudioDevice, SIGNAL(readyRead()), this, SLOT(audioDataReady()));
		
}

void TaudioIN::audioDataReady() {
	qint64 bytesReady = m_audioInput->bytesReady();
	qint64 bSize = m_buffer.size();
	qint64 toRead = qMin(bytesReady, bSize);
	qint64 dataRead = m_IOaudioDevice->read(m_buffer.data(), toRead) / 2;
// 	float *posInBuff = m_buffer;
	
// 	int i = 0;	
	for (int i = 0; i < dataRead; i++) {
	  qint16 value = *reinterpret_cast<qint16*>(m_buffer.data()+i*2);
	  *(m_floatBuff + m_floatsWriten) = float(value) / 32768.0f;
	  m_floatsWriten++;
	  if (m_floatsWriten == m_pitch->aGl().framesPerChunk) {
		m_pitch->searchIn(m_floatBuff);
		m_floatsWriten = 0;
	  }
	}
	 
}
	
	





