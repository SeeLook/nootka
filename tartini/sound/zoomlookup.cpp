/***************************************************************************
                          zoomlookup.cpp  -  description
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

#include "zoomlookup.h"

ZoomLookup::ZoomLookup()
{
  _size = 0;
}

ZoomLookup::~ZoomLookup()
{
}

void ZoomLookup::clear()
{
  _table.clear();
  //_hasValue.clear();
  _size = 0;
}

void ZoomLookup::setZoomLevel(double zoomLevel_)
{
  if(_zoomLevel == zoomLevel_) return;
  _zoomLevel = zoomLevel_;
  clear();
}

void ZoomLookup::setSize(int size_)
{
  _size = size_;
  _table.resize(size_);
  //_hasValue.resize(size_, false);
}


/*
void ZoomLookup::setValue(int x, ZoomElement *z)
{
  _table[x] = *z;
  setHasValue(x);
}
*/
