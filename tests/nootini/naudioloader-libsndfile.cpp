/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <QtCore/qendian.h>
#include <QtCore/qdebug.h>
#include <QtWidgets/qmessagebox.h>


/*static*/
int NaudioLoader::m_range = 1;

NaudioLoader::NaudioLoader(QObject* parent) :
  QObject(parent),
  m_pf(0),
  m_sndFile(0)
{
}


NaudioLoader::~NaudioLoader() {
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
  if (!fileName.isEmpty()) {
    m_fileName = fileName;
    SF_INFO sndInfo;
    m_sndFile = sf_open(fileName.toStdString().data(), SFM_READ, &sndInfo);
    if (sndInfo.format & SF_FORMAT_WAV && sndInfo.format & SF_FORMAT_PCM_16) {
        qDebug() << "supported WAV format detected" << "frames" << sndInfo.frames  << "channels" << sndInfo.channels
                 << "sample rate" << sndInfo.samplerate << "sections" << sndInfo.sections << "seekable" << sndInfo.seekable;
        ok = true;
        m_channelsNr = sndInfo.channels;
        m_samplesCount = sndInfo.frames;
        m_pf->setSampleRate(sndInfo.samplerate, m_range);
        m_totalChunks = m_samplesCount / m_pf->aGl()->framesPerChunk + 1;
        qDebug() << "chunks to go:" << m_totalChunks;
    } else
        QMessageBox::warning(0, QLatin1String("Nootini"), tr("Only WAV with 16 bit per sample are supported."));
  }
  return ok;
}


void NaudioLoader::startLoading() {
  connect(m_pf, SIGNAL(noteFinished(TnoteStruct*)), this, SLOT(forwardNoteFinished(TnoteStruct*)), Qt::UniqueConnection);
  connect(m_pf, SIGNAL(volume(float)), this, SLOT(chunkProcessed()), Qt::UniqueConnection);
  sf_seek(m_sndFile, 0, SEEK_SET);
  performThread();
}


void NaudioLoader::fillTartiniParams(TartiniParams* tp) {
  if (m_pf) {
    m_pf->aGl()->threshold = tp->threshold;
    m_pf->aGl()->doingHarmonicAnalysis = tp->doingHarmonicAnalysis;
    m_pf->aGl()->equalLoudness = Tcore::gl()->A->equalLoudness;
    m_pf->aGl()->dBFloor = tp->dBFloor;
    m_pf->aGl()->doingAutoNoiseFloor = tp->doingAutoNoiseFloor;
    m_pf->setSkipStillerVal(Tcore::gl()->A->skipStillerVal / 100.0);
    m_pf->setMinimalDuration(Tcore::gl()->A->minDuration);
    m_pf->setSplitByVolChange(Tcore::gl()->A->minSplitVol > 0.0);
    m_pf->setSplitVolume(Tcore::gl()->A->minSplitVol / 100.0);
    m_pf->stop(true);
    m_totalChunks = m_samplesCount / m_pf->aGl()->framesPerChunk + 1;
  }
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void NaudioLoader::chunkProcessed() {
  if (m_pf->currentChunk() > m_totalChunks - 1) {
    emit processingFinished();
    sf_close(m_sndFile);
  } else {
    emit chunkReady();
    performThread();
  }
}


void NaudioLoader::threadFinished() {

}


void NaudioLoader::performThread() {
  qint16 chL;
  qint16 frame[2];
  qint16* outBuffer = new qint16[m_pf->aGl()->framesPerChunk];
  for (int i = 0; i < m_pf->aGl()->framesPerChunk; ++i) {
    if (m_samplesCount > m_pf->currentChunk() * m_pf->aGl()->framesPerChunk + i) {
      if (m_channelsNr == 2) {
        sf_readf_short(m_sndFile, frame, 1);
        chL = frame[1];
//         chR = frame[0];
//         chL = ((qint32)chL + (qint32)chR) / 2; // mix channels to mono, FIXME: but it doesn't work always
      } else {
        sf_read_short(m_sndFile, &chL, 1);
      }
        outBuffer[i] = chL;
    } else
        outBuffer[i] = 0;
  }
  m_pf->copyToBufferOffline(outBuffer);
  delete[] outBuffer;
}


void NaudioLoader::forwardNoteFinished(TnoteStruct* note) {
  emit noteFinished(note);
}






