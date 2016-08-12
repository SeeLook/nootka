/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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

#ifndef TPITCHFINDER_H
#define TPITCHFINDER_H


#include "nootkasoundglobal.h"
#include <QObject>
#include <QMutex>
#include <music/tnotestruct.h>
#include "tartiniparams.h"


// This part of code id directly taken from Tartini musicnotes.h --------------------
	/** Converts the frequencies freq (in hertz) into their note number on the midi scale
    i.e. the number of semi-tones above C0
    Note: The note's pitch will contain its fractional part
    Reference = http://www.borg.com/~jglatt/tutr/notenum.htm
		@param freq The frequency in Hz
		@return The pitch in fractional part semitones from the midi scale. */
inline qreal freq2pitch(qreal freq)
{
#ifdef log2
	return -36.3763165622959152488 + 12.0*log2(freq);
#else
	return -36.3763165622959152488 + 39.8631371386483481*log10(freq);
#endif
}

		/** Does the opposite of the function above */
inline qreal pitch2freq(qreal note) {
  return TnoteStruct::pitchToFreq(note);
}
//-----------------------------------------------------------------------------------


class Channel;
class NoteData;
class MyTransforms;


/** 
 * The main purpose of this class is to recognize pitch of audio data flowing through it. 
 * Finding pitch method(s) are taken from Tartini project written by Philip McLeod.
 * It collects audio data by calling @p copyToBuffer() method,
 * when buffer is full pitch detection is performed.
 * It emits signal with volume @p volume(float)
 * signal with pitch in every processed chunk @p pichInChunk(float)
 * signal when detected note starts  @p noteStarted(qreal pitch, qreal frequency)
 * and signal when note ends @p noteFinished(TnoteStruct)
 * Emitted values are as they are without any additional computation
 * like middle a adjusting or checking note range (ambitus)
 */
class NOOTKASOUND_EXPORT TpitchFinder : public QObject
{
	Q_OBJECT
	
public:
	explicit TpitchFinder(QObject *parent = 0);
	virtual ~TpitchFinder();

	enum EnoteState {
		e_silence, // nothing was noticed in processed chunk - probably silence
		e_noticed, // note was noticed by Tartini and is loud enough but is too short for Nootka
		e_playing  // note is playing longer then minimal duration
	};
	
	/** Those are levels of pitch detection ranges.
		 * @p e_high - for violin, flute, piccolo - corresponds with treble clef.
		 * 							It starts form about F in small octave.
		 * @p e_middle - for guitars, cello and so - corresponds with treble dropped and bass clefs
		 * 							It starts form about F in contra octave.
		 * @p e_low - for bass guitar and double bass - corresponds with bass dropped clef
		 * 							It is sufficient to detect lowest notes.
		 */
  enum Erange {
		e_high = 0,
		e_middle = 1,
		e_low = 2
  };
	
	TartiniParams* aGl() { return m_aGl; } /** global settings for pitch recognition. */

	bool isBussy() { return m_isBussy; }
	
	int currentChunk() { return m_chunkNum; }
	void setCurrentChunk(int curCh) { m_chunkNum = curCh; }
	void incrementChunk() { m_chunkNum++; }
	
      /**
       * Copies @p nBufferFrames of @p data to buffer.
       * Parallel thread detects that appropriate amount of audio data is ready
       * and starts pitch detection
       */
  void copyToBuffer(void* data, unsigned int nBufferFrames);

      /**
       * Informs @p TpitchFinder that audio input stops.
       * Given Boolean switch decides about resetting.
       */
  void stop(bool resetAfter = false);

			/** Changes default 44100 sample rate to given value. It takes effect only after resetFinder().
				* @p range is TaudioParams::Erange cast. Default is e_middle
				* Better don't call this during processing. */
	void setSampleRate(unsigned int sRate, int range = 1);
				
				/** Only notes with volume above this value are sending. 
					* If note has got such volume it is observed till its end - even below. */
	void setMinimalVolume(float vol) { m_minVolume = vol; }

