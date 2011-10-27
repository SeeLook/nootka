/***************************************************************************
                          array1d.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef ARRAY1D_H
#define ARRAY1D_H

//#include <algorithm>
#include <iostream>
#include <memory>
//#define NDEBUG //removes the asserts
#include "myassert.h"
#include <stdlib.h>
#include "useful.h"

/** Warning!: Use only for arrays of basic types or pointers
    Constructors are not called on creation of elements
    Destructors are not called on the elements in the array when removed
  */


// 1 dimentional array
template<class T> class Array1d
{
 private:
  T* data;
  int dataSize;
  int allocatedSize;

 public:

  /** Construct an empty Array1d
    */
  Array1d() {
	  dataSize = 0;
    allocatedSize = 0;
	  data = NULL;
  }

  /** Construct an Array1d of size length. The values are uninitialised
    * @param length The size of the new Array1d
    */
  Array1d(int length) {
    dataSize = length;
    allocatedSize = nextPowerOf2(dataSize);
    data = (T*)malloc(allocatedSize * sizeof(T));
    myassert(data != NULL);
  }

  /** Construct an Array1d of size length. The values are all initialied to val
    * @param length The size of the new Array1d
    * @param val Values will initialied to this
    */
  Array1d(int length, const T &val) {
    dataSize = length;
    allocatedSize = nextPowerOf2(dataSize);
    data = (T*)malloc(allocatedSize * sizeof(T));
    myassert(data != NULL);
    fill(val);
  }

  /** Construct an Array1d of size length
    * Initalise the values from (copying) an exsiting array
    * @param src The array to copy from
    * @param length The amount of data to copy and the new size of this array.
    */
  Array1d(const T *src, int length) {
    dataSize = length;
    allocatedSize = nextPowerOf2(dataSize);
    data = (T*)malloc(allocatedSize * sizeof(T));
    myassert(data != NULL);
    for(T *p = data; p != end();) *p++ = *src++;
  }

  /** Copy constructor
    * @param r The Array1d to (deep) copy
    */
  Array1d(Array1d<T> const &r) {
    dataSize = r.size();
    allocatedSize = nextPowerOf2(dataSize);
    data = (T*)malloc(allocatedSize * sizeof(T));
    //copy_raw(r.begin(), dataSize);
    copy_raw(r.begin());
  }

  /** Assignment operator. Loses old values and copys from r
    * @param r The Array1d to (deep) copy
    */
  Array1d<T> &operator = (Array1d<T> const &r) {
    resize_copy(r.begin(), r.size());
    return *this;
  }

  ~Array1d() {
    if(data) free(data);
  }
  T& operator()(int x) { return at(x); }
  T& operator[](int x) { return at(x); }
  const T& operator[](int x) const { return at(x); }
  T& at(int x) {
    myassert(data != NULL);
    myassert(x >= 0 && x < size());
    return data[x];
  }
  const T& at(int x) const {
    myassert(data != NULL);
    myassert(x >= 0 && x < size());
    return data[x];
  }
  int size() const { return dataSize; }
  T *begin() const { return data; }
  T *end() const { return data+dataSize; }
  bool isEmpty() { return (data==NULL); }

  T &front() const { return *data; }
  T &back() const { return data[dataSize-1]; }
  int capacity() { return allocatedSize; }
  int getIndex(T *element) { return element - data; }

  /** Resizes this to newSize, making all data uninitized
    * If newSize == size() then it dosn't do anything
    */
  void resize_raw(int newSize) {
    if(newSize == dataSize) return;
    if(newSize > allocatedSize) {
      if(data) free(data);
      allocatedSize = nextPowerOf2(newSize);
      data = (T*)malloc(allocatedSize * sizeof(T)); //I think this is faster than realloc
    }
    dataSize = newSize;
  }

   /** Resize this, any new values will be uninitized
     */
  void resize(int newSize) {
    if(newSize > allocatedSize) {
      allocatedSize = nextPowerOf2(newSize);
      data = (T*)realloc(data, allocatedSize * sizeof(T));
    }
    dataSize = newSize;
  }

   /** Resize this, any new values will be initized to val
     */
  void resize(int newSize, const T &val) {
    if(newSize > allocatedSize) {
      allocatedSize = nextPowerOf2(newSize);
      data = (T*)realloc(data, allocatedSize * sizeof(T));
    }
    if(newSize > dataSize) {
      T *theEnd = data+newSize;
      for(T* p=data+dataSize; p<theEnd;) *p++ = val;
      //for(T* p=data+dataSize; p<theEnd;) std::_Construct(p++, val);
    }
    dataSize = newSize;
  }

  /** Resizes to newSize copying data from array
    * @param src The array to copy from
    * @param length The amount of data to copy and the new size of this array.
    */
  void resize_copy(const T *src, int length) {
    resize_raw(length);
    //copy_raw(src, length);
    copy_raw(src);
  }

  /** Grow the array by one, putting val on the end
    * @param val The value to add to the end of the array
    */
  void push_back(const T &val) {
    if(++dataSize > allocatedSize) {
      allocatedSize = nextPowerOf2(dataSize);
      data = (T*)realloc(data, allocatedSize * sizeof(T));
    }
    back() = val;
  }

  /** Shrink the array by one
    * Note: Use back() to get the value before pop_back() if needed
    * Note: The array should not be empty. I.e no checking is done
    */
  void pop_back() {
    dataSize--;
  }

  /** Copies data from src to this, assuming this is large enough to hold it
    * @param src The array to copy from
    * @param length The amount of data to copy
    */
  //void copy_raw(const T *src, int length) {
  void copy_raw(const T *src) {
    T *theEnd = end();
    for(T *p = begin(); p != theEnd;) *p++ = *src++;
  }

  /** Empites the array back to size 0
    */
  void clear() {
    if(data) { free(data); data = NULL; }
	  dataSize = 0;
    allocatedSize = 0;
  }

  /** Fill the whole array with val
    */
  void fill(const T &val) {
    T *theEnd = end();
    for(T *p = begin(); p != theEnd;) *p++ = val;
  }

  /** Multiply each element by val
    */
  void operator*=(const T &val) {
    T *theEnd = end();
    for(T *p = begin(); p != theEnd;) *p++ *= val;
  }

  /** Divide each element by val
    */
  void operator/=(const T &val) {
    T *theEnd = end();
    for(T *p = begin(); p != theEnd;) *p++ /= val;
  }

  /** Shift all values to the left by n.
    * losing the first n values and not initialising the last n
    */
  void shift_left(int n) {
	  if(n < 1 || n >= size()) return;
    memmove(begin(), begin()+n, (size()-n)*sizeof(T));
  }

  /** Shift all values to the right by n.
    * losing the last n values and not initialising the first n
    */
  void shift_right(int n) {
	  if(n < 1 || n >= size()) return;
    memmove(begin()+n, begin(), (size()-n)*sizeof(T));
  }
};

template<class T>
std::ostream& operator<<(std::ostream &o, Array1d<T> &a)
{
  o << '[';
  for(int j=0; j<a.size(); j++) {
    o << a(j);
    if(j != a.size()-1) o << ' ';
  }
  o << ']' << std::endl;
  return o;
}

#endif
