/***************************************************************************
                          conversions.h  -  
                             -------------------
    begin                : 16/01/2006
    copyright            : (C) 2003-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
 
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

double linear2dB(double x);
double dB2Linear(double x);
double dB2Normalised(double x);
double normalised2dB(double x);
double dB2ViewVal(double x);
double same(double x);
double oneMinus(double x);

double dB2Normalised(double x, double theCeiling, double theFloor);
