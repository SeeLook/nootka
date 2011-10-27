/***************************************************************************
                          FixedAveragingFilter.cpp  -  description
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
#include "FixedAveragingFilter.h"
#include <algorithm>
#include "myassert.h"
#include "useful.h"

FixedAverageFilter::FixedAverageFilter(int size)
{
  init(size);
}

void FixedAverageFilter::init(int size)
{
  _size = size;
  total_sum = 0;
  _x.resize(size);
  reset();
}

void FixedAverageFilter::filter(const float *input, float *output, int n)
{
  int j;
  if(n > _size) {
    for(j=0; j<_size; j++) {
      total_sum += input[j] - _x[j];
      output[j] = total_sum / _size;
    }
    for(j=_size; j<n; j++) {
      total_sum += input[j] - input[j-_size];
      output[j] = total_sum / _size;
    }
    std::copy(input+n-_size, input+n, _x.begin());
  } else {
    for(j=0; j<n; j++) {
      total_sum += input[j] - _x[j];
      output[j] = total_sum / _size;
    }
    _x.shift_left(n);
    std::copy(input, input+n, _x.end()-n);
  }
}

void FixedAverageFilter::reset()
{
  total_sum = 0.0;
  _x.fill(0.0f);
}
