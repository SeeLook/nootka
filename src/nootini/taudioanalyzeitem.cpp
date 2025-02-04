/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
 *   seelook@gmail.com                                                     *
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

#include "taudioanalyzeitem.h"
#include <trtaudioin.h>
#include <tsound.h>

#include <QtCore/qdatastream.h>
#include <QtCore/qdebug.h>
#include <QtCore/qendian.h>

TaudioAnalyzeItem *TaudioAnalyzeItem::m_instance = nullptr;

TaudioAnalyzeItem::TaudioAnalyzeItem(QQuickItem *parent)
    : QQuickItem(parent)
{
    m_instance = this;
    connect(SOUND, &Tsound::initialized, this, &TaudioAnalyzeItem::soundInitSlot);
}

TaudioAnalyzeItem::~TaudioAnalyzeItem()
{
}

// #################################################################################################
// ###################                PROTECTED         ############################################
// #################################################################################################

void TaudioAnalyzeItem::soundInitSlot()
{
    if (SOUND->sniffer) {
        m_sniffer = SOUND->sniffer;
        connect(m_sniffer, &TcommonListener::volumeChanged, this, &TaudioAnalyzeItem::volumeSlot);
        connect(m_sniffer, &TcommonListener::noteFinished, this, &TaudioAnalyzeItem::noteFinishedSlot);
    }
}

void TaudioAnalyzeItem::volumeSlot()
{
    Tnote n;
    if (m_sniffer->finder()->chunkPitch() > 0.0)
        n.fromMidi(qRound(m_sniffer->finder()->chunkPitch()));
    emit dataReady(m_sniffer->pcmVolume(),
                   m_sniffer->finder()->energy(),
                   m_sniffer->finder()->isOnSet(),
                   (n.isValid() ? QString("%1 <b>[%2]</b>").arg(m_sniffer->finder()->chunkPitch()).arg(n.toText()) : QString())
                       + QString(" ID: %1").arg(m_sniffer->finder()->currentNoteId()));
}

void TaudioAnalyzeItem::noteFinishedSlot(const TnoteStruct &n)
{
    emit noteData(n.startChunk, n.endChunk, n.pitch.isValid() ? n.pitch.styledName() : "\ue4e5");
}

// #################################################################################################
// ###################                STATIC            ############################################
// #################################################################################################

void TaudioAnalyzeItem::processAudioFile(const QString &fileName)
{
    ANALYZER->m_audioFile.setFileName(fileName);
    if (ANALYZER->m_audioFile.exists())
        connect(SOUND, &Tsound::initialized, ANALYZER, &TaudioAnalyzeItem::doProcess);
}

// #################################################################################################
// ###################                PROTECTED         ############################################
// #################################################################################################

void TaudioAnalyzeItem::doProcess()
{
    quint16 channelsNr;
    quint32 sampleRate;
    int samplesCount;
    QDataStream in;
    bool ok = true;
    SOUND->sniffer->finder()->setOffLine(true);
    if (m_audioFile.open(QIODevice::ReadOnly)) {
        in.setDevice(&m_audioFile);
        QString audioExt = m_audioFile.fileName().right(3);
        if (audioExt == QLatin1String("pcm") || audioExt == QLatin1String("raw")) {
            quint64 headInfo;
            in >> headInfo;
            if (headInfo & 0x97042300) {
                channelsNr = 1;
                int sr = headInfo % 0x97042300;
                if (sr == 44)
                    sampleRate = 44100;
                else if (sr == 88)
                    sampleRate = 88200;
                else
                    sampleRate = sr * 1000;
                samplesCount = (m_audioFile.size() - 8) / 2;
            } else {
                qDebug() << "[Nootini] Raw files created out of Nootka are not supported yet";
                channelsNr = 1;
                sampleRate = 48000;
                ok = false;
            }
        } else { // WAV file
            qint32 headChunk;
            in >> headChunk;
            headChunk = qFromBigEndian<qint32>(headChunk);
            QString headStr((char *)&headChunk);

            quint32 chunkSize;
            in >> chunkSize;
            chunkSize = qFromBigEndian<quint32>(chunkSize);
            in >> headChunk;
            headChunk = qFromBigEndian<qint32>(headChunk);
            headStr = QString((char *)&headChunk);

            if (headChunk == 1163280727) { // 1163280727 is 'value' of 'WAVE' text in valid WAV file
                in >> headChunk;
                headChunk = qFromBigEndian<qint32>(headChunk);

                in >> chunkSize;
                quint16 audioFormat;
                in >> audioFormat;
                audioFormat = qFromBigEndian<quint16>(audioFormat);
                if (headChunk == 544501094 && audioFormat == 1) { // 544501094 is 'value' of 'fmt ' text in valid WAV file
                    in >> channelsNr;
                    channelsNr = qFromBigEndian<quint16>(channelsNr);
                    in >> sampleRate;
                    sampleRate = qFromBigEndian<quint32>(sampleRate);
                    quint32 byteRate;
                    in >> byteRate;
                    byteRate = qFromBigEndian<quint32>(byteRate);
                    quint16 blockAlign;
                    in >> blockAlign;
                    blockAlign = qFromBigEndian<quint16>(blockAlign);
                    quint16 bitsPerSample;
                    in >> bitsPerSample;
                    bitsPerSample = qFromBigEndian<quint16>(bitsPerSample);
                    if (bitsPerSample != 16) {
                        qDebug() << "[Nootini] Only *.wav with 16 bit per sample are supported.";
                        ok = false;
                    }
                    in >> headChunk;
                    headChunk = qFromBigEndian<qint32>(headChunk);
                    headStr = QString((char *)&headChunk);

                    quint32 audioDataSize;
                    in >> audioDataSize;
                    audioDataSize = qFromBigEndian<quint32>(audioDataSize);

                    samplesCount = (audioDataSize / (channelsNr * 2));
                } else {
                    qDebug() << "[Nootini] Unsupported audio format in file:" << m_audioFile.fileName();
                    ok = false;
                }
            } else {
                qDebug() << "[Nootini] " << m_audioFile.fileName() << "is not valid *,wav file";
                ok = false;
            }
        }
    }

    if (ok) {
        qDebug() << "[Nootini]" << m_audioFile.fileName() << sampleRate << channelsNr;
        auto pf = SOUND->sniffer->finder();
        qint16 chL, chR;
        qint16 *buffer = new qint16[pf->aGl()->framesPerChunk];
        for (int j = 0; j < samplesCount / pf->aGl()->framesPerChunk; ++j) {
            for (int i = 0; i < pf->aGl()->framesPerChunk; ++i) {
                if (samplesCount > pf->currentChunk() * pf->aGl()->framesPerChunk + i) {
                    in >> chL;
                    chL = qFromBigEndian<qint16>(chL);
                    if (channelsNr == 2) {
                        in >> chR;
                        //           chR = qFromBigEndian<qint16>(chR);
                        //           chL = ((qint32)chL + (qint32)chR) / 2; // mix channels to mono FIXME: mixing doesn't work in all cases
                    }
                    buffer[i] = chL;
                } else
                    buffer[i] = 0;
            }
            pf->copyToBufferOffline(buffer);
            qApp->processEvents();
        }
        delete[] buffer;
    }
    if (m_audioFile.isOpen())
        m_audioFile.close();
}
