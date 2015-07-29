/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "tqtaudioin.h"
#include "tpitchfinder.h"
#include <QAudioInput>
#include <QIODevice>
#include <QThread>
#include <QDebug>


TaudioIN::TaudioIN(TaudioParams* params, QObject *parent) :
  TcommonListener(params, parent),
  m_inDevice(0),
  m_buffer(0),
  m_thread(new QThread)
{
  QAudioDeviceInfo defaultIn = QAudioDeviceInfo::defaultInputDevice();
  QAudioFormat format;
    format.setChannelCount(1);
    format.setSampleRate(48000);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
  if (!defaultIn.isFormatSupported(format)) {
    qDebug() << "Format 48000/16 mono is not suported";
    format = defaultIn.nearestFormat(format);
    qDebug() << "Format is" << format.sampleRate() << format.channelCount() << format.sampleSize();
  }

  m_audioIN = new QAudioInput(defaultIn, format, this);
  m_audioIN->setBufferSize(2048);
  finder()->setSampleRate(m_audioIN->format().sampleRate()); // framesPerChunk is determined here

//  moveToThread(m_thread);
//  connect(m_thread, &QThread::started, this, &TaudioIN::dataReady);
}


TaudioIN::~TaudioIN() {
  stopListening();
  m_thread->deleteLater();
}


void TaudioIN::startListening() {
  if (detectingState() != e_detecting) {
    m_inDevice = m_audioIN->start();
    if (m_inDevice) {
      m_buffer = new qint16[m_audioIN->bufferSize()];
      connect(m_inDevice, &QIODevice::readyRead, this, &TaudioIN::dataReady);
//      connect(m_inDevice, &QIODevice::readyRead, m_thread, &QThread::start);
      setState(e_detecting);
      qDebug() << "started with buffer" << m_audioIN->bufferSize();
    }
  }
}


void TaudioIN::stopListening() {
  if (detectingState() != e_stopped) {
    m_audioIN->stop();
    delete m_buffer;
    m_buffer = 0;
    resetVolume();
    resetChunkPitch();
    setState(e_stopped);
    finder()->resetFinder();
  }
}


void TaudioIN::dataReady() {
  int bytesReady = m_audioIN->bytesReady();
//  qDebug() << bytesReady << "ready to read";
  int toRead = qMin(bytesReady, m_audioIN->bufferSize());
  int dataRead = m_inDevice->read((char*)m_buffer, toRead) / 2;
//  if (dataRead > 1024) {
//    dataRead = 1024;
//    qDebug() << dataRead << "Audio data was cut off. Buffer is too small !!!!";
//  }
  for (int i = 0; i < dataRead; i++) {
    finder()->fillBuffer(*(m_buffer + i) / 32760.0f);
  }
}





