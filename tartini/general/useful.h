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

/*
#if (defined (WIN32) || defined (_WIN32))
  #include  <math.h>
  //  Win32 doesn't seem to have the lrint or lrintf functions.
  //  Therefore implement inline versions of these functions here.
  __inline long int 
  lrint (double flt)
  { int intgr;
    _asm { fld flt
      fistp intgr
      } ;
    return intgr ;
  } 
  
  __inline long int 
  lrintf (float flt)
  { int intgr;
    _asm
    { fld flt
      fistp intgr
      } ;
    return intgr ;
  }
#else
*/
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

//return a pointer to a 2d array with each element of size size
void **malloc2d(const int row, const int col, const int size);
//like malloc2d but changes an existing 2d array
void **realloc2d(void **ptr, const int row, const int col, const int old_rol, const int old_col, const int size);
//frees the memory created by malloc2d
void free2d(void **ptr, const int row);

//returns x squared
inline double sq(const double x)
{
  return x*x;
}

//return x to the power of integer y
//works for all values of x and y
double powi(const double x, const int y);

double nearestPowerOf2(double x);

#ifndef pow10
//rasises 10^x
inline double pow10(double x) { return pow(10.0, x); }
#endif

#ifndef pow2
//rasises 2^x
inline double pow2(double x) { return pow(2.0, x); }
#endif

#ifdef WINDOWS
//From log rules  log_b(x) = log_a(x) / log_a(b)
inline double log2(double x) { return log(x) / 0.69314718055994530941723212145818; }
#endif

inline double logBaseN(double baseN, double x) {
  return log(x) / log(baseN);
}

