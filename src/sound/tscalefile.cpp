/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tscalefile.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

TscaleFile::TscaleFile() :
  m_audioArr(0)
{}

TscaleFile::~TscaleFile()
{
    deleteData();
}


bool TscaleFile::loadAudioData(QString& path) {
  QFile wavFile(path);
  if (!wavFile.exists())
      return false;
  wavFile.open(QIODevice::ReadOnly);
  QDataStream wavStream(&wavFile);
  
  int fmtSize;
  char *chunkName = new char[4];
  wavStream.skipRawData(16);
  wavStream.readRawData(chunkName, 4);
  fmtSize = *((int*)chunkName);
  wavStream.readRawData(chunkName, 2);
  short wavFormat = *((short*)chunkName);
  
  quint32 dataSizeFromChunk;
  wavStream.readRawData(chunkName, 2);
  unsigned short m_chanels = *((unsigned short*)chunkName);
  wavStream.readRawData(chunkName, 4);
  quint32 sampleRate = *((quint32*)chunkName);
  wavStream.skipRawData(fmtSize - 8 + 4);
  wavStream.readRawData(chunkName, 4);
  dataSizeFromChunk = *((quint32*)chunkName);
//   qDebug() << "data size: " << dataSizeFromChunk << 4740768;
  // we check is wav file this proper one ? 4740766
  if (m_chanels != 1 || wavFormat != 1 || sampleRate != 22050 || dataSizeFromChunk != 4740766) {
      qDebug() << "wav file error occured " << dataSizeFromChunk << m_chanels
              << wavFormat << sampleRate;
      return false;
  }
  
  m_audioArr = new qint16[dataSizeFromChunk / 2];
  wavStream.readRawData((char*)m_audioArr, dataSizeFromChunk);
  wavFile.close();
  return true;
}


void TscaleFile::deleteData() {
  if (m_audioArr) {
    delete m_audioArr;
    m_audioArr = 0;
  }
}



