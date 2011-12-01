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
#ifndef TARTINI_CORE_H
#define TARTINI_CORE_H

#ifdef __cplusplus
extern "C" {
#endif


#include <fftw3.h>
//#include <vector>
//#include <cmath>
#define __USE_ISOC99 1
#include <math.h>

#ifndef bool
#define bool int
#define false 0
#define true 1
#endif

//!!!Equivalent of public functions:
typedef struct {
  int n, k, size;
  fftwf_plan planAutocorrTime2FFT, planAutocorrFFT2Time;
  float *autocorrTime, *autocorrFFT;
  double sample_rate;
  float *autocorr_buf;
	double noiseFloor; //in dB
} Tartini;


//Usage: 
//      int buffer_size = 2048; // in frames
//      int step_size = 1024; // in frames
//      int sample_rate = 44100;
//      float * buffer = (float*)malloc(buffer_size * sizeof(float));
//
//      Tartini t* = new_tartini(buffer_size, sample_rate)
//
//      Loop:
//      //Populate_Buffer(buffer);
//		valid = find_pitch(t, buffer, &freq, &logPower);
//		pitch = freq2pitch(freq);
//		//print_tuning(pitch, 1.0 - (logPower / t->noiseFloor), valid);

Tartini *new_tartini(int n, double sample_rate);
void delete_tartini(Tartini *t);

/**
  @param input An array of floats to be processed to find the pitch of
	@param logPower The volume is returned in this
	@param freq The freq is returned in this
*/
bool find_pitch(Tartini *t, kiss_fft_scalar *input, double *freq, double *logPower);

//Also freq2pitch and toInt may be useful
//!!!Ends


const double dBFloor = -120.0; //in dB
extern char *noteNames[12];

char* noteName(int pitch);
/** Performs an autocorrelation on the input
  @param input An array of length n, in which the autocorrelation is taken
  @param ouput This should be an array of length k.
               This is the correlation of the signal with itself
               for delays 1 to k (stored in elements 0 to k-1)
  @return The sum of squares of the input. (ie the zero delay correlation)
  Note: Use the init function to set values of n and k before calling this.
*/
double autocorr(Tartini *t, const __int32 *input, float *output);
/** The Normalised Square Difference Function.
    @param input. An array of length n, in which the ASDF is taken
    @param ouput. This should be an array of length k
    @return The sum of squares of the input
*/
double nsdf(Tartini *t, const kiss_fft_scalar *input, float *output);

//int findNSDFMaxima(float *input, int len, std::vector<int> &maxPositions);
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
int findNSDFMaxima(float *input, int len, int *maxPositions /*Array of length "len"*/, int * NumActualIndicies /*returns Number of populated entries in maxPositions*/);
/** @return The index of the first sub maximum.
  This is now scaled from (threshold * overallMax) to 0.
*/
int findNSDFsubMaximum(float *input, int len, float threshold);

/*
template<class T>
inline T bound(T var, T lowerBound, T upperBound)
{
  //this way will deal with NAN, setting it to lowerBound
  if(var >= lowerBound) {
    if(var <= upperBound) return var;
    else return upperBound;
  } else return lowerBound;
}
*/

//C version
__inline double bound_double(double var, double lowerBound, double upperBound)
{
  //this way will deal with NAN, setting it to lowerBound
  if(var >= lowerBound) {
    if(var <= upperBound) return var;
    else return upperBound;
  } else return lowerBound;
}


/** Calculate the x position and the y position of the min/max of a parabola at the same time
  @param y_1, y0 and y1 are the values of the function at xOffset-1, xOffset and xOffset+1
  @param xOffset the x value at which y0 is a function of
  @param x The x result will be put in here
  @param y The y result will be put in here
*/
/*
template<class T>
inline void parabolaTurningPoint2(T y_1, T y0, T y1, T xOffset, T *x, T *y)
{
  T yTop = y_1 - y1;
  T yBottom = y1 + y_1 - 2 * y0;
  if(yBottom != 0.0) {
    *x = xOffset + yTop / (2 * yBottom);
    *y = y0 - ((yTop*yTop) / (8 * yBottom));
  } else {
    *x = xOffset;
    *y = y0;
  }
}
*/
__inline void parabolaTurningPoint2(float y_1, float y0, float y1, float xOffset, float *x, float *y)
{
  float yTop = y_1 - y1;
  float yBottom = y1 + y_1 - 2 * y0;
  if(yBottom != 0.0) {
    *x = xOffset + yTop / (2 * yBottom);
    *y = y0 - ((yTop*yTop) / (8 * yBottom));
  } else {
    *x = xOffset;
    *y = y0;
  }
}


/** Converts the frequencies freq (in hertz) into their note number on the midi scale
    i.e. the number of semi-tones above C0
    Note: The note's pitch will contain its fractional part
    Reference = http://www.borg.com/~jglatt/tutr/notenum.htm
  @param freq The frequency in Hz
  @return The pitch in fractional part semitones from the midi scale.
*/

//!!!
  //  Win32 doesn't seem to have the lrint or lrintf functions.
  //  Therefore implement inline versions of these functions here.

  __inline long int 
  lrint (double flt)
  {
    return (int)(floor(flt + 0.5));
    /*!!!WinCE won't do this of course (in non-intel); what is lrint anyway? Is it the above?
    int intgr;
    _asm { fld flt
      fistp intgr
      } ;
    return intgr ;
    */
  } 
  
  __inline long int 
  lrintf (float flt)
  { 
    return (int)(floor(flt + 0.5));
    /*!!!WinCE won't do this of course (in non-intel); what is lrint anyway? Is it the above?
    int intgr;
    _asm
    { fld flt
      fistp intgr
      } ;
    return intgr ;
    */
  }
//!!!



__inline double freq2pitch(double freq)
{
#ifdef log2
	return -36.3763165622959152488 + 12.0*log2(freq);
#else
	return -36.3763165622959152488 + 39.8631371386483481*log10(freq);
#endif
}

__inline int toInt(const float x) {
  return lrintf(x);
  //return int(floor(x + 0.5));
}

/*
inline int toInt(const double x) {
  return lrint(x);
  //return int(floor(x + 0.5));
}
*/

__inline int cycle(const int a, const int b) //###Called by RtAudio.cpp: Hmm!
{
    if(a >= 0) return a % b;
    else return b + ((a+1) % b) - 1;
}

#ifdef __cplusplus
}
#endif

#endif //TARTINI_CORE_H

