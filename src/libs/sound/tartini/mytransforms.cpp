/***************************************************************************
                        mytransforms.cpp  -  description
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

#include "mytransforms.h"
#include "array1d.h"
#include "../tartiniparams.h"
#include "bspline.h"
#include "channel.h"
#include "filters/Filter.h"
#include "conversions.h"

#include "useful.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <float.h>

#include <complex>

MyTransforms::MyTransforms()
{
  m_beenInit = false;
}


MyTransforms::~MyTransforms()
{
  uninit();
}


void MyTransforms::init(TartiniParams* tParams, int windowSize, int padding, double rate, int numHarmonics_)
{
  m_params = tParams;
  const int myFFTMode = FFTW_ESTIMATE;
  uninit();
  if(padding == 0)
      padding = (windowSize + 1) / 2;

  m_windowSize = windowSize;
  m_corrPadd = padding;
  m_size = m_windowSize + m_corrPadd;
  m_rate = rate;
  m_numHarmonics = numHarmonics_;

  dataTemp = (float*)fftwf_malloc(sizeof(float) * m_windowSize);
  dataTime = (float*)fftwf_malloc(sizeof(float) * m_windowSize);
  dataFFT = (float*)fftwf_malloc(sizeof(float) * m_windowSize);
  autocorrTime = (float*)fftwf_malloc(sizeof(float) * m_size);
  autocorrFFT = (float*)fftwf_malloc(sizeof(float) * m_size);
  hanningCoeff = (float*)fftwf_malloc(sizeof(float) * m_windowSize);

  planAutocorrTime2FFT = fftwf_plan_r2r_1d(m_size, autocorrTime, autocorrFFT, FFTW_R2HC, myFFTMode);
  planAutocorrFFT2Time = fftwf_plan_r2r_1d(m_size, autocorrFFT, autocorrTime, FFTW_HC2R, myFFTMode);
  
  planDataTime2FFT = fftwf_plan_r2r_1d(m_windowSize, dataTime, dataFFT, FFTW_R2HC, myFFTMode);
  planDataFFT2Time = fftwf_plan_r2r_1d(m_windowSize, dataFFT, dataTime, FFTW_HC2R, myFFTMode); //???

  harmonicsAmpLeft = (float*)fftwf_malloc(m_numHarmonics * sizeof(float));
  harmonicsPhaseLeft = (float*)fftwf_malloc(m_numHarmonics * sizeof(float));
  harmonicsAmpCenter = (float*)fftwf_malloc(m_numHarmonics * sizeof(float));
  harmonicsPhaseCenter = (float*)fftwf_malloc(m_numHarmonics * sizeof(float));
  harmonicsAmpRight = (float*)fftwf_malloc(m_numHarmonics * sizeof(float));
  harmonicsPhaseRight = (float*)fftwf_malloc(m_numHarmonics * sizeof(float));

  hanningScalar = 0;
  for(int j=0; j<m_windowSize; j++) {
    hanningCoeff[j] = (1.0 - cos(double(j+1) / double(m_windowSize+1) * twoPI)) / 2.0;
    hanningScalar += hanningCoeff[j];
  }
  hanningScalar /= 2; // to normalize the FFT coefficients we divide by the sum of the hanning window / 2

  fastSmooth = new fast_smooth(m_windowSize/8);
  m_beenInit = true;
}

void MyTransforms::uninit()
{
  if(m_beenInit) {
    fftwf_free(harmonicsAmpLeft);
    fftwf_free(harmonicsPhaseLeft);
    fftwf_free(harmonicsAmpCenter);
    fftwf_free(harmonicsPhaseCenter);
    fftwf_free(harmonicsAmpRight);
    fftwf_free(harmonicsPhaseRight);
    fftwf_destroy_plan(planDataFFT2Time);
    fftwf_destroy_plan(planDataTime2FFT);
    fftwf_destroy_plan(planAutocorrFFT2Time);
    fftwf_destroy_plan(planAutocorrTime2FFT);
    fftwf_free(autocorrFFT);
    fftwf_free(autocorrTime);
    fftwf_free(dataFFT);
    fftwf_free(dataTime);
    fftwf_free(dataTemp);
    delete fastSmooth;
    m_beenInit = false;
  }
}

/** Performs an autocorrelation on the input
  @param input An array of length n, in which the autocorrelation is taken
  @param ouput This should be an array of length k.
               This is the correlation of the signal with itself
               for delays 1 to k (stored in elements 0 to k-1)
  @return The sum of squares of the input. (ie the zero delay correlation)
  Note: Use the init function to set values of n and k before calling this.
*/
double MyTransforms::autocorr(float *input, float *output)
{
  float fsize = float(m_size);
  
  //pack the data into an array which is zero padded by k elements
  std::copy(input, input+m_windowSize, autocorrTime);
  std::fill(autocorrTime+m_windowSize, autocorrTime+m_size, 0.0f);

  //Do a forward FFT
  fftwf_execute(planAutocorrTime2FFT);

  //calculate the (real*real + ima*imag) for each coefficient
  //Note: The numbers are packed in half_complex form (refer fftw)
  //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
  for(int j=1; j<m_size/2; j++) {
    autocorrFFT[j] = sq(autocorrFFT[j]) + sq(autocorrFFT[m_size-j]);
    autocorrFFT[m_size-j] = 0.0f;
  }
  autocorrFFT[0] = sq(autocorrFFT[0]);
  autocorrFFT[m_size/2] = sq(autocorrFFT[m_size/2]);

  //Do an inverse FFT
  fftwf_execute(planAutocorrFFT2Time);

  //extract the wanted elements out, and normalise
  for(float *p1=output, *p2=autocorrTime+1; p1<output+m_corrPadd;)
    *p1++ = *p2++ / fsize;
    
  return double(autocorrTime[0]) / double(m_size);
}

