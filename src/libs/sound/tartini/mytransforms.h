/***************************************************************************
                          mytransforms.h  -  description
                             -------------------
    begin                : Sat Dec 11 2004
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

#ifndef MYTRANSFORMS_H
#define MYTRANSFORMS_H

#include <fftw3.h>
#include "analysisdata.h"
#include "fast_smooth.h"

class TartiniParams;
class Channel;
class Filter;

class MyTransforms
{

 public:
  bool equalLoudness;

  MyTransforms();
  virtual ~MyTransforms();

    /**
    * Initalises the parameters of a class instance. This must be called before use
    * @p windowSize The size of the data windows to be processed
    * @p padding The number of outputs wanted (autocorr size = windowSize + padding).
    * Set padding = 0, to get default windowSize / 2
    * @p rate The sampling rate of the incoming signal to process
    */
  void init(TartiniParams* tParams, int windowSize, int padding, double rate, int numHarmonics_ = 40);
  void uninit();
  double autocorr(float *input, float *output);
  double autoLogCorr(float *input, float *output);
  double asdf(float *input, float *output);
  double nsdf(float *input, float *output);
  static int findNSDFMaxima(float *input, int len, std::vector<int> &maxPositions);
  static int findNSDFsubMaximum(float *input, int len, float threshold);
  void calculateAnalysisData(int chunk, Channel *ch);
  void doChannelDataFFT(Channel *ch, float *curInput, int chunk);
  void calcHarmonicAmpPhase(float *harmonicAmp, float *harmonicPhase, int binsPerHarmonic);
  void doHarmonicAnalysis(float *input, AnalysisData &analysisData, double period);
  float get_fine_clarity_measure(double period);
  double get_max_note_change(float *input, double period);
  void applyHanningWindow(float *d);

private:
  int             m_windowSize; /**< size of audio data window (2048 by default) */
  int             m_corrPadd; /**< padding for autocorrelation */
  int             m_size; /**< @p m_windowSize + @p m_corrPadd */
  fftwf_plan      planDataTime2FFT, planDataFFT2Time;
  fftwf_plan      planAutocorrTime2FFT, planAutocorrFFT2Time;
  float          *dataTemp;
  float          *dataTime, *dataFFT;
  float          *autocorrTime, *autocorrFFT;
  float          *hanningCoeff, hanningScalar;
  float          *harmonicsAmpLeft, *harmonicsPhaseLeft;
  float          *harmonicsAmpCenter, *harmonicsPhaseCenter;
  float          *harmonicsAmpRight, *harmonicsPhaseRight;
  bool            m_beenInit;
  double          m_rate;
  int             m_numHarmonics;
  fast_smooth    *fastSmooth;
  TartiniParams  *m_params;
};

#endif
