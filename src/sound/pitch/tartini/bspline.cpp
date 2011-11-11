/***************************************************************************
   bspline.cpp  - 
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

#include "bspline.h"
#include <math.h>
#include "useful.h"

//a linear interpolation
//quicker than b-spline
inline float interpolate_linear(int len, const float *array, float x)
{
  int x0 = (int)x;
  //int x0 = lrint(x-0.4999);
  if(x0 >= 0 && x0 < len - 1) {
    float y0 = array[x0];
    float y1 = array[x0+1];
    x -= float(x0);
    return (y0 + x*(y1-y0));
  } else {
    if(x0 < 0) return array[0];
    else return array[len-1];
  }
}

//a cubic b-spline interpolation
//NOTE: the values don't always pass through the in points
//      but it gives a nice smooth curve
//slower than linear
float interpolate_b_spline(int len, const float *array, float x)
{
  int x0 = int(ceil(x));
  if((x0 < 0) || (x0 > len)) return 0; //{ printf("find_interpolate error\n"); exit(1); } //exit with an error
  float y_2, y_1, y0, y1;
  //check the end cases
  y_2 = (x0 <= 1) ? array[0] : array[x0-2];
  y_1 = (x0 == 0) ? array[0] : array[x0-1];
  y0 = (x0 >= len) ? y_1 : array[x0];
  y1 = (x0 >= len-1) ? y0 : array[x0+1];
  
  //if x is an integer value we can do very quick
  if(float(x0) == x) return (y_1 + 4*y0 + y1) / 6.0f;
  
  x = float(x0)-x;
  float xx = x*x;
  float xxx = xx*x;
  
  return (xxx*y_2 +
	  (1 + 3*(x + xx - xxx))*y_1 +
	  (4 - 6*xx + 3*xxx)*y0 +
	  (1 - 3*(x - xx) - xxx)*y1) / 6.0f;
}


//Uses a Hermite curve
float interpolate_cubic(int len, const float *data, double x)
{
	int inpos = int(x);
	float finpos = float(x - double(inpos));
	float xm1;
	//take care of boundary cases on the left
	if(inpos < 1) {
		if(inpos < 0)	return data[0]; //just return the left most value
		else
			xm1 = data[inpos];
	} else
		 xm1 = data[inpos - 1];

	float x0;
	float x1;
	float x2;
	//take care of boundary cases on the right
	if(inpos > len-3) {
		if(inpos > len-2) {
			if(inpos > len-1)	return data[len-1];
			else {
				x0 = x1 = x2 = data[inpos];
			}
		} else {
			x0 = data[inpos];
			x1 = x2 = data[inpos+1];
		}
	} else {
		x0 = data[inpos];
		x1 = data[inpos+1];
		x2 = data[inpos+2];
	}

	float a = (3 * (x0-x1) - xm1 + x2) / 2;
	float b = 2*x1 + xm1 - (5*x0 + x2) / 2;
	float c = (x1 - xm1) / 2;
	return (((a * finpos) + b) * finpos + c) * finpos + x0;
}


//#include <fenv.h>
//#include <algorithm>

//NOTE: Only LINEAR has been optimised a little for speed. The others could be made more efficient
//in_len & *in are the array of floats for input
//out_len & *out are the array where the (resampled) stretched array output is stored
//start = where the to start resampling from the *in array
//len = the length of the resampling from the *in array
//type = LINEAR or BSPLINE
void stretch_array(int in_len, const float *in, int out_len, float *out, float start, float len, int type)
{
  float x = start;
  //float x = (start >= 0) ? start : 0.0f;
  //if(floor(start + len) >= in_len-1) len = float(in_len-2) - start; //FIXME: should pad the ending, not squash up array
  float step = len / float(out_len);
  
  if(type == LINEAR) {
    for(int j=0; j<out_len; j++) {
      out[j] = interpolate_linear(in_len, in, x);
      x += step;
    }
/*
    int prevRoundMode = fegetround();
    fesetround(FE_DOWNWARD);
    int j=0;
    //while(x < 0.0f) { out[j++] = 0.0f; x+=step; }
    //int small_out_len = std::min(lrint(start + out_len*step), in_len-2);
    while(j<out_len) {
      int x0 = lrint(x);
      //assert(x0 >= 0);
      //printf("j=%d, %d, %f, %d\n", j, x0, x, in_len-2);
      if(x0 < 0 || x0 > len - 2) { out[j++]=0; continue;}  //{ printf("find_interpolate error\n"); exit(1); } //exit with an error
      //assert(x0 <= in_len - 2);
      float y0 = in[x0];
      //float y1 = in[x0+1];
      out[j++] = y0 + (x-(float)x0)*(in[x0+1]-y0);
      x += step;
    }
    //while(j<out_len) {
    //  out[j] = 0.0f;
    //}
    fesetround(prevRoundMode);
    */
  } else if(type == BSPLINE) {
      for(int j=0; j<out_len; j++) {
	  out[j] = interpolate_b_spline(in_len, in, x);
	  x += step;
      }
  } else {
      for(int j=0; j<out_len; j++) {
	  out[j] = interpolate_cubic(in_len, in, x);
	  x += step;
      }
  }
}
