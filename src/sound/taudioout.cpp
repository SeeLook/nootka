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

#include "taudioout.h"
#include "taudioparams.h"
#include "RtMidi.h"
#include <QTimer>
#include <QFile>
#include <QStringList>
#include <QAudio>
// #include <QMutex>
#include <QDebug>
// #include <QBuffer>

#define SAMPLE_RATE (44100)

//------------------ static methods ------------------------------------------------------

QAudioFormat TaudioOUT::templAudioFormat = QAudioFormat();

void TaudioOUT::prepTemplFormat() {
  templAudioFormat.setChannelCount(2);
// templAudioFormat.setChannelCount(1);
  templAudioFormat.setSampleSize(16);
  templAudioFormat.setSampleRate(44100);
//   templAudioFormat.setSampleRate(22050);
  templAudioFormat.setSampleType(QAudioFormat::SignedInt);
  templAudioFormat.setCodec("audio/pcm");
}


QStringList TaudioOUT::getAudioDevicesList() {
  QStringList devList;
  prepTemplFormat();
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


// QBuffer buff;

//---------------------------------------------------------------------------------------
//                CONSTRUCTOR
//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams* params, QString& path, QObject* parent) :
  QObject(parent),
  m_wavFile(path + "sounds/classical-guitar.wav"),
  m_playable(false),
  m_midiOut(0),
  m_audioOutput(0),
  m_IOaudioDevice(0),
  m_devName("anything"),
  m_audioArr(0),
  m_params(params),
  m_period(20)
{
  prepTemplFormat();
  m_timer = new QTimer(this);
  setAudioOutParams(params);
}

TaudioOUT::~TaudioOUT()
{
  deleteAudio();
  deleteMidi();
}

//---------------------------------------------------------------------------------------
//              METHODS
//---------------------------------------------------------------------------------------
void TaudioOUT::setAudioOutParams(TaudioParams* params) {
  m_timer->disconnect();
  if (params->midiEnabled) { // Midi output
    if (!m_midiOut) { // prepare midi and delete audio
//       deleteAudio();
      setMidiParams();
      if (m_playable)
          connect(m_timer, SIGNAL(timeout()), this, SLOT(midiNoteOff()));
    }    
  } else { // Audio output
      deleteMidi();
      if (m_devName != params->OUTdevName || !m_audioOutput) { //device doesn't exists or name changed
          if (setAudioDevice(params->OUTdevName))
            m_playable = loadAudioData();
          else
            m_playable = false;
      }
      if (m_playable) {
          connect(m_timer, SIGNAL(timeout()), this, SLOT(timeForAudio()));
          m_IOaudioDevice = m_audioOutput->start();
          m_buffer.resize(m_audioOutput->periodSize()*2);
          m_period = (SAMPLE_RATE*2) / m_audioOutput->periodSize();
          qDebug() << "period [ms]" << m_period;
          connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)),
                 this, SLOT(stateSlot(QAudio::State))); 
//           connect(m_IOaudioDevice, SIGNAL(), this, SLOT(bytesWritenSlot(qint64)));
//           connect(m_audioOutput, SIGNAL(notify()), this, SLOT(midiNoteOff()));
//           m_audioOutput->setNotifyInterval(1500);
          /*buff.open(QIODevice::ReadOnly);
          connect(m_timer, SIGNAL(timeout()), this, SLOT(midiNoteOff()));
          connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)),
                 this, SLOT(stateSlot(QAudio::State)));*/
      }
  }
}


bool TaudioOUT::setAudioDevice(QString& name) {
  bool fnd = false;
  QList<QAudioDeviceInfo> dL = QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
  if (dL.size()) {
    for(int i = 0; i < dL.size(); i++) { // Is there device on the list ??
        if (dL[i].deviceName() == name) { // There is !!
            m_deviceInfo = dL[i];
            fnd = true;
            break;
        }
    }
    if (!fnd) { // no device on the list - load default
        m_deviceInfo = QAudioDeviceInfo::defaultOutputDevice();
        fnd = true;
    }
  }
  if (m_audioOutput) {
      delete m_audioOutput;
      m_audioOutput = 0;
  }
  if (fnd) {
    if (m_deviceInfo.isFormatSupported(templAudioFormat)) {
        qDebug() << "out:" << m_deviceInfo.deviceName();
        m_devName = m_deviceInfo.deviceName(); //TODO: save device name to globals ?!
        m_audioOutput = new QAudioOutput(m_deviceInfo, templAudioFormat, this);
    } else {
        qDebug() << m_deviceInfo.deviceName() << "format unsupported !!";
        fnd = false;
    }    
  } else {
      qDebug() << "no output devices found";
  }
    return fnd;
}


// QMutex mutex;