//return a random number between 0 and 1 (inclusive)
inline double prand()
{
  return double(rand()) / double(RAND_MAX);
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

inline int intFloor(const float x) {
  return lrintf(x-0.5);
}

inline int intFloor(const double x) {
  return lrint(x-0.5);
}

/*Round x up to the nearest multiple of multiple*/
inline int roundUp(const int x, const int multiple)
{
  int remainder = x % multiple;
  return (remainder == 0) ? x : x - remainder + multiple;
}

/** Returns true if value1 is within plusMinus of value2 (inclusive)
*/
template<class T>
inline bool within(T plusMinus, T value1, T value2)
{
  return (value1 >= value2-plusMinus) ? (value1 <= value2+plusMinus) : false;
}

/** Returns true if x is between lowerBound and upperBound (inclusive)
*/
template<class T>
inline bool between(T x, T lowerBound, T upperBound)
{
  return (x >= lowerBound) && (x <= upperBound);
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

/*
struct MinMax
{
    MinMax::MinMax() : min(0.0) , max(0.0) {};
    MinMax::MinMax(float min_, float max_) : min(min_) , max(max_) {};
    float min;
    float max;
};
*/
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
/* Returns the minimum & maximum values between two pointers */
MinMax minMax(float *begin, float *end);
/* Returns the minimum & maximum values between two pointers, ignoring anything outside the two bounds (exclusive) */
MinMax minMax(float *begin, float *end, float lowBound, float highBound);
/* Returns the average value between two pointers */
float average(float *begin, float *end);
/* Returns the average value between two pointers ignoring anything outside the two bounds (exclusive) */
float average(float *begin, float *end, float lowBound, float highBound);
int calcIndex(double frameTime, double baseX, int size);


/*//makes  lowerBound <= var <= upperBound
template<class T>
void bound(T *var, T lowerBound, T upperBound)
{
  if(*var < lowerBound) *var = lowerBound;
  if(*var > upperBound) *var = upperBound;
}
*/

/** Return the index with the maximum value in an array.
  * If more than one value with the maximum, the first is returned.
  * @param aFirst Pointer to the beginning of the array
  * @param length The length of the array
  * @return The index of the maximum
  */
template <class ForwardIterator>
int maxIndex(ForwardIterator aFirst, int length)
{
  int bestIndex = 0;
  for(int j=1; j<length; j++) { if(aFirst[j] > aFirst[bestIndex]) bestIndex = j; }
  return bestIndex;
}

/** Return the index with the minimum value in an array.
  * If more than one value with the minimum, the first is returned.
  * @param aFirst Pointer to the beginning of the array
  * @param length The length of the array
  * @return The index of the minimum
  */
template <class ForwardIterator>
int minIndex(ForwardIterator aFirst, int length)
{
  int bestIndex = 0;
  for(int j=1; j<length; j++) { if(aFirst[j] < aFirst[bestIndex]) bestIndex = j; }
  return bestIndex;
}

#include <utility>

/** minMaxElement returns an iterator to the min and max values between __first and __last, in a pair
    Note: This is like the std::min_element and std::max_element functions, but performs both at the
    same time, so should be more efficient
    @return a std::pair of which .first is an iterator to the min, and .second to the max
*/
//template<typename _ForwardIter, typename _Compare1, typename _Compare2>
//std::pair<_ForwardIter, _ForwardIter> minMaxElement(_ForwardIter __first, _ForwardIter __last, _Compare1 __lessComp, _Compare2 __greaterComp)
template<typename _ForwardIter, typename _Compare>
std::pair<_ForwardIter, _ForwardIter> minMaxElement(_ForwardIter __first, _ForwardIter __last, _Compare __lessComp)
{
  // concept requirements
  //__glibcpp_function_requires(_ForwardIteratorConcept<_ForwardIter>)
  //__glibcpp_function_requires(_BinaryPredicateConcept<_Compare,
  //typename iterator_traits<_ForwardIter>::value_type,
  //typename iterator_traits<_ForwardIter>::value_type>)

  std::pair<_ForwardIter, _ForwardIter> __result(__first, __first);
  if (__first == __last) return __result;
  while (++__first != __last) {
	  if (__lessComp(*__first, *__result.first)) __result.first = __first;
    //if (__greaterComp(*__first, *__result.second)) __result.second = __first;
    if (__lessComp(*__result.second, 
    *__first))
     __result.second = __first;
  }
  return __result;
}

/** Add sequence 'B' element-wise to sequence 'A' and store result in 'A'
*/
template <class ForwardIterator>
void addElements(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst)
{
  //while(aFirst != aLast) *bFirst++ += *aFirst++;
  //while(aFirst != aLast) { *bFirst += *aFirst; bFirst++; aFirst++; }
  while(aFirst != aLast) { *aFirst += *bFirst; ++bFirst; ++aFirst; }
}

/** Add a scaled sequence 'B' element-wise to sequence 'A' and store result in 'A'
*/
template <class ForwardIterator, class ElementType>
void addElements(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType scaler)
{
  //while(aFirst != aLast) *bFirst++ += *aFirst++;
  while(aFirst != aLast) { *aFirst += (*bFirst) * scaler; ++bFirst; ++aFirst; }
}

/** Copy element-wise 'A' into 'B' scaling the scaler
*/
template <class ForwardIterator, class ElementType>
void copyElementsScale(ForwardIterator aFirst, ForwardIterator aLast, ForwardIterator bFirst, ElementType scaler)
{
  while(aFirst != aLast) *bFirst++ = (*aFirst++) * scaler;
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

bool copyFile(const char *src, const char *dest);
bool moveFile(const char *src, const char *dest);

int nextPowerOf2(int x);

#include <algorithm>

/*
template<class ForwardIterator, class ElementType>
ForwardIterator binary_search_closest(ForwardIterator first, ForwardIterator last, const ElementType &value)
{
  ForwardIterator i = std::lower_bound(first, last, value);
  if(i < last-1) {
    if(fabs(value - *i) < fabs(value - *(i+1))) return i;
    else return i+1;
  }
  return i;
}
*/

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

#include <iostream>

/** Print out a range of elements to cout. eg [1 2 3 4]
*/
template <class ForwardIterator>
ForwardIterator print_elements(ForwardIterator aFirst, ForwardIterator aLast)
{
  std::cout << "[";
  if(aFirst != aLast) std::cout << *aFirst++;
  while(aFirst != aLast) std::cout << " " << *aFirst++;
  std::cout << "]" << std::endl;
}

#endif
