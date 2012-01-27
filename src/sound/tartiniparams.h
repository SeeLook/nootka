/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#ifndef TARTINIPARAMS_H
#define TARTINIPARAMS_H

    /** Types of detection methods. */
enum EanalysisModes { e_MPM = 0, e_AUTOCORRELATION = 1, e_MPM_MODIFIED_CEPSTRUM = 2 };

class TartiniParams
{
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
  double topPitch; // The highest possible note pitch allowed (lowest possible is 0 in Tartini)
  qint16 loPitch; // The lowest possible note. Filtered in searchIn() method
  double ampThresholds[7][2];
  
};

#endif // TARTINIPARAMS_H
