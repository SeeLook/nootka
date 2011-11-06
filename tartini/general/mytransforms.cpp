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
 ***************************************************************************/

#include "mytransforms.h"
#include "myassert.h"
#include "array1d.h"
#include "equalloudness.h"
#include "gdata.h"
#include "bspline.h"
#include "channel.h"
#include "Filter.h"
#include "conversions.h"
#include "musicnotes.h"

#include "useful.h"
#include <algorithm>
#include <vector>
#include <numeric>
#include <float.h>

//#ifdef WINDOWS
//#define isnan _isnan
//#endif

//#ifdef MACX
//#define isnan(x) (x != x)
//#endif

#include <complex>

MyTransforms::MyTransforms()
{
  beenInit = false;
}
  
MyTransforms::~MyTransforms()
{
  uninit();
}

/** init() Initalises the parameters of a class instance. This must be called before use
  @param n_ The size of the data windows to be processed
  @param k_ The number of outputs wanted (autocorr size = n_ + k_). Set k_ = 0, to get default n_/2
  @param rate_ The sampling rate of the incoming signal to process
  @param threshold_ The ratio of highest peak to the first peak allowed to be chosen
*/
void MyTransforms::init(int n_, int k_, double rate_, /*float threshold_, */bool equalLoudness_, int numHarmonics_)
{
  const int myFFTMode = FFTW_ESTIMATE;
  //const int myFFTMode = FFTW_MEASURE;
  //const int myFFTMode = FFTW_PATIENT;
  uninit();
  if(k_ == 0) k_ = (n_ + 1) / 2;
  
  n = n_;
  k = k_;
  size = n + k;
  rate = rate_;
  //_threshold = threshold_;
  equalLoudness = equalLoudness_;
  numHarmonics = numHarmonics_;

  dataTemp = (float*)fftwf_malloc(sizeof(float) * n);
  dataTime = (float*)fftwf_malloc(sizeof(float) * n);
  dataFFT  =  (float*)fftwf_malloc(sizeof(float) * n);
  autocorrTime = (float*)fftwf_malloc(sizeof(float) * size);
  autocorrFFT  = (float*)fftwf_malloc(sizeof(float) * size);
  //storeFFT  = (float*)fftwf_malloc(sizeof(float) * size);
  //equalLoudnessTable = (float*)fftwf_malloc(sizeof(float) * n);
  hanningCoeff  = (float*)fftwf_malloc(sizeof(float) * n);

  planAutocorrTime2FFT = fftwf_plan_r2r_1d(size, autocorrTime, autocorrFFT, FFTW_R2HC, myFFTMode);
  planAutocorrFFT2Time = fftwf_plan_r2r_1d(size, autocorrFFT, autocorrTime, FFTW_HC2R, myFFTMode);
  
  planDataTime2FFT = fftwf_plan_r2r_1d(n, dataTime, dataFFT, FFTW_R2HC, myFFTMode);
  planDataFFT2Time = fftwf_plan_r2r_1d(n, dataFFT, dataTime, FFTW_HC2R, myFFTMode); //???

  harmonicsAmpLeft = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsPhaseLeft = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsAmpCenter = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsPhaseCenter = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsAmpRight = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  harmonicsPhaseRight = (float*)fftwf_malloc(numHarmonics * sizeof(float));
  //storeFFTAmp1 = (float*)malloc(n/2 * sizeof(float));
  //storeFFTAmp2 = (float*)malloc(n/2 * sizeof(float));
  
  freqPerBin = rate / double(size);
  //buildEqualLoudnessTable(50.0);
  //init hanningCoeff. The hanning windowing function
  hanningScalar = 0;
  for(int j=0; j<n; j++) {
    hanningCoeff[j] = (1.0 - cos(double(j+1) / double(n+1) * twoPI)) / 2.0;
    hanningScalar += hanningCoeff[j];
  }
  hanningScalar /= 2; //to normalise the FFT coefficients we divide by the sum of the hanning window / 2

  fastSmooth = new fast_smooth(n/8);
  //printf("fast_smooth size = %d\n", n/8);
  beenInit = true;
}

