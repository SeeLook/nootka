/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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

#ifndef TPITCHFINDER_H
#define TPITCHFINDER_H


#include "nootkasoundglobal.h"
#include <QtCore/qobject.h>
#include <QtCore/qmutex.h>
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
class QThread;
class NoteData;
class MyTransforms;


/**
 * This is buffer that stores audio data converted to float,
 * before it is filtered and send to @class Channel.
 */
class TfloatBuffer {

public:
  TfloatBuffer(quint32 buffSize) {
    processed = true;
    data = new float[buffSize];
    size = buffSize;
  }

  ~TfloatBuffer() { delete data; }

  float*      data; /** Pointer to float array of buffer size */
  bool        processed; /** Set it to @p TRUE when data has been used already */
  quint32     size;

  void resize(quint32 newSize) {
    if (newSize != size) {
      delete data;
      data = new float[newSize];
      size = newSize;
    }
  }
};


/** 
 * The main purpose of this class is to recognize pitch of audio data flowing through it. 
 * Finding pitch method(s) are taken from Tartini project written by Philip McLeod.
 * It collects samples by calling @p fillBuffer(float sample) method, 
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

	bool isBusy() { return m_isBusy; }
	
	int currentChunk() { return m_chunkNum; }
	void setCurrentChunk(int curCh) { m_chunkNum = curCh; }
	void incrementChunk() { m_chunkNum++; }
	
			/** Adds given sample to the buffer at the current position, 
				* when buffer is full, @p bufferReady() is invoked and
				* current buffer is swapped. */
	void fillBuffer(float sample) {
    *(m_currentBuff->data + m_posInBuffer) = sample;
    m_posInBuffer++;
    m_workVol = qMax<float>(m_workVol, sample);
    if (m_posInBuffer == m_aGl->framesPerChunk)
      bufferReady();
  }
	
			/** Changes default 44100 sample rate to given value. It takes effect only after resetFinder().
				* @p range is TaudioParams::Erange cast. Default is e_middle
				* Better don't call this during processing. */
	void setSampleRate(unsigned int sRate, int range = 1);
	
	void resetFinder(); /** Cleans all buffers, sets m_chunkNum to 0. */
				
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
       * and no data is retrieving until detection in current chunk is finished.
       * Setting this to true overrides @p copyInThread to @p TRUE and @p nrChunksToReset to 0 */
  void setOffLine(bool off);
  bool isOffline() { return m_isOffline; }

      /** Determines whether current buffer is copied to Tartini @p Channel.
       * By default it is being done in detecting thread,
       * but it requires accurate feeding @p fillBuffer() with the same portion of data
       * less or equal of frame buffer size - RtAudio call backs did it well.
       * Otherwise TpitchFinder has no time to properly swap between two buffers.
       * When it is set to @p FALSE coping is performed in @p bufferReady()
       * out of detecting thread.
       * It is performed in audio call back thread, so
       * It requires bigger audio buffer (1024 bytes - 21 ms)
       * - @p QtAudioInput works like this.  */
  void setCopyInThread(bool cit) { m_copyInThread = cit; }
  bool copyInThread() { return m_copyInThread; }

      /** This value determines how often @p Channel is reset.
       * By default it is every 1000 chunks - channel data occupies around 20 MB.
       * However values less than 100 are not recommended.
       * Resetting channel is done only when no note is detected,
       * so if something is played still - no resetting is performed.
       * If set to 0 - reset is not performed as well. */
  void setNrChunksToReset(quint16 chunksNr) { m_chunksToReset = chunksNr; }
  quint16 nrChunksToReset() { return m_chunksToReset; }

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
	
	
private:
	void detect();
  void bufferReady(); /** Performed when all required amount of samples is collected it current buffer. */
  void copyToChannel(); /** Performs filtering (if enabled) and copies @p m_currentBuff to @p Channel */
	
	
private:
  QThread					        *m_thread;
  MyTransforms            *m_transforms;
  TfloatBuffer            *m_filteredChunk, *m_prevChunk, *m_currentBuff;
  QList<TfloatBuffer*>     m_buffers; // list of real buffers (number depends on platform)
	int							         m_posInBuffer, m_currentBufferNr, m_processingBuffNr;
	float						        *m_workChunk; // virtual buffer pointing to real ones


  bool             m_doReset, m_isOffline, m_copyInThread;
	TartiniParams   *m_aGl;
	Channel         *m_channel;
	int              m_chunkNum;
	bool             m_isBusy;
  int              m_prevNoteIndex;
  float            m_minVolume;
	float						 m_minDuration;
	float						 m_rateRatio; // multiplexer of the sample rate determined from pitch detection range
	QMutex					 m_mutex, m_offLineMutex;
	float 					 m_volume, m_chunkPitch;
	EnoteState			 m_state, m_prevState;
  float            m_pcmVolume, m_workVol;
  TnoteStruct      m_newNote, m_currentNote;
  bool             m_splitByVol;
  qreal            m_minVolToSplit, m_chunkTime, m_skipStillerVal, m_averVolume;
  int              m_minChunks;
  quint16          m_chunksToReset;

};

#endif // TPITCHFINDER_H
