/***************************************************************************
                          FastSmoothAveragingFilter.h  -  description
                             -------------------
    begin                : 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef FAST_SMOOTHED_AVERAGING_FILTER_H
#define FAST_SMOOTHED_AVERAGING_FILTER_H

#include "Filter.h"

/** A class to perform smoothing/bluring on data using a hanning (cos shaped) window.
  * It uses and fast internal rotation algorithm
  * O(t) = n + size. Where n is the length of data, and size is the width of the filter
  */
class FastSmoothedAveragingFilter : public Filter
{
  int _size, _size_left, _size_right;
  double _angle;
  double _cos_angle, _sin_angle;
  double _sum;
  Array1d<float> _x; // the last size input values

  double cos_sum;
  double sin_sum;
  double total_sum;

public:
  FastSmoothedAveragingFilter() { }
  FastSmoothedAveragingFilter(int size);
  void init(int size);
  void filter(const float *input, float *output, int n);
  void reset();
  int delay() { return _size/2; }
};


#endif
