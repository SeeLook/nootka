/***************************************************************************
                          notedata.cpp  -  description
                             -------------------
    begin                : Mon May 23 2005
    copyright            : (C) 2005 by Philip McLeod
    email                : pmcleod@cs.otago.ac.nz

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Please read LICENSE.txt for details.

   Adjusted to Nootka by Tomasz Bojczuk
    seelook@gmail.com
    Copyright (C) 2011-2016
 ***************************************************************************/

#ifndef NOTEDATA_H
#define NOTEDATA_H

#include "analysisdata.h"
#include "SmartPtr.h"
#include "array1d.h"
#include <vector>

class TartiniParams;
class Channel;

/**
 * @author Philip McLeod
*/
class NoteData
{

public:
  Array1d<float> nsdfAggregateData;
  Array1d<float> nsdfAggregateDataScaled;
  double nsdfAggregateRoof; //keeps the sum of scalers. i.e. The highest possible aggregate value
  float firstNsdfPeriod;
  float currentNsdfPeriod;

  NoteData(TartiniParams *tParams = 0) { m_params = tParams; }
  NoteData(Channel *channel_, TartiniParams *tParams);
  NoteData(Channel *channel_, int startChunk_, AnalysisData *analysisData, TartiniParams *tParams);
  ~NoteData();

  SmartPtr<Array1d<int> > maxima;
  SmartPtr<Array1d<int> > minima;

  void    resetData(); /**< Reset @p numPeriods() tp 0. */
  bool    isValid() { return (numChunks() > 2); }
  void    setStartChunk(int startChunk_) { m_startChunk = startChunk_; }
  void    setEndChunk(int endChunk_) { m_endChunk = endChunk_; }
  int     startChunk() { return m_startChunk; }
  int     endChunk() { return m_endChunk; }
  void    addData(AnalysisData *analysisData, float periods);
  int     numChunks() { return m_endChunk - m_startChunk; }
  double  noteLength(); /**< Returns the length of the note (in seconds) */
  float   numPeriods() { return m_numPeriods; }
  double  avgFreq() { return numPeriods() / noteLength(); } /**< in Hertz */
  double  avgPitch() { return m_avgPitch; } /**< The average of this note, in fractions of semi-tones. */
  void    setPeriodOctaveEstimate(float periodOctaveEstimate_) { m_periodOctaveEstimate = periodOctaveEstimate_; }
  float   periodOctaveEstimate() { return m_periodOctaveEstimate; }
  float   volume() { return m_volume; }
  void    recalcAvgPitch();

private:
  Channel      *channel;
  int           m_startChunk; /**< the chunk at which this note starts on */
  int           m_endChunk; /**< the chunk after the last one in the note */
  float         m_maxLogRMS; /**< The maximum RMS volume during the note */
  float         m_maxIntensityDB; /**< The maximum intensity volume during the note */
  float         m_maxCorrelation;
  float         m_maxPurity;
  float         m_numPeriods;
  float         m_periodOctaveEstimate; /**< This is in terms of the periodRatio at the beginning of the note */
  float         m_volume; /**< A normalized volume between 0 and 1 */
  float         m_avgPitch;
  int           m_prevExtremumTime;
  float         m_prevExtremumPitch;

  TartiniParams *m_params;
};

#endif