double MyTransforms::autoLogCorr(float *input, float *output)
{
  float fsize = float(m_size);
  
  //pack the data into an array which is zero padded by k elements
  std::copy(input, input+m_windowSize, autocorrTime);
  std::fill(autocorrTime+m_windowSize, autocorrTime+m_size, 0.0f);

  //Do a forward FFT
  fftwf_execute(planAutocorrTime2FFT);

  //calculate the (real*real + ima*imag) for each coefficient
  //Note: The numbers are packed in half_complex form (refer fftw)
  //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
  for(int j=1; j<m_size/2; j++) {
    autocorrFFT[j] = (sq(autocorrFFT[j]) + sq(autocorrFFT[m_size-j]));
    autocorrFFT[m_size-j] = 0.0f;
  }
  autocorrFFT[0] = (sq(autocorrFFT[0]));
  autocorrFFT[m_size/2] = (sq(autocorrFFT[m_size/2]));

  //Do an inverse FFT
  fftwf_execute(planAutocorrFFT2Time);

  //extract the wanted elements out, and normalise
  for(float *p1=output, *p2=autocorrTime+1; p1<output+m_corrPadd;)
    *p1++ = *p2++ / fsize;
    
  return double(autocorrTime[0]) / double(m_size);
}

/** The Average Square Difference Function.
    @param input. An array of length n, in which the ASDF is taken
    @param ouput. This should be an array of length k
*/
double MyTransforms::asdf(float *input, float *output)
{
  double sumSq = autocorr(input, output); //Do an autocorrelation and return the sum of squares of the input
  double sumRightSq = sumSq, sumLeftSq = sumSq;
  for(int j=0; j<m_corrPadd; j++) {
    sumLeftSq  -= sq(input[m_windowSize-1-j]);
    sumRightSq -= sq(input[j]);
    output[j] = sumLeftSq + sumRightSq - 2*output[j];
  }
  return sumSq;
}


/** The Normalised Square Difference Function.
    @param input. An array of length n, in which the ASDF is taken
    @param ouput. This should be an array of length k
    @return The sum of square
*/
double MyTransforms::nsdf(float *input, float *output)
{
  double sumSq = autocorr(input, output); //the sum of squares of the input
  double totalSumSq = sumSq * 2.0;
    if(m_params->analysisType == e_MPM || m_params->analysisType == e_MPM_MODIFIED_CEPSTRUM) { //nsdf
    for(int j=0; j<m_corrPadd; j++) {
      totalSumSq  -= sq(input[m_windowSize-1-j]) + sq(input[j]);
      //dividing by zero is very slow, so deal with it seperately
      if(totalSumSq > 0.0) output[j] *= 2.0 / totalSumSq;
      else output[j] = 0.0;
    }
  } else { //autocorr
    for(int j=0; j<m_corrPadd; j++) {
      //dividing by zero is very slow, so deal with it seperately
      if(totalSumSq > 0.0) output[j] /= sumSq;
      else output[j] = 0.0;
    }
  }
  return sumSq;
}


