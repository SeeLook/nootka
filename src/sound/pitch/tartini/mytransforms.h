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
	  tomaszbojczuk@gmail.com
	  Copyright (C) 2011
 ***************************************************************************/

#ifndef MYTRANSFORMS_H
#define MYTRANSFORMS_H

#include <fftw3.h>
#include "analysisdata.h"
#include "fast_smooth.h"

class Channel;
class Filter;

class MyTransforms
{
  int n, k, size; //n = size of data, k = size of padding for autocorrelation, size = n+k
  fftwf_plan planDataTime2FFT, planDataFFT2Time;
  fftwf_plan planAutocorrTime2FFT, planAutocorrFFT2Time;
  float *dataTemp;
  float *dataTime, *dataFFT;
  float *autocorrTime, *autocorrFFT; 
  float *hanningCoeff, hanningScalar;
  bool beenInit;
  double freqPerBin;
  double rate;
  fast_smooth *fastSmooth;
  
 public:
  bool equalLoudness;
  
  MyTransforms();
  virtual ~MyTransforms();

  void init(int n_, int k_, double rate_, bool equalLoudness_ = true);
  void uninit();
  double autocorr(float *input, float *output);
  double nsdf(float *input, float *output);
  static int findNSDFMaxima(float *input, int len, std::vector<int> &maxPositions);
  static int findNSDFsubMaximum(float *input, int len, float threshold);
  void calculateAnalysisData(int chunk, Channel *ch);
  void doChannelDataFFT(Channel *ch, float *curInput, int chunk);
  void applyHanningWindow(float *d);
  static double calcFreqCentroidFromLogMagnitudes(float *buffer, int len);
};
  
#endif
