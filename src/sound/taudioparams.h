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


#ifndef TAUDIOPARAMS_H
#define TAUDIOPARAMS_H

#include <QString>


  /** class describes audio input parameters. */
class TaudioParams {
  
public:
// audio input settings
  bool INenabled; // is audio input enabled
  QString INdevName; // input device name
  float a440diff; // difference betwen standard a1 440Hz and user prefered base pitch
  qint16 noiseLevel; // ?????
    /** If true - pitch is average of all visible pitches
     * if false - the first detected in sound over noise is taken.  */
  bool isVoice; 

// audio output settings
  bool OUTenabled; // audio output enabled
  QString OUTdevName;
  bool midiEnabled; // default false
  QString midiPortName; // default empty to find system default
  unsigned char midiInstrNr; // default 0 - grand piano
};


#endif // TAUDIOPARAMS_H