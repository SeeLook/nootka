/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TNOTESTRUCT_H
#define TNOTESTRUCT_H


#include <nootkacoreglobal.h>
#include "tnote.h"

/** 
 * Structure that stores pitch and its params as such as frequency and duration in [s].  
 */
class NOOTKACORE_EXPORT TnoteStruct
{

public:
	TnoteStruct(const Tnote& p, qreal pF, qreal f = 0.0, qreal dur = 0.0) : pitch(p), pitchF(pF), freq(f), duration(dur) {}

      /** Single param of int type is required  - usually note index in a channel. */
	TnoteStruct(int _index, float _pitchF = 0, double _vol = 0, double _dur = 0) :
    index(_index),
    pitchF(_pitchF), basePitch(qRound(_pitchF)),
    volume(_vol), duration(_dur)
  {}
	
	TnoteStruct() {	freq = 0; duration = 0; pitchF = 0;	} /** Default constructor  */

			/** Checks is float value of a note pitch different than its root pitch in range of given threshold.
			 * Returns @p TRUE if @p pitchF value is into threshold range or @p FALSE when not
			 * This way intonation accuracy is checked */
	bool inTune(float threshold) {
		if (qAbs(pitchF - (float)qRound(pitchF)) >= threshold)
			return false;
		else
			return true;
	}
	
	void set(qreal midiPitch = 0.0, qreal f = 0.0, qreal dur = 0.0) {
		midiPitch ? pitch = Tnote(qRound(midiPitch) - 47) : pitch = Tnote(); pitchF = midiPitch; freq = f; duration = dur;
	} /** Sets values of note structure, or sets them to NULL if no values defined. */
	
	int     index;            /** Note index in entire channel */
	Tnote   pitch; 		        /** Note pitch like C, D, E or C3, D#  */
	qreal   pitchF; 	        /** Chromatic note number in MIDI scale, C1 = 60 */
	int     basePitch;        /** Midi value (rounded to integer) of a pitch */
	qreal   freq; 		        /** Frequency of a note */
	qreal   duration;         /** Duration of a note */
	double  volume;           /** volume of a note*/
	float   signalStrenght;   /** Max amplitude of PCM data in chunk  */
};


#endif // TNOTESTRUCT_H
