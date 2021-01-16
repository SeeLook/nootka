/***************************************************************************
 *   Copyright (C) 2013-2021 by Tomasz Bojczuk                             *
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

#include "toggscale.h"
#include <music/tinstrument.h>
#include <tpath.h>
#include <QtCore/qfile.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qthread.h>
#include <QtCore/qfileinfo.h>
#include <QtCore/qmath.h>
#include <QtCore/qtimer.h>
#include <QtGui/qguiapplication.h>
#include <unistd.h>
#include <stdlib.h>

#include <QtCore/qdebug.h>


#if defined(Q_OS_WIN32)
  #include <windows.h>
  #define SLEEP(msecs) Sleep(msecs)
#else
  #define SLEEP(msecs) usleep(msecs * 1000)
#endif

/*Static*/
size_t ToggScale::readOggStatic(void* dst, size_t size1, size_t size2, void* fh) {
  SoggFile* of = reinterpret_cast<SoggFile*>(fh);
  size_t len = size1 * size2;
  if ( of->curPtr + len > of->filePtr + of->fileSize ) {
    len = of->filePtr + of->fileSize - of->curPtr;
  }
  memcpy(dst, of->curPtr, len);
  of->curPtr += len;
  return len;
}

int ToggScale::seekOggStatic(void* fh, ogg_int64_t offset, int type) {
  auto of = reinterpret_cast<SoggFile*>(fh);
  switch(type) {
    case SEEK_SET:
      of->curPtr = of->filePtr + offset; break;
    case SEEK_CUR:
      of->curPtr += offset; break;
    case SEEK_END:
      of->curPtr = of->filePtr + of->fileSize - offset; break;
    default:
      return -1;
  }
  if ( of->curPtr < of->filePtr ) {
    of->curPtr = of->filePtr;
    return -1;
  }
  if ( of->curPtr > of->filePtr + of->fileSize ) {
    of->curPtr = of->filePtr + of->fileSize;
    return -1;
  }
  return 0;
}


int ToggScale::closeOggStatic(void* fh) {
  Q_UNUSED(fh)
  return 0;
}


long int ToggScale::tellOggStatic(void* fh) {
  SoggFile* of = reinterpret_cast<SoggFile*>(fh);
  return (of->curPtr - of->filePtr);
}

//###########################################################################
//########################## PUBLIC #########################################
//###########################################################################

/**
 * Contains decoded audio data of a single note,
 * it is not initialized by default, @p reserve method creates buffer of declared @p size
 */
class TdecodedNote {
  public:
    ~TdecodedNote() {
      if (noteData)
        delete[] noteData;
    }

    void reserve(int size) {
      if (noteData)
        qDebug() << "[TdecodedNote] memory for data already reserved";
      else
        noteData = new qint16[size];
    }

    qint16 *noteData = nullptr;
    uint start = 0;
    uint stop = 0;
};


#define LOWEST_NOTE (-35)
#define HIGHEST_NOTE (63)


int minDataAmount = 10000;


ToggScale::ToggScale() :
  QObject(),
  m_oggInMemory(nullptr),
  m_thread(new QThread),
  m_sampleRate(44100),
  m_doDecode(true), m_isDecoding(false), m_isReady(true),
  m_oggConnected(false), m_touchConnected(false),
  m_instrument(-1),
  m_alreadyDecoded(0)
{
  m_touch = new soundtouch::SoundTouch();
  m_touch->setChannels(1);
#if defined (Q_OS_UNIX) // increase minimal audio data must to be processed when system works with PulseAudio
  QFileInfo pulseBin(QStringLiteral("/usr/bin/pulseaudio"));
  if (pulseBin.exists()) // it is necessary both for Nootka with native PA and PA in ALSA bridge mode
    minDataAmount = 15000;
#endif
#if defined (Q_OS_ANDROID)
  minDataAmount = 15000;
#endif
  moveToThread(m_thread);
  connect(m_thread, &QThread::started, this, &ToggScale::decodeOgg);
  connect(m_thread, &QThread::finished, this, &ToggScale::noteDecoded);
  m_oggConnected = true;
}


ToggScale::~ToggScale()
{
  deleteData();
}


void ToggScale::deleteData() {
  stopDecoding();
  if (m_thread->isRunning()) {
    m_thread->terminate();
    m_thread->quit();
  }
  if (m_oggInMemory) {
    delete m_oggInMemory;
    m_oggInMemory = nullptr;
  }
  if (m_pcmArray) {
    delete[] m_pcmArray;
    m_pcmArray = nullptr;
  }
  delete m_touch;
  m_thread->deleteLater();
}


