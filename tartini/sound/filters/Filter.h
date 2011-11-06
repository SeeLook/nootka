/***************************************************************************
                          Filter.h  -  description
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
#ifndef FILTER_H
#define FILTER_H

#include "array1d.h"

typedef unsigned int uint;

class Filter
{
public:
  //virtual float filter(float input) = 0;
  virtual void filter(const float *input, float *output, int n) = 0;
  virtual void reset() = 0;
  virtual int delay() { return 0; }
  //virtual void getState(FilterState *filterState) const = 0;
  //virtual void setState(const FilterState *filterState) = 0;
  virtual ~Filter() { }
};

#endif
