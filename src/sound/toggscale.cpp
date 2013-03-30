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
#include <stdlib.h>
// #include <unistd.h>

/*Static*/
size_t ToggScale::readOggStatic(void* dst, size_t size1, size_t size2, void* fh) {
    SoggFile* of = reinterpret_cast<SoggFile*>(fh);
    size_t len = size1 * size2;
    if ( of->curPtr + len > of->filePtr + of->fileSize ) {
        len = of->filePtr + of->fileSize - of->curPtr;
    }
    memcpy( dst, of->curPtr, len );
    of->curPtr += len;
//     qDebug() << len;
    return len;
}

int ToggScale::seekOggStatic(void* fh, ogg_int64_t offset, int type) {
    SoggFile* of = reinterpret_cast<SoggFile*>(fh);
    switch(type) {
      case SEEK_SET:
          of->curPtr = of->filePtr + offset;
          break;
        case SEEK_CUR:
          of->curPtr += offset;
          break;
        case SEEK_END:
          of->curPtr = of->filePtr + of->fileSize - offset;
          break;
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
    return 0;
}


long int ToggScale::tellOggStatic(void* fh) {
    SoggFile* of = reinterpret_cast<SoggFile*>(fh);
    return (of->curPtr - of->filePtr);
}

/*************************************************************************/


ToggScale::ToggScale(QString& path) :
  QObject(),
  m_oggFile(path + "sounds/classical-guitar.ogg"),
  m_oggBuffer(0),
  m_pcmBuffer(0)
{

}


ToggScale::~ToggScale()
{
  deleteData();
}


void ToggScale::deleteData() {
  if (m_oggBuffer) {
    delete m_oggBuffer;
    m_oggBuffer = 0;
  }
  delete m_pcmBuffer;
  m_pcmBuffer = 0;
}


qint16 ToggScale::getSample(int offset) {
//   if (offset < 1024)
    return m_pcmBuffer[offset];
//   else {
//     qDebug() << "no data";
//     return 0;
//   }
}

void ToggScale::setPos(ogg_int64_t offset) {
  int ret = ov_pcm_seek(&m_ogg, offset);
//   qDebug() << "seek back:" << ret;
  if (!ret)
    readFromOgg();
}

void ToggScale::readFromOgg() {
  int bitStream;
  long int read = 0;
  int loops = 0;
  int pos = 0;
  while (loops < 1000 && pos < 170000) {
    read = ov_read(&m_ogg, (char*)m_pcmBuffer + pos, 176000, 0, 2, 1, &bitStream);
    pos += read;
    loops++;
//     sleep(1);
  }
//   qDebug() << "readFromOgg" << read << "loops" << loops;
}


bool ToggScale::loadAudioData() {
  if (m_oggBuffer)
    return true;
  QFile oggFile(m_oggFile);
  if (!oggFile.exists())
      return false;
  
  oggFile.open(QIODevice::ReadOnly);
  QDataStream oggStream(&oggFile);
  m_oggBuffer = new qint8[oggFile.size()];
  oggStream.readRawData((char*)m_oggBuffer, oggFile.size());
  
  ov_callbacks myCallBacks;
  m_oggWrap.curPtr = m_oggBuffer;
  m_oggWrap.filePtr = m_oggBuffer;
  m_oggWrap.fileSize = oggFile.size();
  
  qDebug() << "ogg file size:" << m_oggWrap.fileSize << "addr" << m_oggWrap.filePtr << "size" << sizeof(m_oggBuffer);
  oggFile.close();
  m_pcmBuffer = new qint16[88200];
  myCallBacks.read_func = readOggStatic;
  myCallBacks.seek_func = seekOggStatic;
  myCallBacks.close_func = closeOggStatic;
  myCallBacks.tell_func = tellOggStatic;
  
  int ret = ov_open_callbacks((void*)&m_oggWrap, &m_ogg, NULL, 0, myCallBacks);
//   int ret = ov_open_callbacks((void*)m_oggBuffer, &m_ogg, NULL, 0, OV_CALLBACKS_NOCLOSE);
  
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

