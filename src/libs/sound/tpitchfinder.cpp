/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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

#include "tpitchfinder.h"
#include "tartini/channel.h"
#include "tartini/filters/Filter.h"
#include "tartini/filters/IIR_Filter.h"
#include "tartini/analysisdata.h"
#include "tartini/mytransforms.h"
#include <QtCore/qthread.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qdir.h>


#if defined(Q_OS_WIN32)
  #include <windows.h>
  #define SLEEP(msecs) Sleep(msecs)
#else
  #include <unistd.h>
  #define SLEEP(msecs) usleep(msecs * 1000)
#endif


/** Common routine that waits 100ms until detecting thread will finish */
void goToSleep(QThread* t) {
  int cnt = 0;
  while (t->isRunning() && cnt < 100) {
    SLEEP(1);
    cnt++;
  }
  if (t->isRunning() && cnt >= 100)
    qDebug() << "[TpitchFinder] Can't stop detecting thread in 100ms";
}

#define BUFF_SIZE (16384)


TpitchFinder::TpitchFinder(QObject* parent) :
  QObject(parent),
  m_thread(new QThread),
  m_filteredChunk(nullptr),
  m_floatBuffer(nullptr),
  m_doReset(false), m_isOffline(false),
  m_channel(nullptr),
  m_chunkNum(0),
  m_prevNoteIndex(-1),
  m_minVolume(0.4),
  m_minDuration(0.15),
  m_rateRatio(1.0),
  m_volume(0),
  m_state(e_silence), m_prevState(e_silence),
  m_pcmVolume(0.0f), m_workVol(0.0f),
  m_splitByVol(true), m_minVolToSplit(0.1),
  m_skipStillerVal(0.0), m_averVolume(0.0)
{
  m_aGl = new TartiniParams();
  m_aGl->chanells = 1;
  m_aGl->rate = 44100;
  m_aGl->windowSize = 2048;
  m_aGl->framesPerChunk = 1; // A trick - setSampleRate() will set it soon
  m_aGl->dBFloor = -150.0; // it is unchanged but if it will in conversions.cpp it is hard coded
  m_aGl->equalLoudness = true;
  m_aGl->doingFreqAnalysis = true;
  m_aGl->doingAutoNoiseFloor = true;
  m_aGl->doingHarmonicAnalysis = false;
  m_aGl->firstTimeThrough = true;
  m_aGl->doingDetailedPitch = true;
  m_aGl->threshold = 93;
  m_aGl->analysisType = e_AUTOCORRELATION;
  m_aGl->topPitch = 140.0;
  m_aGl->loPitch = 15;

  m_aGl->ampThresholds[AMPLITUDE_RMS][0]           = -85.0; m_aGl->ampThresholds[AMPLITUDE_RMS][1]           = -0.0;
  m_aGl->ampThresholds[AMPLITUDE_MAX_INTENSITY][0] = -30.0; m_aGl->ampThresholds[AMPLITUDE_MAX_INTENSITY][1] = -20.0;
  m_aGl->ampThresholds[AMPLITUDE_CORRELATION][0]   =  0.40; m_aGl->ampThresholds[AMPLITUDE_CORRELATION][1]   =  1.00;
  m_aGl->ampThresholds[FREQ_CHANGENESS][0]         =  0.50; m_aGl->ampThresholds[FREQ_CHANGENESS][1]         =  0.02;
  m_aGl->ampThresholds[DELTA_FREQ_CENTROID][0]     =  0.00; m_aGl->ampThresholds[DELTA_FREQ_CENTROID][1]     =  0.10;
  m_aGl->ampThresholds[NOTE_SCORE][0]              =  0.03; m_aGl->ampThresholds[NOTE_SCORE][1]              =  0.20;
  m_aGl->ampThresholds[NOTE_CHANGE_SCORE][0]       =  0.12; m_aGl->ampThresholds[NOTE_CHANGE_SCORE][1]       =  0.30;

  m_framesReady = 0;
  m_averVolume = 0.0;
  m_currentNote.init(0, 0, 0);
  setSampleRate(m_aGl->rate);
  m_channel = new Channel(this, aGl()->windowSize);
  m_transforms = new MyTransforms();
  m_transforms->init(m_aGl, aGl()->windowSize, 0, aGl()->rate);
  moveToThread(m_thread);
  connect(m_thread, &QThread::started, this, &TpitchFinder::detectingThread);
//   connect(m_thread, &QThread::finished, this, &TpitchFinder::threadFinished);

  m_ringBuffer = new qint16[BUFF_SIZE]; // big enough for any circumstances
  m_writePos = 0;
  m_readPos = 0;
  m_doProcess = true;
  m_thread->start(QThread::HighestPriority);
  m_lastDetectedNote.init(0, 0, 0);
}


