#ifndef PRONY_H
#define PRONY_H

const double Pi = 3.1415926535897932384;
const double TwoPi = 3.1415926535897932384 * 2.0;
const double HalfPi = 3.1415926535897932384 * 0.5;

struct PronyData { //fitted parameters
  double amp;     //amplitude
  double phase;   //phase in radians
  double omega;     //frequency in radians per unit
  double yOffset;
  double error;   //squared error of the fit

  PronyData() { reset(); }
  void reset() { amp = phase = omega = yOffset = error = 0.0; }

  double freqHz(double dt) { return omega / (dt*TwoPi); } //dt is the time between samples in the input.
};

//PronyData pronyFitOld(const float *x, int length, int gap=1, double dt=1.0);
//PronyData pronyFit(const float *x, int length, int gap=1, double dt=1.0, bool allowOffset=false);
//PronyData pronyFit(const float *x, int length, int gap=1, bool allowOffset=false);
bool pronyFit(PronyData *result, const float *x, int length, int gap, bool allowOffset);

#endif
