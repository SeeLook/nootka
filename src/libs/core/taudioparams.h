/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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


#ifndef TAUDIOPARAMS_H
#define TAUDIOPARAMS_H

#include <nootkacoreglobal.h>
#include <QtCore/qstring.h>


/**
 * Class describes audio input and output parameters.
 */
class NOOTKACORE_EXPORT TaudioParams
{

public:

  bool    JACKorASIO; /**< forces to use JACK under Linux or ASIO under Windows (if they are available) */

  bool    INenabled; /**< is audio input enabled */
  QString INdevName; /**< input device name */
  qreal   a440diff; /**< difference between standard a1 440Hz and user preferred base pitch */
  int     midAfreq = 440; /**< Orchestral pitch - frequency of middle A  */
  int     transposition; /**< shift (interval) between score/note name and sound/instrument  */
  qreal   minimalVol; /**< only detected notes louder than this value are sending to Nootka */
  qreal   minDuration; /**< minimal duration of a sound above which it is detected */
  int     detectMethod; /**< pitch detection method (0 - MPM, 1 - auto-correlation, 2 - MPM modified cepstrum) */
  quint8  intonation; /**< accuracy of intonation in detected note - corresponds with @p Eaccuracy */
  bool    stoppedByUser; /**< Stores user action when he stopped sniffing himself @default FALSE */
  bool    audibleMetro; /**< Audible metronome ticking */
  bool    countBefore; /**< Before playing or sniffing play a measure of ticking */
  int     quantization; /**< Can be 6 or 12 (rhythmic duration of 16th or 8th), 4 (16th triplet is not yet supported) */

  bool    equalLoudness; /**< if TRUE - noise filters are performed - FALSE by default */
// duplex mode
  bool    forwardInput; /**< if true, all captured audio data is pushed to output */

// audio output settings
  bool    OUTenabled; /**< audio output enabled */
  QString OUTdevName; /**< output device name */
  bool    midiEnabled; /**< default false */
  QString midiPortName; /**< default empty to find system default */
  unsigned char midiInstrNr; /**< default 0 - grand piano */
  int     audioInstrNr; /**< corresponds with Einstrument enum numbers, default 1 classical guitar */

  QString dumpPath; /**< Path when captured PCM audio data will be dumped, if empty (default) - no dump performed */
};


#endif // TAUDIOPARAMS_H
