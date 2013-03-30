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


#ifndef TOGGSCALE_H
#define TOGGSCALE_H

#include <QString>
#include <QObject>
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"



class ToggScale : public QObject
{
  
public:
  
    ToggScale(QString &path); 
    virtual ~ToggScale();
    
        /** Loads ogg file with scale to RAM. If everything is ok returns true */
    bool loadAudioData();
        /** Unloads audio data from buffer. */
    void deleteData();
    
    
  /* To read ogg data from RAM */
    struct SoggFile {
        qint8* curPtr;
        qint8* filePtr;
        size_t fileSize;
    };
    
    void setPos(ogg_int64_t offset);
    qint16 getSample(int offset);
    
public slots:
    void readFromOgg();
    
signals:
    void decodingDone();
    
private:
    static size_t readOggStatic(void* dst, size_t size1, size_t size2, void* fh);
    static int    seekOggStatic(void *fh, ogg_int64_t to, int type );
    static int    closeOggStatic(void* fh);
    static long   tellOggStatic(void *fh );
    
private:
    qint8             *m_oggBuffer;
    QString           m_oggFile; /** Path to ogg file with sounds */
    OggVorbis_File    m_ogg;
    qint16            *m_pcmBuffer;
    SoggFile          m_oggWrap;
};

#endif // TOGGSCALE_H