qint16 ToggScale::getNoteSample(int noteNr, int offset) {
  if (noteNr >= LOWEST_NOTE && noteNr <= HIGHEST_NOTE) {
      if (m_pcmArray[noteNr - LOWEST_NOTE].noteData)
        return m_pcmArray[noteNr - LOWEST_NOTE].noteData[offset];
  } else
      return 0;

  // TODO: It should never happen, so delete condition after tests
  qDebug() << "[ToggScale] note" << noteNr << "has been not decoded yet !!!!!";
  return 0;
}


uint ToggScale::startLoopSample(int noteNr) {
  return m_pcmArray[noteNr - LOWEST_NOTE].start;
}


uint ToggScale::stopLoopSample(int noteNr) {
  return m_pcmArray[noteNr - LOWEST_NOTE].stop;
}


void ToggScale::decodeNote(int noteNr) {
  if (noteNr >= LOWEST_NOTE && noteNr <= HIGHEST_NOTE && m_pcmArray[noteNr - LOWEST_NOTE].noteData == nullptr) {
      m_pcmArray[noteNr - LOWEST_NOTE].reserve(m_sampleRate * 2);
      m_currentBuffer = m_pcmArray[noteNr - LOWEST_NOTE].noteData;
  } else {
      emit oggReady();
      emit noteDecoded();
      return;
  }

  m_pcmArrayfilled = true;
  int baseNote = noteNr;
  m_isReady = false;
  if (noteNr < m_firstNote || noteNr > m_lastNote) { // prepare SoundTouch
    if (noteNr < m_firstNote) {
        baseNote = m_firstNote;
        m_innerOffset = noteNr - m_firstNote;
    } else if (noteNr > m_lastNote) {
        baseNote = m_lastNote;
        m_innerOffset = noteNr - m_lastNote;
    }
  } else
      m_innerOffset = 0.0;
  adjustSoundTouch();
  int fasterOffset = m_instrument == 1 ? 880 : 0; // 20 ms later for classical guitar doe to recordings issues
  stopDecoding();
  m_noteInProgress = noteNr;
  /*int ret = */ov_pcm_seek(&m_ogg, (baseNote - m_firstNote) * 44100 * 2 + fasterOffset);
  if (QThread::currentThread() == m_thread) {
      if (m_touchConnected)
        decodeAndResample();
      else
        decodeOgg();
  } else
      m_thread->start();
}


void ToggScale::setSampleRate(unsigned int rate) {
  if (m_sampleRate != rate) {
    m_sampleRate = rate;
    resetPCMArray();
    adjustSoundTouch();
#if !defined (Q_OS_ANDROID)
    if (m_sampleRate != 44100) {
      QTextStream o(stdout);
      o << "\033[01;37m Audio data will be resampled to " << m_sampleRate << "\033[01;00m\n";
    }
#endif
  }
}


void ToggScale::setPitchOffset(qreal pitchOff) {
  if (pitchOff != m_pitchOffset) {
    m_pitchOffset = pitchOff;
    resetPCMArray();
    adjustSoundTouch();
  }
}


