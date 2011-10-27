/***************************************************************************
                          myassert.h  -  Cause a crash in debug mode
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

#ifndef MYASSERT
#define MYASSERT

#ifdef MYDEBUG // Cause a crash in debug mode so you can trace it with your debugger

#define myassert(expr) if(!(expr)) { \
  fprintf(stderr, "Assert Failed: %s in %s line %u.\nCausing a trace crash.\n", #expr, __FILE__, __LINE__); \
  fflush(stderr); \
  *((int*)0) = 0; } //cause a crash

#else // Do nothing when not in debug mode
#define myassert(expr) ;
#endif

#define myAlwaysAssert(expr) if(!(expr)) { \
  fprintf(stderr, "Assert Failed: %s in %s line %u.\nCausing a trace crash.\n", #expr, __FILE__, __LINE__); \
  fflush(stderr); \
  *((int*)0) = 0; } //cause a crash

#endif //MYASSERT
