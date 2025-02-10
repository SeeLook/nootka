/***************************************************************************
                          conversions.cpp  -  
                             -------------------
    begin                : 16/01/2006
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
   
   Adjusted to Nootka by Tomasz Bojczuk
	  seelook@gmail.com
	  Copyright (C) 2011
 ***************************************************************************/

#include <math.h>
#include "useful.h"

double dbfloor = -150.0;

/* x is between 0 and 1. 1 becomes 0dB and 0 becomes dBFloor (-ve) */
double linear2dB(double x) {
	return (x > 0.0) ? bound((log10(x) * 20.0), dbfloor , 0.0) : dbfloor;
}
double dB2Linear(double x) {
  return pow10(x / 20.0);
}
double dB2Normalised(double x) {
	return bound(1.0 - (x / dbfloor), 0.0, 1.0);
}
double normalised2dB(double x) {
	return (1.0 - x) * dbfloor;
}
double dB2ViewVal(double x) {
  return pow10(1.0 + x / 40.0);
}
double same(double x) { return x; }
double oneMinus(double x) { return 1.0 - x; }

double dB2Normalised(double x, double theCeiling, double theFloor) {
  return bound(1.0 + ((x-theCeiling) / (theCeiling-theFloor)), 0.0, 1.0);
}
