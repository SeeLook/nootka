/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "taudioin.h"
#include <QAudio>
#include <QDebug>
#include <QTimer>


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
    m_deviceInfo(QAudioDeviceInfo::defaultInputDevice())
{    
	setAudioDevice(m_deviceInfo.deviceName());
// 	setAudioDevice("plughw:CARD=default,DEV=0");


}

TaudioIN::~TaudioIN()
{
	m_buffer.clear();
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

void TaudioIN::startSniffing() {
	
	
	m_buffer.resize(4096*2); // 2048 samples, 16 bits each
	m_buffer.fill(0);
	m_IOaudioDevice = m_audioInput->start();
	connect(m_IOaudioDevice, SIGNAL(readyRead()), this, SLOT(sniffedDataReady()));
		
}

void TaudioIN::sniffedDataReady() {
	qint64 bytesReady = m_audioInput->bytesReady();
	qint64 bSize = m_buffer.size();
	qint64 toRead = qMin(bytesReady, bSize);
	qint64 dataRead = m_IOaudioDevice->read(m_buffer.data(), toRead);
	TsamplePeak peak;
	
	int i = 0;	
	for (i = 0; i < dataRead/2; i++) {
	  qint16 value = *reinterpret_cast<qint16*>(m_buffer.data()+i*2);
	}
	 
}
	
	