TpitchFinder::~TpitchFinder()
{
  m_doProcess = false;
  if (m_thread->isRunning())
    goToSleep(m_thread);

#if !defined (Q_OS_ANDROID)
  destroyDumpFile();
#endif
  if (m_filteredChunk)
      delete m_filteredChunk;
  delete m_floatBuffer;
  delete m_transforms;
  if (m_channel)
    delete m_channel;
  delete m_aGl;
  delete m_thread;
  delete m_ringBuffer;
}

//##########################################################################################################
//########################################## PUBLIC ########################################################
//##########################################################################################################
void TpitchFinder::setOffLine(bool off) {
  if (off != m_isOffline) {
    m_isOffline = off;
    if (m_isOffline) {
      m_doProcess = false; // terminate the thread
      if (m_thread->isRunning())
        goToSleep(m_thread);
    }
  }
}


void TpitchFinder::setSampleRate(unsigned int sRate, int range) {
  if (m_framesReady > 0) {
    qDebug() << "[TpitchFinder] Detection in progress. Don't call setSampleRate now!!!";
    return;
  }

  unsigned int oldRate = m_aGl->rate, oldFramesPerChunk = m_aGl->framesPerChunk;
  m_aGl->rate = sRate;
  m_rateRatio = range == 2 ? 2.0 : 1.0; // e_low for 2, for other cases e_middle
//   switch (range) {
//     case 0:
//       m_rateRatio = 0.5; break; // e_high - lowest pitch is F small
//     case 2:
//       m_rateRatio = 2.0; break; // e_low - lowest pitch is ... very low
//     default:
//       m_rateRatio = 1.0; break; // e_middle - lowest pitch is F contra
//   }
//     qDebug() << "m_rateRatio is " << m_rateRatio;
  if (sRate > 96000)
    m_aGl->framesPerChunk = static_cast<quint32>(4096 * m_rateRatio);
  else if (sRate > 48000)
    m_aGl->framesPerChunk = static_cast<quint32>(2048 * m_rateRatio);
  else
    m_aGl->framesPerChunk = static_cast<quint32>(1024 * m_rateRatio);
  bool doReset = false;
  if (oldRate != m_aGl->rate || oldFramesPerChunk != m_aGl->framesPerChunk) {
    m_aGl->windowSize = 2 * m_aGl->framesPerChunk;
    delete m_filteredChunk;
    m_filteredChunk = 0;
    delete m_floatBuffer;
    if (aGl()->equalLoudness)
      m_filteredChunk = new float[aGl()->framesPerChunk];
    m_floatBuffer = new float[aGl()->framesPerChunk];
    doReset = true;
    m_chunkTime = static_cast<qreal>(aGl()->framesPerChunk) / static_cast<qreal>(aGl()->rate);
    setMinimalDuration(m_minDuration); // recalculate minimum chunks number
//    qDebug() << "range" << m_rateRatio
//             << "framesPerChunk" << m_aGl->framesPerChunk << "windowSize" << m_aGl->windowSize
//             << "min chunks" << m_minChunks << "chunk time" << m_chunkTime << m_framesReady;
  }
  if (doReset)
    resetFinder();
}


void TpitchFinder::stop(bool resetAfter) {
//   qDebug() << "[TpitchFinder] stop";
  m_framesReady = 0;
  m_volume = 0.0;
  m_doReset = resetAfter;
  m_state = e_silence;
  m_prevState = e_silence;
  m_plaingWasDetected = false;
  m_restStarted = false;
  m_restNote.endChunk = 0;
#if !defined (Q_OS_ANDROID)
  destroyDumpFile();
#endif
}


/**
 * This method is invoked by audio callback thread, so it has to be as light as possible.
 * It only copies sent @p data to @p m_ringBuffer and sets appropriate @p m_readPos.
 * It increases @p m_framesReady size, so if there is enough data pitch detection will be performed.
 * In case when too much data incomes in short time (on by one callbacks)
 * and amount of gathered, unprocessed data is bigger than buffer size
 * it starts gathering process from buffer beginning - audio data consistency is lost,
 * but it happens only when audio stream starts (Pulse audio mostly),
 * so its doesn't harm further pitch detection
 */
