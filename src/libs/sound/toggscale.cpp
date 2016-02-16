/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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
#include <QFile>
#include <QDataStream>
#include <QDebug>
#include <QThread>
#include <QFileInfo>
#include <stdlib.h>
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

//###########################################################################
//########################## PUBLIC #########################################
//###########################################################################

int minDataAmount = 10000;

ToggScale::ToggScale() :
  QObject(),
  m_oggInMemory(0),
  m_pcmBuffer(0),
  m_thread(new QThread),
  m_sampleRate(44100),
  m_prevNote(-10000),
  m_doDecode(true), m_isDecoding(false), m_isReady(true),
  m_pitchOffset(0.0f), m_innerOffset(0.0),
  m_oggConnected(false), m_touchConnected(false),
  m_instrument(-1)
{
	m_touch = new soundtouch::SoundTouch();
	m_touch->setChannels(1);
#if defined (Q_OS_UNIX) // increase minimal audio data must to be processed when system works with PulseAudio
	QFileInfo pulseBin("/usr/bin/pulseaudio");
	if (pulseBin.exists()) // it is necessary both for Nootka with native PA and PA in ALSA bridge mode
		minDataAmount = 15000;
#endif
  moveToThread(m_thread);
  connect(m_thread, SIGNAL(started()), this, SLOT(decodeOgg()));
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
    m_oggInMemory = 0;
  }
  delete m_pcmBuffer;
  m_pcmBuffer = 0;
  delete m_touch;
  m_thread->deleteLater();
}


qint16 ToggScale::getSample(int offset) {
    return m_pcmBuffer[offset];
}



void ToggScale::setNote(int noteNr) {
  if (noteNr == m_prevNote)
    return;
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
  int fasterOffset = 1000;
	if (baseNote - m_firstNote == 0)
			fasterOffset = 0;
  stopDecoding();
  m_prevNote = noteNr;
	int ret = ov_pcm_seek(&m_ogg, (baseNote - m_firstNote) * 44100 * 2 - fasterOffset);
  m_thread->start();
}


void ToggScale::setSampleRate(unsigned int rate) {
  if (m_sampleRate != rate) {
    m_sampleRate = rate;
    adjustSoundTouch();
  }
}


void ToggScale::setPitchOffset(float pitchOff) {
  if (pitchOff != m_pitchOffset) {
    m_pitchOffset = pitchOff;
    adjustSoundTouch();
  }
}


bool ToggScale::loadAudioData(int instrument) {
	QString fileName;
	if (instrument != m_instrument) {
		switch ((Einstrument)instrument) {
			case e_classicalGuitar:
				fileName = Tpath::sound("classical-guitar");
				m_firstNote = -11; m_lastNote = 41;
				break;
			case e_electricGuitar:
				fileName = Tpath::sound("electric-guitar");
				m_firstNote = -11; m_lastNote = 41;
				break;
			case e_bassGuitar:
				fileName = Tpath::sound("bass-guitar");
				m_firstNote = -24; m_lastNote = 21;
				break;
			default:
				return false;
		}
	} else
			return false;

  QFile oggFile(fileName);
  if (!oggFile.exists())
      return false;
  
  oggFile.open(QIODevice::ReadOnly);
  QDataStream oggStream(&oggFile);
	if (m_oggInMemory)
			delete m_oggInMemory;
  m_oggInMemory = new qint8[oggFile.size()];
  oggStream.readRawData((char*)m_oggInMemory, oggFile.size());
  
  ov_callbacks myCallBacks;
  m_oggWrap.curPtr = m_oggInMemory;
  m_oggWrap.filePtr = m_oggInMemory;
  m_oggWrap.fileSize = oggFile.size();
  
  oggFile.close();
	
	if (m_pcmBuffer)
			delete m_pcmBuffer;
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
  
//   vorbis_info *oggInfo = ov_info(&m_ogg, -1);
//   qDebug() << oggInfo->rate << oggInfo->channels << (bool)ov_seekable(&m_ogg);
//   char **ptr=ov_comment(&m_ogg, -1)->user_comments;
//     vorbis_info *vi=ov_info(&m_ogg, -1);
//     while(*ptr){
//       fprintf(stderr,"%s\n",*ptr);
//       ++ptr;
//     }  
  
  return true;  
}


//###########################################################################
//########################## PROTECTED ######################################
//###########################################################################

void ToggScale::stopDecoding() {
  if (m_isDecoding) {
      qDebug("decoding in progress");
      m_doDecode = false;
      do {
        SLEEP(1);
      } while (m_isDecoding);
      m_doDecode = true;
  }
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
    if (pos > minDataAmount) // amount of data needed by single loop of rtAudio outCallBack
      m_isReady = true;
    loops++;
  }
  m_isDecoding = false;
//   qDebug() << "readFromOgg" << pos << "loops" << loops;
  m_thread->quit();
}


void ToggScale::decodeAndResample() {
  int bitStream;
  m_isDecoding = true;
  int maxSize = 44100 * 2 - 8192; // two sec. of audio minus some silence on the end
  long int tmpPos = 0, tmpRead = 0;
  uint pos = 0, read = 0;
  int samplesReady = 0;
  
  float **oggChannels;
  float *left ;
  float *tmpTouch = new float[8192];
  
  while (m_doDecode && pos < maxSize) {
    /// 1. Grab audio data from ogg
    if (tmpPos < 172000) { // almost 2 sec. of a note
        tmpRead = ov_read_float(&m_ogg, &oggChannels, 2048, 0);
        tmpPos += tmpRead;
        left = oggChannels[0]; // pointer to lefta channel chunk
        if (tmpRead > 0) { /// 2. push data to SoundTouch
            m_touch->putSamples((SAMPLETYPE*)left, tmpRead);
        }
    }
    samplesReady = m_touch->numSamples();
    if (samplesReady > 0) { /// 3. Get resampled/offsetted data from SoundTouch
      read = m_touch->receiveSamples((SAMPLETYPE*)tmpTouch, samplesReady);      
      for (int i = 0; i < read; i++) /// 4. Convert samples to 16bit integer
          *(m_pcmBuffer + pos + i) = qint16(*(tmpTouch + i) * 32768);
      pos += read;
    }
    if (pos > minDataAmount) // below this value SoundTouch is not able to prepare data
      m_isReady = true;
  }
  m_isDecoding = false;
//   qDebug() << "decodeAndResample finished" << pos;
  m_touch->clear();
  m_thread->quit();
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
      float newRate =  44100.0f / (float)m_sampleRate;
      m_touch->setRate(newRate);
    }
//     qDebug() << "SoundTouch sampleRate" << m_sampleRate << "pitch offset" << m_innerOffset + m_pitchOffset;
    if (!m_touchConnected)
        connect(m_thread, SIGNAL(started()), this, SLOT(decodeAndResample()));
    m_touchConnected = true;
    if (m_oggConnected)
				disconnect(m_thread, SIGNAL(started()), this, SLOT(decodeOgg()));
    m_oggConnected = false;
  } else {
			if (!m_oggConnected)
					connect(m_thread, SIGNAL(started()), this, SLOT(decodeOgg()));
			m_oggConnected = true;
			if (m_touchConnected)
					disconnect(m_thread, SIGNAL(started()), this, SLOT(decodeAndResample()));
			m_touchConnected = false;
  }
}