/**
    Find the highest maxima between each pair of positive zero crossings.
    Including the highest maxima between the last +ve zero crossing and the end if any.
    Ignoring the first (which is at zero)
    In this diagram the disired maxima are marked with a *

                  *             *
    \      *     /\      *     /\
    _\____/\____/__\/\__/\____/__
      \  /  \  /      \/  \  /
       \/    \/            \/

  @param input The array to look for maxima in
  @param len Then length of the input array
  @param maxPositions The resulting maxima positions are pushed back to this vector
  @return The index of the overall maximum
*/
int MyTransforms::findNSDFMaxima(float *input, int len, std::vector<int> &maxPositions)
{
  int pos = 0;
  int curMaxPos = 0;
  int overallMaxIndex = 0;

  while(pos < (len-1)/3 && input[pos] > 0.0f) pos++; //find the first negitive zero crossing
  while(pos < len-1 && input[pos] <= 0.0f) pos++; //loop over all the values below zero
  if(pos == 0) pos = 1; // can happen if output[0] is NAN
  
  while(pos < len-1) {
    if(input[pos] > input[pos-1] && input[pos] >= input[pos+1]) { //a local maxima
      if(curMaxPos == 0) curMaxPos = pos; //the first maxima (between zero crossings)
      else if(input[pos] > input[curMaxPos]) curMaxPos = pos; //a higher maxima (between the zero crossings)
    }
    pos++;
    if(pos < len-1 && input[pos] <= 0.0f) { //a negative zero crossing
      if(curMaxPos > 0) { //if there was a maximum
        maxPositions.push_back(curMaxPos); //add it to the vector of maxima
        if(overallMaxIndex == 0) overallMaxIndex = curMaxPos;
        else if(input[curMaxPos] > input[overallMaxIndex]) overallMaxIndex = curMaxPos;
        curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
      }
      while(pos < len-1 && input[pos] <= 0.0f) pos++; //loop over all the values below zero
    }
  }

  if(curMaxPos > 0) { //if there was a maximum in the last part
    maxPositions.push_back(curMaxPos); //add it to the vector of maxima
    if(overallMaxIndex == 0) overallMaxIndex = curMaxPos;
    else if(input[curMaxPos] > input[overallMaxIndex]) overallMaxIndex = curMaxPos;
    curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
  }
  return overallMaxIndex;
}


/** @return The index of the first sub maximum.
  This is now scaled from (threshold * overallMax) to 0.
*/
int MyTransforms::findNSDFsubMaximum(float *input, int len, float threshold)
{
  std::vector<int> indices;
  int overallMaxIndex = findNSDFMaxima(input, len, indices);
  threshold += (1.0 - threshold) * (1.0 - input[overallMaxIndex]);
  float cutoff = input[overallMaxIndex] * threshold;
  for(uint j=0; j<indices.size(); j++) {
    if(input[indices[j]] >= cutoff) return indices[j];
  }
  return 0; //stop the compiler warning
}


