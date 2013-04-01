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


/** @class ToggScale manages audio data (musical scale) taken from ogg file.
 * It keeps it in m_oggInMemory array and decode it when setNote is called.
* Decompressed data are available through getSample() method. 
* Data is decompressed in separate thread and smoe SLEEP calls are performed 
* if data is not ready. */
class ToggScale : public QObject
{
  Q_OBJECT
  
public:
  
    ToggScale(QString &path); 
    virtual ~ToggScale();
    
        /** Loads ogg file with scale to RAM. If everything is ok returns true */
    bool loadAudioData();
        /** Unloads audio data from buffer. */
    void deleteData();
    
    
        /** To read ogg data from RAM */
    struct SoggFile {
        qint8* curPtr;
        qint8* filePtr;
        size_t fileSize;
    };
    
    void setPos(ogg_int64_t offset); /** DEPRECATED */
        /** Prepares m_pcmBuffer */
    void setNote(int noteNr);
    qint16 getSample(int offset);
    void setSampleRate(unsigned int rate);
    unsigned int sampleRate() { return m_sampleRate; }
        /** TRUE when appropirate data amount in a buffer is ready. */
    bool isReady() { return m_isReady; }
    
    
public slots:
        /** Preforms decoding. Usually is invoked by m_thread.start() 
         * called from setNote() method. */
    void decodeOgg();
    
private:
      /** Methods needed by vorbisfile library. */
    static size_t readOggStatic(void* dst, size_t size1, size_t size2, void* fh);
    static int    seekOggStatic(void *fh, ogg_int64_t to, int type );
    static int    closeOggStatic(void* fh);
    static long   tellOggStatic(void *fh );
    
private:
    qint8             *m_oggInMemory;
    QString           m_oggFileName; /** Path to ogg file with sounds */
    OggVorbis_File    m_ogg; /** ogg vorbis handler */
    qint16            *m_pcmBuffer; /** buffer with decompressed data of selected note. */
    SoggFile          m_oggWrap; /** Structure wraped m_oggInMemory used by ogg vorbis. */
    QThread           *m_thread;
    unsigned int      m_sampleRate;
    int               m_prevNote;
    bool              m_doDecode; /** If new note is going to be decoded it goes to FALSE - then stops decoding loop */
    bool              m_isDecoding; /** TRUE during decoding/resampling process. */
    bool              m_isReady;
};

#endif // TOGGSCALE_H