	qreal chunkTime() { return m_chunkTime; } /** Duration time of chunk for current sample rate and frames per chunk. */
	void setMinimalDuration(float dur) { m_minDuration = dur; m_minChunks = qRound((qreal)m_minDuration / m_chunkTime); }
	float minimalDuration() { return m_minDuration; } /** Minimum acceptable duration of a note to be pitch-detected. */
	int minChunksNumber() { return m_minChunks; } /** Minimal number of chunks in note to be pitch-detected. */

      /** Determines whether increased volume of played note split it.
       * Tartini doesn't detect this so extra checking will be done if @p TRUE.
       * Volume threshold can be set through @p setSplitValue() */
	void setSplitByVolChange(bool sp) { m_splitByVol = sp; }
	bool isSplitByVolume() { return m_splitByVol; }

	void setSplitVolume(qreal volToSplit) { m_minVolToSplit = qMax<qreal>(volToSplit, 0.05); }
	qreal minVolumeToSplit() { return m_minVolToSplit; }

      /** multiplexer of sound volume (aka %) that determines minimum volume of next note to be pitch-detected.
       * i.e. - value of 0.8 determines that note has to have at least 80% volume of average volume */
	void setSkipStillerVal(qreal skipStill) { m_skipStillerVal = skipStill; }
	qreal skipStillerValue() { return m_skipStillerVal; }
	
	TnoteStruct* lastNote() { return &m_currentNote; }

      /** In offline mode pitch detecting isn't performed in separate thread.
       * After collecting audio data in buffer, detection is performed
       * and no data is retrieving until detection in current chunk is finished. */
	bool isOffline() { return m_isOffline; }
	void setOffLine(bool off);

      /** Pointer to detection processing @class Channel.
       * WARRING!
       * In online mode it is reset every 1000 chunks,
       * only in offline mode all detected data exists. */
	Channel* ch() { return m_channel; }
	MyTransforms* transforms() { return m_transforms; }

      /** Returns current range of pitch detection */
	Erange pitchRange() { if (m_rateRatio == 0.5) return e_high; else if (m_rateRatio == 2.0) return e_low; else return e_middle; }

signals:
  void pitchInChunk(float); /** Pitch in chunk that has been just processed */
  void volume(float);
	void noteStarted(qreal pitch, qreal freq, qreal duration);
	void noteFinished(TnoteStruct*); /** Emitting parameters of finished note (pitch, freq, duration) */
	
protected slots:
	void startPitchDetection(); /** Starts searching thread */
	void processed(); /** Performs signal emitting after chunk was done but out of processing thread */
  void detectingThread();
  void threadFinished();
	
	
private:
	void detect();

      /** Cleans all buffers, sets m_chunkNum to 0. */
  void resetFinder();
	
	
private:
  QThread              *m_thread;
  MyTransforms         *m_transforms;
  float                *m_filteredChunk, *m_workChunk, *m_prevChunk;
  float                *m_floatBuffer;
  qint16               *m_tokenBuffer;
  unsigned int          m_writePos, m_readPos;
  volatile quint32      m_framesReady; /**< Number of frames ready for processing */
  volatile bool         m_doProcess, m_doReset; /**< When @p TRUE when detecting thread p  */

  bool                  m_isOffline;
  TartiniParams        *m_aGl;
  Channel              *m_channel;
  int                   m_chunkNum;
  bool                  m_isBussy;
  int                   m_prevNoteIndex;
  float                 m_minVolume;
  float                 m_minDuration;
  float                 m_rateRatio; /**< multiplexer of the sample rate determined from pitch detection range */
  QMutex                m_mutex;
  float                 m_volume, m_chunkPitch;
  EnoteState            m_state, m_prevState;
  float                 m_pcmVolume, m_workVol;
  TnoteStruct           m_newNote, m_currentNote;
  bool                  m_splitByVol;
  qreal                 m_minVolToSplit, m_chunkTime, m_skipStillerVal, m_averVolume;
  int                   m_minChunks;

};

#endif // TPITCHFINDER_H
