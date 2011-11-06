/***************************************************************************
                           equalloudness.cpp
                           -------------------
   Interpolates an equal-loudness lookup table to get the loudness of a sound in phons
   at the given frequencies and intensities (given in deci-Bells)
    
    begin                : Nov 2004
    copyright            : (C) 2004-2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   
   Please read LICENSE.txt for details.
 ***************************************************************************/

//   Reference:
//   Extraperlated from the equal loudness curves at
//   http://hyperphysics.phy-astr.gsu.edu/hbase/sound/eqloud.html

#include "equalloudness.h"
#include "useful.h"
#include <algorithm>

//These are the sampling intensities in out lookup table (in dB)
double i0[14] = {0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120, 130};

//the best hearing is at 3700Hz. And another max at 13000Hz
//These are the sampling frequencies in out lookup table
double f0[23] =                  {  20,  30,  40,  50,  70, 100,    200,300,400,500,700,1000,  1600,  2000,3000,3700,5000,7000,10000,    12000,13000,14000,16000};

double loudnessTable[16][23] = { { -30, -30, -30, -30, -30, -30,    -30,-30,-25,-22,-20, -20,   -20,   -20, -20, -15, -17, -30,  -30,      -30,  -30,  -30,  -30}, //-20dB
                                 { -30, -30, -30, -30, -30, -30,    -30,-20,-15,-12,-10, -10,    -8,    -8,  -5,  -2,  -6, -20,  -30,      -30,  -30,  -30,  -30}, //-10dB
                                 { -30, -30, -30, -30, -30, -30,    -20,-10, -5, -2,  0,   0,     2,     2,   6,   9,   5, -10,  -20,      -25,  -22,  -30,  -30}, // 0dB
                                 { -30, -30, -30, -30, -30, -27,      0,  6,  9, 10, 10,  10,    10,    12,  17,  18,  15,   4,   -1,        2,    0,   -5,  -30}, //10dB
                                 { -30, -30, -30, -30, -20,  -5,     12, 18, 20, 20, 20,  20,    20,    23,  27,  28,  25,   5,    9,       12,   12,    5,  -30}, //20dB
                                 { -30, -30, -28, -20,   0,  11,     25, 30, 31, 32, 32,  30,    31,    33,  37,  38,  36,  25,   21,       24,   30,   18,  -30}, //30dB
                                 { -30, -25, -10,   0,  15,  23,     38, 42, 44, 45, 44,  40,    41,    43,  48,  50,  47,  37,   31,       36,   42,   36,  -30}, //40dB
                                 { -30, -10,   4,  14,  28,  38,     50, 52, 54, 55, 52,  50,    51,    53,  57,  59,  57,  48,   44,       47,   52,   48,  -30}, //50dB
                                 { -15,   5,  20,  30,  43,  50,     60, 63, 65, 65, 62,  60,    61,    64,  68,  70,  58,  58,   54,       59,   64,   60,  -20}, //60dB
                                 {   0,  22,  40,  48,  58,  62,     72, 75, 76, 76, 73,  70,    72,    75,  79,  81,  78,  67,   66,       68,   74,   70,  -10}, //70dB
                                 {  18,  41,  55,  61,  70,  75,     72, 84, 85, 85, 82,  80,    81,    85,  90,  91,  88,  78,   75,       78,   82,   80,    0}, //80dB
                                 {  38,  60,  71,  75,  82,  86,     92, 94, 94, 93, 92,  90,    92,    96, 101, 103, 100,  89,   86,       88,   92,   90,   10}, //90dB
                                 {  58,  75,  83,  88,  93,  97,    101,103,103,103,102, 100,   103,   107, 113, 115, 110, 100,   95,       96,   97,   95,   20}, //100dB
                                 {  75,  88,  95,  99, 103, 107,    110,111,111,112,111, 110,   112,   117, 124, 125, 120, 109,  105,      105,  105,  100,   30}, //110dB
                                 {  89, 100, 106, 110, 113, 115,    119,120,120,120,120, 120,   122,   127, 134, 133, 128, 118,  113,      111,  109,  105,   40}, //120dB
                                 { 102, 112, 117, 120, 122, 124,    128,129,130,130,130, 130,   132,   135, 142, 140, 137, 131,  128,      121,  111,  104,   50}};//130dB
