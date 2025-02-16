/***************************************************************************
                          channel.h  -  description
                             -------------------
    begin                : Sat Jul 10 2004
    copyright            : (C) 2004-2005 by Philip McLeod
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

#ifndef CHANNEL_H
#define CHANNEL_H

#include "array1d.h"
#include <vector>
#include "analysisdata.h"
#include "notedata.h"
#include "large_vector.h"
#include "filters/Filter.h"
#include <QMutex>
#include "../tpitchfinder.h"

class fast_smooth;
class TpitchFinder;

class Channel
{

public:
  Array1d<float> directInput;
  Array1d<float> nsdfData;
  Array1d<float> nsdfAggregateData;
  Array1d<float> nsdfAggregateDataScaled;
  double nsdfAggregateRoof; //keeps the sum of scalers. i.e. The highest possible aggregate value
  Array1d<float> fftData1;
  Array1d<float> fftData2;
  Array1d<float> fftData3;
  Array1d<float> cepstrumData;
  large_vector<NoteData> noteData;
  Filter *highPassFilter;

  double rmsFloor; //in dB
  double rmsCeiling; //in dB

  void lock() { mutex->lock(); isLocked = true; }
  void unlock() { isLocked = false; mutex->unlock(); }
  bool locked() { return isLocked; } //For same thread testing of asserts only

  Channel(TpitchFinder *parent_, int size_, int k_=0);
  virtual ~Channel();
  float *begin() { return directInput.begin(); }
  float *end() { return directInput.end(); }
  int size() { return directInput.size(); }
  float &at(int pos) { return directInput.at(pos); }
  int rate() { return m_parent->aGl()->rate ; }
  virtual void resize(int newSize, int k_=0);
  virtual void shift_left(int n);
  int framesPerChunk() { return m_parent->aGl()->framesPerChunk ; }

    /** Analysis the current data and add it to the end of the lookup table.
    * NOTE: The Channel should be locked before calling this. */
  void processNewChunk(FilterState *filterState);

  bool isVisible() { return m_visible; }
  void setVisible(bool state=true) { m_visible = state; }
  void reset();

  double timePerChunk() { return double(m_parent->aGl()->framesPerChunk) / double(m_parent->aGl()->rate) ; }
  int totalChunks() { return m_lookup.size(); }
  double totalTime() { return double(MAX(totalChunks()-1, 0)) * timePerChunk(); }
  int chunkAtTime(double t) { return toInt(chunkFractionAtTime(t)) ; }
  double chunkFractionAtTime(double t) { return t / timePerChunk(); }
  int currentChunk() { return m_parent->currentChunk(); } //this one should be use to retrieve current info
  double timeAtChunk(int chunk) { return double(chunk) * timePerChunk(); }

  AnalysisData *dataAtChunk(int chunk) { return (isValidChunk(chunk)) ? &m_lookup[chunk] : NULL; }
  AnalysisData *dataAtCurrentChunk() { return dataAtChunk(currentChunk()); }
  AnalysisData *dataAtTime(double t) { return dataAtChunk(chunkAtTime(t)); }
  large_vector<AnalysisData>::iterator dataIteratorAtChunk(int chunk) { return m_lookup.iterator_at(chunk); }
  
  bool hasAnalysisData() { return !m_lookup.empty(); }
  bool isValidChunk(int chunk) { return (chunk >= 0 && chunk < totalChunks()); }
  bool isValidTime(double t) { return isValidChunk(chunkAtTime(t)); }
  
  float averagePitch(int begin, int end);
  float averageMaxCorrelation(int begin, int end);

  float threshold() { return _threshold; }
  void setIntThreshold(int thresholdPercentage) { _threshold = float(thresholdPercentage) / 100.0f; }

  bool isNotePlaying() { return m_noteIsPlaying; }

      /** @p noteId the index of the note to inquire about.
      * Returns @p TRUE if the loudest part of the note is above the noiseThreshold */
  bool isVisibleNote(int noteId) { return noteId == NO_NOTE ? false : true; }

  bool isVisibleChunk(AnalysisData *data);
  bool isChangingChunk(AnalysisData *data);
  bool isNoteChanging(int chunk);

    /** @p noteId - the index of the note to inquire about
    * Returns @p TRUE if the note is long enough */
  bool isLabelNote(int noteId) { return (noteId >= 0 && noteData[noteId].isValid()) ? true : false; }

  NoteData *getLastNote() { return noteData.empty() ? NULL : &noteData.back(); }
  NoteData *getCurrentNote();
  NoteData *getNote(int noteIndex) { return (noteIndex >= 0 && noteIndex < (int)noteData.size()) ? &noteData[noteIndex] : NULL; }
  int getCurrentNoteIndex() { return int(noteData.size())-1; }
  void backTrackNoteChange(int chunk);
  void processNoteDecisions(int chunk, float periodDiff);
  void noteBeginning(int chunk);
  void noteEnding(int chunk);
  float calcOctaveEstimate();
  void recalcNotePitches(int chunk);

    /** Choose the correlation index (with no starting octave estimate)
    * For use with at the start of the note.  */
  void chooseCorrelationIndex1(int chunk);

    /** This uses an octave estimate to help chose the correct correlation index
    * Returns true if the new correlation index is different from the old one */
  bool chooseCorrelationIndex(int chunk, float periodOctaveEstimate);
  void calcDeviation(int chunk);
  bool isFirstChunkInNote(int chunk);
  void resetNSDFAggregate(float period);
  void addToNSDFAggregate(const float scaler, float periodDiff);

    /** Calculate (the middle half of) pitches within the current window of the input
    * Calculates pitches for positions 1/4 of size() to 3/4 of size()
    * e.g. for size() == 1024, does indexs 256 through < 768
    * @param period The period estimate
    * @return The change in period size. */
  float calcDetailedPitch(float *input, double period, int chunk);
  bool firstTimeThrough() { return m_parent->aGl()->firstTimeThrough; }
  bool doingDetailedPitch() { return m_parent->aGl()->doingDetailedPitch; }

  float periodOctaveEstimate(int chunk); // A estimate from over the whole duration of the note, to help get the correct octave

private:
  TpitchFinder               *m_parent;
  bool                        m_visible;
  bool                        m_noteIsPlaying;
  large_vector<AnalysisData>  m_lookup;
  float _threshold;
  QMutex *mutex;
  bool isLocked;
  fast_smooth *fastSmooth;

};


#endif
