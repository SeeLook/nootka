/***************************************************************************
                          large_vector.h  -  description
                             -------------------
    begin                : Wed Jun 1 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef LARGE_VECTOR_H
#define LARGE_VECTOR_H

#include <vector>
#include "array1d.h"
#include "SmartPtr.h"

/** large_vector works like vector.
  Same:
    It has constant time push_back
    It has constant time access []
  Different:
    Pointers to elements always stay valid during a push_back
    Elements are not guaranteed to be consecutive in memory
  
  Storage:
    It is stored as a one layer tree.
    ie. there is one parent node with as many children buffers as needed.
    The size of the buffers can be specified with a second parameter to the constructor
    operator= does a shallow copy. i.e points to the same array
*/
template<typename T>
class large_vector
{
private:
  uint _buffer_size;
  //std::vector<std::vector<T> *> buf_ptrs();
  SmartPtr<Array1d<std::vector<T> *> > _buf_ptrs;
  
  Array1d<std::vector<T> *> &buf_ptrs() { return *_buf_ptrs; }

  void addBuffer(uint num=0) {
    buf_ptrs().push_back(new std::vector<T>(num));
    buf_ptrs().back()->reserve(_buffer_size);
  }
  void removeBuffer() {
    delete buf_ptrs().back();
    buf_ptrs().pop_back();
  }

public:
  class iterator
  {
    large_vector<T> *_parent;
    uint _pos;
    
   public:
    iterator(large_vector<T> *parent, int pos) : _parent(parent), _pos(pos) { }
    iterator(const iterator &it) : _parent(it._parent), _pos(it._pos) { }
    uint pos() const { return _pos; }
    iterator& operator++() { ++_pos; return (*this); }
    iterator& operator++(int) { _pos++; return (*this); }
    iterator& operator--() { --_pos; return (*this); }
    iterator& operator--(int) { _pos--; return (*this); }
    T& operator*() { return (*_parent)[_pos]; }
    const T& operator*() const { return (*_parent)[_pos]; }
    T* operator->() { return &(operator*()); }
    const T* operator->() const { return &(operator*()); }
    bool operator!=(const iterator &it) const { return _pos != it.pos(); }
    bool operator==(const iterator &it) const { return _pos == it.pos(); }
    bool operator<(const iterator &it) const { return _pos < it.pos(); }
    bool operator>(const iterator &it) const { return _pos > it.pos(); }
    //iterator& operator=(const iterator &it) { return (*this); }
  };

  large_vector(uint size=0, uint buffer_size=2048) {
    _buf_ptrs = new Array1d<std::vector<T> *>();
    _buffer_size = buffer_size;
    while(size > _buffer_size) {
      buf_ptrs().push_back(new std::vector<T>(_buffer_size));
      size-=_buffer_size;
    }
    //buf_ptrs().push_back(new std::vector<T>(size));
    //buf_ptrs().back()->reserve(_buffer_size);
    addBuffer(size);
  }
  ~large_vector() {
    if(_buf_ptrs.getNumRef() == 1) {
      for(int j=0; j<buf_ptrs().size();j++) delete buf_ptrs()[j];
    }
  }
  