void TpitchFinder::copyToBuffer(void* data, unsigned int nBufferFrames) {
  if (m_framesReady + nBufferFrames > BUFF_SIZE) {
    qDebug() << "[TpitchFinder] Fulfilled with data. Skipping!";
    m_framesReady = 0;
    m_readPos = 0;
    m_writePos = 0;
    return;
  }

  qint16* dataPtr = static_cast<qint16*>(data);
  unsigned int framesToCopy = nBufferFrames;
  if (m_writePos + nBufferFrames >= BUFF_SIZE)
    framesToCopy = BUFF_SIZE - m_writePos;
  if (framesToCopy) {
    std::copy(dataPtr, dataPtr + framesToCopy, m_ringBuffer + m_writePos);
    m_writePos += framesToCopy;
  }
  if (m_writePos >= BUFF_SIZE) {
    m_writePos = 0;
    if (framesToCopy < nBufferFrames) {
      framesToCopy = nBufferFrames - framesToCopy;
      std::copy(dataPtr, dataPtr + framesToCopy, m_ringBuffer + m_writePos);
      m_writePos += framesToCopy;
      qDebug() << "[TpitchFinder] Part of audio data was shifted:" << framesToCopy << "position is" << m_writePos;
    }
  }
  m_framesReady += nBufferFrames;
}


#if !defined (Q_OS_ANDROID)
/**
 * By settings valid (existing and writable) path dumping incoming audio data to file becomes enabled.
 * Data are dumped into default file 'nootka_dump' with numbered prefix.
 * File is closed whenever @p stop() is called and prefix increased.
 * If there is less data (below 2 seconds) - file is deleted.
 * Then new data is dumped into another file.
 * Default file name can be changed through @p setDumpFileName() method.
 *
 * IT SHOULD NEVER BE CALLED DURING PITCH DETECTION PROCESS (when @p m_framesReady is greater than 0)!
 */
void TpitchFinder::setDumpDirPath(const QString& dumpPath) {
  if (m_dumpPath.isEmpty() != dumpPath.isEmpty()) {
    if (dumpPath.isEmpty()) {
        destroyDumpFile();
    } else {
        m_dumpPath = dumpPath;
        if (m_dumpName.isEmpty())
          m_dumpName = QStringLiteral("nootka_dump");
    }
  }
  m_dumpPath = dumpPath;
}


void TpitchFinder::copyToBufferOffline(qint16* data) {
  std::copy(data, data + aGl()->framesPerChunk, m_ringBuffer); // 2 bytes are size of qint16
  m_framesReady = m_aGl->framesPerChunk;
  m_doProcess = true;
  detectingThread();
}
#endif

//##########################################################################################################
//##########################       PROTECTED SLOTS    ######################################################
//##########################################################################################################

void TpitchFinder::detectingThread() {
  while (m_doProcess) {
    int loops = 0;
    while (m_framesReady >= aGl()->framesPerChunk && loops < BUFF_SIZE / aGl()->framesPerChunk) {

      #if !defined (Q_OS_ANDROID)
        if (!m_dumpPath.isEmpty()) {
          if (!m_dumpFile)
            createDumpFile();
          if (m_dumpFile)
            m_dumpFile->write((char*)(m_ringBuffer + m_readPos), aGl()->framesPerChunk * 2);
        }
      #endif
      m_workVol = 0;
      for (unsigned int i = 0; i < aGl()->framesPerChunk; ++i) {
        qint16 value = *(m_ringBuffer + m_readPos + i);
        float sample = float(double(value) / 32760.0);
        *(m_floatBuffer + i) = sample;
        m_workVol = qMax<float>(m_workVol, sample);
      }
      m_pcmVolume = m_workVol;
      m_framesReady -= aGl()->framesPerChunk;
      if (!m_isOffline) {
        m_readPos += aGl()->framesPerChunk;
        if (m_readPos >= BUFF_SIZE)
          m_readPos = 0;
      }

      startPitchDetection();
      processed();
      loops++;
    }
    if (m_isOffline)
        m_doProcess = false;
    else {
        m_thread->usleep(500);
        if (m_doReset && m_framesReady == 0 && m_chunkNum > 0)
          resetFinder();
    }
  }

  if (!m_isOffline && m_thread->isRunning())
    m_thread->quit();
}


void TpitchFinder::startPitchDetection() {
  m_isBussy = true;
  if (m_doReset) { // copy last chunk to keep capturing data continuous
    resetFinder();
    if (aGl()->equalLoudness) // initialize channel with previous data
      std::copy(m_filteredChunk, m_filteredChunk + aGl()->framesPerChunk, m_channel->end() - aGl()->framesPerChunk);
    else
      std::copy(m_floatBuffer, m_floatBuffer + aGl()->framesPerChunk, m_channel->end() - aGl()->framesPerChunk);
  }
  m_channel->shift_left(aGl()->framesPerChunk); // make room in channel for new audio data
  if (aGl()->equalLoudness) { // filter it and copy to channel
      m_channel->highPassFilter->filter(m_floatBuffer, m_filteredChunk, aGl()->framesPerChunk);
      for(int i = 0; i < aGl()->framesPerChunk; i++)
          m_filteredChunk[i] = bound(m_filteredChunk[i], -1.0f, 1.0f);
      std::copy(m_filteredChunk, m_filteredChunk + aGl()->framesPerChunk, m_channel->end() - aGl()->framesPerChunk);
  } else // copy without filtering
      std::copy(m_floatBuffer, m_floatBuffer + aGl()->framesPerChunk, m_channel->end() - aGl()->framesPerChunk);

  detect();
}


