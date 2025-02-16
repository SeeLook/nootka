/***************************************************************************
   bspline.h  - 
    Functions for interpolating between values in an array.
    Both linear and cubic b-spline functions
                             -------------------
    begin                : 2002
    copyright            : (C) 2002-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/
#ifndef BSPLINE_H
#define BSPLINE_H

#define LINEAR           0
#define BSPLINE          1
#define HERMITE_CUBIC    2

float interpolate_linear(int len, const float *array, float x);
float interpolate_b_spline(int len, const float *array, float x);
float interpolate_cubic(int len, const float *data, double x);
void stretch_array(int in_len, const float *in, int out_len, float *out, float start, float len, int type);

#endif