/** Estimate the period or the fundamental frequency.
  Finds the first maximum of NSDF which past the first
  positive zero crossing and is over the threshold.
  If no maxima are over the threshold then the the highest maximum is returned.
  If no positive zero crossing is found, zero is returned.
  @param input. An array of length n.
  @param threshold. A number between 0 and 1 at which maxima above are accepted.
  @return The estimated period (in samples), or zero if none found.
*/
void MyTransforms::calculateAnalysisData(int chunk, Channel *ch)
{
  AnalysisData &analysisData = *ch->dataAtChunk(chunk);
  AnalysisData *prevAnalysisData = ch->dataAtChunk(chunk-1);
  float *output = ch->nsdfData.begin();
  float *curInput = ch->directInput.begin();

  std::vector<int> nsdfMaxPositions;
  
  analysisData.maxIntensityDB() = linear2dB(fabs(*std::max_element(curInput, curInput+m_windowSize, absoluteLess<float>())));
	
  doChannelDataFFT(ch, curInput, chunk);
  std::copy(curInput, curInput+m_windowSize, dataTime);
  if(m_params->doingFreqAnalysis && (ch->firstTimeThrough() || m_params->doingFreqAnalysis)) {
    //calculate the Normalised Square Difference Function
    double logrms = linear2dB(nsdf(dataTime, ch->nsdfData.begin()) / double(m_windowSize)); /**< Do the NSDF calculation */
    analysisData.logrms() = logrms;
	if(m_params->doingAutoNoiseFloor && !analysisData.done) {
      //do it for gdata. this is only here for old code. remove some stage
	  if(chunk == 0) {
      m_params->ampThresholds[AMPLITUDE_RMS][0] = 0.0;
      m_params->ampThresholds[AMPLITUDE_RMS][1] = m_params->dBFloor;
	  }
	  if(logrms+15 < m_params->ampThresholds[AMPLITUDE_RMS][0]) 
		  m_params->ampThresholds[AMPLITUDE_RMS][0] = logrms+15;
	  if(logrms > m_params->ampThresholds[AMPLITUDE_RMS][1]) 
		  m_params->ampThresholds[AMPLITUDE_RMS][1] = logrms;
      //do it for the channel
      if(chunk == 0) { 
		ch->rmsFloor = 0.0;
		ch->rmsCeiling = m_params->dBFloor; // gdata->dBFloor();
	  }
      if(logrms+15 < ch->rmsFloor) ch->rmsFloor = logrms+15;
      if(logrms > ch->rmsCeiling) ch->rmsCeiling = logrms;
    }

    findNSDFMaxima(ch->nsdfData.begin(), m_corrPadd, nsdfMaxPositions);
    if(!analysisData.done) {
    }

    //store some of the best period estimates
    analysisData.periodEstimates.clear();
    analysisData.periodEstimatesAmp.clear();
    float smallCutoff = 0.4f;
    for(std::vector<int>::iterator iter = nsdfMaxPositions.begin(); iter < nsdfMaxPositions.end(); iter++) {
      if(output[*iter] >= smallCutoff) {
        float x, y;
        //do a parabola fit to find the maximum
        parabolaTurningPoint2(output[*iter-1], output[*iter], output[*iter+1], float(*iter + 1), &x, &y);
        y = bound(y, -1.0f, 1.0f);
        analysisData.periodEstimates.push_back(x);
        analysisData.periodEstimatesAmp.push_back(y);
      }
    }
    
    float periodDiff = 0.0f;
    if(analysisData.periodEstimates.empty()) { //no period found
      analysisData.calcScores(m_params);
      analysisData.done = true;
      //goto finished; //return;
    } else {
      //calc the periodDiff
      if(chunk > 0 && prevAnalysisData->highestCorrelationIndex != -1) {
        float prevPeriod = prevAnalysisData->periodEstimates[prevAnalysisData->highestCorrelationIndex];
        std::vector<float>::iterator closestIter = binary_search_closest(analysisData.periodEstimates.begin(), analysisData.periodEstimates.end(), prevPeriod);
        periodDiff = *closestIter - prevPeriod;
        if(absolute(periodDiff) > 8.0f) periodDiff = 0.0f;
      }

      int nsdfMaxIndex = int(std::max_element(analysisData.periodEstimatesAmp.begin(), analysisData.periodEstimatesAmp.end()) - analysisData.periodEstimatesAmp.begin());
      analysisData.highestCorrelationIndex = nsdfMaxIndex;

      if(!analysisData.done) {
        if(m_params->analysisType == e_MPM_MODIFIED_CEPSTRUM) {
            ch->chooseCorrelationIndex(chunk, float(analysisData.cepstrumIndex)); //calculate pitch
        } else {
          if(ch->isNotePlaying() && chunk > 0) {
              ch->chooseCorrelationIndex(chunk, ch->periodOctaveEstimate(chunk-1)); //calculate pitch
          } else {
              ch->chooseCorrelationIndex1(chunk); //calculate pitch
          }
        }
        ch->calcDeviation(chunk);
      }

      analysisData.changeness() = 0.0f;
	  if(m_params->doingHarmonicAnalysis) {
        std::copy(dataTime, dataTime + m_windowSize, dataTemp);
        if(analysisData.chosenCorrelationIndex >= 0)
          doHarmonicAnalysis(dataTemp, analysisData, analysisData.periodEstimates[analysisData.chosenCorrelationIndex]/*period*/);
      }
    }

    if(m_params->doingFreqAnalysis && ch->doingDetailedPitch() && ch->firstTimeThrough()) {
      float periodDiff2 = ch->calcDetailedPitch(curInput, analysisData.period, chunk);
      periodDiff = periodDiff2;
    }

    if(!analysisData.done) {
      analysisData.calcScores(m_params);
      ch->processNoteDecisions(chunk, periodDiff);
      analysisData.done = true;
    }
  }

  if(!analysisData.done) {
    int j;
    //calc rms by hand
    double rms = 0.0;
    for(j=0; j<m_windowSize; j++) {
      rms += sq(dataTime[j]);
    }
    analysisData.logrms() = linear2dB(rms / float(m_windowSize));
    analysisData.calcScores(m_params);
    analysisData.done = true;
  }

}


