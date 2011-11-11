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
 ***************************************************************************/

#include "analysisdata.h"
#include "gdata.h"

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
  //volumeValue = 0.0f;
  highestCorrelationIndex = -1;
  chosenCorrelationIndex = -1;
  periodRatio = 1.0f;
  cepstrumIndex = -1;
  cepstrumPitch = 0.0f;
  //periodOctaveEstimate = -1.0f;
  noteIndex = NO_NOTE;
  notePlaying = false;
  done = false;
/*
  std::vector<float> periodEstimates;
  std::vector<float> periodEstimatesAmp;
  std::vector<float> harmonicAmpNoCutOff;
  std::vector<float> harmonicAmpRelative;
  std::vector<float> harmonicAmp;
  std::vector<float> harmonicFreq;
  std::vector<float> harmonicNoise;
  FilterState filterState; //the state of the filter at the beginning of the chunk
*/
}

/*
bool AnalysisData::isValid()
{
  if(rms > gdata->noiseThreshold()) return true;
  return false;
}
*/
void AnalysisData::calcScores()
{
  double a[NUM_AMP_MODES-2];
  int j;
  //double temp = 0.0;
  for(j=0; j<NUM_AMP_MODES-2; j++) {
    a[j] = bound(((*amp_mode_func[j])(values[j]) - (*amp_mode_func[j])(gdata->ampThreshold(j,0))) / ((*amp_mode_func[j])(gdata->ampThreshold(j,1)) - (*amp_mode_func[j])(gdata->ampThreshold(j,0))), 0.0, 1.0);
  }
/*
  bool aYes = false;
  bool aNo = false;
  for(j=0; j<NUM_AMP_MODES-1; j++) {
    if(a[j] == 0.0) aNo = true;
    if(a[j] == 1.0) aYes = true;
    temp += a[j] * gdata->ampWeight(j);
  }
  if(aNo == true) noteScore() = 0.0;
  else if(aYes == true) noteScore() = 1.0;
  else noteScore() = temp;
*/
  noteScore() = a[AMPLITUDE_RMS] * a[AMPLITUDE_CORRELATION];
  //noteChangeScore() = a[AMPLITUDE_CORRELATION] * a[FREQ_CHANGENESS] * a[DELTA_FREQ_CENTROID];
  //noteChangeScore() = ((1.0 - a[AMPLITUDE_CORRELATION]) + (1.0 - a[FREQ_CHANGENESS]) + (a[DELTA_FREQ_CENTROID])) / 3.0;
  noteChangeScore() = (1.0 - a[FREQ_CHANGENESS]);
}