bool ToggScale::loadAudioData(int instrument) {
  if (instrument != m_instrument) {
    QString fileName;
    m_instrument = instrument;
    switch (static_cast<Tinstrument::Etype>(instrument)) {
      case Tinstrument::ClassicalGuitar:
        fileName = Tpath::sound("classical-guitar");
        m_firstNote = -11; m_lastNote = 41;
        m_soundContinuous = false;
        break;
      case Tinstrument::ElectricGuitar:
        fileName = Tpath::sound("electric-guitar");
        m_firstNote = -11; m_lastNote = 41;
        m_soundContinuous = false;
        break;
      case Tinstrument::BassGuitar:
        fileName = Tpath::sound("bass-guitar");
        m_firstNote = -24; m_lastNote = 21;
        m_soundContinuous = false;
        break;
#if !defined (Q_OS_ANDROID)
      case Tinstrument::Bandoneon:
        fileName = Tpath::sound("bandoneon");
        m_firstNote = -11; m_lastNote = 48;
        m_soundContinuous = true;
        break;
      case Tinstrument::AltSax:
        fileName = Tpath::sound("alto-sax");
        m_firstNote = 1; m_lastNote = 34;
        m_soundContinuous = true;
        break;
      case Tinstrument::TenorSax:
        fileName = Tpath::sound("tenor-sax");
        m_firstNote = -3; m_lastNote = 30;
        m_soundContinuous = true;
        break;
#endif
      default:
        fileName = Tpath::sound("piano");
        m_firstNote = -23; m_lastNote = 61;
        m_soundContinuous = false;
        break;
    }
    QFile oggFile(fileName);
    if (!oggFile.exists())
      return false;

    oggFile.open(QIODevice::ReadOnly);
    QDataStream oggStream(&oggFile);
    if (m_oggInMemory)
      delete m_oggInMemory;
    m_oggInMemory = new qint8[oggFile.size()];
    oggStream.readRawData(reinterpret_cast<char*>(m_oggInMemory), oggFile.size());

    ov_callbacks myCallBacks;
    m_oggWrap.curPtr = m_oggInMemory;
    m_oggWrap.filePtr = m_oggInMemory;
    m_oggWrap.fileSize = oggFile.size();

    oggFile.close();

    myCallBacks.read_func = readOggStatic;
    myCallBacks.seek_func = seekOggStatic;
    myCallBacks.close_func = closeOggStatic;
    myCallBacks.tell_func = tellOggStatic;

    resetPCMArray();

    int ret = ov_open_callbacks((void*)&m_oggWrap, &m_ogg, NULL, 0, myCallBacks);

    if (ret < 0) {
      qDebug() << "[ToggScale] Can't open ogg stream";
      delete m_oggInMemory;
      m_oggInMemory = nullptr;
      return false;
    }
    //   vorbis_info *oggInfo = ov_info(&m_ogg, -1);
    //   qDebug() << oggInfo->rate << oggInfo->channels << (bool)ov_seekable(&m_ogg);
    //   char **ptr=ov_comment(&m_ogg, -1)->user_comments;
    //     vorbis_info *vi=ov_info(&m_ogg, -1);
    //     while(*ptr){
    //       fprintf(stderr,"%s\n",*ptr);
    //       ++ptr;
    //     }
  }
  return m_oggInMemory != nullptr;
}


//###########################################################################
//########################## PROTECTED ######################################
//###########################################################################

void ToggScale::stopDecoding() {
  if (m_isDecoding) {
      qDebug("[ToggScale] decoding in progress");
      do {
        SLEEP(1);
      } while (m_isDecoding);
  }
}


void ToggScale::decodeOgg() {
  int bitStream;
  m_isDecoding = true;
  long int read = 0;
  int maxSize = 44100 * 4 - 4096; // 2 for two seconds of note * 2 for two bytes of sample - silence at the end of note
  int loops = 0;
  m_alreadyDecoded = 0;
  while (m_doDecode && loops < 500 && m_alreadyDecoded < maxSize) {
    read = ov_read(&m_ogg, reinterpret_cast<char*>(m_currentBuffer) + m_alreadyDecoded, maxSize - m_alreadyDecoded, 0, 2, 1, &bitStream);
    m_alreadyDecoded += read;
    if (m_alreadyDecoded > minDataAmount && !m_isReady) { // amount of data needed by single loop of audio outCallBack
      m_isReady = true;
      emit oggReady();
    }
    loops++;
  }
  if (soundContinuous()) {
    TdecodedNote& decNote = m_pcmArray[m_noteInProgress - LOWEST_NOTE];
    decNote.start = crossZeroBeforeMaxAmlp(8820, m_noteInProgress + 47); // 200ms of a sound
    decNote.stop = crossZeroBeforeMaxAmlp(52920, m_noteInProgress + 47); // 1200ms of a sound
  }
  m_isDecoding = false;
  if (m_thread->isRunning())
    m_thread->quit();
  else
    emit noteDecoded();
}