/* using the given fractional period, constructs a smaller subwindow
which slide through the window. The smallest clarity measure of this
moving sub window is returned.*/
float MyTransforms::get_fine_clarity_measure(double period)
{
    int tempN = m_windowSize - int(ceil(period));
    float *tempData = new float[tempN];
    float bigSum=0, corrSum=0, matchVal, matchMin=1.0;
    //create some data points at the fractional period delay into tempData
    stretch_array(m_windowSize, dataTime, tempN, tempData, period, tempN, LINEAR);
    int dN = int(floor(period)); //tempN / 4;
    for(int j=0; j<dN; j++) {
      bigSum += sq(dataTime[j]) + sq(tempData[j]);
      corrSum += dataTime[j] * tempData[j];
    }
    matchMin = (2.0*corrSum) / bigSum;
    for(int j=0; j<tempN - dN; j++) {
      bigSum -= sq(dataTime[j]) + sq(tempData[j]);
      corrSum -= dataTime[j] * tempData[j];
      bigSum += sq(dataTime[j+dN]) + sq(tempData[j+dN]);
      corrSum += dataTime[j+dN] * tempData[j+dN];
      matchVal = (2.0*corrSum) / bigSum;
      if(matchVal < matchMin) matchMin = matchVal;
    }
    delete[] tempData;

  return matchMin;
}

double MyTransforms::get_max_note_change(float *input, double period)
{
  int i, j, j2;
  int max_subwindow = m_windowSize / 4;
  int subwindow_size;
  if(period < 1.0) return 0.0; //period too small
  if(period > m_windowSize/2) { printf("period = %lf\n", period); return 0.0; }
  int iPeriod = int(floor(period));
  if(period < double(max_subwindow)) subwindow_size = int(floor(period * (double(max_subwindow) / period)));
  else subwindow_size = int(floor(period));
  int num = m_windowSize-subwindow_size-iPeriod;

  std::vector<int> offsets;
  for(j=-4; j<=4; j++) offsets.push_back(j); //do a total of 9 subwindows at once. 4 either side.
  int ln = offsets.size();

  std::vector<float> left(ln);
  std::vector<float> right(ln);
  std::vector<float> left_pow(ln);
  std::vector<float> right_pow(ln);
  std::vector<float> pow(ln);
  std::vector<float> err(ln);
  std::vector<float> result(ln);
  std::vector<float> unsmoothed(num);
  std::vector<float> smoothed(num);
  std::vector<float> smoothed_diff(num);
  //calc the values of pow and err for the first in each row.
  for(i=0; i<ln; i++) {
    left_pow[i] = right_pow[i] = pow[i] = err[i] = 0;
    for(j=0, j2=iPeriod+offsets[i]; j<subwindow_size-offsets[i]; j++, j2++) {
      left_pow[i] += sq(input[j]);
      right_pow[i] += sq(input[j2]);
      err[i] += sq(input[j] - input[j2]);
    }
  }
 
  //TODO: speed up this for loop
  for(j=0; j<num-1; j++) {
    for(i=0; i<ln; i++) {
      pow[i] = left_pow[i] + right_pow[i];
      result[i] = 1.0 - (err[i] / pow[i]);

      err[i] += -sq(input[j] - input[j+iPeriod+offsets[i]]) + sq(input[j+subwindow_size-offsets[i]] - input[j+subwindow_size+iPeriod]);
      left_pow[i] += -sq(input[j]) + sq(input[j+subwindow_size-offsets[i]]);
      right_pow[i] += -sq(input[j+iPeriod+offsets[i]]) + sq(input[j+iPeriod+subwindow_size]);
    }

    int pos = int(std::max_element(result.begin(), result.begin()+ln) - result.begin());
    if(pos > 0 && pos < ln-1)
      unsmoothed[j] = double(iPeriod + offsets[pos]) + parabolaTurningPoint(result[pos-1], result[pos], result[pos+1]);
    else
      unsmoothed[j] = double(iPeriod + offsets[pos]);
  }
  fastSmooth->fast_smoothB(&(unsmoothed[0]), &(smoothed[0]), num-1);
  int max_pos = 0;
  for(j=0; j<num-2; j++) {
    smoothed_diff[j] = fabs(smoothed[j+1] - smoothed[j]);
    if(smoothed_diff[j] > smoothed_diff[max_pos]) max_pos = j;
  }
  double ret = smoothed_diff[max_pos] / period * double(m_rate) * double(subwindow_size) / 10000.0;
  return ret;
}


