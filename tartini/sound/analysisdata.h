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
 ***************************************************************************/

#ifndef ANALYSIS_DAYA_H
#define ANALYSIS_DAYA_H

//#include "useful.h"
#include <functional>
#include <vector>
#include "Filter.h"
#include "IIR_Filter.h"

#include "conversions.h"

//enum AmplitudeModes { AMPLITUDE_RMS, AMPLITUDE_MAX_INTENSITY, AMPLITUDE_CORRELATION, AMPLITUDE_PURITY, FREQ_CHANGENESS };
enum AmplitudeModes { AMPLITUDE_RMS, AMPLITUDE_MAX_INTENSITY, AMPLITUDE_CORRELATION, FREQ_CHANGENESS, DELTA_FREQ_CENTROID, NOTE_SCORE, NOTE_CHANGE_SCORE };

#define NUM_AMP_MODES 7
extern const char *amp_mode_names[NUM_AMP_MODES];
extern const char *amp_display_string[NUM_AMP_MODES];
extern double(*amp_mode_func[NUM_AMP_MODES])(double);
extern double(*amp_mode_inv_func[NUM_AMP_MODES])(double);

#define NO_NOTE -1

class AnalysisData
{
public: 
  float values[NUM_AMP_MODES];
  float period; /*< The period of the fundamental (in samples) */
  float fundamentalFreq; /*< The fundamental frequency in hertz */
  float pitch; /*< The pitch in semi-tones */
  float _freqCentroid;
  double pitchSum; /*< The sum of pitches so far in a note */
  double pitch2Sum; /*< The sum of pitches squared so far in a note */
  double shortTermMean; /*< The mean note so far */
  double shortTermDeviation; /*< The deviation of the note so far (based on standard deviation) */
  double longTermMean; /*> The mean note so far */
  double longTermDeviation; /*< The deviation of the note so far (based on standard deviation) */
  double spread;
  double spread2;
  float vibratoPitch;
  float vibratoWidth;
  float vibratoWidthAdjust;
  float vibratoSpeed;
  float vibratoPhase;
  float vibratoError;
  int reason; /*< The reason why there was a note change */
  //float correlation; /*< How well the fundamental frequency fits the signal (0=no fit, 1=perfet fit) */
  //float logrms; /*< The Root-mean-square, a measure of intensity/volume in the chunk */
  //float maxIntensityDB;
  int highestCorrelationIndex;
  int chosenCorrelationIndex;
  float periodRatio; /*< The ratio of the current period to the period at the beginning of the current note */

  int cepstrumIndex;
  float cepstrumPitch;
  //float periodOctaveEstimate; /*< A estimate from over the whole duration of the note, to help get the correct octave */
  //float volumeValue; /*< A value between 0 and 1 related to volume and correlation */
  //float changeness; /*< 0 for a stedy note, larger for a fast changing frequency */
  std::vector<float> periodEstimates;
  std::vector<float> periodEstimatesAmp;
  std::vector<float> harmonicAmpNoCutOff;
  std::vector<float> harmonicAmpRelative;
  std::vector<float> harmonicAmp;
  std::vector<float> harmonicFreq;
  std::vector<float> harmonicNoise;
  FilterState filterState; //the state of the filter at the beginning of the chunk
  int noteIndex; //The index of the note in the noteData, or NO_NOTE
  bool notePlaying;
  bool done;
  //bool isValid();
  AnalysisData();
  void calcScores();

  float &logrms() { return values[AMPLITUDE_RMS]; }
  float &maxIntensityDB() { return values[AMPLITUDE_MAX_INTENSITY]; }
  float &correlation() { return values[AMPLITUDE_CORRELATION]; }
  float &changeness() { return values[FREQ_CHANGENESS]; }
  float &freqCentroid() { return _freqCentroid; }
  float &deltaFreqCentroid() { return values[DELTA_FREQ_CENTROID]; }
  float volumeValue() { return (dB2Normalised(values[AMPLITUDE_RMS]) + values[AMPLITUDE_CORRELATION]-1.0f) * 0.2; }
  float &noteScore() { return values[NOTE_SCORE]; }
  float &noteChangeScore() { return values[NOTE_CHANGE_SCORE]; }
};

struct lessFundametalFreq : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.fundamentalFreq < y.fundamentalFreq; }
};

struct greaterFundametalFreq : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.fundamentalFreq > y.fundamentalFreq; }
};

struct lessPitch : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.pitch < y.pitch; }
};

struct greaterPitch : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.pitch > y.pitch; }
};

/*
struct lessCorrelation : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.correlation < y.correlation; }
};

struct greaterCorrelation : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.correlation > y.correlation; }
};

struct lessVolumeValue : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.volumeValue < y.volumeValue; }
};

struct greaterVolumeValue : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
	bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.volumeValue > y.volumeValue; }
};

struct lessLogRMS : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.logrms < y.logrms; }
};

struct greaterLogRMS : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.logrms > y.logrms; }
};

struct lessMaxIntensityDB : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.maxIntensityDB < y.maxIntensityDB; }
};

struct greaterMaxIntensityDB : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.maxIntensityDB > y.maxIntensityDB; }
};

struct lessChangeness : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.changeness < y.changeness; }
};

struct greaterChangeness : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.changeness > y.changeness; }
};
*/

struct lessValue : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  int v;
  lessValue(int v_) { v = v_; }
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.values[v] < y.values[v]; }
};

struct greaterValue : public std::binary_function<AnalysisData &, AnalysisData &, bool> {
  int v;
  greaterValue(int v_) { v = v_; }
  bool operator()(const AnalysisData &x, const AnalysisData &y) { return x.values[v] > y.values[v]; }
};

#endif
