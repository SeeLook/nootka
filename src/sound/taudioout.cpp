/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk            *
 *   tomaszbojczuk@gmail.com                 *
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
 *  You should have received a copy of the GNU General Public License    *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "taudioout.h"
#include "taudioparams.h"
#include "tnote.h"
#include "RtMidi.h"
#include <QTimer>
#include <QFile>
#include <QStringList>
#include <QAudio>


//------------------ static methods ------------------------------------------------------

QAudioFormat TaudioOUT::templAudioFormat = QAudioFormat();

QStringList TaudioOUT::getAudioDevicesList() {
	templAudioFormat.setChannelCount(2);
  templAudioFormat.setSampleSize(16);
  templAudioFormat.setSampleRate(44100);
  templAudioFormat.setSampleType(QAudioFormat::SignedInt);
  templAudioFormat.setCodec("audio/pcm");
  QStringList devList;
  QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for(int i = 0; i<dL.size(); i++) {
        if (dL[i].isFormatSupported(templAudioFormat))
            devList << dL[i].deviceName();
    }
  return devList;
}

QStringList TaudioOUT::getMidiPortsList() {
  RtMidiOut *midiOut = 0;
  try {
    midiOut = new RtMidiOut();
  }
  catch ( RtError &error ) {
      error.printMessage();
  }
  QStringList portList;
  if (midiOut->getPortCount())
    for (int i = 0; i < midiOut->getPortCount(); i++)
      portList << QString::fromStdString(midiOut->getPortName(i));
  delete midiOut;
  return portList;  
}


//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams* params, QString& path, QObject* parent) :
  QObject(parent),
  m_wavFile(path + "sounds/classical-guitar.wav"),
  m_isMidi(false),
  m_playable(false),
  m_midiOut(0),
  m_audioOutput(0),
  m_IOaudioDevice(0),
  m_devName(""),
  m_audioArr(0)
{
  
  setAudioOutParams(params);
}

TaudioOUT::~TaudioOUT()
{
  if (m_audioArr)
    delete m_audioArr;
}

void TaudioOUT::setAudioOutParams(TaudioParams* params) {
  
  if (params->midiEnabled) { // Midi output
    if (!m_midiOut) { // prepare midi and delete audio
      m_isMidi = true;
      
    }    
  } else { // Audio output
    if (!m_audioOutput) { // prepare audio
      
    }
    
  }
}

bool TaudioOUT::loadAudioData() {
  QFile wavFile(m_wavFile);
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
  m_sampleRate = *((quint32*)chunkName);
  wavStream.skipRawData(fmtSize - 8 + 4);
  wavStream.readRawData(chunkName, 4);
  dataSizeFromChunk = *((quint32*)chunkName);
//    qDebug() << "data size: " << dataSizeFromChunk << 4740768;
  // we check is wav file this proper one ? 4740766
  if (m_chanels != 1 || wavFormat != 1 || m_sampleRate != 22050 || dataSizeFromChunk != 4740766) {
      qDebug() << "wav file error occured " << dataSizeFromChunk << m_chanels
              << wavFormat << m_sampleRate;
      return false;
  }
  m_audioArr = new char[dataSizeFromChunk];
  wavStream.readRawData(m_audioArr, dataSizeFromChunk);

  wavFile.close();
  return true;
}



//-------------------------------- slots ----------------------------------------------------
void TaudioOUT::feedAudioBuffer() {

}


void TaudioOUT::midiNoteOff() {
  m_timer->stop();
  m_message[0] = 128; // note Off
  m_message[1] = m_prevMidiNote;
  m_message[2] = 0; // volume
  m_midiOut->sendMessage(&m_message);
  m_prevMidiNote = 0;
}



