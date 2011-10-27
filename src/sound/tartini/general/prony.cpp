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
 * @param x The data to process
 * @param length The number of elements in x
 * @param gap The number samples used as a delay size (best is around 1/4 of a cycle)
 * @param dt Time between samples
 */
/*
PronyData pronyFitOld(const float *x, int length, int gap, double dt)
{
    int j;
    double sumX1X2=0.0, sumX2X2=0.0, sumX2=0.0, real_z1, phase;
    std::complex<double> z1, invz1, invz1sqN, Om12, c, d1, b1;
    PronyData result;

    for (j=gap; j < length-gap; j++){
      sumX1X2 += (x[j-gap] + x[j+gap]) * x[j];
      sumX2X2 += x[j] * x[j];
    }
    sumX1X2 *= 2;
    sumX2X2 *= 4;
    real_z1 = sumX1X2 / sumX2X2;
    z1 = std::complex<double>(real_z1, sqrt(1-real_z1*real_z1));

    result.freq = arg(z1) / (dt*TwoPi);

    invz1 = std::conj(z1); // valid as norm(z1) = 1
    invz1sqN = std::polar(1.0, double(length*2.0*arg(invz1)));
    Om12 = (1.0-invz1sqN)/(1.0-invz1*invz1);
    c=1.0;
    d1=0.0;
    for (j=0; j<length; j++) {
      d1 += c * double(x[j]);
      c *= invz1;
    }
    b1 = (double(length)*d1 - Om12*conj(d1)) / (length*length - norm(Om12));
    //phase = arg(b1)/(TwoPi) + 0.25 + dt * jmiddle * result.f.val; //0.25 converts cos to sin. phase is measured at jmiddle
    phase = arg(b1)/(TwoPi) + 0.25; //0.25 converts cos to sin. phase at beginning
    result.phase = phase-floor(phase);
    result.amp = 2*abs(b1);

    return result;
}
*/

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
    myassert(result != NULL);
    myassert(x != NULL);
    int j;
    //double sumX1X2=0.0, sumX2X2=0.0, sumX2=0.0, real_z1, phase;
    //double XX[4], XXinv[4], XY[2];
    double alpha[3];
    double omega;
    double error = 0.0;
    //std::complex<double> z1, invz1, invz1sqN, Om12, c, d1, b1;
    //PronyData result;

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

/** A random number between -1 and 1 */
/*
double myrand() {
    return (double(rand()) / RAND_MAX) * 2.0 - 1.0;
}

void testProny()
{
    const int size = 128;
    double freq = 8.43;
    double amp = 2.5;
    double phase = 0.27 * TwoPi;
    double yOffset = 0.43;
    float x[size];
    double noise;
    for(int j=0; j<size; j++) {
      //x[j] = sin(double(j)/size * TwoPi * freq + phase) * amp;
      noise = myrand() * 0.00;
      x[j] = (sin(double(j)/size * TwoPi * freq + phase) + noise)* amp + yOffset;
      //printf("%f\n", x[j]);
      //printf("%f\n", noise);
    }
    PronyData d = pronyFit(x, size, 1, 1.0/128.0);
    printf("Frequency = %f\n", d.freq);
    printf("Amplitude = %f\n", d.amp);
    printf("Phase     = %f\n", d.phase);
    printf("yOffset   = %f\n\n", d.yOffset);

    d = pronyFit2(x, size, 1, 1.0/128.0, false);
    printf("Frequency = %f\n", d.freq);
    printf("Amplitude = %f\n", d.amp);
    printf("Phase     = %f\n", d.phase);
    printf("yOffset   = %f\n\n", d.yOffset);

    d = pronyFit2(x, size, 1, 1.0/128.0, true);
    printf("Frequency = %f\n", d.freq);
    printf("Amplitude = %f\n", d.amp);
    printf("Phase     = %f\n", d.phase);
    printf("yOffset   = %f\n\n", d.yOffset);
}
*/