/*
double loudnessTable[14][23] = { {-110, -90, -76, -65, -53, -42,    -20,-10, -5, -2,  0,   0,     2,     2,   6,   9,   5, -10,  -20,      -25,  -22,  -30, -110}, // 0dB
                                 { -90, -70, -57, -51, -38, -27,      0,  6,  9, 10, 10,  10,    10,    12,  17,  18,  15,   4,   -1,        2,    0,   -5,  -90}, //10dB
                                 { -70, -55, -42, -35, -20,  -5,     12, 18, 20, 20, 20,  20,    20,    23,  27,  28,  25,   5,    9,       12,   12,    5,  -60}, //20dB
                                 { -57, -40, -28, -20,   0,  11,     25, 30, 31, 32, 32,  30,    31,    33,  37,  38,  36,  25,   21,       24,   30,   18,  -50}, //30dB
                                 { -43, -25, -10,   0,  15,  23,     38, 42, 44, 45, 44,  40,    41,    43,  48,  50,  47,  37,   31,       36,   42,   36,  -40}, //40dB
                                 { -30, -10,   4,  14,  28,  38,     50, 52, 54, 55, 52,  50,    51,    53,  57,  59,  57,  48,   44,       47,   52,   48,  -30}, //50dB
                                 { -15,   5,  20,  30,  43,  50,     60, 63, 65, 65, 62,  60,    61,    64,  68,  70,  58,  58,   54,       59,   64,   60,  -20}, //60dB
                                 {   0,  22,  40,  48,  58,  62,     72, 75, 76, 76, 73,  70,    72,    75,  79,  81,  78,  67,   66,       68,   74,   70,  -10}, //70dB
                                 {  18,  41,  55,  61,  70,  75,     72, 84, 85, 85, 82,  80,    81,    85,  90,  91,  88,  78,   75,       78,   82,   80,    0}, //80dB
                                 {  38,  60,  71,  75,  82,  86,     92, 94, 94, 93, 92,  90,    92,    96, 101, 103, 100,  89,   86,       88,   92,   90,   10}, //90dB
                                 {  58,  75,  83,  88,  93,  97,    101,103,103,103,102, 100,   103,   107, 113, 115, 110, 100,   95,       96,   97,   95,   20}, //100dB
                                 {  75,  88,  95,  99, 103, 107,    110,111,111,112,111, 110,   112,   117, 124, 125, 120, 109,  105,      105,  105,  100,   30}, //110dB
                                 {  89, 100, 106, 110, 113, 115,    119,120,120,120,120, 120,   122,   127, 134, 133, 128, 118,  113,      111,  109,  105,   40}, //120dB
                                 { 102, 112, 117, 120, 122, 124,    128,129,130,130,130, 130,   132,   135, 142, 140, 137, 131,  128,      121,  111,  104,   50}};//130dB
*/
                                 
double lookupLoudnessTable(double row, double col)
{
  row += 2; //0dB is second row in table
  int irow = int(floor(row)); //floor to ensure -ve number are still rounded down
  int irow2 = irow+1;
  double rrow = row - double(irow); //fractional remainder 
  if(irow < 0) return -30.0; //{ irow = 0; irow2 = 0; rrow = 0.0; }
  if(irow >= 13) { irow = 13; irow2 = 13; rrow = 0.0; }
  
  int icol = int(floor(col));
  int icol2 = icol+1;
  double rcol = col - double(icol); //fractional remainder 
  //printf("rcol = %lf, %lf, %lf\n", col, double(col), rcol);
  if(icol < 0) { icol = 0; icol2 = 0; rcol = 0.0; }
  if(icol >= 22) { icol = 22; icol2 = 22; rcol = 0.0; }
  //printf("icol=%d, %d, %lf\n", icol, icol2, rcol);
  
  //[a b] -> [e]
  //[c d] -> [f] 
  double &a = loudnessTable[irow][icol];
  double &b = loudnessTable[irow][icol2];
  double &c = loudnessTable[irow2][icol];
  double &d = loudnessTable[irow2][icol2];

  double e = a + (b - a) * rcol;
  double f = c + (d - c) * rcol;

  return e + (f - e) * rrow;
}

double dbToPhons(double freq, double intensity)
{
  //Todo: Finish writing this function
  double row = intensity / 10.0;
  double *pCol = std::upper_bound(f0, f0+23, freq);
  int icol = int(pCol - f0);
  //printf("%d\n", icol);
  if(icol <= 0) return lookupLoudnessTable(row, 0.0);
  else if(icol >= 23) return lookupLoudnessTable(row, 23.0);
  else {
    int icol0 = MAX(0, icol-1);
    double &a = f0[icol0];
    double &b = f0[icol];
    double col = double(icol0) + (freq - a) / (b - a);
    //printf("%lf, %lf, %lf, %lf, %lf\n", a, b, freq, (freq - a) / (b - a), col);
    return lookupLoudnessTable(row, col);
  }
  //return 0;
}

