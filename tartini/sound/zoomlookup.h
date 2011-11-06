/***************************************************************************
                          zoomlookup.h  -  description
                             -------------------
    begin                : Wed Feb 2 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

#ifndef ZOOMLOOKUP_H
#define ZOOMLOOKUP_H

#include <qcolor.h>
//#include "array1d.h"
#include <vector>
#include "myassert.h"

class AnalysisData;

class ZoomElement
{
private:
  float _low, _high, _corr;
  QColor _color;
  int _noteIndex;
  int _midChunk;
  bool _isValid;

public:
  ZoomElement() { _isValid = false; }
  float low() { return _low; }
  float high() { return _high; }
  float corr() { return _corr; }
  QColor color() { return _color; }
  int noteIndex() { return _noteIndex; }
  int midChunk() { return _midChunk; }
  bool isValid() { return _isValid; }
  void set(float low_, float high_, float corr_, QColor color_, int noteIndex_, int midChunk_) {
    _low = low_;
    _high = high_;
    _corr = corr_;
    _color = color_;
    _noteIndex = noteIndex_;
    _midChunk = midChunk_;
    _isValid = true;
  }
};

class ZoomLookup
{
private:
  int _size;
  double _zoomLevel;
public:
  std::vector<ZoomElement> _table;

/*#ifdef MACX
  std::bit_vector _hasValue;
#else
  std::vector<bool> _hasValue;
#endif
*/
  ZoomLookup();
  virtual ~ZoomLookup();

  void clear();
  void setZoomLevel(double zoomLevel_);
  int size() { return _size; }
  void setSize(int size_);
  ZoomElement &at(int x) { myassert(x >= 0 && x < int(_table.size())); return _table[x]; }
  //bool hasValue(int x) { myassert(x >= 0 && x < int(_hasValue.size())); return _hasValue[x]; }
  //void setHasValue(int x, bool val=true) { myassert(x >= 0 && x < int(_hasValue.size())); _hasValue[x] = val; }
  //void setValue(int x, ZoomElement *z);
};

#endif
