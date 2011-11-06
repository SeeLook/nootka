/***************************************************************************
                          useful.cpp  -  Generic useful functions
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
#include "musicnotes.h"
#include "mystring.h"
#include "myassert.h"

//#include <stdio.h>
//#include <errno.h>
//#include "myassert.h"

//MusicKey EvenTempered;
//MusicKey JustIntonation;
std::vector<MusicKey> gMusicKeys;
std::vector<MusicScale> gMusicScales;
//char *gMusicKeyName[7] = { "A", "B", "C", "D", "E", "F", "G" };
//int   gMusicKeyRoot[7] = {  9,   11,  0,   2,   4,   5,   7  };
char *gMusicKeyName[NUM_MUSIC_KEYS] = { "A             ", "A#/Bb", "B", "C", "C#/Db", "D", "D#/Eb", "E", "F", "F#/Gb", "G", "G#/Ab" };
int   gMusicKeyRoot[NUM_MUSIC_KEYS] = {  9,   10,      11,  0,   1,       2,   3,       4,   5,   6,        7,  8      };
int   gMusicKey = 3; //C
//                       0    1     2    3     4    5    6     7    8     9    10    11
char *noteNames[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

/** Just Intonation
  1:1   Root
  9:8   2nd
  6:5   Minor 3rd
  5:4   Major 3rd
  4:3   Forth
  25:18 F# - Geoff
  3:2   Fifth
  (8:5   Minor 6th?) (25:16 Minor 6th - Geoff)
  5:3   Major 6th
  
  15:8  Major 7th
  2:1   Octave
*/

/** Pythagorean Tuning
Ref = Wikipedia
D  1:1     Root
Eb 256:242
E  9:8
F  32:27   Minor 3rd
F# 81:64   Major 3rd
G  4:3     4th
G# 729:512
A  3:2     5th
Bb 128:81
B  27:16
C  16:9
C# 243:128
*/

void initMusicStuff()
{
  double evenTemperedScale[12]  = { 0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0, 11.0 };
  /** Note types: 0x01=Major Scale Note, 0x02=Minor Scale Note, 0x04=Root Note */
  //int    twelveNoteType[12]      = { 7,   0,   3,   2,   1,   3,   0,   3,   2,   1,   2,    1    };
  int    twelveNoteType[12]      = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
  //double justIntonationRatios[8] = { 1.0, 6.0/5, 5.0/4, 4.0/3, 3.0/2, 8.0/5, 5.0/3, 15.0/8 };
  double justIntonationRatios[10] = { 1.0, 9.0/8, 6.0/5, 5.0/4, 4.0/3, 25.0/18, 3.0/2, 25.0/16, 5.0/3, 15.0/8 };
  //int    justIntonationType[8]   = { 7,   2,     1,     3,     3,     2,     1,     1      };
  //int    justIntonationType[8]   = { 0, 3, 4, 5, 7, 8, 9, 11 };
  int    justIntonationType[10]   = { 0, 2, 3, 4, 5, 6, 7, 8, 9, 11 };
  double pythagoreanRatio[12]    = { 1.0, 256.0/242, 9.0/8, 32.0/27, 81.0/64, 4.0/3, 729.0/512, 3.0/2, 128.0/81, 27.0/16, 16.0/9, 243.0/128 };
  //int    pythagoreanType[12]   = { 0,   2,         1,     2,       1,       1,     2,         1,     2,        1,       2,      1         };
  double meantoneTemperamentScale[12]  = { 0, 76, 193, 310, 386, 503, 579, 697, 773, 890, 1007, 1083 };
  gMusicKeys.resize(4);
  gMusicKeys[0].setName("Even Tempered");
  gMusicKeys[0].setScaleMidi(evenTemperedScale, twelveNoteType, 12); 
  gMusicKeys[1].setName("Just Intonation");
  gMusicKeys[1].setScaleRatios(justIntonationRatios, justIntonationType, 10);
  gMusicKeys[2].setName("Pythagorean Tuning");
  gMusicKeys[2].setScaleRatios(pythagoreanRatio, twelveNoteType, 12);
  gMusicKeys[3].setName("Meantone Temperament");
  gMusicKeys[3].setScaleCents(meantoneTemperamentScale, twelveNoteType, 12);

  int   allNoteScale[12]      = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
  int   majorScale[7]         = { 0, 2, 4, 5, 7, 9, 11 };
  int   naturalMinorScale[7]  = { 0, 2, 3, 5, 7, 8, 10 };
  int   harmonicMinorScale[7] = { 0, 2, 3, 5, 7, 8, 11 };
  int   ascendingMelodicMinorScale[7] = { 0, 2, 3, 5, 7, 9, 11 };
  //int   naturalMinorScale[7]  = { 0, 2, 4, 5, 7, 9, 11 };
  //int   harmonicMinorScale[7] = { 0, 2, 4, 5, 8, 9, 11 };
  //int   ascendingMelodicMinorScale[7] = { 0, 2, 4, 6, 8, 9, 11 };
  gMusicScales.resize(5);
  //gMusicScales[0].addScale("All Notes", allNoteScale, 12, 0);
  //gMusicScales[1].addScale("Major", majorScale, 7, 0);
  //gMusicScales[2].addScale("Minor (Natural)", naturalMinorScale, 7, 0);
  //gMusicScales[3].addScale("Minor (Harmonic)", harmonicMinorScale, 7, 0);
  //gMusicScales[4].addScale("Minor (Ascending Melodic)", ascendingMelodicMinorScale, 7, 0);
  gMusicScales[MusicScale::Chromatic].addScale("All Notes", allNoteScale, 12, 0);
  gMusicScales[MusicScale::Major].addScale("Major", majorScale, 7, 0);
  gMusicScales[MusicScale::NaturalMinor].addScale("Minor (Natural)", naturalMinorScale, 7, 0);
  gMusicScales[MusicScale::HarmonicMinor].addScale("Minor (Harmonic)", harmonicMinorScale, 7, 0);
  gMusicScales[MusicScale::MelodicMinor].addScale("Minor (Ascending Melodic)", ascendingMelodicMinorScale, 7, 0);
}

