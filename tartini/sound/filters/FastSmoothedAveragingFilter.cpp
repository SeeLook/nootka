/***************************************************************************
                          FastSmoothAveragingFilter.cpp  -  description
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
#include <iostream>
#include "FastSmoothedAveragingFilter.h"
#include <algorithm>
#include "myassert.h"
#include "useful.h"

/** Construct a FastSmoothedAverageFilter
  * @param size The total width of the hanning window. To keep data centered use an odd size
  */
FastSmoothedAveragingFilter::FastSmoothedAveragingFilter(int size)
{
  init(size);
}

void FastSmoothedAveragingFilter::init(int size)
{
  _size = size;
  _size_left = _size / 2;
  _size_right = _size - _size_left;
  _angle = -2*M_PI/double(size+1); 
  _sin_angle = sin(_angle);
  _cos_angle = cos(_angle);
  _sum = 0;
  for(int j=0; j<size; j++) {
    _sum += 1.0 - cos((j+1)*_angle);
  }

  cos_sum = sin_sum = total_sum = 0.0;
  _x.resize(size);
  reset();
}

#include "fast_smooth.h"

void FastSmoothedAveragingFilter::filter(const float *input, float *output, int n)
{
  //blur stays centered if odd
  int j;
  if(n > _size) {
    for(j=0; j<_size; j++) {
      cos_sum += input[j];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= _x[j];
      total_sum += input[j] - _x[j];
      output[j] = (total_sum - cos_sum) / _sum;
    }
    for(j=_size; j<n; j++) {
      cos_sum += input[j];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= input[j-_size];
      total_sum += input[j] - input[j-_size];
      output[j] = (total_sum - cos_sum) / _sum;
    }
    std::copy(input+n-_size, input+n, _x.begin());
  } else {
    for(j=0; j<n; j++) {
      cos_sum += input[j];
      fast_complex_rotate(sin_sum, cos_sum, _sin_angle, _cos_angle);
      cos_sum -= _x[j];
      total_sum += input[j] - _x[j];
      output[j] = (total_sum - cos_sum) / _sum;
    }
    _x.shift_left(n);
    std::copy(input, input+n, _x.end()-n);
  }
}

void FastSmoothedAveragingFilter::reset()
{
  cos_sum = sin_sum = total_sum = 0.0;
  _x.fill(0.0f);
}