void TpitchFinder::processed() {
  emit pitchInChunk(m_chunkPitch);
  if (m_state != m_prevState) {
    if (m_prevState == e_noticed) {
      if (m_state == e_playing) {
        if (m_lastDetectedNote.endChunk) {
          int restDur = m_currentNote.startChunk - m_lastDetectedNote.endChunk - 1;
          if (restDur > 0) {
            if (restDur < m_minChunks) {
                if (m_plaingWasDetected) {
                  m_lastDetectedNote.duration += restDur * m_chunkTime;
//                   QTextStream o(stdout);
//                   o << "\033[01;37m   [TpitchFinder] NOTE finished FIXED " << m_lastDetectedNote.pitchF << " "
//                     << m_lastDetectedNote.duration * 1000.0 << " \033[01;00m\n";
                  emit noteFinished(&m_lastDetectedNote);
                  // TODO; if it occurs, Tsound might split previously detected duration and it will improper
                }
            } else {
                if (m_plaingWasDetected && m_restStarted) {
                  m_restNote.duration = restDur * m_chunkTime;
//                   qDebug() << "=======[TpitchFinder] rest finished" << m_restNote.duration * 1000.0;
                  emit noteFinished(&m_restNote);
                  m_restStarted = false;
                }
            }
          }
        }
        m_plaingWasDetected = true;
//         qDebug() << "=======[TpitchFinder] NOTE started" << m_currentNote.pitchF << m_currentNote.duration * 1000.0;
        emit noteStarted(m_currentNote.getAverage(3, minChunksNumber()), m_currentNote.freq, m_currentNote.duration); // new note started
      }
    } else if (m_prevState == e_playing) {
        if (m_state == e_silence || m_state == e_noticed) {
//           qDebug() << "=======[TpitchFinder] NOTE finished" << m_currentNote.pitchF << m_currentNote.duration * 1000.0;
          emit noteFinished(&m_currentNote); // previous note was finished
          m_lastDetectedNote = m_currentNote;
          if (m_averVolume == 0.0)
            m_averVolume = m_currentNote.maxVol;
          else
            m_averVolume = (m_averVolume + m_currentNote.maxVol) / 2.0;
        }
    }
  }
  m_prevState = m_state;
  emit volume(m_volume);

  if (m_state != e_playing) {
    int restDur = (m_state == e_silence ? m_chunkNum : m_newNote.startChunk) - m_lastDetectedNote.endChunk - 1;
    if (!m_restStarted && m_plaingWasDetected && restDur >= m_minChunks) {
//       qDebug() << "=======[TpitchFinder] rest started" << restDur * m_chunkTime * 1000.0;
      emit noteStarted(-1.0, 0.0, restDur * m_chunkTime);
      m_restStarted = true;
    }
    if (restDur * m_chunkTime > 4000) {
      m_restStarted = false;
      m_lastDetectedNote.endChunk = m_chunkNum;
    }
  }
//   qDebug() << "[TpitchFinder] processed, volume" << m_volume << "state" << (m_state == e_silence ? "silence" : (m_state == e_noticed ? "noticed" : "playing"));
}


