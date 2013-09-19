/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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


#ifndef TAUDIOPARAMS_H
#define TAUDIOPARAMS_H

#include <QString>


  /** class describes audio input and output parameters. */
class TaudioParams 
{
		
public:
	
	/** Those are levels of pitch detection ranges.
		 * @p e_high - for violin, flute, piccolo - corresponds with treble clef.
		 * 							It starts form about F in small octave.
		 * @p e_middle - for guitars, celo and so - corresponds with treble droped and bass clefs
		 * 							It starts form about F in contra octave.
		 * @p e_low - for bass guitar and double bass - corresponds with bass dropped clef
		 * 							It is sufficient to detect lowest notes.
		 */
  enum Erange {
		e_high = 0,
		e_middle = 1,
		e_low = 2
  };
	
	
  bool useJACK; // TRUE - prefer JACK over ALSA or PulseAudio, FALSE - take PA (if available) or ALSA
// audio input settings
  bool INenabled; // is audio input enabled
  QString INdevName; // input device name
  float a440diff; // difference between standard a1 440Hz and user preferred base pitch
  float minimalVol; // only above this value detected note is sending to Nootka
  float minDuration; // minimal duration of a sound above which it is detected
    /** If true - pitch is average of all visible pitches
     * if false - the first detected in sound over noise is taken.  */
  bool isVoice;
	Erange range; // pitch detection range
	quint8 intonation; // accuracy of intonation in detected note - corresponds with Eaccuracy

// audio output settings
  bool OUTenabled; // audio output enabled
  QString OUTdevName; // output device name
  bool midiEnabled; // default false
  QString midiPortName; // default empty to find system default
  unsigned char midiInstrNr; // default 0 - grand piano
  int audioInstrNr; // corresponds with Einstrument enum numbers, default 1 classical guitar
};


#endif // TAUDIOPARAMS_H