/***************************************************************************
                          useful.h  -  Generic useful functions
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
#ifndef USEFUL_H
#define USEFUL_H

#include <stdlib.h>


//to get the lrint and lrintf functions we must use the C99 defines
#define _ISOC9X_SOURCE  1
#define _ISOC99_SOURCE  1
#define __USE_ISOC9X  1
#define __USE_ISOC99  1
#include  <math.h>
//#endif //(defined (WIN32) || defined (_WIN32))

#define MIN(x,y)    ((x)<(y) ? (x) : (y))
#define MAX(x,y)    ((x)>(y) ? (x) : (y))
//#define MID(x,y,z)  MAX(MIN(x,y),z)
#define ABS(x)      ((x)<0 ? -(x) : (x))
#define SIGN(x)     (((x) == 0) ? 0 : (((x)>0) ? 1 : -1))

#define PI  3.14159265358979323846
#define twoPI 6.28318530717958647692



//returns x squared
inline double sq(const double x)
{
  return x*x;
}

//return x to the power of integer y
//works for all values of x and y
double powi(const double x, const int y);


#ifndef pow10
//rasises 10^x
inline double pow10(double x) { return pow(10.0, x); }
#endif


#ifdef WINDOWS
//From log rules  log_b(x) = log_a(x) / log_a(b)
inline double log2(double x) { return log(x) / 0.69314718055994530941723212145818; }
#endif

inline double logBaseN(double baseN, double x) {
  return log(x) / log(baseN);
}


/*cycle is like mod except it deals with negative numbers nicely*/
inline double cycle(const double a, const double b)
{
    if(a >= 0.0) return fmod(a, b);
    else return b + fmod(a, b);
}

inline int cycle(const int a, const int b)
{
    if(a >= 0) return a % b;
    else return b + ((a+1) % b) - 1;
}

inline double myround(const double x) {
  return floor(x + 0.5);
}

inline int toInt(const float x) {
  return lrintf(x);
  //return int(floor(x + 0.5));
}

inline int toInt(const double x) {
  return lrint(x);
  //return int(floor(x + 0.5));
}

//makes  lowerBound <= var <= upperBound
template<class T>
inline T bound(T var, T lowerBound, T upperBound)
{
/*
  if(var < lowerBound) var = lowerBound;
  if(var > upperBound) var = upperBound;
  return var;
*/
  //this way will deal with NAN, setting it to lowerBound
  if(var >= lowerBound) {
    if(var <= upperBound) return var;
    else return upperBound;
  } else return lowerBound;
}

/** Returns the x position of the min/max of a parabola
  @param a, b and c are the values of the function at -1, 0 and 1
  @return The x position, or zero if no solution.
*/
inline double parabolaTurningPoint(double a, double b, double c)
{
  double bottom = (2*(c + a - 2*b));
  return (bottom == 0.0) ? 0.0 : ((a - c) / bottom);
}

/** Calculate the x position and the y position of the min/max of a parabola at the same time
  @param y_1, y0 and y1 are the values of the function at xOffset-1, xOffset and xOffset+1
  @param xOffset the x value at which y0 is a function of
  @param x The x result will be put in here
  @param y The y result will be put in here
*/
template<class T>
inline void parabolaTurningPoint2(T y_1, T y0, T y1, T xOffset, T *x, T *y)
{
  T yTop = y_1 - y1;
  T yBottom = y1 + y_1 - 2 * y0;
  if(yBottom != 0.0) {
    *x = xOffset + yTop / (2 * yBottom);
    *y = y0 - (sq(yTop) / (8 * yBottom));
  } else {
    *x = xOffset;
    *y = y0;
  }
}


struct MinMax
{
    MinMax() 
	{
		min = 0.0;
		max = 0.0;
	};
    MinMax(float min_, float max_) 
	{
		min = min_;
		max = max_;
	};
    float min;
    float max;
};


// #include <utility>

/** Add sequence 'B' element-wise to sequence 'A' and store result in 'A'
*/
template <class ForwardIterator>
void addElements(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst)
{
  while(aFirst != aLast) { *aFirst += *bFirst; ++bFirst; ++aFirst; }
}

/** Add a scaled sequence 'B' element-wise to sequence 'A' and store result in 'A'
*/
template <class ForwardIterator, class ElementType>
void addElements(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType scaler)
{
  while(aFirst != aLast) { *aFirst += (*bFirst) * scaler; ++bFirst; ++aFirst; }
}


/** Copy element-wise 'A' into 'B' dividing by div
*/
template <class ForwardIterator, class ElementType>
void copyElementsDivide(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType div)
{
  while(aFirst != aLast) *bFirst++ = (*aFirst++) / div;
}

#include <functional>

template <class T>
inline T absolute(const T &x) { return (x >= 0) ? x : -x; }

template <class T>
struct absoluteLess : public std::binary_function<T, T, bool>
{
  bool operator()(const T &x, const T &y) const { return absolute(x) < absolute(y); }
};

template <class T>
struct absoluteGreater : public std::binary_function<T, T, bool>
{
  bool operator()(T &x, T &y) const { return absolute(x) > absolute(y); }
};


int nextPowerOf2(int x);

#include <algorithm>


/** Given an ordered sequence, 'A', return an iterator to the closest element to value
*/
template <class ForwardIterator, class ElementType>
ForwardIterator binary_search_closest(ForwardIterator aFirst, ForwardIterator aLast, const ElementType &value)
{
  std::pair<ForwardIterator, ForwardIterator> range = std::equal_range(aFirst, aLast, value);
  if(range.first != aFirst) --range.first;
  if(range.second != aLast) ++range.second;
  ForwardIterator best = range.first;
  ForwardIterator it = best;
  while(++it != range.second) {
    if(absolute(*it - value) < absolute(*best - value)) best = it;
  }
  return best;
}

#endif
