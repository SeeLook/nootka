/***************************************************************************
                          analysisdata.cpp  -  description
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
	  tomaszbojczuk@gmail.com
	  Copyright (C) 2011
 ***************************************************************************/

#include "analysisdata.h"
#include "../tpitchfinder.h"
// #include "gdata.h"

extern TpitchFinder::audioSetts *glAsett;

const char *amp_mode_names[NUM_AMP_MODES] = { "RMS Amplitude (dB)", "Max Amplitude (dB)", "Amplitude Correlation", "Frequency Changness", "Delta Frequency Centroid", "Note Score", "Note Change Score" };
const char *amp_display_string[NUM_AMP_MODES] = { "RMS Amp Threshold = %0.2f, %0.2f", "Max Amp Threshold = %0.2f, %0.2f", "Amp Corr Threshold = %0.2f, %0.2f", "Freq Changeness Threshold = %0.2f, %0.2f", "Delta Freq Centroid Threshold = %0.2f, %0.2f", "Note Score Threshold = %0.2f, %0.2f", "Note Change Score Threshold = %0.2f, %0.2f" };
double(*amp_mode_func[NUM_AMP_MODES])(double) = { &dB2Normalised, &dB2Normalised, &same, &oneMinus, &same, &same, &same };
double(*amp_mode_inv_func[NUM_AMP_MODES])(double) = { &normalised2dB, &normalised2dB, &same, &oneMinus, &same, &same, &same };

AnalysisData::AnalysisData()
{
  std::fill(values, values+NUM_AMP_MODES, 0.0f);
  period = 0.0f;
  fundamentalFreq = 0.0f;
  pitch = 0.0f;
  _freqCentroid = 0.0f;
  pitchSum = 0.0f;
  pitch2Sum = 0.0;
  shortTermMean = 0.0;
  shortTermDeviation = 0.0;
  longTermMean = 0.0;
  longTermDeviation = 0.0;
  spread = spread2 = 0.0;
  vibratoPitch = 0.0f;
  vibratoWidth = 0.0f;
  vibratoSpeed = 0.0f;
  vibratoWidthAdjust = 0.0f;
  vibratoPhase = 0.0f;
  vibratoError = 0.0f;
  reason = 0;
  highestCorrelationIndex = -1;
  chosenCorrelationIndex = -1;
  periodRatio = 1.0f;
  cepstrumIndex = -1;
  cepstrumPitch = 0.0f;
  noteIndex = NO_NOTE;
  notePlaying = false;
  done = false;

}


void AnalysisData::calcScores()
{
  double a[NUM_AMP_MODES-2];
  int j;
  for(j=0; j<NUM_AMP_MODES-2; j++) {
//     a[j] = bound(((*amp_mode_func[j])(values[j]) - 
// 	  (*amp_mode_func[j])(gdata->ampThreshold(j,0))) / ((*amp_mode_func[j])(gdata->ampThreshold(j,1)) - 
// 	  (*amp_mode_func[j])(gdata->ampThreshold(j,0))), 0.0, 1.0);
	a[j] = bound(((*amp_mode_func[j])(values[j]) - 
	  (*amp_mode_func[j])(glAsett->ampThresholds[j][0])) / ((*amp_mode_func[j])(glAsett->ampThresholds[j][1]) - 
	  (*amp_mode_func[j])(glAsett->ampThresholds[j][0])), 0.0, 1.0);
  }
  noteScore() = a[AMPLITUDE_RMS] * a[AMPLITUDE_CORRELATION];
  noteChangeScore() = (1.0 - a[FREQ_CHANGENESS]);
}
