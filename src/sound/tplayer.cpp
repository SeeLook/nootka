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


#include "tplayer.h"
#include "tglobals.h"
#include "tnote.h"
#include <QFile>
#include <QDebug>

#define BUFFER_SIZE (256)
#define SAMPLE_RATE (44100)

extern Tglobals *gl;


/* static */
QStringList Tplayer::getAudioDevicesList() {
    QStringList devList;
    PaError err = Pa_Initialize();
    if (err != paNoError)
        return devList;

    int devCnt = Pa_GetDeviceCount();
    if (devCnt < 1)
        return devList;
    const PaDeviceInfo *devInfo;
    for (int i = 0; i < devCnt; i++) {
         devInfo = Pa_GetDeviceInfo(i);
//        QMessageBox::information(0, "", QString::fromStdString(devInfo->name)
//                                 + "Default sample rate: " + QString::number((int)devInfo->defaultSampleRate)
//                    + "ch: " + QString::number((int)devInfo->maxOutputChannels));

        if (devInfo->maxOutputChannels > 1)
            devList << QString::fromLocal8Bit(devInfo->name);
    }
    return devList;
}



int Tplayer::paCallBack(const void *inBuffer, void *outBuffer, unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo,
                        PaStreamCallbackFlags statusFlags, void *userData) {

    m_samplesCnt++;
    if (m_samplesCnt < m_maxCBloops-1) {
        short *data = (short*)userData;
        short *out = (short*)outBuffer;
        int off = m_samplesCnt*framesPerBuffer/2;
        short sample;
        for (int i = 0; i < framesPerBuffer/2; i++) {
            sample = data[m_noteOffset + i + off];
            *out++ = sample;
            *out++ = sample;
            *out++ = sample;
            *out++ = sample;
        }
        return paContinue;
    } else
        return paComplete;
}

int Tplayer::m_samplesCnt = -1;
int Tplayer::m_maxCBloops = 2 * SAMPLE_RATE / BUFFER_SIZE;
int Tplayer::m_noteOffset = 0;
/* end static */

Tplayer::Tplayer()
{
    m_playable = true;
    if (getAudioData()) {
        m_paErr = Pa_Initialize();
        if(m_paErr) {
            m_playable = false;
            return;
        }
        m_paParam.channelCount = 2;
        m_paParam.sampleFormat = paInt16;
//        m_paParam.suggestedLatency = Pa_GetDeviceInfo(m_paParam.device)->defaultLowOutputLatency;
        m_paParam.hostApiSpecificStreamInfo = NULL;
        m_outStream = 0;
        setDevice();
    } else
        m_playable = false;

}

Tplayer::~Tplayer() {
    Pa_CloseStream(m_outStream);
    Pa_Terminate();
    delete m_audioArr;
}


void Tplayer::setDevice() {
    if (m_outStream && QString::fromLocal8Bit(Pa_GetDeviceInfo(m_paParam.device)->name)
            == gl->AoutDeviceName) {
//        qDebug() << "device was not changed";
        return;
    }
    m_playable = true;
    if (m_outStream) { // stream was palyed and we stop it
        if (Pa_IsStreamStopped(m_outStream) == 0)
            Pa_CloseStream(m_outStream);
        else
            Pa_StopStream(m_outStream);
    }
    if (gl->AoutDeviceName == "") // default system device
        m_paParam.device = Pa_GetDefaultOutputDevice();
    else { // or device by name from Tglobals.
        int devId = -1;
        for (int i = 0; i < Pa_GetDeviceCount(); i++) {
                const PaDeviceInfo *devInfo = Pa_GetDeviceInfo(i);
                if (QString::fromLocal8Bit(devInfo->name) == gl->AoutDeviceName)
                    if (devInfo->maxOutputChannels > 1) {
                    devId = i;
                    break;
                }
        }
        if (devId != -1)
            m_paParam.device = devId;
        else
            m_paParam.device = Pa_GetDefaultOutputDevice();
    }
    if (m_paParam.device != paNoDevice) {
        m_paParam.suggestedLatency = Pa_GetDeviceInfo(m_paParam.device)->defaultLowOutputLatency;
        m_paErr = Pa_OpenStream(&m_outStream, NULL, &m_paParam, SAMPLE_RATE,
                                BUFFER_SIZE, paClipOff, paCallBack, m_audioArr);
    } else {
        m_playable = false;
        return;
    }

    if(m_paErr) {
        m_playable = false;
//        qDebug() << QString::fromStdString(Pa_GetErrorText(m_paErr));
    }
}

