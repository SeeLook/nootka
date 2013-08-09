/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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



#include <QObject>
#include "tartini/mytransforms.h"
#include "tartiniparams.h"

// This part of code id directly taken from Tartini musicnotes.h --------------------
	/** Converts the frequencies freq (in hertz) into their note number on the midi scale
    i.e. the number of semi-tones above C0
    Note: The note's pitch will contain its fractional part
    Reference = http://www.borg.com/~jglatt/tutr/notenum.htm
		@param freq The frequency in Hz
		@return The pitch in fractional part semitones from the midi scale. */
inline double freq2pitch(double freq)
{
#ifdef log2
	return -36.3763165622959152488 + 12.0*log2(freq);
#else
	return -36.3763165622959152488 + 39.8631371386483481*log10(freq);
#endif
}

		/** Does the opposite of the function above */
inline double pitch2freq(double note)
{
	double result = pow10((note + 36.3763165622959152488) / 39.8631371386483481);
	return result;
}
//-----------------------------------------------------------------------------------


class Channel;


/** The main purpose of this class is to recognize pitch
 * of aduio data flowing throught it. 
 * Finding pitch method(s) are taken from Tartini project
 * writen by Philip McLeod.
 */
class TpitchFinder : public QObject
{
	Q_OBJECT
	
public:
    explicit TpitchFinder(QObject *parent = 0);
    virtual ~TpitchFinder();
  
    MyTransforms myTransforms;
        /** global settings for pitch recognize. */
    TartiniParams* aGl() { return m_aGl; }
  
        /** Starts thread searching in @param chunk,
        * whitch is pointer to array of floats of audio data. 
        * First copy it to channel obiect. */
    void searchIn(float *chunk);
    bool isBussy() { return m_isBussy; }
    
    int currentChunk() { return m_chunkNum; }
    void setCurrentChunk(int curCh) { m_chunkNum = curCh; }
    void incrementChunk() { m_chunkNum++; }
    void setIsVoice(bool voice);
        /** Changes default 44100 sample rate to given value. It takes effect only after resetFinder().
				 * @p range is TaudioParams::Erange cast. Default is e_middle
				 * Better don't call this during processing. */
    void setSampleRate(unsigned int sRate, int range = 1);
      /** Cleans all buffers, sets m_chunkNum to 0. */
    void resetFinder();
    void setAmbitus(qint16 loPitch, double topPitch) { 
          m_aGl->loPitch = loPitch; m_aGl->topPitch = topPitch; }
          /** Only notes with volume above this value are sending. 
           * If note has got such volume it is observed till its end - even below. */
    void setMinimalVolume(float vol) { m_minVolume = vol; }
    void setMinimalDuration(float dur) { m_minDuration = dur; }
    
signals:
      /** Signal emited when pitch is detected. 
      * @param pitch is float type of midi note.
      * @param freq if current frequency. */
  void found(float pitch, float freq);
			/** Pitch in chunk that just has been processed */
  void pichInChunk(float pitch);
  void volume(float volume);
	
protected:
	void run();
	
private:
      /** Checks was note detected but signal not sent
       * and emits found(m_prevPitch, m_prevFreq) */
  void emitFound();
  
  float           *m_filteredChunk, *m_workChunk, *m_prevChunk;
  double           m_prevPitch, m_prevFreq;

  bool            m_doReset;
	TartiniParams   *m_aGl; 
	Channel         *m_channel;
	int             m_chunkNum;
	bool            m_isBussy;
  bool            m_isVoice; // calculates average pitch in chunks range instead pitch in single chunk
  int             m_prevNoteIndex;
  int             m_noticedIndex;
  float           m_minVolume;
	float						m_minDuration;
	float						m_rateRatio; // multiplexer of the sample rate determined from pitch detection range
	
};

#endif // TPITCHFINDER_H