/**
0  Root (1st)
1
2  (2nd)
3  Minor 3rd
4  Major 3rd (3rd)
5  4th
6
7  5th
8  Minor 6th
9  Major 6th (6th)
10 Minor 7th
11 Major 7th (7th)
*/

char* noteName(int pitch)
{
  return noteNames[cycle(pitch, 12)];
}

/**
@param note The midi note number
@return The note octave. Middle C (midi note 60) is defined as octave 4. Making midi note 0 in octave -1
*/
int noteOctave(int pitch)
{
  return (pitch / 12) - 1;
}

/**
@param pitch The midi note number
@return The midi note within one octave. Range = 0 to 11. Where 0=C, 1=C# ... 11 = B.
*/
int noteValue(int pitch)
{
  return cycle(pitch, 12);
}

bool isBlackNote(int pitch)
{
  switch(cycle(pitch, 12)) {
  case 1:
  case 3:
  case 6:
  case 8:
  case 10:
    return true;
  default:
    return false;
  }
}

MusicScale::~MusicScale()
{
  if(pName) free(pName);
}

void MusicScale::addScale(const char *theName, const int *theNotes, int length, int semitoneOffset_)
{
  if(pName) free(pName);
  pNotes.resize_copy(theNotes, length);
  pSemitoneLookup.resize(12, false);
  for(int j=0; j<length; j++) {
    myassert(theNotes[j] >= 0 && theNotes[j] < 12);
    pSemitoneLookup[theNotes[j]] = true;
  }
  pName = copy_string(theName);
  _semitoneOffset = semitoneOffset_;
}

MusicKey::~MusicKey()
{
  if(pName) free(pName);
}

/**
  @param a the array of ratios
  @param n the size of the array
  e.g. [0.0, 1.0, 2.0, 3.0, 4.0, ... 11.0] for equal tempered
*/
void MusicKey::setScaleMidi(double *theNoteOffsets, int *types, int n) {
  noteOffsets.resize(n);
  noteTypes.resize(n);
  for(int j=0; j<n; j++) {
    noteOffsets[j] = theNoteOffsets[j];
    noteTypes[j] = types[j];
  }
}

/**
  @param a the array of ratios
  @param n the size of the array
  e.g. [0, 100, 200, 300, 400, ... 1100] for equal tempered
*/
void MusicKey::setScaleCents(double *theNoteOffsets, int *types, int n) {
  noteOffsets.resize(n);
  noteTypes.resize(n);
  for(int j=0; j<n; j++) {
    noteOffsets[j] = theNoteOffsets[j]/100.0;
    noteTypes[j] = types[j];
  }
}

/** 
  @param a the array of ratios
  @param n the size of the array
  e.g. [1.0, 5.0/4, 4.0/3, 3.0/2]
*/
void MusicKey::setScaleRatios(double *theNoteOffsets, int *types, int n) {
  double k = freq2pitch(1.0);
  noteOffsets.resize(n);
  noteTypes.resize(n);
  for(int j=0; j<n; j++) {
    noteOffsets[j] = freq2pitch(theNoteOffsets[j]) - k;
    noteTypes[j] = types[j];
  }
}

void MusicKey::setName(const char *theName)
{
  if(pName) free(pName);
  pName = copy_string(theName);
}

int MusicKey::nearestNoteIndex(double x) {
  return (int)(binary_search_closest(noteOffsets.begin(), noteOffsets.end(), x) - noteOffsets.begin());
}

double MusicKey::nearestNote(double x) {
  return *binary_search_closest(noteOffsets.begin(), noteOffsets.end(), x);
}

double MusicKey::nearestNoteDistance(double x) {
  return fabs(*binary_search_closest(noteOffsets.begin(), noteOffsets.end(), x) - x);
}

