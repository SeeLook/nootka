/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
#include <QTimer>
#include <QFile>
#include <QAudio>
#include <QDebug>

#define SAMPLE_RATE (44100)

//------------------ static methods ------------------------------------------------------

QAudioFormat TaudioOUT::templAudioFormat = QAudioFormat();

void TaudioOUT::prepTemplFormat() {
  templAudioFormat.setChannelCount(2);
  templAudioFormat.setSampleSize(16);
  templAudioFormat.setSampleRate(44100);
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


//---------------------------------------------------------------------------------------
//                CONSTRUCTOR
//---------------------------------------------------------------------------------------
TaudioOUT::TaudioOUT(TaudioParams* params, QString& path, QObject* parent) :
  TabstractPlayer(parent),
  m_wavFile(path + "sounds/classical-guitar.wav"),
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
}


//---------------------------------------------------------------------------------------
//              METHODS
//---------------------------------------------------------------------------------------
void TaudioOUT::setAudioOutParams(TaudioParams* params) {
    m_timer->disconnect();
    if (m_devName != params->OUTdevName || !m_audioOutput) { //device doesn't exists or name changed
        if (setAudioDevice(params->OUTdevName))
          playable = loadAudioData();
        else
          playable = false;
    }
    if (playable) {
        connect(m_timer, SIGNAL(timeout()), this, SLOT(timeForAudio()));
        m_IOaudioDevice = m_audioOutput->start();
//          qDebug() << (m_IOaudioDevice); // device memory address
        m_buffer.resize(m_audioOutput->periodSize()*2);
        m_period = (SAMPLE_RATE*2) / m_audioOutput->periodSize() - 10;
        // m_period has to be smaller in case of QtMultimedia bug
        // to deliver data to m_IOaudioDevice in time
//           qDebug() << "period [ms]" << m_period;
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
//        connect(m_audioOutput, SIGNAL(stateChanged(QAudio::State)), this, SLOT(deviceStateSlot(QAudio::State)));
    } else {
        qDebug() << m_deviceInfo.deviceName() << "format unsupported !!";
        fnd = false;
    }    
  } else {
      qDebug() << "no output devices found";
  }
    return fnd;
}


bool TaudioOUT::play(int noteNr) {
  if (!playable)
        return false;
  
    noteNr = noteNr + qRound(m_params->a440diff);
    if (noteNr < -11 || noteNr > 41)
        return false;
    
    if (m_timer->isActive()) {
      m_timer->stop();
    }
    if (m_audioOutput && m_audioOutput->state() == QAudio::StoppedState) {
      qDebug("is stoped so let it reset");
      /** Mostly it occurs under MacOs (let's say only)
       * Maybe it is not so elegant to restore output in this way
       * but it works and quite fast. */
      delete m_audioOutput;
      m_audioOutput = new QAudioOutput(m_deviceInfo, templAudioFormat, this);
      m_IOaudioDevice = m_audioOutput->start();
    } 
    m_doPlay = true;
    m_samplesCnt = 0;
    int fasterOffset = 1000;
    if (noteNr + 11 == 0)
      fasterOffset = 0;
    // note pos in array is shifted 1000 samples before to start from silence
    // but not for first note in array (C in Contra)
    m_noteOffset = (noteNr + 11) * SAMPLE_RATE - fasterOffset;
    timeForAudio();
    m_timer->start(m_period);
//   }
  return true;
}

void TaudioOUT::stop() {
    if (m_timer->isActive()) {
      m_timer->stop();
//         m_audioOutput->stop();
    } //else
//       qDebug("stoped already");
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
    if (m_audioOutput->bytesFree() == -128) {
      perSize = m_audioOutput->periodSize();
      chunks = 1;
      // It seems to be QtMultimedia bug
      // On some platforms (64bit Ubuntu) bytesFree() returns -128
      // It means it has 128 bytes ocupated more than possible - a cancer ?!?
      // But it is ready to get data and plays
    }
//     qDebug() << "period:" << m_audioOutput->periodSize() << "free:" << m_audioOutput->bytesFree() << chunks;
    qint16 sample;
    if (chunks == 0) {
      return;
    }
      while (m_doPlay && chunks) {
        qint16 *out = (qint16*)m_buffer.data();
        for(int i=0; i < perSize/8; i++) {
            sample = m_audioArr[m_noteOffset + m_samplesCnt];
            *out++ = sample;
            *out++ = sample;
            *out++ = sample;
            *out++ = sample;
            m_samplesCnt++;
          if (m_samplesCnt == 40000) {
              m_doPlay = false;
              break;
          }
        }
        m_IOaudioDevice->write(m_buffer.data(), (m_audioOutput->periodSize()/8)*8);
        chunks--;
      }
      if (!m_doPlay) {
        m_timer->stop();
        emit noteFinished();
      }
  } // else
      //qDebug("QAudio::StoppedState"); // occurs under macOs
}



void TaudioOUT::deviceStateSlot(QAudio::State auStat) {
  QString statTxt = "output state: ";
   switch (auStat) {
    case QAudio::ActiveState : statTxt += "active"; break;
    case QAudio::IdleState : statTxt += "iddle"; break;
    case QAudio::SuspendedState : statTxt += "suspended"; break;
    case QAudio::StoppedState : statTxt += "stoped"; break;
   }
//   qDebug() << statTxt;
}

