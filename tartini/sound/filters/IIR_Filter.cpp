/***************************************************************************
                          IIR_Filter.cpp  -  description
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
#include "IIR_Filter.h"
#include <algorithm>
#include "myassert.h"
#include "useful.h"

/*
void IIR_Filter::make_FIR(double *b, uint n)
{
	_b.resize(n);
	copy(b, b+n, _b.begin());
	_a.clear();
  clear();
	gain = 1.0;
}
*/

/** Create an IIR Filter from the coeff's a direct form II transposed structure
  * if a[0] is not 1.0 then all the coefficients are normalized be dividing by a[0]
  */
IIR_Filter::IIR_Filter(double *b, double *a, int len_b, int len_a)
{
  init(b, a, len_b, len_a);
}

/** Initialise the IIR Filter from the coeff's a direct form II transposed structure
  * if a[0] is not 1.0 then all the coefficients are normalized be dividing by a[0]
  */
void IIR_Filter::init(double *b, double *a, int len_b, int len_a)
{
  if(len_a == -1) len_a = len_b; //same number of terms in a and b

  _b.resize_copy(b, len_b);
  _a.resize_copy(a+1, len_a-1);

  if(a[0] != 1.0) { //normalize
    _a /= a[0];
    _b /= a[0];
  }
  
  _x.resize(_b.size()-1);
  _y.resize(_a.size());
  reset();
	//gain = 1.0;
}

/** Resets the filter back to an initial state of zeros
  */
void IIR_Filter::reset()
{
  _x.fill(0.0);
  _y.fill(0.0);
}

void IIR_Filter::print()
{
	int j;
	for(j=0; j<_b.size(); ++j)
		std::cout << "b[" << j+1 << "] = " << _b[j] << std::endl;
  for(j=0; j<_a.size(); ++j)
    std::cout << "a[1.0 " << j << "] = " << _a[j] << std::endl;
}

/** Apply the filter to a single input value
  * @return The filtered value. Note: The output is delayed by the filter size
*/
/*
float IIR_Filter::filter(float input)
{
	double result = 0.0;
  uint j;

	_x.push_front(input);
	for(j=0; j<_b.size(); j++) result += _b[j] * _x[j];
	//print_deque(x);
	_x.pop_back();

	for(j=0; j<_a.size(); j++) result -= _a[j] * _y[j];
	//print_deque(y);
	_y.push_front(result);
	_y.pop_back();
	return result;
}
*/

/** Apply the filter to a block of data
  * @param input The data to be filtered
  * @param ourput Where the filtered result is stored. Note: The output is delayed by the filter size.
  * @param n The size of data
*/
void IIR_Filter::filter(const float *input, float *output, int n)
{
  int j, k;
  int sizeX = _x.size();
  int sizeY = _y.size();
  /*if(n+sizeX != bufx_size) {
    if(bufx) free(bufx);
    bufx_size = n+sizeX;
    bufx = (double *)malloc(bufx_size * sizeof(double));
  }
  if(m+sizeY != bufy_size) {
    if(bufy) free(bufy);
    bufy_size = m+sizeY;
    bufy = (double *)malloc(bufy_size * sizeof(double));
  }*/
  bufx.resize_raw(sizeX+n);
  bufy.resize_raw(sizeY+n);

  for(int j=0; j<sizeX; j++) bufx[j] = _x[j];
  for(int j=0; j<sizeY; j++) bufy[j] = _y[j];
  for(int j=0; j<n; j++) bufx[sizeX+j] = input[j];

  double *y = bufy.begin() + sizeY;
  double *x = bufx.begin() + sizeX;
  float *outy = output;

  /* do the special common case of 2, 2 more efficiently */
  if(sizeX == 2 && sizeY == 2) {
    //printf("special case of 2, 2\n");
    for(int j=0; j<n; j++) {
      double b0 = _b[0];
      double b1 = _b[1];
      double b2 = _b[2];
      double a0 = _a[0];
      double a1 = _a[1];
      //*y = _b[0]*x[0] + _b[1]*x[-1] + _b[2]*x[-2] - _a[0]*y[-1] - _a[1]*y[-2];
      *y = b0*x[0] + b1*x[-1] + b2*x[-2] - a0*y[-1] - a1*y[-2];
      //*outy++ = bound(*y, -1.0, 1.0);
      *outy++ = *y;
      y++;
      x++;
    }
    _x[0] = bufx[n];
    _x[1] = bufx[n+1];
    _y[0] = bufy[n];
    _y[1] = bufy[n+1];
  } else { /* General case */
    //printf("General case\n");
    for(j=0; j<n; j++) {
      *y = 0.0;
      for(k=0; k<sizeX+1; k++) *y += _b[k]*x[-k];
      for(k=0; k<sizeY;   k++) *y -= _a[k]*y[-k-1];
      //*outy++ = bound(*y, -1.0, 1.0);
      *outy++ = *y;
      y++;
      x++;
    }
    for(k=0; k<sizeX; k++) _x[k] = bufx[n+k];
    for(k=0; k<sizeY; k++) _y[k] = bufy[n+k];
  }

}

void IIR_Filter::getState(FilterState *filterState) const
{
  filterState->_x = _x;
  filterState->_y = _y;
}

void IIR_Filter::setState(const FilterState *filterState)
{
  _x = filterState->_x;
  _y = filterState->_y;
}
