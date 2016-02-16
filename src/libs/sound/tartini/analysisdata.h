/***************************************************************************
                          analysisdata.h  -  description
                             -------------------
    begin                : Fri Dec 17 2004
    copyright            : (C) 2004 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.   
   
   Adjusted to Nootka by Tomasz Bojczuk
	  seelook@gmail.com
	  Copyright (C) 2011-2016
 ***************************************************************************/

#ifndef ANALYSIS_DATA_H
#define ANALYSIS_DATA_H


#include "nootkasoundglobal.h"
#include <functional>
#include <vector>
#include "filters/Filter.h"
#include "filters/IIR_Filter.h"

#include "conversions.h"


class TartiniParams;


enum AmplitudeModes { AMPLITUDE_RMS, AMPLITUDE_MAX_INTENSITY, AMPLITUDE_CORRELATION, FREQ_CHANGENESS, DELTA_FREQ_CENTROID, NOTE_SCORE, NOTE_CHANGE_SCORE };

#define NUM_AMP_MODES 7
extern double(*amp_mode_func[NUM_AMP_MODES])(double);
extern double(*amp_mode_inv_func[NUM_AMP_MODES])(double);

#define NO_NOTE -1


class NOOTKASOUND_EXPORT AnalysisData
{
public:
  float values[NUM_AMP_MODES];
  float period; /**< The period of the fundamental (in samples) */
  float fundamentalFreq; /**< The fundamental frequency in hertz */
  float pitch; /**< The pitch in semi-tones */
  double pitchSum; /**< The sum of pitches so far in a note */
  double pitch2Sum; /**< The sum of pitches squared so far in a note */
  double shortTermMean; /**< The mean note so far */
  double shortTermDeviation; /**< The deviation of the note so far (based on standard deviation) */
  double longTermMean; /**< The mean note so far */
  double longTermDeviation; /**< The deviation of the note so far (based on standard deviation) */
  double spread;
  double spread2;

  int reason; /**< The reason why there was a note change */
  int highestCorrelationIndex;
  int chosenCorrelationIndex;
  float periodRatio; /**< The ratio of the current period to the period at the beginning of the current note */
  float pcmVolume; /** max amplitude of raw PCM data in a chunk. */

  int cepstrumIndex;
  float cepstrumPitch;

  std::vector<float> periodEstimates;
  std::vector<float> periodEstimatesAmp;
  std::vector<float> harmonicAmpNoCutOff;
  std::vector<float> harmonicAmpRelative;
  std::vector<float> harmonicAmp;
  std::vector<float> harmonicFreq;
  std::vector<float> harmonicNoise;
  FilterState filterState; /**< the state of the filter at the beginning of the chunk */
  int noteIndex; /**< The index of the note in the noteData, or NO_NOTE */
  bool notePlaying;
  bool done;
  AnalysisData();
  void calcScores(TartiniParams *tParams);

  double normalVolume() { return qBound(0.0, 1.0 - (logrms() / -150.0), 1.0); } /**< Normalized volume of a chunk */
  float &logrms() { return values[AMPLITUDE_RMS]; }
  float &maxIntensityDB() { return values[AMPLITUDE_MAX_INTENSITY]; }
  float &correlation() { return values[AMPLITUDE_CORRELATION]; }
  float &changeness() { return values[FREQ_CHANGENESS]; }
  float volumeValue() { return (dB2Normalised(values[AMPLITUDE_RMS]) + values[AMPLITUDE_CORRELATION]-1.0f) * 0.2; }
  float &noteScore() { return values[NOTE_SCORE]; }
  float &noteChangeScore() { return values[NOTE_CHANGE_SCORE]; }
};

#endif