void TaudioOUT::play(int noteNr) {
  if (!m_playable)
        return;
  
  if (m_params->midiEnabled) {
    if (m_prevMidiNote) {  // note is played and has to be turned off. Volume is pushed.
        m_doEmit = false;
        midiNoteOff();
    }
    m_doEmit = true;
    m_prevMidiNote = noteNr + 47;
    m_message[0] = 144; // note On
    m_message[1] = m_prevMidiNote;
    m_message[2] = 100; // volume
    m_midiOut->sendMessage(&m_message);
    m_timer->start(1500);
  
  } else { // play audio
    if (noteNr < -11 || noteNr > 41)
        return;
    
    /*buff.seek((noteNr + 11)*SAMPLE_RATE*2 - 1000);
    if (m_audioOutput->state() == QAudio::StoppedState)
        m_audioOutput->start(&buff);
    else
        m_audioOutput->resume();
    
    if (m_timer->isActive())
      m_timer->stop();
    m_timer->start(1500);*/    
      
    if (m_timer->isActive()) {
      m_timer->stop();
    }
    if (m_audioOutput && m_audioOutput->state() == QAudio::StoppedState) {
      qDebug("stoped");
      m_IOaudioDevice = m_audioOutput->start();
    } 
    m_doPlay = true;
//     mutex.lock();
    m_samplesCnt = 0;
    // note pos in array is shifted 1000 samples before to start from silence
    m_noteOffset = (noteNr + 11)*SAMPLE_RATE - 1000;
//     mutex.unlock();
    timeForAudio();
    m_timer->start(m_period);
  }
}

void TaudioOUT::deleteAudio() {
  if (m_audioOutput) {
    if (m_timer->isActive())
      m_timer->stop();
    m_audioOutput->stop();
    delete m_audioOutput;
    m_audioOutput = 0;
  }
  if (m_audioArr) {
    delete m_audioArr;
    m_audioArr = 0;
  }
}


void TaudioOUT::deleteMidi() {
  if (m_midiOut) {
    if (m_timer->isActive())
      m_timer->stop();
    m_midiOut->closePort();
    delete m_midiOut;
    m_midiOut = 0;
  }
}


void TaudioOUT::setMidiParams() {
  deleteMidi();
  deleteAudio();
  m_playable = true;
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
      qDebug() << "midi device:" << m_params->midiPortName << 
      "instr:" << (int)m_params->midiInstrNr /*<< "address:" << (int)m_midiOut*/;
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
  
  m_audioArr = new qint16[dataSizeFromChunk/2];
  wavStream.readRawData((char*)m_audioArr, dataSizeFromChunk);
  //TODO: make 1000 bytes space on the begining and fill it with 0
  
//   buff.setData((char*)m_audioArr, dataSizeFromChunk);

  wavFile.close();
  return true;
}

//---------------------------------------------------------------------------------------
//-------------------------------- slots ----------------------------------------------------
//---------------------------------------------------------------------------------------
void TaudioOUT::timeForAudio() {
  if (m_audioOutput && m_audioOutput->state() != QAudio::StoppedState) {
    int perSize = qMin(m_audioOutput->periodSize(), m_buffer.size());
    int chunks = m_audioOutput->bytesFree() / perSize;
//     qDebug() << "period:" << m_audioOutput->periodSize() << "free:" << m_audioOutput->bytesFree() << chunks;
    qint16 sample;
    if (chunks == 0) {
//       qDebug("empty chunk");
      return;
    }
      while (m_doPlay && chunks) {
//       mutex.lock();
        qint16 *out = (qint16*)m_buffer.data();
        for(int i=0; i < perSize/8; i++) {
            sample = m_audioArr[m_noteOffset + m_samplesCnt];
            *out++ = sample;
            *out++ = sample;
            *out++ = sample;
            *out++ = sample;
            m_samplesCnt++;
          if (m_samplesCnt == 40000) {
              qDebug("enought");
              m_doPlay = false;
//               mutex.unlock();
              break;
          }
        }
        m_IOaudioDevice->write(m_buffer.data(), (m_audioOutput->periodSize()/8)*8);
        chunks--;
//       mutex.unlock();
      }
      if (!m_doPlay) {
        m_timer->stop();
        emit noteFinished();
      }
  } else
      qDebug("QAudio::StoppedState");
}


void TaudioOUT::midiNoteOff() {
  /*if (!m_midiOut) {
    m_audioOutput->suspend();
    m_timer->stop();
    emit noteFinished();
    return;
  }*/
  m_timer->stop();
  m_message[0] = 128; // note Off
  m_message[1] = m_prevMidiNote;
  m_message[2] = 0; // volume
  m_midiOut->sendMessage(&m_message);
  m_prevMidiNote = 0;
  if (m_doEmit)
    emit noteFinished();
}

void TaudioOUT::stateSlot(QAudio::State st) {
//   if (st == QAudio::IdleState && !m_doPlay) {
     qDebug() << st;
//      m_timer->stop();
// //      m_audioOutput->reset();
//      emit noteFinished();
//   }
}

void TaudioOUT::bytesWritenSlot(qint64 len) {
  qDebug() << "bytesWritten:" << len;
}


// void TaudioOUT::playBuffer() {
//   qDebug() << m_audioOutput->elapsedUSecs();
//   m_audioOutput->stop();
//   emit noteFinished();
// }


