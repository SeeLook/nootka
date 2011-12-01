//Open Tuner: Copyright 2002-2008P hilip McLeod, 2008, 2009 Mark Billington.

//This file is part of "Open Tuner".
//
//"Open Tuner" is free software: you can redistribute it and/or modify
//it under the terms of the GNU General Public License as published by
//the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//"Open Tuner" is distributed in the hope that it will be useful,
//but WITHOUT ANY WARRANTY; without even the implied warranty of
//MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License
//along with "Open Tuner".  If not, see <http://www.gnu.org/licenses/>.
////////////////////////////////////////////////////////////////////////////////////////////////////
/*
* Tartini_core.c
*
* Licence: GPL
*
* Copyright (C) 2002-2008
* Philip McLeod
* 
*/

#include "kiss_fftr.h"
#include "tartini_core.h"
#include <stdlib.h>


char *noteNames[12] = { "C ", "C#", "D ", "D#", "E ", "F ", "F#", "G ", "G#", "A ", "A#", "B " };
char* noteName(int pitch) {return noteNames[cycle(pitch, 12)];}


#define sq(x) (x*x) //I'm wondering if the effort of a function call is slowing stuff down; hence a #define?


double linear2dB(double x) { /* x is between 0 and 1. 1 becomes 0dB and 0 becomes dBFloor (-ve) */
  return (x > 0.0) ? bound_double((log10(x) * 20.0), dBFloor, 0.0) : dBFloor;
}
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------


Tartini *new_tartini(int n/*The size of the incoming buffers (windows)*/, double sample_rate /*The number of samples per second (often 44100)*/)
{
  const int myFFTMode = FFTW_ESTIMATE;
  Tartini *t = (Tartini*)malloc(sizeof(Tartini));
  t->n = n;
  t->k = n/2;
  t->size = n+t->k;
  t->sample_rate = sample_rate;
	t->noiseFloor = 0.0; //in dB

  t->autocorrTime = (float*)fftwf_malloc(sizeof(float) * t->size);
  t->autocorrFFT  = (float*)fftwf_malloc(sizeof(float) * t->size);
  t->autocorr_buf = (float*)fftwf_malloc(sizeof(float) * t->k);

  t->planAutocorrTime2FFT = fftwf_plan_r2r_1d(t->size, t->autocorrTime, t->autocorrFFT, FFTW_R2HC, myFFTMode);
  t->planAutocorrFFT2Time = fftwf_plan_r2r_1d(t->size, t->autocorrFFT, t->autocorrTime, FFTW_HC2R, myFFTMode);
  return t;
}
//--------------------------------------------------------------------------------------------------


void delete_tartini(Tartini *t)
{
  if(t) {
    fftwf_destroy_plan(t->planAutocorrFFT2Time);
    fftwf_destroy_plan(t->planAutocorrTime2FFT);
		fftwf_free(t->autocorr_buf);
    fftwf_free(t->autocorrFFT);
    fftwf_free(t->autocorrTime);
    free(t);
  }
}
//--------------------------------------------------------------------------------------------------


bool find_pitch(Tartini *t, kiss_fft_scalar *input, double *freq, double *logPower)
{
    // Process the input samples (bufferSize sample frames) that were read
    double power = nsdf(t, input, t->autocorr_buf);
    float x = 0.0f, y = 0.0f;
    int indexOfMax = 1;
    *logPower = linear2dB(power/(double)t->n); //MTB inserted /t->n (similar to main tartini app); otherwise power is not from 0->1, so linear2dB always clips to 0
    if(*logPower < t->noiseFloor) t->noiseFloor = *logPower;

    indexOfMax = findNSDFsubMaximum(t->autocorr_buf, t->k, 0.9);

    //do a parabola fit to find the maximum
    parabolaTurningPoint2(t->autocorr_buf[indexOfMax-1],
                          t->autocorr_buf[indexOfMax],
                          t->autocorr_buf[indexOfMax+1],
                          (float)(indexOfMax + 1), &x, &y);
    *freq = (double)(t->sample_rate) / x;
    y = bound_double(y, -1.0f, 1.0f);

    if (y > 0.6)
      	if(*logPower > t->noiseFloor + 50)// + 25.0)
            return true;
        else
            return false;
    else 
        return false;
}
//--------------------------------------------------------------------------------------------------