void ToggScale::decodeAndResample() {
  m_isDecoding = true;
  int maxSize = 44100 * 2 - 8192; // two sec. of audio minus some silence on the end
  long int tmpPos = 0, tmpRead = 0;
  uint read = 0;
  m_alreadyDecoded = 0;
  int samplesReady = 0;

  float **oggChannels;
  float *left ;
  float *tmpTouch = new float[8192];

  while (m_doDecode && m_alreadyDecoded < maxSize) {
    /// 1. Grab audio data from ogg
    if (tmpPos < 172000) { // almost 2 sec. of a note
        tmpRead = ov_read_float(&m_ogg, &oggChannels, 2048, 0);
        tmpPos += tmpRead;
        left = oggChannels[0]; // pointer to left channel chunk
    /// 2. push data to SoundTouch
        if (tmpRead > 0)
            m_touch->putSamples(static_cast<SAMPLETYPE*>(left), tmpRead);
    }
    samplesReady = m_touch->numSamples();
    /// 3. Get re-sampled/offset data from SoundTouch
    if (samplesReady > 0) {
      read = m_touch->receiveSamples(static_cast<SAMPLETYPE*>(tmpTouch), samplesReady);
    /// 4. Convert samples to 16bit integer
      for (int i = 0; i < read; i++)
          *(m_currentBuffer + m_alreadyDecoded + i) = static_cast<qint16>(*(tmpTouch + i) * 32768);
      m_alreadyDecoded += read;
    }
    if (m_alreadyDecoded > minDataAmount && !m_isReady) { // below this value SoundTouch is not able to prepare data
      m_isReady = true;
      emit oggReady();
    }
  }
  if (soundContinuous()) {
    TdecodedNote& decNote = m_pcmArray[m_noteInProgress - LOWEST_NOTE];
    decNote.start = crossZeroBeforeMaxAmlp(8820, m_noteInProgress + 47); // 200ms of a sound
    decNote.stop = crossZeroBeforeMaxAmlp(52920, m_noteInProgress + 47); // 1200ms of a sound
  }
  m_isDecoding = false;
  m_touch->clear();
  if (m_thread->isRunning())
    m_thread->quit();
  else
    emit noteDecoded();
  delete[] tmpTouch;
}

//###########################################################################
//########################## PRIVATE ########################################
//###########################################################################

void ToggScale::adjustSoundTouch() {
  if (m_innerOffset != 0.0 || m_pitchOffset != 0.0 || m_sampleRate != 44100) { // SoundTouch has got a job
      m_touch->setSampleRate(44100);
      m_touch->setPitchSemiTones(m_innerOffset + m_pitchOffset);
      if (m_sampleRate != 44100) {
        qreal newRate =  44100.0 / static_cast<qreal>(m_sampleRate);
        m_touch->setRate(newRate);
      }
//       qDebug() << "SoundTouch sampleRate" << m_sampleRate << "pitch offset" << m_innerOffset + m_pitchOffset;
      if (!m_touchConnected)
        connect(m_thread, &QThread::started, this, &ToggScale::decodeAndResample);
      m_touchConnected = true;
      if (m_oggConnected)
        disconnect(m_thread, &QThread::started, this, &ToggScale::decodeOgg);
      m_oggConnected = false;
  } else {
      if (!m_oggConnected)
        connect(m_thread, &QThread::started, this, &ToggScale::decodeOgg);
      m_oggConnected = true;
      if (m_touchConnected)
        disconnect(m_thread, &QThread::started, this, &ToggScale::decodeAndResample);
      m_touchConnected = false;
  }
}


void ToggScale::resetPCMArray() {
  if (m_pcmArrayfilled || m_pcmArray == nullptr) {
    if (m_pcmArray)
      delete[] m_pcmArray;
    m_pcmArray = new TdecodedNote[99];
    m_pcmArrayfilled = false;
  }
}


uint ToggScale::crossZeroBeforeMaxAmlp(uint lookFrom, int midiNoteNr) {
  int firstZeroCross = lookFrom;
  int periodSamples = qCeil(44100.0 / qPow(10.0, (static_cast<qreal>(midiNoteNr) + 36.3763165622959152488) / 39.8631371386483481));
  for (int s = lookFrom; s < lookFrom + periodSamples; ++s) {
    if (m_currentBuffer[s] > 0 && m_currentBuffer[s - 1] <= 0) {
      firstZeroCross = s;
      break;
    }
  }
  qint16 maxAmplitude = 0;
  uint maxAmplitudeSampleNr = lookFrom;
  for (uint s = firstZeroCross; s < firstZeroCross + periodSamples; ++s) {
    if (m_currentBuffer[s] > maxAmplitude) {
      maxAmplitude = qAbs(m_currentBuffer[s]);
      maxAmplitudeSampleNr = s;
    }
  }
  do {
    maxAmplitudeSampleNr--;
    if (m_currentBuffer[maxAmplitudeSampleNr] > 0 && m_currentBuffer[maxAmplitudeSampleNr - 1] <= 0)
      break;
  } while (maxAmplitudeSampleNr > firstZeroCross);

  return maxAmplitudeSampleNr;
}
