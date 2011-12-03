//#define _GLIBCXX_USE_C99
#define __WANT_EXTENSIONS__
//#include <math.h>
#include <cmath>
#include <cstdio>
//#define _GLIBCXX_USE_C99_COMPLEX 1
#include <complex>
#include <stdlib.h>

#include "prony.h"
#include "useful.h"
#include "mymatrix.h"
#include "array1d.h"



/** Prony Spectral Line Extimation (see S. Kay "Spectrum Analysis - A Modern Perspective" Pro. IEEE, Vol 69, #11, Nov 1981)
 * @param result Where the results of the calculation are stored
 * @param x The data to process
 * @param length The number of elements in x
 * @param gap The number samples used as a delay size (best is around 1/4 of a cycle)
 * @return true if the prony fit was a success. else false 
 */
//PronyData pronyFit(const float *x, int length, int gap, double dt, bool allowOffset)
bool pronyFit(PronyData *result, const float *x, int length, int gap, bool allowOffset)
{
    int j;
    double alpha[3];
    double omega;
    double error = 0.0;

    Array1d<float> x1(length);
    Array1d<float> x2(length);
    int gap2 = gap*2;
    int n = length - gap2;
    for(j=0; j < n; j++) {
      x1[j] = x[j]+x[j+gap2];
    }
    if(allowOffset) {
      if(!pinv(NULL, x+gap, x1.begin(), n, alpha)) return false;
      omega = acos(alpha[1]/2)/gap;
    } else {
      if(!pinv(x+gap, x1.begin(), n, alpha)) return false;
      omega = acos(alpha[0]/2)/gap;
    }
    if(std::isnan(omega)) return false;

    //result.freq = omega / (dt*TwoPi);
    result->omega = omega;
    for(j=0; j<length; j++) {
      x1[j] = cos(j*omega);
      x2[j] = sin(j*omega);
    }
    if(allowOffset) {
      if(!pinv(NULL, x1.begin(), x2.begin(), x, length, alpha)) return false;
      result->yOffset = alpha[0];
      result->amp = hypot(alpha[1], alpha[2]);
      result->phase = (HalfPi - atan2(alpha[2], alpha[1]));
    } else {
      if(!pinv(x1.begin(), x2.begin(), x, length, alpha)) return false;
      result->amp = hypot(alpha[0], alpha[1]);
      result->phase = (HalfPi - atan2(alpha[1], alpha[0]));
    }

    //calculate the squared error
    for(j=0; j<length; j++) {
      error += sq(result->amp * sin(j*omega + result->phase) + result->yOffset  -  x[j]);
    }
    result->error = error / length;

    return true;
}

