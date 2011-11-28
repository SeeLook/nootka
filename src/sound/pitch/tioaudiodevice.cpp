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


#include "tioaudiodevice.h"


const quint16 framesPerChunk = 1024;
qint16 mPeak = 0;


TIOaudioDevice::TIOaudioDevice(QObject* parent):
  QIODevice(parent),
  m_maxPeak(0),
  m_floatBuffer(0),
  m_floatsWriten(0)
{
  m_floatBuffer = new float[framesPerChunk];
}

TIOaudioDevice::~TIOaudioDevice() {
  if (m_floatBuffer)
	delete m_floatBuffer;
}


//------------------------------------------------------------------------------------
//------------          protected virtual methods       ------------------------------
//------------------------------------------------------------------------------------

qint64 TIOaudioDevice::writeData(const char* data, qint64 len) {
  if (len%2)
	qDebug() << "data lenght is odd";
  const unsigned char *audioData = reinterpret_cast<const unsigned char *>(data);
  const int numSamples = len / 2;
  for(int i = 0; i < numSamples; i++) {
	qint16 value = *reinterpret_cast<qint16*>(audioData);
	mPeak = qMax(mPeak, value);
	*(m_floatBuffer + m_floatsWriten) = float(value) / 32768.0f;
	
	if (m_floatsWriten == framesPerChunk) {
	  m_maxPeak = mPeak;
	  emit chunkReady();
	  mPeak = 0;
	  m_floatsWriten = -1;
	}
	m_floatsWriten++;
  }
}



qint64 TIOaudioDevice::readData(char* data, qint64 maxlen) {
    Q_UNUSED(data)
    Q_UNUSED(maxlen)
    return 0;
}