//This will be happy with integers provided the input vector can be squared without clipping
double nsdf(Tartini *t, const kiss_fft_scalar *input, float *output)
{
  double sumSq = autocorr(t, input, output); //the sum of squares of the input
  double totalSumSq = sumSq * 2.0;
  int j;

  //NSDF
  for(j=0; j<t->k; j++) {//###I *think* this is a signal-to-noise thing: so can I live with the big numbers everywhere instead?
    totalSumSq  -= sq(input[t->n-1-j]/32768.0f) + sq(input[j]/32768.0f);

    //dividing by zero is very slow, so deal with it seperately
    if(totalSumSq > 0.0) output[j] *= 2.0 / totalSumSq;
    else output[j] = 0.0;
  }

  return sumSq;
}
//--------------------------------------------------------------------------------------------------


//This will be happy with integers provided the input vector can be squared without clipping
//We probably don't need to normalise at the end...
extern double stdAutoCorr(const kiss_fft_scalar *input, float *output, unsigned long const tSize, unsigned long const pBiscN, unsigned long const tK);
double autocorr(Tartini *t, const __int32 *input, float *output)
{
  double dRet = 0;

  dRet = stdAutoCorr(input, output, t->size, t->n, t->k);
  return dRet;
}
//--------------------------------------------------------------------------------------------------


//!!!As far as I can see, this function will be happy with signed int inputs, 
//except threshold may change, and also presumably the 1.0's 
//It is just filtering findNSDFMaxima's output to be above 90% magnitude (given a threshold of 0.9); um, I think!
int s_aIndicies[1024]; //###need to #define to make common with input to new_tartini's n/2
int findNSDFsubMaximum(float *input, int len, float threshold)
{
  //push_back and resize (any kind of realloc) is very slow in WinCE
  //vector_int *indices = new_vector_int();
  int ret = 0, NumActualIndicies = 0;
  int overallMaxIndex = findNSDFMaxima(input, len, s_aIndicies, &NumActualIndicies);
  //!!!MTB: Moved down: you have to declare vars at the top in MS C: //threshold += (1.0 - threshold) * (1.0 - input[overallMaxIndex]);
  float cutoff = input[overallMaxIndex] * (threshold + (1.0 - threshold) * (1.0 - input[overallMaxIndex]));
  unsigned int j;
  for(j=0; j<NumActualIndicies; j++) {
    if(input[s_aIndicies[j]] >= cutoff) { ret = s_aIndicies[j]; break; }
  }
  //delete_vector_int(indices);
  return ret;
}
//--------------------------------------------------------------------------------------------------


int findNSDFMaxima(float *input, int len, int *maxPositions, int * NumActualIndicies) //!!!This function will be equally happy with (signed) int input, it just compares values to zero and each other
{
  int pos = 0;
  int curMaxPos = 0;
  int overallMaxIndex = 0;

  * NumActualIndicies = 0;

  while(pos < (len-1)/3 && input[pos] > 0.0f) pos++; //find the first negitive zero crossing
  while(pos < len-1 && input[pos] <= 0.0f) pos++; //loop over all the values below zero
  if(pos == 0) pos = 1; // can happen if output[0] is NAN
  
  while(pos < len-1) {
    //myassert(!(input[pos] < 0)); //don't assert on NAN
    if(input[pos] > input[pos-1] && input[pos] >= input[pos+1]) { //a local maxima
      if(curMaxPos == 0) curMaxPos = pos; //the first maxima (between zero crossings)
      else if(input[pos] > input[curMaxPos]) curMaxPos = pos; //a higher maxima (between the zero crossings)
    }
    pos++;
    if(pos < len-1 && input[pos] <= 0.0f) { //a negative zero crossing
      if(curMaxPos > 0) { //if there was a maximum
        //maxPositions.push_back(curMaxPos); //add it to the vector of maxima
        //push_back_int(maxPositions, curMaxPos); //add it to the vector of maxima //###push back, here is my problem!

        maxPositions[(*NumActualIndicies)++] = curMaxPos;

        if(overallMaxIndex == 0) overallMaxIndex = curMaxPos;
        else if(input[curMaxPos] > input[overallMaxIndex]) overallMaxIndex = curMaxPos;
        curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
      }
      while(pos < len-1 && input[pos] <= 0.0f) pos++; //loop over all the values below zero
    }
  }

  if(curMaxPos > 0) { //if there was a maximum in the last part
    //maxPositions.push_back(curMaxPos); //add it to the vector of maxima
    //push_back_int(maxPositions, curMaxPos); //add it to the vector of maxima
    maxPositions[(*NumActualIndicies)++] = curMaxPos;

    if(overallMaxIndex == 0) overallMaxIndex = curMaxPos;
    else if(input[curMaxPos] > input[overallMaxIndex]) overallMaxIndex = curMaxPos;
    curMaxPos = 0; //clear the maximum position, so we start looking for a new ones
  }
  return overallMaxIndex;
}
//--------------------------------------------------------------------------------------------------
