/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "toggscale.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QThread>
// #include <stdlib.h>
#include <unistd.h>


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
    memcpy( dst, of->curPtr, len );
    of->curPtr += len;
    return len;
}

int ToggScale::seekOggStatic(void* fh, ogg_int64_t offset, int type) {
    SoggFile* of = reinterpret_cast<SoggFile*>(fh);
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
        of->curPtr = of->filePtr; return -1;
    }
    if ( of->curPtr > of->filePtr + of->fileSize ) {
        of->curPtr = of->filePtr + of->fileSize; return -1;
    }
    return 0;
}


int ToggScale::closeOggStatic(void* fh) {
    return 0;
}


long int ToggScale::tellOggStatic(void* fh) {
    SoggFile* of = reinterpret_cast<SoggFile*>(fh);
    return (of->curPtr - of->filePtr);
}

/*************************************************************************/


ToggScale::ToggScale(QString& path) :
  QObject(),
  m_oggFileName(path + "sounds/classical-guitar.ogg"),
  m_oggInMemory(0),
  m_pcmBuffer(0),
  m_thread(new QThread),
  m_sampleRate(44100),
  m_prevNote(-1),
  m_doDecode(true), m_isDecoding(false), m_isReady(true),
  m_pitchOffset(0.04f)
{
  m_touch = new soundtouch::SoundTouch();
  initTouch();
  moveToThread(m_thread);
  connect(m_thread, SIGNAL(started()), this, SLOT(decodeOgg()));
//   connect(m_thread, SIGNAL(started()), this, SLOT(decodeAndResample()));
}


ToggScale::~ToggScale()
{
  deleteData();
}


void ToggScale::deleteData() {
  if (m_thread->isRunning())
    m_thread->quit();
  if (m_oggInMemory) {
    delete m_oggInMemory;
    m_oggInMemory = 0;
  }
  delete m_pcmBuffer;
  m_pcmBuffer = 0;
  delete m_thread;
}


qint16 ToggScale::getSample(int offset) {
    return m_pcmBuffer[offset];
}

void ToggScale::setPos(ogg_int64_t offset) {
  int ret = ov_pcm_seek(&m_ogg, offset);
  if (!ret)
    decodeOgg();
}


void ToggScale::setNote(int noteNr) {
  if (noteNr == m_prevNote)
    return;
  m_isReady = false;
  int fasterOffset = 1000;
  if (noteNr + 11 == 0)
    fasterOffset = 0;
  if (m_isDecoding) {
      qDebug("decoding in progress");
      m_doDecode = false;
      do {
        SLEEP(1);
      } while (m_isDecoding);
      m_doDecode = true;
  }
  m_prevNote = noteNr;
  int ret = ov_pcm_seek(&m_ogg, (noteNr + 11) * 44100 * 2 - fasterOffset);
  m_thread->start();
}



void ToggScale::decodeOgg() {
  int bitStream;
  m_isDecoding = true;
  long int read = 0;
  int maxSize = 44100 * 4 - 4096; // 2 for two seconds of note * 2 for two bytes of sample - silence at the end of note
  int loops = 0;
  long int pos = 0;
  while (m_doDecode && loops < 500 && pos < maxSize) {
    read = ov_read(&m_ogg, (char*)m_pcmBuffer + pos, maxSize - pos, 0, 2, 1, &bitStream);
    pos += read;
    if (pos > 2048) // amount of data needed by single loop of rtAudio outCallBack
      m_isReady = true;
    loops++;
//     SLEEP(1);
  }
  m_isDecoding = false;
  qDebug() << "readFromOgg" << pos << "loops" << loops;
  m_thread->quit();
}


void ToggScale::decodeAndResample() {
  int bitStream;
  m_isDecoding = true;
  int maxSize = m_sampleRate * 2 - 4096;
  long int tmpPos = 0, tmpRead = 0;
  uint pos = 0, read = 0;
  int samplesReady = 0;
  char* tmpBuff = new char[2048];
  while (pos < maxSize) {
    if (tmpPos < 172000) {
        tmpRead = ov_read(&m_ogg, tmpBuff, 2048, 0, 2, 1, &bitStream);
        tmpPos += tmpRead;
    }
    samplesReady = m_touch->numSamples();
    if (samplesReady > 0) {
      read = m_touch->receiveSamples(m_pcmBuffer + pos, samplesReady);
      pos += read;
    }
    if (pos > 4096)
      m_isReady = true;
    if (tmpRead > 0) {
      m_touch->putSamples((SAMPLETYPE*)tmpBuff, tmpRead / 2);
    }
  }
  m_isDecoding = false;
  qDebug() << "readFromOgg" << pos;
  m_thread->quit();
}



void ToggScale::setSampleRate(unsigned int rate) {
  
}



bool ToggScale::loadAudioData() {
  if (m_oggInMemory)
    return true;
  QFile oggFile(m_oggFileName);
  if (!oggFile.exists())
      return false;
  
  oggFile.open(QIODevice::ReadOnly);
  QDataStream oggStream(&oggFile);
  m_oggInMemory = new qint8[oggFile.size()];
  oggStream.readRawData((char*)m_oggInMemory, oggFile.size());
  
  ov_callbacks myCallBacks;
  m_oggWrap.curPtr = m_oggInMemory;
  m_oggWrap.filePtr = m_oggInMemory;
  m_oggWrap.fileSize = oggFile.size();
  
  qDebug() << "ogg file size:" << m_oggWrap.fileSize << "addr" << m_oggWrap.filePtr << "size" << sizeof(m_oggInMemory);
  oggFile.close();
  m_pcmBuffer = new qint16[2 * m_sampleRate];
  myCallBacks.read_func = readOggStatic;
  myCallBacks.seek_func = seekOggStatic;
  myCallBacks.close_func = closeOggStatic;
  myCallBacks.tell_func = tellOggStatic;
  
  int ret = ov_open_callbacks((void*)&m_oggWrap, &m_ogg, NULL, 0, myCallBacks);
  
  if (ret < 0) {
    qDebug() << "cant open ogg stream";
    return false;
  }
  
  vorbis_info *oggInfo = ov_info(&m_ogg, -1);
  qDebug() << oggInfo->rate << oggInfo->channels << (bool)ov_seekable(&m_ogg);
//   char **ptr=ov_comment(&m_ogg, -1)->user_comments;
//     vorbis_info *vi=ov_info(&m_ogg, -1);
//     while(*ptr){
//       fprintf(stderr,"%s\n",*ptr);
//       ++ptr;
//     }  
//   
  return true;  
}


void ToggScale::initTouch() {
  m_touch->setChannels(1);
  m_touch->setSampleRate(44100);
  m_touch->setPitch(1.0 + m_pitchOffset);
}