void MyTransforms::uninit()
{
  if(beenInit) {
    //free(storeFFTAmp2);
    //free(storeFFTAmp1);
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
    //fftwf_free(equalLoudnessTable);
    //fftwf_free(storeFFT);
    fftwf_free(autocorrFFT);
    fftwf_free(autocorrTime);
    fftwf_free(dataFFT);
    fftwf_free(dataTime);
    fftwf_free(dataTemp);
    delete fastSmooth;
    beenInit = false;
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
  myassert(beenInit);
  float fsize = float(size);
  
  //pack the data into an array which is zero padded by k elements
  std::copy(input, input+n, autocorrTime);
  std::fill(autocorrTime+n, autocorrTime+size, 0.0f);

  //Do a forward FFT
  fftwf_execute(planAutocorrTime2FFT);

  //std::copy(autocorrFFT, autocorrFFT+size, storeFFT); //keep a copy of the fft for later use
/*
  if(equalLoudness) { //use equal-loudness curves
    double radius, ratio, dB, phons, newRadius;
    double speakerLoudness = 100; //in dB
    //double maxDB = -99999999.0;
    for(int j=1; j<size/2; j++) {
      radius = sqrt(sq(autocorrFFT[j]) + sq(autocorrFFT[size-j])) / fsize;
      if(!(radius > 0.0)) continue; //includes NAN
      dB = 50.0; //20.0 * log10(radius) + speakerLoudness;
      phons = dbToPhons(double(j) * freqPerBin, dB);
      //newRadius = pow10((phons-90.0) / 20.0);
      //ratio = newRadius / radius;
      ratio = pow10((phons - dB) / 20.0);
      autocorrFFT[j] *= ratio;
      autocorrFFT[size-j] *= ratio;
      //if(newRadius > maxDB) maxDB = newRadius;
    }
    autocorrFFT[0] = 0;
    autocorrFFT[size/2] = 0;
    //printf("maxNewRadius = %lf\n", maxDB); 
       
    //Do an inverse FFT
    fftwf_execute(planAutocorrFFT2Time);
    //normalize
    for(int x=0; x<n; x++)
      autocorrTime[x] /= fsize;
    
    //fix up the zero padding again
    std::fill(autocorrTime+n, autocorrTime+size, 0.0f);
    //copy back to the input array so other calculation can be done on this
    std::copy(autocorrTime, autocorrTime+n, input);
    
    //Do a forward FFT
    fftwf_execute(planAutocorrTime2FFT);
  }
*/
  //calculate the (real*real + ima*imag) for each coefficient
  //Note: The numbers are packed in half_complex form (refer fftw)
  //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
  for(int j=1; j<size/2; j++) {
    autocorrFFT[j] = sq(autocorrFFT[j]) + sq(autocorrFFT[size-j]);
    autocorrFFT[size-j] = 0.0f;
  }
  autocorrFFT[0] = sq(autocorrFFT[0]);
  autocorrFFT[size/2] = sq(autocorrFFT[size/2]);

  //Do an inverse FFT
  fftwf_execute(planAutocorrFFT2Time);

  //extract the wanted elements out, and normalise
  //for(int x=0; x<k; x++)
  //  output[x] = autocorrTime[x+1] / fsize;
  for(float *p1=output, *p2=autocorrTime+1; p1<output+k;)
    *p1++ = *p2++ / fsize;
    
  return double(autocorrTime[0]) / double(size);
}

double MyTransforms::autoLogCorr(float *input, float *output)
{
  myassert(beenInit);
  float fsize = float(size);
  
  //pack the data into an array which is zero padded by k elements
  std::copy(input, input+n, autocorrTime);
  std::fill(autocorrTime+n, autocorrTime+size, 0.0f);

  //Do a forward FFT
  fftwf_execute(planAutocorrTime2FFT);

  //calculate the (real*real + ima*imag) for each coefficient
  //Note: The numbers are packed in half_complex form (refer fftw)
  //ie. R[0], R[1], R[2], ... R[size/2], I[(size+1)/2+1], ... I[3], I[2], I[1]
  for(int j=1; j<size/2; j++) {
    autocorrFFT[j] = (sq(autocorrFFT[j]) + sq(autocorrFFT[size-j]));
    autocorrFFT[size-j] = 0.0f;
  }
  autocorrFFT[0] = (sq(autocorrFFT[0]));
  autocorrFFT[size/2] = (sq(autocorrFFT[size/2]));

  //Do an inverse FFT
  fftwf_execute(planAutocorrFFT2Time);

  //extract the wanted elements out, and normalise
  for(float *p1=output, *p2=autocorrTime+1; p1<output+k;)
    *p1++ = *p2++ / fsize;
    
  return double(autocorrTime[0]) / double(size);
}

/** The Average Square Difference Function.
    @param input. An array of length n, in which the ASDF is taken
    @param ouput. This should be an array of length k
*/
double MyTransforms::asdf(float *input, float *output)
{
  myassert(beenInit);
  double sumSq = autocorr(input, output); //Do an autocorrelation and return the sum of squares of the input
  double sumRightSq = sumSq, sumLeftSq = sumSq;
  for(int j=0; j<k; j++) {
    sumLeftSq  -= sq(input[n-1-j]);
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
  myassert(beenInit);
  double sumSq = autocorr(input, output); //the sum of squares of the input
  //double sumSq = autoLogCorr(input, output); //the sum of squares of the input

  //double sumRightSq = sumSq, sumLeftSq = sumSq;
  double totalSumSq = sumSq * 2.0;
  if(gdata->analysisType() == MPM || gdata->analysisType() == MPM_MODIFIED_CEPSTRUM) { //nsdf
    for(int j=0; j<k; j++) {
      //sumLeftSq  -= sq(input[n-1-j]);
      //sumRightSq -= sq(input[j]);
      ////output[j] = 1.0 - (sumLeftSq + sumRightSq - 2*output[j]) / (sumLeftSq + sumRightSq);
      ////output[j] = 2.0 * output[j] / (sumLeftSq + sumRightSq);
      //output[j] *= 2.0 / (sumLeftSq + sumRightSq);
      totalSumSq  -= sq(input[n-1-j]) + sq(input[j]);
      //dividing by zero is very slow, so deal with it seperately
      if(totalSumSq > 0.0) output[j] *= 2.0 / totalSumSq;
      else output[j] = 0.0;
    }
  } else { //autocorr
    for(int j=0; j<k; j++) {
      //output[j] = output[j] * float(n) / float(n-1 - j) / sumSq;
      //output[j] = output[j] / sumSq;
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
    myassert(!(input[pos] < 0)); //don't assert on NAN
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
  myassert(0); //should never get here
  return 0; //stop the compiler warning
}

/** Estimate the period or the fundamental frequency.
  Finds the first maximum of NSDF which past the first
  positive zero crossing and is over the threshold.
  If no maxima are over the threshold then the the highest maximum is returned.
  If no positive zero crossing is found, zero is returned.
  @param input. An array of length n.
  @param threshold. A number between 0 and 1 at which maxima above are acceped.
  @return The estimated period (in samples), or zero if none found.
*/
//void MyTransforms::calculateAnalysisData(float *input, AnalysisData &analysisData, Channel *ch, float threshold)
void MyTransforms::calculateAnalysisData(/*float *input, */int chunk, Channel *ch/*, float threshold*/)
{
  myassert(ch);
  myassert(ch->dataAtChunk(chunk));
  AnalysisData &analysisData = *ch->dataAtChunk(chunk);
  AnalysisData *prevAnalysisData = ch->dataAtChunk(chunk-1);
  //Array1d<float> output(k);
  float *output = ch->nsdfData.begin();
  float *curInput = (equalLoudness) ? ch->filteredInput.begin() : ch->directInput.begin();

  std::vector<int> nsdfMaxPositions;
  //int pos = 0;
  //int curMaxPos = 0;
  //float corrError = 0.0f;
  //freqPerBin = rate / 2.0 / double(size);

  //analysisData.maxIntensity = fabs(*std::max_element(input, input+n, absoluteLess()));
  analysisData.maxIntensityDB() = linear2dB(fabs(*std::max_element(curInput, curInput+n, absoluteLess<float>())));
  
  //if(gdata->doingActiveFFT()) {
    //std::copy(curInput, curInput+n, dataTime);
  doChannelDataFFT(ch, curInput, chunk);
    //std::copy(dataTemp, dataTemp+n, dataTime);
  //}
  std::copy(curInput, curInput+n, dataTime);
  
  //if(!gdata->doingActiveAnalysis()) return;
  //if(gdata->doingFreqAnalysis()) {
  if(gdata->doingFreqAnalysis() && (ch->firstTimeThrough() || gdata->doingActiveAnalysis())) {
    //calculate the Normalised Square Difference Function
    //analysisData.rms = nsdf(dataTime, output.begin()) / double(n/*size*/);
    //analysisData.rms = nsdf(dataTime, output) / double(n/*size*/);
    double logrms = linear2dB(nsdf(dataTime, ch->nsdfData.begin()) / double(n)); /**< Do the NSDF calculation */
    analysisData.logrms() = logrms;
    if(gdata->doingAutoNoiseFloor() && !analysisData.done) {
      //do it for gdata. this is only here for old code. remove some stage
      if(chunk == 0) { gdata->rmsFloor() = 0.0; gdata->rmsCeiling() = gdata->dBFloor(); }
      if(logrms+15 < gdata->rmsFloor()) gdata->rmsFloor() = logrms+15;
      if(logrms > gdata->rmsCeiling()) gdata->rmsCeiling() = logrms;

      //do it for the channel
      if(chunk == 0) { ch->rmsFloor = 0.0; ch->rmsCeiling = gdata->dBFloor(); }
      if(logrms+15 < ch->rmsFloor) ch->rmsFloor = logrms+15;
      if(logrms > ch->rmsCeiling) ch->rmsCeiling = logrms;
    }
    //if(!analysisData.done) {
      //analysisData.notePlaying = ch->isNotePlaying();
      //if(ch->isNotePlaying()) 
      //  addTo(ch->nsdfData.begin(), ch->nsdfData.end(), ch->nsdfAggregateData.begin());
    //}

    //analysisData.freqCentroid() = calcFreqCentroid(storeFFT, size);
    analysisData.freqCentroid() = calcFreqCentroidFromLogMagnitudes(ch->fftData1.begin(), ch->fftData1.size());
    if(prevAnalysisData)
      analysisData.deltaFreqCentroid() = bound(fabs(analysisData.freqCentroid() - prevAnalysisData->freqCentroid())*20.0, 0.0, 1.0);
    else 
      analysisData.deltaFreqCentroid() = 0.0;
    
    findNSDFMaxima(ch->nsdfData.begin(), k, nsdfMaxPositions);
    if(!analysisData.done) {
      //if(ch->isNotePlaying()) {
        //analysisData.periodOctaveEstimate = ch->calcOctaveEstimate(chunk, threshold);
/*
        findNSDFMaxima(ch->nsdfAggregateData.begin(), k, nsdfAggregateMaxPositions);
        myassert(!nsdfAggregateMaxPositions.empty());
  
        //get the highest nsdfAggregateMaxPosition
        uint j;
        int nsdfAggregateMaxIndex = 0;
        int nsdfAggregateChoosenIndex = 0;
        for(j=1; j<nsdfAggregateMaxPositions.size(); j++) {
          if(ch->nsdfAggregateData[nsdfAggregateMaxPositions[j]] > ch->nsdfAggregateData[nsdfAggregateMaxPositions[nsdfAggregateMaxIndex]]) nsdfAggregateMaxIndex = j;
        }
        //get the choosen nsdfAggregateMaxPosition
        float nsdfAggregateCutoff = ch->nsdfAggregateData[nsdfAggregateMaxPositions[nsdfAggregateMaxIndex]] * threshold;
        for(j=0; j<nsdfAggregateMaxPositions.size(); j++) {
          if(ch->nsdfAggregateData[nsdfAggregateMaxPositions[j]] >= nsdfAggregateCutoff) { nsdfAggregateChoosenIndex = j; break; }
        }
        analysisData.periodOctaveEstimate = float(nsdfAggregateMaxPositions[nsdfAggregateChoosenIndex]+1); //add 1 for index offset, ie position 0 = 1 period
*/
    }

    //store some of the best period estimates
    analysisData.periodEstimates.clear();
    analysisData.periodEstimatesAmp.clear();
    //float smallThreshold = 0.7f;
    //float smallCutoff = output[overallMaxIndex] * smallThreshold;
    float smallCutoff = 0.4f;
    for(std::vector<int>::iterator iter = nsdfMaxPositions.begin(); iter < nsdfMaxPositions.end(); iter++) {
      if(output[*iter] >= smallCutoff) {
        //analysisData.periodEstimates.push_back(double(*iter + 1) + parabolaTurningPoint(output[*iter-1], output[*iter], output[*iter+1]));
        //analysisData.periodEstimatesAmp.push_back(output[*iter]); //TODO: These should be calculated more accurately
        float x, y;
        //do a parabola fit to find the maximum
        parabolaTurningPoint2(output[*iter-1], output[*iter], output[*iter+1], float(*iter + 1), &x, &y);
        y = bound(y, -1.0f, 1.0f);
        analysisData.periodEstimates.push_back(x);
        analysisData.periodEstimatesAmp.push_back(y);
      }
    }
    
    float periodDiff = 0.0f;
    //if(maxPositions.empty()) { //no period found
    if(analysisData.periodEstimates.empty()) { //no period found
      //analysisData.correlation() = 0.0f;
      analysisData.calcScores();
      analysisData.done = true;
      //goto finished; //return;
    } else {
      //calc the periodDiff
      if(chunk > 0) {
        float prevPeriod = prevAnalysisData->periodEstimates[prevAnalysisData->highestCorrelationIndex];
        std::vector<float>::iterator closestIter = binary_search_closest(analysisData.periodEstimates.begin(), analysisData.periodEstimates.end(), prevPeriod);
        //print_elements(analysisData.periodEstimates.begin(), analysisData.periodEstimates.end());
        //printf("closestIter = %f, %f\n", *closestIter, prevPeriod);
        periodDiff = *closestIter - prevPeriod;
        if(absolute(periodDiff) > 8.0f) periodDiff = 0.0f;
      }

      int nsdfMaxIndex = int(std::max_element(analysisData.periodEstimatesAmp.begin(), analysisData.periodEstimatesAmp.end()) - analysisData.periodEstimatesAmp.begin());
      analysisData.highestCorrelationIndex = nsdfMaxIndex;

      if(!analysisData.done) {
        //if(gdata->doingActiveCepstrum()) {
        if(gdata->analysisType() == MPM_MODIFIED_CEPSTRUM) {
            ch->chooseCorrelationIndex(chunk, float(analysisData.cepstrumIndex)); //calculate pitch
        } else {
          if(ch->isNotePlaying() && chunk > 0) {
            //ch->chooseCorrelationIndex(chunk, ch->getLastNote()->periodOctaveEstimate()); //calculate pitch
            //ch->chooseCorrelationIndex(chunk, ch->periodOctaveEstimate(std::max(0, chunk-1))); //calculate pitch
            ch->chooseCorrelationIndex(chunk, ch->periodOctaveEstimate(chunk-1)); //calculate pitch
          } else {
            ch->chooseCorrelationIndex1(chunk); //calculate pitch
          }
          //ch->chooseCorrelationIndex(chunk, (float)ch->rate() / 440.0f); //calculate pitch
        }
        ch->calcDeviation(chunk);

        ch->doPronyFit(chunk); //calculate vibratoPitch, vibratoWidth, vibratoSpeed
      }

      analysisData.changeness() = 0.0f;
      //analysisData.changeness() = get_max_note_change(dataTime, analysisData.period);

      if(gdata->doingHarmonicAnalysis()) {
        std::copy(dataTime, dataTime+n, dataTemp);
        if(analysisData.chosenCorrelationIndex >= 0)
          doHarmonicAnalysis(dataTemp, analysisData, analysisData.periodEstimates[analysisData.chosenCorrelationIndex]/*period*/);
        //doHarmonicAnalysis(input, analysisData, period);
      }

      //analysisData.volumeValue = (dB2Normalised(analysisData.logrms) + (analysisData.correlation-1.0f)) * 0.2;
    }

    //float periodDiff = 0.0f;
    if(gdata->doingFreqAnalysis() && ch->doingDetailedPitch() && ch->firstTimeThrough()) {
      //periodDiff = ch->calcDetailedPitch(curInput, analysisData.period, chunk);
      float periodDiff2 = ch->calcDetailedPitch(curInput, analysisData.period, chunk);
      //printf("chunk=%d, %f, %f\n", chunk, periodDiff, periodDiff2);
      periodDiff = periodDiff2;

      ch->pitchLookup.push_back(ch->detailedPitchData.begin(), ch->detailedPitchData.size());
      ch->pitchLookupSmoothed.push_back(ch->detailedPitchDataSmoothed.begin(), ch->detailedPitchDataSmoothed.size());
/*      float periodDiff1 = (rate / pitch2freq(ch->detailedPitchData.back()));
      float periodDiff2 = (rate / pitch2freq(ch->detailedPitchData.front()));
      periodDiff = periodDiff1 - periodDiff2;
      printf("%f, %f, %f\n", periodDiff1, periodDiff2, periodDiff);
*/
    }

    if(!analysisData.done) {
      analysisData.calcScores();
      ch->processNoteDecisions(chunk, periodDiff);
      analysisData.done = true;
    }

    if(gdata->doingFreqAnalysis() && ch->doingDetailedPitch() && ch->firstTimeThrough()) {
      ch->calcVibratoData(chunk);
    }
  }

  if(gdata->doingFreqAnalysis() && ch->doingDetailedPitch() && (!ch->firstTimeThrough())) {
    ch->pitchLookup.copyTo(ch->detailedPitchData.begin(), chunk*ch->detailedPitchData.size(), ch->detailedPitchData.size());
    ch->pitchLookupSmoothed.copyTo(ch->detailedPitchDataSmoothed.begin(), chunk*ch->detailedPitchDataSmoothed.size(), ch->detailedPitchDataSmoothed.size());
  }

  if(!analysisData.done) {
    int j;
    //calc rms by hand
    double rms = 0.0;
    for(j=0; j<n; j++) {
      rms += sq(dataTime[j]);
    }
    //analysisData.rms = sqrt(analysisData.rms);
    analysisData.logrms() = linear2dB(rms / float(n));
    analysisData.calcScores();
    analysisData.done = true;
  }

}

/*
void MyTransforms::chooseCorrelationIndex(AnalysisData &analysisData, float threshold, float periodOctaveEstimate)
{    
  uint iterPos;
  int choosenMaxIndex = 0;
  if(analysisData.periodEstimates.empty()) return; //no period found
  if(gdata->analysisType() == MPM || gdata->analysisType() == MPM_MODIFIED_CEPSTRUM) {

    //choose a cutoff value based on the highest value and a relative threshold
    //float cutoff = analysisData.periodEstimatesAmp[analysisData.highestCorrelationIndex] * threshold;
    //find the first of the maxPositions which is above the cutoff
    //for(iterPos = 0; iterPos < analysisData.periodEstimatesAmp.size(); iterPos++) {
    //  if(analysisData.periodEstimatesAmp[iterPos] >= cutoff) { choosenMaxIndex = iterPos; break; }
    //}

    //choose the periodEstimate which is closest to the periodOctaveEstimate
    float minDist = fabs(analysisData.periodEstimates[0] - periodOctaveEstimate);
    float dist;
    for(iterPos = 1; iterPos < analysisData.periodEstimates.size(); iterPos++) {
      dist = fabs(analysisData.periodEstimates[iterPos] - periodOctaveEstimate);
      if(dist < minDist) { minDist = dist; choosenMaxIndex = iterPos; }
    }

    //myassert(wantedMaxPos > 0);
  } else {
    choosenMaxIndex = analysisData.highestCorrelationIndex;
  }
  analysisData.chosenCorrelationIndex = choosenMaxIndex;
  analysisData.correlation() = analysisData.periodEstimatesAmp[choosenMaxIndex];
  
  //double period = analysisData.periodEstimates[choosenMaxIndex];
  analysisData.period = analysisData.periodEstimates[choosenMaxIndex];

  //float clarityMin = get_fine_clarity_measure(period);
  
  //double freq = rate / period;
  double freq = rate / analysisData.period;
  analysisData.fundamentalFreq = float(freq);
  analysisData.note = bound(freq2note(freq), 0.0, gdata->topNote());
  analysisData.mean = 0.0f;
  analysisData.standard_deviation = 0.0f;
  //if(isnan(analysisData.note)) analysisData.note = 0.0f;
}
*/

/* using the given fractional period, constructs a smaller subwindow
which slide through the window. The smallest clarity measure of this
moving sub window is returned.*/
float MyTransforms::get_fine_clarity_measure(double period)
{
    //printf("%f, ", analysisData.periodEstimates[choosenMaxIndex]);
//    printf("%f, ", analysisData.periodEstimatesAmp[choosenMaxIndex]);
    int tempN = n - int(ceil(period));
    float *tempData = new float[tempN];
    float bigSum=0, corrSum=0, matchVal, matchMin=1.0;
    //create some data points at the fractional period delay into tempData
    stretch_array(n, dataTime, tempN, tempData, period, tempN, LINEAR);
    int dN = int(floor(period)); //tempN / 4;
    for(int j=0; j<dN; j++) {
      bigSum += sq(dataTime[j]) + sq(tempData[j]);
      corrSum += dataTime[j] * tempData[j];
    }
    matchMin = (2.0*corrSum) / bigSum;
//    printf("%f, ", matchMin);
    for(int j=0; j<tempN - dN; j++) {
      bigSum -= sq(dataTime[j]) + sq(tempData[j]);
      corrSum -= dataTime[j] * tempData[j];
      bigSum += sq(dataTime[j+dN]) + sq(tempData[j+dN]);
      corrSum += dataTime[j+dN] * tempData[j+dN];
      matchVal = (2.0*corrSum) / bigSum;
      if(matchVal < matchMin) matchMin = matchVal;
    }
//    printf("%f\n", matchMin);
    delete[] tempData;

  return matchMin;
}

double MyTransforms::get_max_note_change(float *input, double period)
{
  //TODO
/*matlab code
l = length(s);
m = floor(l / 4); % m is the maximum size sub-window to use
% w is the sub-window size
if p < m
    w = p * floor(m / p);
else
    w = p;
end

n = -4:4;
ln = length(n);
left = cell(1, ln);
right = cell(1, ln);
left_pow = zeros(1, ln);
right_pow = zeros(1, ln);
pow = zeros(1, ln);
err = zeros(1, ln);

for i = 1:ln
    left{i} = s(1:(w-n(i))); % a smaller delay period has a larger window size, to ensure only the same data is used
    right{i} = s(1+p+n(i):w+p);
    left_pow(i) = sum(left{i}.^2);
    right_pow(i) = sum(right{i}.^2);
    err(i) = sum((left{i} - right{i}) .^ 2);
end
*/
  int i, j, j2;
  int max_subwindow = n / 4;
  int subwindow_size;
  if(period < 1.0) return 0.0; //period too small
  if(period > n/2) { printf("period = %lf\n", period); return 0.0; }
  int iPeriod = int(floor(period));
  if(period < double(max_subwindow)) subwindow_size = int(floor(period * (double(max_subwindow) / period)));
  else subwindow_size = int(floor(period));
  int num = n-subwindow_size-iPeriod;

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
  //printf("subwindow_size=%d, num=%d, period=%lf\n", subwindow_size, num, period);
/*matlab code
for j = 1:(num-1)
    for i = 1:ln
        pow(i) = (left_pow(i) + right_pow(i));
        resulte(i, j) = err(i);
        resultp(i, j) = pow(i);
        result(i, j) = 1 - (err(i) / pow(i));

        %err = err - (s(j) - s(j+p)).^2 + (s(j+w) - s(j+w+p)).^2;
        err(i) = err(i) - ((s(j) - s(j+p+n(i))).^2) + (s(j+w-n(i)) - s(j+w+p)).^2;
        left_pow(i) = left_pow(i) - s(j).^2 + s(j+w-n(i)).^2;
        right_pow(i) = right_pow(i) - s(j+p+n(i)).^2 + s(j+p+w).^2;
    end
end

for i = 1:ln
    pow(i) = (left_pow(i) + right_pow(i));
    result(i, num) = 1 - (err(i) / pow(i));
end
*/
  //TODO: speed up this for loop
  for(j=0; j<num-1; j++) {
    for(i=0; i<ln; i++) {
      pow[i] = left_pow[i] + right_pow[i];
      result[i] = 1.0 - (err[i] / pow[i]);

      err[i] += -sq(input[j] - input[j+iPeriod+offsets[i]]) + sq(input[j+subwindow_size-offsets[i]] - input[j+subwindow_size+iPeriod]);
      left_pow[i] += -sq(input[j]) + sq(input[j+subwindow_size-offsets[i]]);
      right_pow[i] += -sq(input[j+iPeriod+offsets[i]]) + sq(input[j+iPeriod+subwindow_size]);
    }
/*matlab code
for j = 1:num
    [dud pos] = max(result(:,j));
    if pos > 1 && pos < ln
        [period(j) dummy] = parabolaPeak(result(pos-1, j), result(pos, j), result(pos+1, j), p+n(pos));
    else
        period(j) = p+n(pos);
    end
    period_int(j) = p+n(pos);
end*/
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
    //printf("%f ", smoothed[j]);
    if(smoothed_diff[j] > smoothed_diff[max_pos]) max_pos = j;
  }
  //printf("\nsmooted_diff=%f\n", smoothed_diff[max_pos]);
  //return smoothed_diff[max_pos] / period * double(rate) * double(n) / 40000.0;
  double ret = smoothed_diff[max_pos] / period * double(rate) * double(subwindow_size) / 10000.0;
  //ret = (ret < 0.19) ? 0.0 : 1.0;
  return ret;
}

/** Builds a lookup table for use in doEqualLoudness
  @param dB The decibel level of at which the human ear filter should be simulated (default 50dB)
*/
/*
void MyTransforms::buildEqualLoudnessTable(double dB)
{
  double phons;
  float maxLoudness = 0.00000001f;
  equalLoudnessTable[0] = 0.0;
  for(int k=1; k<n/2; k++) {
    phons = dbToPhons(double(k) * freqPerBin, dB);
    equalLoudnessTable[k] = pow10((phons - dB) / 20.0);
    if(equalLoudnessTable[k] > maxLoudness) maxLoudness = equalLoudnessTable[k];
  }
  //normalise
  for(int y=1; y<n/2; y++) {
    equalLoudnessTable[y] /= maxLoudness;
    //printf("[%d] = %f\n", j, equalLoudnessTable[j]);
  }
}
*/

/** Modify the coefficients in Fourier space to give approximate the human ear filter.
*/
/*
//void MyTransforms::doEqualLoudness(Channel *ch)
void MyTransforms::doEqualLoudness()
{
  fftwf_execute(planDataTime2FFT);
  
  //double logSize = log10(double(ch->fftData1.size()));
  //Adjust the coefficents, both real and imaginary part by same amount
  for(int j=1; j<n/2; j++) {
    //LOG RULES: log(sqrt(x)) = log(x) / 2.0
    //LOG RULES: log(a * b) = log(a) + log(b)
    //storeFFTAmp1[j] = sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j]));
    //ch->fftData1[j] = sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j]));
    //ch->fftData1[j] = log10(sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j])) / ds);
    //ch->fftData1[j] = log10(sq(dataFFT[j]) + sq(dataFFT[n-j])) / 2.0 - logSize;
    dataFFT[j] *= equalLoudnessTable[j]; //real part
    dataFFT[n-j] *= equalLoudnessTable[j]; //imaginary part
    //storeFFTAmp2[j] = sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j]));
    //ch->fftData2[j] = sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j]));
    //ch->fftData2[j] = log10(sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j])) / ds );
    //ch->fftData2[j] = log10(sq(dataFFT[j]) + sq(dataFFT[n-j])) / 2.0 - logSize;
  }
  //storeFFTAmp1[0] = sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2]));
  //ch->fftData1[0] = sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2]));
  //ch->fftData1[0] = log10(sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2])) / ds);
  //ch->fftData1[0] = log10(sq(dataFFT[0]) + sq(dataFFT[n/2])) / 2.0 - logSize;
  dataFFT[0] = 0; //remove the mean/DC component
  dataFFT[n/2] = 0;
  //storeFFTAmp2[0] = sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2]));
  //ch->fftData2[0] = sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2]));
  //ch->fftData2[0] = log10(sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2])) / ds);
  //ch->fftData2[0] = log10(sq(dataFFT[0]) + sq(dataFFT[n/2])) / 2.0 - logSize;

  fftwf_execute(planDataFFT2Time);

  float fn = float(n);
  //normalize
  //for(int x=0; x<n; x++)
  //  dataTime[x] /= fn;
  for(float *ptr=dataTime; ptr<dataTime+n;)
    *ptr++ /= fn;
}
*/

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
  myassert(0); //shouldn't get here.
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
  std::copy(curInput, curInput+n, dataTime);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  int nDiv2 = n/2;
  //LOG RULES: log(sqrt(x)) = log(x) / 2.0
  //LOG RULES: log(a * b) = log(a) + log(b)
  myassert(ch->fftData1.size() == nDiv2);
  double logSize = log10(double(ch->fftData1.size())); //0.0
  //Adjust the coefficents, both real and imaginary part by same amount
  double sqValue;
  const double logBase = 100.0;
  for(int j=1; j<nDiv2; j++) {
    sqValue = sq(dataFFT[j]) + sq(dataFFT[n-j]);
    //ch->fftData1[j] = log10(sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j])) / ds);
    //ch->fftData2[j] = log10(1.0 + sqrt(sqValue));
    //ch->fftData2[j] = log10(0.1 + 2.0*sqrt(sqValue) / double(nDiv2) * 9.9) / 2.0 + 0.5;
    ch->fftData2[j] = logBaseN(logBase, 1.0 + 2.0*sqrt(sqValue) / double(nDiv2) * (logBase-1.0));
    if(sqValue > 0.0)
      ch->fftData1[j] = bound(log10(sqValue) / 2.0 - logSize, gdata->dBFloor(), 0.0);
    else ch->fftData1[j] = gdata->dBFloor();
  }
  //ch->fftData1[0] = log10(sqrt(sq(dataFFT[0]) + sq(dataFFT[n/2])) / ds);
  sqValue = sq(dataFFT[0]) + sq(dataFFT[nDiv2]);
  //ch->fftData2[0] = log10(1.0 + sqrt(sqValue));
  //ch->fftData2[0] = log10(0.1 + 2.0*sqrt(sqValue) / double(nDiv2) * 9.9) / 2.0 + 0.5;
  ch->fftData2[0] = logBaseN(logBase, 1.0 + 2.0*sqrt(sqValue) / double(nDiv2) * (logBase-1.0));
  if(sqValue > 0.0)
    ch->fftData1[0] = bound(log10(sqValue) / 2.0 - logSize, gdata->dBFloor(), 0.0);
  else ch->fftData1[0] = gdata->dBFloor();

  //printf("n = %d, fff = %f\n", nDiv2, *std::max_element(ch->fftData2.begin(), ch->fftData2.end()));

  //if(gdata->doingActiveCepstrum()) {
  if(gdata->analysisType() == MPM_MODIFIED_CEPSTRUM) {
    //std::complex<float> z, y;
    for(int j=1; j<nDiv2; j++) {
      //z = std::complex<float>(dataFFT[j], dataFFT[n-j]);
      //z = std::polar(std::abs(z)+1.0f, std::arg(z));
      //y = std::log(z);
      //dataFFT[j] = y.real();
      //dataFFT[n-j] = y.imag();
      //dataFFT[j] = ch->fftData1[j];
      //dataFFT[j] = log10(1 + sq(dataFFT[j]) + sq(dataFFT[n-j])) * 2;
      //dataFFT[j] = log10(1.0 + sqrt(sq(dataFFT[j]) + sq(dataFFT[n-j])));
      dataFFT[j] = ch->fftData2[j];
      dataFFT[n-j] = 0.0;
    }
    //dataFFT[0] = ch->fftData1[0];
    //dataFFT[0] = log10(1 + sq(dataFFT[0]) + sq(dataFFT[nDiv2])) * 2;
    //dataFFT[0] = log10(1.0 + sqrt(sq(dataFFT[0]) + sq(dataFFT[nDiv2])));
    //dataFFT[0] = log10(1.0 + sqrt(sq(dataFFT[0]) + sq(dataFFT[nDiv2])));
    dataFFT[0] = ch->fftData2[0];
    //dataFFT[0] = 0.0;
    dataFFT[nDiv2] = 0.0;
    fftwf_execute(planDataFFT2Time);
    //std::copy(dataTime, dataTime + nDiv2, ch->cepstrumData.begin());
    //float theMax = *std::max_element(dataTime, dataTime+nDiv2);
    //printf("theMax = %f\n", dataTime[0]);

    for(int j=1; j<n; j++) {
      dataTime[j] /= dataTime[0];
    }
    dataTime[0] = 1.0;
    for(int j=0; j<nDiv2; j++) ch->cepstrumData[j] = dataTime[j];
    AnalysisData &analysisData = *ch->dataAtChunk(chunk);
    //analysisData.cepstrumIndex = findCepstrumMaximum(dataTime, nDiv2, 0.8f);
    analysisData.cepstrumIndex = findNSDFsubMaximum(dataTime, nDiv2, 0.6f);
    analysisData.cepstrumPitch = freq2pitch(double(analysisData.cepstrumIndex) / ch->rate());
/*
    //Take out everything above the pitch and reverse the cepstrum to get the frequency curve
    int maxIndex = int(std::max_element(dataTime+10, dataTime+nDiv2) - dataTime) - 3;
    maxIndex /= 2;
    //maxIndex = 8;
    if(maxIndex < 0) maxIndex = 0;
    //printf("maxIndex = %d\n", maxIndex);
    for(int j=maxIndex; j<n-maxIndex; j++) dataTime[j] = 0.0;

    //for(int j=0; j<nDiv2; j++) ch->cepstrumData[j] = dataTime[j];

    fftwf_execute(planDataTime2FFT);

    for(int j=0; j<nDiv2; j++) ch->fftData3[j] = sqrt(sq(dataFFT[j])+sq(dataFFT[n-j]))/2;
*/
  }
}

void MyTransforms::calcHarmonicAmpPhase(float *harmonicsAmp, float *harmonicsPhase, int binsPerHarmonic)
{
  int harmonic;
  for(int j=0; j<numHarmonics; j++) {
    harmonic = (j+1) * binsPerHarmonic;
    if(harmonic < n) {
      harmonicsAmp[j] = sqrt(sq(dataFFT[harmonic]) + sq(dataFFT[n-harmonic]));
      harmonicsPhase[j] = atan2f(dataFFT[n-harmonic], dataFFT[harmonic]);
    } else {
      harmonicsAmp[j] = 0.0;
      harmonicsPhase[j] = 0.0;
    }
  }
}

void MyTransforms::doHarmonicAnalysis(float *input, AnalysisData &analysisData, double period)
{
  double numPeriodsFit = floor(double(n) / period); //2.0
  double numPeriodsUse = numPeriodsFit - 1.0;
  int iNumPeriodsUse = int(numPeriodsUse);
  double centerX = float(n) / 2.0;
  //printf("iNumPeriodsUse = %d\n", iNumPeriodsUse);
  //do left
  //stretch_array(n, input, n, dataTime, (float(n) / 2.0) - period, period, LINEAR);
  double start = centerX - (numPeriodsFit / 2.0) * period;
  double length = (numPeriodsUse) * period;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpLeft, harmonicsPhaseLeft, iNumPeriodsUse);
  //printf("left\n");
  //for(int jj=0; jj < 6; jj++)
  //  printf("[%d %lf %lf]", jj, harmonicsAmpLeft[jj], harmonicsPhaseLeft[jj]);
  //printf("\n");
  
  //do center
  //float start = (float(n) - period) / 2.0;
  //stretch_array(n, input, n, dataTime, start, period, LINEAR);
  //start = centerX - ((numPeriodsUse) / 2.0) * period;
  start += period / 2.0;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpCenter, harmonicsPhaseCenter, iNumPeriodsUse);
  //printf("centre\n");
  //for(int jj=0; jj < 6; jj++)
  //  printf("[%d %lf %lf]", jj, harmonicsAmpCenter[jj], harmonicsPhaseCenter[jj]);
  //printf("\n");
  
  //do right
  //stretch_array(n, input, n, dataTime, float(n)/2.0, period, LINEAR);
  //start = centerX - ((numPeriodsFit / 2.0) - 1) * period;
  start += period / 2.0;
  stretch_array(n, input, n, dataTime, start, length, LINEAR);
  applyHanningWindow(dataTime);
  fftwf_execute(planDataTime2FFT);
  calcHarmonicAmpPhase(harmonicsAmpRight, harmonicsPhaseRight, iNumPeriodsUse);
  //printf("right\n");
  //for(int jj=0; jj < 6; jj++)
  //  printf("[%d %lf %lf]", jj, harmonicsAmpRight[jj], harmonicsPhaseRight[jj]);
  //printf("\n");
  
  double freq = rate / period;
  int harmonic;
  
  analysisData.harmonicAmpNoCutOff.resize(numHarmonics);
  analysisData.harmonicAmp.resize(numHarmonics);
  analysisData.harmonicFreq.resize(numHarmonics);
  analysisData.harmonicNoise.resize(numHarmonics);

  for(int j=0; j<numHarmonics; j++) {
    harmonic = (j+1) * iNumPeriodsUse;
    //analysisData.harmonicAmp[j] = float(analysisData.correlation / double(j+1));
    //analysisData.harmonicFreq[j] = float(freq * double(j+1));
    //analysisData.harmonicAmp[j] = log10(sqrt(sq(dataFFT[j+1]) + sq(dataFFT[n-(j+1)]))) / 10.0;
    //analysisData.harmonicAmpNoCutOff[j] = analysisData.harmonicAmp[j] = log10(harmonicsAmpCenter[j]) / 5.0;
    //analysisData.harmonicAmpNoCutOff[j] = analysisData.harmonicAmp[j] = log10(harmonicsAmpCenter[j] / hanningScalar);
    //analysisData.harmonicAmpNoCutOff[j] = analysisData.harmonicAmp[j] = log10(harmonicsAmpCenter[j]/(double) n)* 20;
    analysisData.harmonicAmpNoCutOff[j] = analysisData.harmonicAmp[j] = log10(harmonicsAmpCenter[j] / hanningScalar) * 20;
    //analysisData.harmonicAmpNoCutOff[j] = analysisData.harmonicAmp[j] = log10(harmonicsAmpCenter[j]);
    //analysisData.harmonicAmp[j] = (analysisData.harmonicAmp[j] - gdata->noiseThresholdDB()) / (-gdata->noiseThresholdDB());
    analysisData.harmonicAmp[j] = 1.0 - (analysisData.harmonicAmp[j] / gdata->ampThreshold(AMPLITUDE_RMS, 0));
    if(analysisData.harmonicAmp[j] < 0.0) analysisData.harmonicAmp[j] = 0.0;
    //should be 1 whole period between left and right. i.e. the same freq give 0 phase difference
    double diffAngle = (harmonicsPhaseRight[j] - harmonicsPhaseLeft[j]) / twoPI;
    //if(diffAngle < 0) diffAngle++;
    //if(diffAngle > 0.5) diffAngle--;
    diffAngle = cycle(diffAngle + 0.5, 1.0) - 0.5;
    double diffAngle2 = (harmonicsPhaseCenter[j] - harmonicsPhaseLeft[j]) / twoPI;
    //if an odd harmonic the phase will be 180 degrees out. So correct for this
    if(j % 2 == 0) diffAngle2 += 0.5;
    //if(diffAngle2 < 0) diffAngle2++;
    //while(diffAngle2 > 0.5) diffAngle2--;
    diffAngle2 = cycle(diffAngle2 + 0.5, 1.0) - 0.5;
    analysisData.harmonicNoise[j] = fabs(diffAngle2 - diffAngle);
    analysisData.harmonicFreq[j] = float(freq * double(j+1)) + (freq*diffAngle);
  }
}
/*
void MyTransforms::applyHighPassFilter(float *input, float *output)
{
  int j;
  highPassFilter->clear();
  for(j=0; j<n; j++) output[j] = bound(highPassFilter->apply(input[j]), -1.0, 1.0);
}
*/
void MyTransforms::applyHanningWindow(float *d)
{
  for(int j=0; j<n; j++) d[j] *= hanningCoeff[j];
}

/**
  @param buffer The input buffer, as outputted from the fftw. ie 1st half real, 2nd half imaginary parts
  @param len The length of the buffer, including real and imaganary parts
  @return The normalised frequency centriod
*/
double MyTransforms::calcFreqCentroid(float *buffer, int len)
{
  double centroid = 0.0;
  double totalWeight = 0.0;
  double absValue;
  for(int j=1; j<len/2; j++) { //ignore the end freq bins, ie j=0
    //calculate centroid
    //absValue = sqrt(sq(dataFFT[j]) + sq(dataFFT[len-j]));
    absValue = sqrt(sq(buffer[j]) + sq(buffer[len-j]));
    centroid += double(j)*absValue;
    totalWeight += absValue;
  }
  centroid /= totalWeight * double(len/2);
  return centroid;
}

/**
  @param buffer The input buffer of logarithmic magnitudes
  @param len The length of the buffer
  @return The normalised frequency centriod
*/
double MyTransforms::calcFreqCentroidFromLogMagnitudes(float *buffer, int len)
{
  double centroid = 0.0;
  double totalWeight = 0.0;
  for(int j=1; j<len; j++) { //ignore the end freq bins, ie j=0
    //calculate centroid
    centroid += double(j)*buffer[j];
    totalWeight += buffer[j];
  }
  return centroid;
  //if(centroid == 0.0) return 0.0;
  //return centroid / (totalWeight * double(len));
}
