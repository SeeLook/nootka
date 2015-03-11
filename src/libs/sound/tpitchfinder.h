/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include "tartini/mytransforms.h"
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
inline qreal pitch2freq(qreal note)
{
	qreal result = pow10((note + 36.3763165622959152488) / 39.8631371386483481);
	return result;
}
//-----------------------------------------------------------------------------------


class Channel;
class QThread;
class NoteData;


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
	
	MyTransforms myTransforms;
	
	TartiniParams* aGl() { return m_aGl; } /** global settings for pitch recognition. */

	bool isBussy() { return m_isBussy; }
	
	int currentChunk() { return m_chunkNum; }
	void setCurrentChunk(int curCh) { m_chunkNum = curCh; }
	void incrementChunk() { m_chunkNum++; }
	
			/** Adds given sample to the buffer at the current position, 
				* when buffer is full, @p startPitchDetection() is invoked and 
				* current buffer is swapped. */
	void fillBuffer(float sample);
	
			/** Changes default 44100 sample rate to given value. It takes effect only after resetFinder().
				* @p range is TaudioParams::Erange cast. Default is e_middle
				* Better don't call this during processing. */
	void setSampleRate(unsigned int sRate, int range = 1);
	
	void resetFinder(); /** Cleans all buffers, sets m_chunkNum to 0. */
				
				/** Only notes with volume above this value are sending. 
					* If note has got such volume it is observed till its end - even below. */
	void setMinimalVolume(float vol) { m_minVolume = vol; }
	void setMinimalDuration(float dur) { m_minDuration = dur; }
	
	TnoteStruct* lastNote() { return m_lastNote; }

      /** In offline mode pitch detecting isn't performed in separate thread.
       * After collecting audio data in buffer detection is performed
       * and no data is retrieving until detection in current is finished.
       *
       */
	bool isOffline() { return m_isOffline; }
	void setOffLine(bool off) { m_isOffline = off; }

      /** Pointer to detection processing @class Channel.
       * WARRING!
       * In online mode it is reset every 1000 chunks,
       * only in offline mode all detected data exists. */
	Channel* ch() { return m_channel; }

      /** List of detected values of every chunk (cleared by @p reset()).
       * WARRING! @p id is not verified - check it by @p dataSize() */
	TnoteStruct& dl(int id) { return m_dataList[id]; }
	int dataSize() { return m_dataList.size(); }
	TnoteStruct& lastData() { return m_dataList.last(); }

signals:
  void pitchInChunk(float); /** Pitch in chunk that has been just processed */
  void volume(float);
	void noteStarted(qreal pitch, qreal freq, qreal duration);
	void noteFinished(TnoteStruct*); /** Emitting parameters of finished note (pitch, freq, duration) */
  void chunkProcessed(TnoteStruct*); /** Emitted in offline mode only, after every processed chunk with analyzed data or 0 */
	
	
protected slots:
	void startPitchDetection(); /** Starts searching thread */
	void processed(); /** Connected with QThread::finished() signal. */
	
	
private:
	void detect();
	
	
private:
  QThread					*m_thread;
  float           *m_filteredChunk, *m_workChunk, *m_prevChunk;
	float						*m_buffer_1, *m_buffer_2; // real buffers
	int							 m_posInBuffer;
	float						*m_currentBuff, *m_filledBuff; // virtual buffers pointing to real ones
  qreal            m_prevPitch, m_prevFreq, m_prevDuration;

  bool             m_doReset, m_isOffline;
	TartiniParams   *m_aGl; 
	Channel         *m_channel;
	int              m_chunkNum;
	bool             m_isBussy;
  int              m_prevNoteIndex;
  int              m_noticedIndex;
  float            m_minVolume;
	float						 m_minDuration;
	float						 m_rateRatio; // multiplexer of the sample rate determined from pitch detection range
	QMutex					 m_mutex;
	float 					 m_volume, m_chunkPitch, m_newNoteStartDur;
	EnoteState			 m_state, m_prevState;
	qreal 					 m_prevVolume, m_lastDuration;
	int 						 m_detectedIndex;
	bool 						 m_couldBeNew;
	TnoteStruct			*m_currentNote, *m_lastNote;
	qreal 					 m_statPitch; // value of previous pitch to detect notes repeats by increased volume
	QList<TnoteStruct> m_dataList; /** List of detected values of every chunk (cleared by @p reset()) */
};

#endif // TPITCHFINDER_H
