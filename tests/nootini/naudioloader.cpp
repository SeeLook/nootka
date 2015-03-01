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

#include "naudioloader.h"
#include <tpitchfinder.h>
#include <QtCore/QtEndian>
#include <QtCore/QDebug>
#include <QtWidgets/QMessageBox>




NaudioLoader::NaudioLoader(QObject* parent) :
  QObject(parent),
  m_pf(0)
{
  moveToThread(&m_thread);
  connect(&m_thread, &QThread::started, this, &NaudioLoader::performThread);
}


NaudioLoader::~NaudioLoader() {
  m_thread.quit();
  m_thread.wait();
  delete m_pf;
}


bool NaudioLoader::setAudioFile(const QString& fileName) {
  if (m_pf) {
    delete m_pf;
    m_pf = 0;
  }
  m_pf = new TpitchFinder;
  m_pf->setOffLine(true);
  bool ok = true;
  m_audioFile.setFileName(fileName);
  if (m_audioFile.open(QIODevice::ReadOnly)) {
    m_in.setDevice(&m_audioFile);
    qint32 headChunk;
    m_in >> headChunk;
    headChunk = qFromBigEndian<qint32>(headChunk);
    QString headStr((char*)&headChunk);

    quint32 chunkSize;
    m_in >> chunkSize;
    chunkSize = qFromBigEndian<quint32>(chunkSize);
    m_in >> headChunk;
    headChunk = qFromBigEndian<qint32>(headChunk);
    headStr = QString((char*)&headChunk);

    if (headChunk == 1163280727) { // 1163280727 is 'value' of 'WAVE' text in valid WAV file
      m_in >> headChunk;
      headChunk = qFromBigEndian<qint32>(headChunk);

      m_in >> chunkSize;
      quint16 audioFormat;
      m_in >> audioFormat;
      audioFormat = qFromBigEndian<quint16>(audioFormat);
      if (headChunk == 544501094 && audioFormat == 1) { // 544501094 is 'value' of 'fmt ' text in valid WAV file
        m_in >> m_channelsNr;
        m_channelsNr = qFromBigEndian<quint16>(m_channelsNr);
        qDebug() << "channels:" << m_channelsNr;
        quint32 sampleRate;
        m_in >> sampleRate;
        sampleRate = qFromBigEndian<quint32>(sampleRate);
        qDebug() << "sample Rate:" << sampleRate;
        quint32 byteRate;
        m_in >> byteRate;
        byteRate = qFromBigEndian<quint32>(byteRate);
        qDebug() << "byte Rate:" << byteRate;
        quint16 blockAlign;
        m_in >> blockAlign;
        blockAlign = qFromBigEndian<quint16>(blockAlign);
        qDebug() << "block align:" << blockAlign;
        quint16 bitsPerSample;
        m_in >> bitsPerSample;
        bitsPerSample = qFromBigEndian<quint16>(bitsPerSample);
        qDebug() << "bits per sample:" << bitsPerSample;
        if (bitsPerSample != 16) {
          QMessageBox::warning(0, "Nootini", tr("Only WAV with 16 bit per sample are supported."));
          ok = false;
        }
        m_in >> headChunk;
        headChunk = qFromBigEndian<qint32>(headChunk);
        headStr = QString((char*)&headChunk);

        quint32 audioDataSize;
        m_in >> audioDataSize;
        audioDataSize = qFromBigEndian<quint32>(audioDataSize);

        m_samplesCount = (audioDataSize / (m_channelsNr * 2));
        m_totalChunks = m_samplesCount / m_pf->aGl()->framesPerChunk + 1;
        qDebug() << "chunks to go:" << m_totalChunks;

        m_pf->setSampleRate(sampleRate);
      } else {
        QMessageBox::warning(0, "Nootini", tr("Unsupported audio format in file:") + "<br>" + fileName);
        ok = false;
      }
    } else {
        QMessageBox::warning(0, "Nootini", fileName + "<br>" + tr("is not valid WAV file"));
        ok = false;
    }
  } else {
    QMessageBox::warning(0, "Nootini", tr("Cannot open file:") + "<br>" + fileName);
  }

  if (!ok && m_audioFile.isOpen())
      m_audioFile.close();
  return ok;
}


void NaudioLoader::startLoading() {
  m_thread.start();
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void NaudioLoader::performThread() {
  if (m_audioFile.isOpen()) {
    qint16 chL, chR;
    for (int i = 0; i < m_samplesCount; ++i) {
      m_in >> chL;
      chL = qFromBigEndian<qint16>(chL);
      if (m_channelsNr == 2) {
        m_in >> chR;
        chR = qFromBigEndian<qint16>(chR);
        chL = ((qint32)chL + (qint32)chR) / 2; // mix channels to mono
      }
      m_pf->fillBuffer(float(chL) / 32760.0f);
    }
    emit processingFinished();
    m_audioFile.close();
  } else
      qDebug() << "Wrong file" << m_audioFile.fileName();
  m_thread.quit();
}







