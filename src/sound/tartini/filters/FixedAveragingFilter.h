/***************************************************************************
                          FixedAveragingFilter.h  -  description
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
#ifndef FIXED_AVERAGING_FILTER_H
#define FIXED_AVERAGING_FILTER_H

#include "Filter.h"

/** An unwindowed (rectangular window) simple averaging filter with a fixed size
  * It uses a simple incremental algorithm
  */
class FixedAverageFilter : public Filter
{
  int _size;
  double total_sum;
  Array1d<float> _x; // the last size input values

 public:
  FixedAverageFilter() { }
  FixedAverageFilter(int size);
  void init(int size);
  void filter(const float *input, float *output, int n);
  void reset();
  int delay() { return _size/2; }
};

#endif