//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################
void TpitchFinder::detect() {
  FilterState filterState;
  m_channel->processNewChunk(&filterState);
  AnalysisData *data = m_channel->dataAtCurrentChunk();
  if (data == 0) { // In fact, it never happens
    qDebug() << "[TpitchFinder] Uh-uh! There is no Analysis data in processed chunk!";
    incrementChunk();
    m_isBussy = false;
    return;
  }
  data->pcmVolume = m_workVol;
  if (data->noteIndex == NO_NOTE) {
      m_chunkPitch = 0.0f;
      m_volume = 0.0f;
  } else {
      m_chunkPitch = data->pitch;
      m_volume = static_cast<float>(data->normalVolume());
  }

  bool noteChanged = false;
  if (data->noteIndex != m_prevNoteIndex) { // Tartini changed note
    noteChanged = true;
    if (data->noteIndex != NO_NOTE && m_prevNoteIndex != NO_NOTE) {
      float diff = qAbs(m_currentNote.pitches()->last() - data->pitch);
      if (diff < 1.0f) {
//         QTextStream o(stdout);
//         o << "\033[01;36m Tartini was mistaken - there is still the same note! "
//         << diff << " " << m_currentNote.pitches()->last() << " " << data->pitch << " " << m_currentNote.duration << " \033[01;00m\n";
        noteChanged = false;
      }
    }
  }

  if (noteChanged /*data->noteIndex != m_prevNoteIndex*/) { // note changed
    if (m_prevNoteIndex != NO_NOTE && m_newNote.numChunks() >= m_minChunks && m_newNote.maxVol >= m_averVolume * m_skipStillerVal) {
      m_currentNote = m_newNote; // summarize previous note if it was long enough
      m_currentNote.sumarize(m_chunkTime);
    }
    if (data->noteIndex != NO_NOTE) { // initialize a new note when it is valid
        m_newNote.init(data->noteIndex, m_chunkNum, data->pitch);
        m_state = e_noticed;
    } else
        m_state = e_silence;
  } else { // note is still playing
    if (data->noteIndex != NO_NOTE) {
      m_newNote.update(m_chunkNum, data->pitch, m_volume);
      m_newNote.maxPCMvol = qMax(m_newNote.maxPCMvol, m_pcmVolume);
      if (m_newNote.maxVol >= m_minVolume && m_newNote.maxVol >= m_averVolume * m_skipStillerVal) { // note was loud enough
        if (m_newNote.numChunks() == m_minChunks) { // note is accepted by Nootka
            m_currentNote = m_newNote;
            m_currentNote.sumarize(m_chunkTime);
            m_state = e_playing;
        } else if (m_splitByVol && m_newNote.numChunks() > m_minChunks) { // the same note for Tartini can be split by Nootka
            if (m_volume - m_newNote.minVol >= m_minVolToSplit && m_volume >= m_averVolume * m_skipStillerVal) {
              m_currentNote = m_newNote;
              m_currentNote.endChunk--;
              m_currentNote.sumarize(m_chunkTime);
              m_newNote.init(data->noteIndex, m_chunkNum, qreal(data->pitch));
              m_state = e_noticed;
            }
        }
      }
    }
  }
  m_prevNoteIndex = data->noteIndex;
  if (!m_isOffline && m_chunkNum > 1000 && data->noteIndex == NO_NOTE)
    m_doReset = true;

  incrementChunk();
  m_isBussy = false;
}


void TpitchFinder::resetFinder() {
  m_doReset = false;
  if (m_channel) {
      delete m_channel;
      m_lastDetectedNote.endChunk = m_lastDetectedNote.endChunk - m_chunkNum; // negative value to rich proper rest duration after reset
      m_chunkNum = 0;
      m_averVolume = 0.0;
      m_transforms->uninit();
      m_channel = new Channel(this, aGl()->windowSize);
      m_transforms->init(m_aGl, aGl()->windowSize, 0, aGl()->rate);
//       qDebug() << "[TpitchFinder] reset channel" << m_lastDetectedNote.endChunk;
  }
}


#if !defined (Q_OS_ANDROID)
void TpitchFinder::createDumpFile() {
  m_dumpFile = new QFile(m_dumpPath + QLatin1String("/") + QString("#%1-").arg(m_dumpSufixNr, 3, 'i', 0, '0') + m_dumpName + QLatin1String(".pcm"));
  if (m_dumpFile->open(QFile::WriteOnly)) {
      qDebug() << "Dumping into" << m_dumpFile->fileName();
      m_dumpSufixNr++;
      quint64 pcmHead = 0x97042300 + aGl()->rate / 1000; // 44100 : 44, 48000 : 48, 88200 : 88, 96000 : 96, etc...
      QDataStream dump(m_dumpFile); // save header with sample rate (normally pcm/raw format doesn't contain any info)
      dump << pcmHead;
  } else {
      qDebug() << "Filed to create dump file" << m_dumpFile->fileName();
      destroyDumpFile();
  }
}


void TpitchFinder::destroyDumpFile() {
  if (m_dumpFile) {
    if (m_dumpFile->isOpen()) {
      if (m_dumpFile->size() < 160000) { // delete file when below 2 seconds
          m_dumpFile->remove();
          m_dumpSufixNr--;
      } else
          m_dumpFile->close();
    }
    delete m_dumpFile;
    m_dumpFile = nullptr;
  }
}
#endif

void TpitchFinder::threadFinished() {
//  qDebug() << "[TpitchFinder] Thread finished";
}