/** Find the index of the first maxima above the threshold times the overall maximum.
  @param threshold A value between 0.0 and 1.0
  @return The index of the first subMaxima
*/
int findFirstSubMaximum(float *data, int length, float threshold)
{
  float maxValue = *std::max_element(data, data+length);
  float cutoffValue = maxValue * threshold;
  for(int j=0; j < length; j++) {
    if(data[j] >= cutoffValue) return j;
  }
  return length;
}

/** Given cepstrum input finds the first maxima above the threshold
    @param threshold The fraction of the 'overall maximum value' the 'cepstrum maximum' must be over.
                     A value between 0.0 and 1.0.
*/
int findCepstrumMaximum(float *data, int length, float threshold)
{
  int pos = 0;
  //loop until the first negative value in data
  while(pos < length-1 && data[pos] > 0.0f) pos++;
  return pos + findFirstSubMaximum(data+pos, length-pos, threshold);
}


/** Do a Windowed FFT for use in the Active FFT data window
*/
void MyTransforms::doChannelDataFFT(Channel *ch, float *curInput, int chunk)
{
  std::copy(curInput, curInput+m_windowSize, dataTime);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  int nDiv2 = m_windowSize/2;
  double logSize = log10(double(ch->fftData1.size())); //0.0
  //Adjust the coefficents, both real and imaginary part by same amount
  double sqValue;
  const double logBase = 100.0;
  for(int j=1; j<nDiv2; j++) {
    sqValue = sq(dataFFT[j]) + sq(dataFFT[m_windowSize-j]);
    ch->fftData2[j] = logBaseN(logBase, 1.0 + 2.0*sqrt(sqValue) / double(nDiv2) * (logBase-1.0));
    if(sqValue > 0.0)
			ch->fftData1[j] = bound(log10(sqValue) / 2.0 - logSize, m_params->dBFloor, 0.0);
    else ch->fftData1[j] = m_params->dBFloor; // gdata->dBFloor();
  }
  sqValue = sq(dataFFT[0]) + sq(dataFFT[nDiv2]);
  ch->fftData2[0] = logBaseN(logBase, 1.0 + 2.0*sqrt(sqValue) / double(nDiv2) * (logBase-1.0));
  if(sqValue > 0.0)
    ch->fftData1[0] = bound(log10(sqValue) / 2.0 - logSize, m_params->dBFloor, 0.0);
  else ch->fftData1[0] = m_params->dBFloor; // gdata->dBFloor();

  if(m_params->analysisType == e_MPM_MODIFIED_CEPSTRUM) {
    for(int j=1; j<nDiv2; j++) {
      dataFFT[j] = ch->fftData2[j];
      dataFFT[m_windowSize-j] = 0.0;
    }
    dataFFT[0] = ch->fftData2[0];
    dataFFT[nDiv2] = 0.0;
    fftwf_execute(planDataFFT2Time);
    for(int j=1; j<m_windowSize; j++) {
      dataTime[j] /= dataTime[0];
    }
    dataTime[0] = 1.0;
    for(int j=0; j<nDiv2; j++) ch->cepstrumData[j] = dataTime[j];
    AnalysisData *analysisData = ch->dataAtChunk(chunk);
	if(analysisData != NULL) {
	  analysisData->cepstrumIndex = findNSDFsubMaximum(dataTime, nDiv2, 0.6f);
	  analysisData->cepstrumPitch = freq2pitch(double(analysisData->cepstrumIndex) / ch->rate());
	}
  }
}

