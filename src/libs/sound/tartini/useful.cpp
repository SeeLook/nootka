/***************************************************************************
                          useful.cpp  -  Generic useful functions
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
#include <string.h>
#include <math.h>

#include "useful.h"
#include <stdio.h>
#include <errno.h>



//return x to the power of integer y
//works for all values of x and y
double powi(const double x, const int y)
{
    if(y > 1) {
	if(y & 0x01) return x*powi(x*x, y>>1);
	else return powi(x*x, y>>1);
    }
    else if(y == 1) return x;
    else if(y == 0) return 1;
    else /*if(y < 0)*/ return 1.0/powi(x, -y);
}


//sets max_pos to the position with the max value in the array
//sets max_value to the max value in the array
//max_pos and/or max_value can be NULL
void max_array(int n, float *data, int *max_pos, float *max_value)
{
  int pos = 0;
  float value = data[0];
  for(int j=1; j<n; j++)
    if(data[j] > value) { value=data[j]; pos=j; }
  if(max_pos) *max_pos = pos;
  if(max_value) *max_value = value;
}


/** Returns the power of 2 greater or equal to x
  */
int nextPowerOf2(int x)
{
  int y = 1;
  while(y < x) y <<= 1;
  return y;
}