void Tplayer::play(Tnote note) {
//    qDebug() << (int)note.getChromaticNrOfNote();
    if (!m_playable)
        return;
    int noteNr = note.getChromaticNrOfNote();
    if (noteNr < -11 || noteNr > 41)
        return; // beyond wav file scale
//     if (Pa_IsStreamActive(m_outStream) == 1) {
//         m_paErr = Pa_AbortStream(m_outStream);
// 		if(m_paErr)
// 	        qDebug() << "abort error:" << QString::fromStdString(Pa_GetErrorText(m_paErr));
// 	} 
	if (Pa_IsStreamStopped(m_outStream) == 0) {
	    m_paErr = Pa_StopStream(m_outStream);
		if(m_paErr)
	        qDebug() << "stop error:" << QString::fromStdString(Pa_GetErrorText(m_paErr));
	}
    m_samplesCnt = -1;
    m_noteOffset = (noteNr + 11)*SAMPLE_RATE;

    m_paErr = Pa_StartStream(m_outStream);
   if(m_paErr) {
       qDebug() << "start stream error:" << QString::fromStdString(Pa_GetErrorText(m_paErr));
   }
}


bool Tplayer::getAudioData() {
    QFile wavFile(gl->path + "sounds/classical-guitar.wav");
    if (!wavFile.exists())
        return false;
    wavFile.open(QIODevice::ReadOnly);
    QDataStream wavStream(&wavFile);

//    char *chunkName = new char[3];
//    wavStream.readRawData(chunkName, 3);
//    QString riffHead(chunkName);
//    wavStream.skipRawData(5);
//    wavStream.readRawData(chunkName, 3);
//    QString waveHead(chunkName);
//    wavStream.skipRawData(1);
//    wavStream.readRawData(chunkName, 3);
//    QString fmtHead(chunkName);
//    if ( (riffHead != "RIF") || (waveHead != "WAV") || (fmtHead != "fmt") ) {
//        qDebug() << "wav file is not valid!!!" << riffHead << waveHead << fmtHead;
//        return;
//    }
//    wavStream.skipRawData(1);
//    delete chunkName;

    int fmtSize;
    char *chunkName = new char[4];
    wavStream.skipRawData(16);
    wavStream.readRawData(chunkName, 4);
    fmtSize = *((int*)chunkName);
    wavStream.readRawData(chunkName, 2);
    short wavFormat = *((short*)chunkName);
//    if (wavFormat != 1) {
//        qDebug() << "has unsuported data format!!! Use only PCM uncompresed, please.";
//        return false;
//    }

    quint32 dataSizeFromChunk;
    wavStream.readRawData(chunkName, 2);
    unsigned short m_chanels = *((unsigned short*)chunkName);
    wavStream.readRawData(chunkName, 4);
    m_sampleRate = *((quint32*)chunkName);
    wavStream.skipRawData(fmtSize - 8 + 4);
    wavStream.readRawData(chunkName, 4);
    dataSizeFromChunk = *((quint32*)chunkName);
//    qDebug() << "data size: " << dataSizeFromChunk << 4740768;
    // we check is wav file this proper one ?
    if (m_chanels != 1 || wavFormat != 1 || m_sampleRate != 22050 || dataSizeFromChunk != 4740768) {
        qDebug() << "wav file error occured " << dataSizeFromChunk << m_chanels
                << wavFormat << m_sampleRate;
        return false;
    }
    m_audioArr = new char[dataSizeFromChunk];
    wavStream.readRawData(m_audioArr, dataSizeFromChunk);

    wavFile.close();
    return true;
}