void MyTransforms::calcHarmonicAmpPhase(float *harmonicsAmp, float *harmonicsPhase, int binsPerHarmonic)
{
  int harmonic;
  for(int j=0; j<m_numHarmonics; j++) {
    harmonic = (j+1) * binsPerHarmonic;
    if(harmonic < m_windowSize) {
      harmonicsAmp[j] = sqrt(sq(dataFFT[harmonic]) + sq(dataFFT[m_windowSize-harmonic]));
      harmonicsPhase[j] = atan2((double)dataFFT[m_windowSize-harmonic], (double)dataFFT[harmonic]);
    } else {
      harmonicsAmp[j] = 0.0;
      harmonicsPhase[j] = 0.0;
    }
  }
}

void MyTransforms::doHarmonicAnalysis(float *input, AnalysisData &analysisData, double period)
{
  double numPeriodsFit = floor(double(m_windowSize) / period); //2.0
  double numPeriodsUse = numPeriodsFit - 1.0;
  int iNumPeriodsUse = int(numPeriodsUse);
  double centerX = float(m_windowSize) / 2.0;
  //printf("iNumPeriodsUse = %d\n", iNumPeriodsUse);
  //do left
  //stretch_array(n, input, n, dataTime, (float(n) / 2.0) - period, period, LINEAR);
  double start = centerX - (numPeriodsFit / 2.0) * period;
  double length = (numPeriodsUse) * period;
  stretch_array(m_windowSize, input, m_windowSize, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpLeft, harmonicsPhaseLeft, iNumPeriodsUse);  
  
  //do center
  start += period / 2.0;
  stretch_array(m_windowSize, input, m_windowSize, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpCenter, harmonicsPhaseCenter, iNumPeriodsUse);
  
  //do right
  start += period / 2.0;
  stretch_array(m_windowSize, input, m_windowSize, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpRight, harmonicsPhaseRight, iNumPeriodsUse);
  
  double freq = m_rate / period;
  int harmonic;
  
  analysisData.harmonicAmpNoCutOff.resize(m_numHarmonics);
  analysisData.harmonicAmp.resize(m_numHarmonics);
  analysisData.harmonicFreq.resize(m_numHarmonics);
  analysisData.harmonicNoise.resize(m_numHarmonics);

  for(int j=0; j<m_numHarmonics; j++) {
    harmonic = (j+1) * iNumPeriodsUse;
    analysisData.harmonicAmpNoCutOff[j] = analysisData.harmonicAmp[j] = log10(harmonicsAmpCenter[j] / hanningScalar) * 20;
    analysisData.harmonicAmp[j] = 1.0 - (analysisData.harmonicAmp[j] / m_params->ampThresholds[AMPLITUDE_RMS][0]);
    if(analysisData.harmonicAmp[j] < 0.0)
        analysisData.harmonicAmp[j] = 0.0;
    //should be 1 whole period between left and right. i.e. the same freq give 0 phase difference
    double diffAngle = (harmonicsPhaseRight[j] - harmonicsPhaseLeft[j]) / twoPI;
    diffAngle = cycle(diffAngle + 0.5, 1.0) - 0.5;
    double diffAngle2 = (harmonicsPhaseCenter[j] - harmonicsPhaseLeft[j]) / twoPI;
    //if an odd harmonic the phase will be 180 degrees out. So correct for this
    if(j % 2 == 0)
        diffAngle2 += 0.5;
    diffAngle2 = cycle(diffAngle2 + 0.5, 1.0) - 0.5;
    analysisData.harmonicNoise[j] = fabs(diffAngle2 - diffAngle);
    analysisData.harmonicFreq[j] = float(freq * double(j+1)) + (freq*diffAngle);
  }
}


void MyTransforms::applyHanningWindow(float *d) {
  for(int j = 0; j < m_windowSize; j++) d[j] *= hanningCoeff[j];
}