  T& operator[](uint pos) {
    //myassert(empty() || pos < size());
    return (*buf_ptrs()[pos / _buffer_size])[pos % _buffer_size];
  }
  T& at(uint pos) {
    myassert(empty() || pos < size());
    return (*buf_ptrs()[pos / _buffer_size])[pos % _buffer_size];
  }
  T &front() { return at(0); }
  T &back() { return at(size()-1); }
  uint size() { return (buf_ptrs().size()-1)*_buffer_size + buf_ptrs().back()->size(); }
  bool empty() { return (buf_ptrs().size()==1) ? buf_ptrs().back()->empty() : false; }
  void push_back(const T &new_element) {
    buf_ptrs().back()->push_back(new_element);
    if(buf_ptrs().back()->size() == _buffer_size) {
      //buf_ptrs().push_back(new std::vector<T>(0));
      //buf_ptrs().back()->reserve(_buffer_size);
      addBuffer();
    }
  }
  T pop_back() {
    if(buf_ptrs().back()->empty()) {
      if(numBuffers() == 1) return T();
      else removeBuffer();
    }
    T temp = buf_ptrs().back()->back();
    buf_ptrs().back()->pop_back();
    return temp;
  }
  void push_back(const T *src, uint length) {
    uint sizeBefore = size();
    increase_size(length);
    copyFrom(src, sizeBefore, length);
  }
  void increase_size(uint num) {
    if(num < bufferSize() - buf_ptrs().back()->size()) {
      buf_ptrs().back()->resize(buf_ptrs().back()->size() + num);
    } else {
      num -= bufferSize() - buf_ptrs().back()->size();
      buf_ptrs().back()->resize(bufferSize());
      addBuffer();
      while(num >= bufferSize()) {
        num -= bufferSize();
        buf_ptrs().back()->resize(bufferSize());
        addBuffer();
      }
      buf_ptrs().back()->resize(num);
    }
  }
  void clear() {
    for(int j=0; j<buf_ptrs().size();j++) delete buf_ptrs()[j];
    buf_ptrs().clear();
    //buf_ptrs().push_back(new std::vector<T>(0));
    //buf_ptrs().back()->reserve(_buffer_size);
    addBuffer();
  }
  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, size()); }
  iterator iterator_at(uint pos) { return iterator(this, pos); }
  
  uint bufferSize() const { return _buffer_size; }
  int numBuffers() { return buf_ptrs().size(); }
  std::vector<T> &getBuffer(uint bufferNum) { return *buf_ptrs()[bufferNum]; }
  //efficient copy to a single block of memory (ie array or vector)
  void copyTo(T *dest, uint start, uint length) {
    myassert(start+length <= size());
    T* ending = dest+length;
    uint curBuf = start / bufferSize();
    uint offset = start % bufferSize();
    if(length <= bufferSize() - offset) {
      std::copy(getBuffer(curBuf).begin()+offset, getBuffer(curBuf).begin()+offset+length, dest);
    } else {
      std::copy(getBuffer(curBuf).begin()+offset, getBuffer(curBuf).end(), dest);
      dest += bufferSize() - offset;
      curBuf++;
      while(uint(ending - dest) > bufferSize()) {
        std::copy(getBuffer(curBuf).begin(), getBuffer(curBuf).end(), dest);
        dest += bufferSize();
        curBuf++;
      }
      std::copy(getBuffer(curBuf).begin(), getBuffer(curBuf).begin()+(ending-dest), dest);
    }
  }
  //efficient copy from a single block of memory (ie array or vector)
  void copyFrom(const T *src, uint start, uint length) {
    myassert(start+length <= size());
    const T* ending = src+length;
    uint curBuf = start / bufferSize();
    uint offset = start % bufferSize();
    if(length <= bufferSize() - offset) {
      std::copy(src, src+length, getBuffer(curBuf).begin()+offset);
    } else {
      std::copy(src, src+(bufferSize()-offset), getBuffer(curBuf).begin()+offset);
      src += bufferSize() - offset;
      curBuf++;
      while(uint(ending - src) > bufferSize()) {
        std::copy(src, src+bufferSize(), getBuffer(curBuf).begin());
        src += bufferSize();
        curBuf++;
      }
      std::copy(src, ending, getBuffer(curBuf).begin());
    }
  }
};


/*
void testLargeVectorFunc()
{
  int j, k;
  int count = 0;
  int x[1024];
  large_vector<int> v;
  printf("begin test\n");
  for(j=0; j<6000; j++) {
    for(k=0; k<1024; k++) x[k] = count++;
    v.push_back(x, 1024);
    v.copyTo(x, j*1024, 1024);
    v.copyFrom(x, j*1024, 1024);
  }
  for(j=0; j<6000*1024; j++) if(v[j] != j) printf("%d, %d\n", v[j], j);

  printf("\n");
  large_vector<int> u;
  u.push_back(10);
  u.push_back(11);
  printf("%d\n", u.back());
  u.pop_back();
  printf("%d\n", u.back());
  u.pop_back();

  printf("done test\n");
  exit(0);
}
*/

#endif
