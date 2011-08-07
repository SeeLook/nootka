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
//#include <cmath>
#include <QFile>
#include <QDebug>


extern Tglobals *gl;


/* static */
int Tplayer::paCallBack(const void *inBuffer, void *outBuffer, unsigned long framesPerBuffer,
                        const PaStreamCallbackTimeInfo *timeInfo,
                        PaStreamCallbackFlags statusFlags, void *userData) {

    m_samplesCnt++;
    if (m_samplesCnt < m_maxCBloops-1) {
        short *data = (short*)userData;
        short *out = (short*)outBuffer;
        int off = m_samplesCnt*framesPerBuffer;
        for (int i = 0; i < framesPerBuffer; i++)
            *out++ = data[m_noteOffset + i + off];
        return paContinue;
    } else
        return paComplete;
}

int Tplayer::m_samplesCnt = -1;
int Tplayer::m_maxCBloops = 44100 / 256;
int Tplayer::m_noteOffset = 0;
/* end static */

Tplayer::Tplayer()
{

    getAudioData();

    m_paErr = Pa_Initialize();
    if(m_paErr)
        return;

    m_paParam.device = Pa_GetDefaultOutputDevice();
    if (m_paParam.device != paNoDevice)
        qDebug() << "found audio dev: " << QString::fromStdString(Pa_GetDeviceInfo(m_paParam.device)->name);

    m_paParam.channelCount = m_chanels;
    m_paParam.sampleFormat = paInt16;
    m_paParam.suggestedLatency = Pa_GetDeviceInfo(m_paParam.device)->defaultLowOutputLatency;
    m_paParam.hostApiSpecificStreamInfo = NULL;

    m_paErr = Pa_OpenStream(&m_outStream, NULL, &m_paParam, m_sampleRate,
                            256, paClipOff, paCallBack, m_audioArr);

    if(m_paErr) {
        qDebug() << QString::fromStdString(Pa_GetErrorText(m_paErr));
        return;
    }
}

Tplayer::~Tplayer() {
    delete m_audioArr;
    Pa_CloseStream(m_outStream);
    Pa_Terminate();
}


void Tplayer::getAudioData() {
      QFile *wavFile = new QFile(gl->path + "sounds/classical-guitar.wav");
      wavFile->open(QIODevice::ReadOnly);
      QDataStream wavStream(wavFile);
      char *chunkName = new char[3];
      wavStream.readRawData(chunkName, 3);
      QString riffHead(chunkName);
      wavStream.skipRawData(5);
      wavStream.readRawData(chunkName, 3);
      QString waveHead(chunkName);
      wavStream.skipRawData(1);
      wavStream.readRawData(chunkName, 3);
      QString fmtHead(chunkName);
      if ( (riffHead != "RIF") || (waveHead != "WAV") || (fmtHead != "fmt") ) {
          qDebug() << "wav file is not valid!!!";
          return;
      }
      int fmtSize;
      delete chunkName;
      chunkName = new char[4];
      wavStream.skipRawData(1);
      wavStream.readRawData(chunkName, 4);
      fmtSize = *((int*)chunkName);
      wavStream.readRawData(chunkName, 2);
      short wavFormat = *((short*)chunkName);
      if (wavFormat != 1) {
            qDebug() << "has unsuported data format!!! Use only PCM uncompresed, please.";
            return;
        }

      quint32 dataSizeFromChunk;

      wavStream.readRawData(chunkName, 2);
      m_chanels = *((unsigned short*)chunkName);
      qDebug() << "m_chanels: " << m_chanels;
      wavStream.readRawData(chunkName, 4);
      m_sampleRate = *((quint32*)chunkName);
      qDebug() << "sample rate: " << m_sampleRate;
      wavStream.skipRawData(fmtSize - 8 + 4);
      wavStream.readRawData(chunkName, 4);
      dataSizeFromChunk = *((quint32*)chunkName);
      qDebug() << "data size: " << (dataSizeFromChunk/1000)/1000 << "MB";
      m_audioArr = new char[dataSizeFromChunk];
      wavStream.readRawData(m_audioArr, dataSizeFromChunk);

      wavFile->close();

}

void Tplayer::play(Tnote note) {
//    qDebug() << (int)note.getChromaticNrOfNote();
    if (Pa_IsStreamActive(m_outStream) == 1) {
        m_paErr = Pa_AbortStream(m_outStream);
		if(m_paErr)
	        qDebug() << "abort error:" << QString::fromStdString(Pa_GetErrorText(m_paErr));
	}
	if (Pa_IsStreamStopped(m_outStream) == 0) {
	    m_paErr = Pa_StopStream(m_outStream);
		if(m_paErr)
	        qDebug() << "stop error:" << QString::fromStdString(Pa_GetErrorText(m_paErr));
	}
    m_samplesCnt = -1;
    m_noteOffset = (note.getChromaticNrOfNote() + 11)*44100;

    m_paErr = Pa_StartStream(m_outStream);
    if(m_paErr) {
        qDebug() << "start stream error:" << QString::fromStdString(Pa_GetErrorText(m_paErr));
    }
}
