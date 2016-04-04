/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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


#ifndef TOGGSCALE_H
#define TOGGSCALE_H

#include "nootkasoundglobal.h"
#include <QString>
#include <QObject>
#include "vorbis/codec.h"
#include "vorbis/vorbisfile.h"
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
  // Linux uses distribution package of SoundTouch
  #include "soundtouch/SoundTouch.h"
#else
  // Mac & Win & Android has compiled-in SoundTouch
  #include "stouch/SoundTouch.h"
#endif


using namespace soundtouch;


/**
* @class ToggScale manages audio data (musical scale) taken from ogg file.
* It keeps it in m_oggInMemory array and decode it when setNote is called.
* Decompressed data are available through getSample() method.
* Data is decompressed in separate thread and some SLEEP calls are performed
* if data is not ready.
*
* @p setSampleRate() and @p setPitchOffset() control appropriate parameters of note.
*
* To get sample @p setNote has to be called first.
* It starts decoding thread which prepares first portion of data.
* Data is ready only when @p isReady() returns true.
* Preparing process takes around 1-10 ms (depends on CPU)
*/
class NOOTKASOUND_EXPORT ToggScale : public QObject
{
  Q_OBJECT

public:

  ToggScale();
  virtual ~ToggScale();

      /** Loads ogg file with scale of given instrument to RAM.
        * If everything is OK, returns true.
        * Also it is setting low and hi notes of a scale.
        * Notes beyond scale are generated with SoundTouch. */
  bool loadAudioData(int instrument);
      /** Unloads audio data from buffer. */
  void deleteData();


      /** To read ogg data from RAM */
  struct SoggFile {
      qint8* curPtr;
      qint8* filePtr;
      size_t fileSize;
  };

      /** Prepares m_pcmBuffer:
        * - determines is pitch offset necessary
        * - seek ogg
        * - starts decoding (re-sampling).
        * - stops previous decoding if performed. */
  void setNote(int noteNr);
  qint16 getSample(int offset);
  unsigned int sampleRate() { return m_sampleRate; }

      /** TRUE when appropriate data amount in a buffer is ready. */
  bool isReady() { return m_isReady; }

  void setSampleRate(unsigned int rate);

      /** Sets decimal offset of a pitch -0.99 to +0.99 */
  void setPitchOffset(float pitchOff);

  int alreadyDecoded() { return m_alreadyDecoded; } /**< Number of already decoded bytes */
  int noteNr() { return m_prevNote; } /**< Number of note in buffer or -10000 if none */

signals:
  void oggReady(); /**< Emitted when appropriate amount of decoded data is ready */


protected slots:
      /** Preforms decoding. Usually is invoked by m_thread.start()
        * called from setNote() method. */
  void decodeOgg();
      /** Decoding and re-sampling or/and pitch shifting */
  void decodeAndResample();
      /** Checks is decoding performed and stops it
        * by setting m_doDecode=false and waiting for stop. */
  void stopDecoding();

private:
    /** Methods needed by vorbisfile library. */
  static size_t readOggStatic(void* dst, size_t size1, size_t size2, void* fh);
  static int    seekOggStatic(void *fh, ogg_int64_t to, int type );
  static int    closeOggStatic(void* fh);
  static long   tellOggStatic(void *fh );

  void adjustSoundTouch(); // sets SoundTouch parameters

private:
  qint8             *m_oggInMemory;
  OggVorbis_File     m_ogg; /**< ogg vorbis handler */
  qint16            *m_pcmBuffer; /**< buffer with decompressed data of selected note. */
  SoggFile           m_oggWrap; /**< Structure wrapped m_oggInMemory used by ogg vorbis. */
  QThread           *m_thread;
  unsigned int       m_sampleRate;
  int                m_prevNote;
  bool               m_doDecode; /**< If new note is going to be decoded it goes to FALSE - then stops decoding loop */
  bool               m_isDecoding; /**< TRUE during decoding/resampling process. */
  bool               m_isReady;
  SoundTouch        *m_touch;
  float              m_pitchOffset; /**< Offset set from outside (by user) */
  float              m_innerOffset; /**< Offset calculated in setNote when SoundTouch has to generate note. */
  bool               m_oggConnected, m_touchConnected;
  int                m_firstNote, m_lastNote; /**< Numbers of first and last notes in file with scale. */
  int                m_instrument; /**< current instrument which samples are loaded */
  int                m_alreadyDecoded; /**< Number of already decoded bytes */

};

#endif // TOGGSCALE_H
