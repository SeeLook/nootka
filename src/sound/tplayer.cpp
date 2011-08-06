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
#include <cmath>
#include <QFile>
#include <QDebug>


extern Tglobals *gl;

Tplayer::Tplayer()
{

    getAudioData();

    m_paErr = Pa_Initialize();
    if(m_paErr)
        return;

    m_paParam.device = Pa_GetDefaultOutputDevice();
    if (m_paParam.device != paNoDevice)
        qDebug() << "found audio dev";

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
      wavStream.skipRawData(1);
      wavStream.readRawData(chunkName, 4);
      fmtSize = getValueFromChunk(chunkName, 4);
      wavStream.readRawData(chunkName, 2);
      short wavFormat = getValueFromChunk(chunkName, 2);
      if (wavFormat != 1) {
            qDebug() << "has unsuported data format!!! Use only PCM uncompresed, please.";
            return;
        }

      unsigned short chanels;
      quint32 sampleRate;
      quint32 dataSizeFromChunk;

      wavStream.readRawData(chunkName, 2);
      chanels = getValueFromChunk(chunkName, 2);
      qDebug() << "chanels: " << chanels;
      wavStream.readRawData(chunkName, 4);
      sampleRate = getValueFromChunk(chunkName, 4);
      qDebug() << "sample rate: " << sampleRate;
      wavStream.skipRawData(fmtSize - 8 + 4);
      wavStream.readRawData(chunkName, 4);
      dataSizeFromChunk = getValueFromChunk(chunkName, 4);
      qDebug() << "data size: " << (dataSizeFromChunk/1000)/1000 << "MB";

      wavFile->close();

}

int Tplayer::getValueFromChunk(char *chunk, int len) {
    int res = 0;
    for (int i = 0; i < len; ++i) {
        res = res + (quint8)chunk[i] * pow(256,i);
    }
    return res;
}
