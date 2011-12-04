/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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



#include <QThread>
#include "tartini/mytransforms.h"
#include "saudioinparams.h"


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

// QT_FORWARD_DECLARE_CLASS(MyTransforms)


/** The main purpose of this class is to recognize pitch
 * of aduio data flowing throught it. 
 * Finding pitch method(s) are taken from Tartini project
 * writen by Philip McLeod.
 */
class TpitchFinder : public QThread
{
	Q_OBJECT
	
public:
    explicit TpitchFinder(QObject *parent = 0);
    virtual ~TpitchFinder();
	
	MyTransforms myTransforms;
	  
	  /** Audio input & pitch recognition settings. */
	struct audioSetts {
	  quint32 rate;
	  quint8 chanells;
	  quint32 windowSize;
	  quint32 framesPerChunk; // in mono signal frames are the same as samples
	  double dBFloor;
	  bool equalLoudness;
	  bool doingFreqAnalysis;
	  bool doingAutoNoiseFloor;
	  bool doingHarmonicAnalysis;
	  bool firstTimeThrough;
	  bool doingDetailedPitch;
	  int threshold; // threshold of lowest loudness in [dB]
	  EanalysisModes analysisType;
	  double topPitch; // The highest possible note pitch allowed (lowest possible is 0)
	  double ampThresholds[7][2];
	  bool isVoice; // calculates average pitch in chunks range instead pitch in single chunk
	};
	  /** global settings for pitch recognize. */
	audioSetts aGl() { return m_aGl; }
	
	  /** Starts thread searching in @param chunk,
	   * whitch is pointer to array of floats of audio data. 
	   * First copy it to channel obiect. */
	void searchIn(float *chunk);
	bool isBussy() { return m_isBussy; }
	
	int currentChunk() { return m_chunkNum; }
	void setCurrentChunk(int curCh) { m_chunkNum = curCh; }
	void incrementChunk() { m_chunkNum++; }
	void setParameters(SaudioInParams &params);
	
signals:
	/** Signal emited when pitch is detected. 
	 * @param pitch is float type of midi note. */
  void pitchFound(float pitch);
  void fundamentalFreq(float freq);
  void noteStoped();
	
protected:
	void run();
	
private:
	audioSetts m_aGl;
	Channel *m_channel;
	int m_chunkNum;
	float *m_workChunk;
	bool m_isBussy;
	
};

#endif // TPITCHFINDER_H
