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
#include <QDebug>

#define SAMPLE_RATE (44100)

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
  m_devName("anything"),
  m_audioArr(0),
  m_params(params)
{
  m_timer = new QTimer(this);
  setAudioOutParams(params);
}

TaudioOUT::~TaudioOUT()
{
  deleteAudio();
  deleteMidi();
}

//---------------------------------------------------------------------------------------
void TaudioOUT::setAudioOutParams(TaudioParams* params) {
  m_timer->disconnect();
  if (params->midiEnabled) { // Midi output
    if (!m_midiOut) { // prepare midi and delete audio
      m_isMidi = true;
      setMidiParams();
      deleteAudio();
      if (m_playable)
          connect(m_timer, SIGNAL(timeout()), this, SLOT(midiNoteOff()));
    }    
  } else { // Audio output
      m_isMidi = false;
      if (m_devName != params->OUTdevName) { //device doesn't exists
          if (setAudioDevice(params->OUTdevName))
            m_playable = loadAudioData();
          else
            m_playable = false;
      }
      if (m_playable) {
          connect(m_timer, SIGNAL(timeout()), this, SLOT(timeForAudio()));
//           m_buffer.resize(2048);
      }
      deleteMidi();
  }
}


bool TaudioOUT::setAudioDevice(QString& name) {
    bool fnd = false;
    QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    for(int i = 0; i < dL.size(); i++) { // Is there device on the list ??
        if (dL[i].deviceName() == name) { // There is !!
            m_deviceInfo = dL[i];
            fnd = true;
            break;
        }
    }
    if (m_audioOutput) {
        delete m_audioOutput;
        m_audioOutput = 0;
    }
    if (!fnd) { // no device on the list - load default
        m_deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
        fnd = true;
    }
//     if (!m_deviceInfo.isFormatSupported(templAudioFormat)) {
//         qDebug() << m_deviceInfo.deviceName() << "format unsupported !!";
//         fnd = false;
//     }
    if (fnd) {
        qDebug() << m_deviceInfo.deviceName();
        m_devName = m_deviceInfo.deviceName();
        m_audioOutput = new QAudioOutput(m_deviceInfo, templAudioFormat, this);
    }
    return fnd;
}

void TaudioOUT::play(Tnote note) {
  if (!m_playable)
        return;
  
  int noteNr = note.getChromaticNrOfNote();
  if (m_params->midiEnabled) {
    if (m_prevMidiNote)  // note is played and has to be turned off. Volume is pushed.
        midiNoteOff();    
    m_prevMidiNote = noteNr + 47;
    m_message[0] = 144; // note On
    m_message[1] = m_prevMidiNote;
    m_message[2] = 100; // volume
    m_midiOut->sendMessage(&m_message);
    m_timer->start(1500);
  
  } else { // play audio
    if (noteNr < -11 || noteNr > 41)
        return;
    qDebug() << "note" << noteNr;
    if (m_timer->isActive()) {
      m_timer->stop();
      m_audioOutput->stop();
    }
    m_samplesCnt = 0;
    m_noteOffset = (noteNr + 11)*SAMPLE_RATE*2;
    m_IOaudioDevice = m_audioOutput->start();
//     qDebug() << m_audioOutput->periodSize();
    m_buffer.resize(m_audioOutput->periodSize());
    timeForAudio();
    m_timer->start(20);
  }

}

void TaudioOUT::deleteAudio() {
  if (m_audioArr) {
    delete m_audioArr;
    m_audioArr = 0;
  }
  if (m_audioOutput) {
    delete m_audioOutput;
    m_audioOutput = 0;
  }
}

void TaudioOUT::deleteMidi() {
  if (m_midiOut) {
    delete m_midiOut;
    m_midiOut = 0;
  }
}



void TaudioOUT::setMidiParams() {
  deleteMidi();
  try {
    m_midiOut = new RtMidiOut();
  }
  catch ( RtError &error ) {
    error.printMessage();
    m_playable = false;
    return;
  }

  if (m_midiOut->getPortCount() > 0) {
      unsigned int portNr = 0;
  #if defined(Q_OS_LINUX)
      if(m_params->midiPortName == "")
      m_params->midiPortName = "TiMidity";  // TiMidity port is prefered under Linux
  #endif
      if (m_params->midiPortName != "") {
          for (int i = 0; i < m_midiOut->getPortCount(); i++) {
            if (QString::fromStdString(m_midiOut->getPortName(i)).contains(m_params->midiPortName)) {
              portNr = i;
              break;
            }
          }
      }

      try {
          m_midiOut->openPort(portNr);
      }
      catch (RtError &error){
          error.printMessage();
          m_playable = false;
          return;
      }
      m_params->midiPortName = QString::fromStdString(m_midiOut->getPortName(portNr));
      // midi program (instrument) change
      m_message.push_back(192);
      m_message.push_back(m_params->midiInstrNr); // instrument number
      m_midiOut->sendMessage(&m_message);
      // some spacial signals
      m_message[0] = 241;
      m_message[1] = 60;
      m_midiOut->sendMessage(&m_message);
        
      m_message.push_back(0); // third message param

      m_message[0] = 176;
      m_message[1] = 7;
      m_message[2] = 100; // volume 100;
      m_midiOut->sendMessage(&m_message);
      
  } else
      m_playable = false;
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
   qDebug() << "data size: " << dataSizeFromChunk << 4740768;
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
void TaudioOUT::timeForAudio() {
  if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
    int chunks = m_audioOutput->bytesFree() / m_audioOutput->periodSize();
    qDebug() << "period:" << m_audioOutput->periodSize() << "free:" << m_audioOutput->bytesFree() << chunks;
    qint16 *data = (qint16*)m_audioArr;
//     qint16 *out = (qint16*)m_buffer.data();
    qint16 sample;
    while (chunks) {
//       qDebug() << chunks << m_samplesCnt;
      qint16 *out = (qint16*)m_buffer.data();
      for(int i=0; i < m_audioOutput->periodSize()/8; i++) {
        sample = data[m_noteOffset + m_samplesCnt];
//         for (int j=0; j<4; j++) {
          *out = sample;
          *out++ = sample;
          *out++ = sample;
          *out++ = sample;
          out++;
//           *(m_buffer.data() + j*2 + i*8) = m_audioArr[m_noteOffset + m_samplesCnt];
//           *(m_buffer.data() + j*2 + i*8 +1) = m_audioArr[m_noteOffset + m_samplesCnt +1];
//         }
        m_samplesCnt++;
        if (m_samplesCnt == SAMPLE_RATE*2) {
            m_audioOutput->stop();
            m_timer->stop();
            qDebug() << "done";
            return;
        }
      }
      m_IOaudioDevice->write(m_buffer.data(), (m_audioOutput->periodSize()/8)*8);
      chunks--;
    }
  }
}


void TaudioOUT::midiNoteOff() {
  m_timer->stop();
  m_message[0] = 128; // note Off
  m_message[1] = m_prevMidiNote;
  m_message[2] = 0; // volume
  m_midiOut->sendMessage(&m_message);
  m_prevMidiNote = 0;
}



