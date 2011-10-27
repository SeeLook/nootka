/***************************************************************************
                          array2d.h  -  description
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

/*
I prefer this than the vector<vector> > approach
I believe this is faster
And array2d give you raw pointers to use rather than all that iterator crap
Note: You can still use alot of the c++ algorithm template stuff on array2d
Warning! Array2d uses malloc so no Constructors or Destructors are called
         on the elements in the array.

Example:
Array2d<GLfloat> myArray(10, 10);
myArray(5, 6) = 4;
for(GLfloat *j = myArray.begin(); j < myArray.end(); j++) *j = 0;
vector<int>::iterator it;
int *ptr = &*it;
*/

#ifndef ARRAY2D_H
#define ARRAY2D_H

//#include <algorithm>
#include <iostream>
//#define NDEBUG //removes the asserts
#include "myassert.h"

#ifndef MIN
#define MIN(x,y)    ((x)<(y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)    ((x)>(y) ? (x) : (y))
#endif

// 2 dimentional array
template<class T> class Array2d
{
 private:
  T* data;
  int width, height;
 public:
  Array2d() {
	  width = 0;
	  height = 0;
	  data = NULL;
  }
  Array2d(int w_, int h_) { 
	  w_ = MAX(w_, 0);
  	h_ = MAX(h_, 0);
  	width=w_;
  	height=h_;
  	data =(T*)malloc(size()*sizeof(T));
  	myassert(data);
    //std::uninitialized_fill(begin(), end(), T());
  }
  Array2d(int w_, int h_, T val) { //initialise fill with val
  	w_ = MAX(w_, 0);
  	h_ = MAX(h_, 0);
  	width=w_;
  	height=h_;
  	data =(T*)malloc(size()*sizeof(T));
  	myassert(data);
  	//for(T *p = data; p != end();)
      //*p++ = val;
    std::uninitialized_fill(begin(), end(), val);
  }
  /*virtual*/ ~Array2d() {
  	if(data) free(data);
  }
  T& operator()(int x, int y) {
  	myassert(x >= 0 && x < width);
  	myassert(y >= 0 && y < height);
  	return( *(data + (width*y + x)));
  }
  T const& operator()(int x, int y) const {
  	myassert(x >= 0 && x < width);
  	myassert(y >= 0 && y < height);
  	return( *(data + (width*y + x)));
  }
  T* operator[](int y) {
  	myassert(y >= 0 && y < height);
	  return(data+width*y);
  }
  int w() { return width; }
  int h() { return height; }
  int size() { return width*height; }
  T *begin() { return data; }
  T *begin(int y) { // same as []
	  myassert(y >= 0 && y < height);
	  return(data+width*y);
  }
  T *end() { return data+size(); }
  T *end(int y) {
	  return begin(y)+width;
  }
  /*virtual*/ bool isEmpty() { return (data==NULL); }
  void resize_raw(int w_, int h_) { //messes up data, but fast
	  w_ = std::max(w_, 0);
	  h_ = std::max(h_, 0);
	  width=w_; height=h_;
	  data = (T*)realloc(data, size()*sizeof(T));
  }
  void resize(int w_, int h_) { //keeps data in order, but doesn't initialise new data
	  w_ = std::max(w_, 0);
	  h_ = std::max(h_, 0);
	  if(w_ < width) {
	    for(int j=1; j<std::min(h_, height); j++)
		    memmove(data+w_*j, data+width*j, w_*sizeof(T));
	    resize_raw(w_, h_);
	  } else if(w_ > width) {
	    int old_height = height;
	    int old_width = width;
	    resize_raw(w_, h_);
	    for(int j=std::min(h_, old_height)-1; j>0; j--)
		    memmove(data+w_*j, data+old_width*j, old_width*sizeof(T));
	  } else
	    resize_raw(w_, h_);
  }
  void resize(int w_, int h_, T val) { //keeps data in order and initialises new stuff to val
	  w_ = std::max(w_, 0);
	  h_ = std::max(h_, 0);
  	int old_height = height;
  	int old_width = width;
  	resize(w_, h_);
  	if(w_ > old_width) {
	    for(int j=0; j<std::min(h_, old_height); j++) {
	  	  T *end = data+j*w_+w_;
		    for(T *p=data+w_*j+old_width; p!=end;)
  		    *p++ = val;
	    }
	  }
	  if(h_ > old_height) {
	    for(int j=old_height; j<h_; j++) {
		    T *end = data+w_*j+w_;
		    for(T *p=data+w_*j; p!=end;)
		      *p++ = val;
	    }
	  }
  }
  void set_w(int w_) { resize(w_, height); }
  void set_h(int h_) { resize(width, h_); }
  void shift_left(int n) { //shift all cols to the left by n. losing the first n cols and not initialising the last n
	  if(n < 1 || n >= width) return;
	  for(int k=0; k<height; k++)
	    memmove(begin(k), begin(k)+n, (width-n)*sizeof(T));
  }
};

template<class T>
std::ostream& operator<<(std::ostream &o, Array2d<T> &a)
{
  if(a.w() == 0 && a.h() == 0) o << "[]" << std::endl;
  for(int k=0; k<a.h(); k++) {
	  o << '[';
	  for(int j=0; j<a.w(); j++) {
	    o << a(j, k);
	    if(j != a.w()-1) o << '\t';
	  }
	  o << ']' << std::endl;
  }
  return o;
}

#endif
