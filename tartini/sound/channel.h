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
 ***************************************************************************/

#ifndef CHANNEL_H
#define CHANNEL_H

#include "array1d.h"
#include "array2d.h"
#include "qcolor.h"
#include <vector>
//#include "myqmutex.h"
#include "analysisdata.h"
#include "zoomlookup.h"
#include "soundfile.h"
#include "notedata.h"
#include "large_vector.h"
#include "Filter.h"

class Channel/* : public Array1d<float>*/
{
private:
  SoundFile *parent;
  float freq; /**< Channel's frequency */
  //float estimate;
  //int frame_num;
  int _pitch_method;
  bool visible;
  bool noteIsPlaying;
  //double timeOffset; /**< Where the file starts in absolute time (in seconds) */
  //std::vector<AnalysisData> lookup;
  large_vector<AnalysisData> lookup;
  float _threshold;
  QMutex *mutex;
  bool isLocked;
  int pronyWindowSize;
  Array1d<float> pronyData;
  fast_smooth *fastSmooth;

public:
  large_vector<float> pitchLookup;
  large_vector<float> pitchLookupSmoothed;
  QColor color;
  //large_vector<float> filteredData;
  Array1d<float> directInput;
  Array1d<float> filteredInput;
  Array1d<float> coefficients_table;
  Array1d<float> nsdfData;
  Array1d<float> nsdfAggregateData;
  Array1d<float> nsdfAggregateDataScaled;
  double nsdfAggregateRoof; //keeps the sum of scalers. i.e. The highest possible aggregate value
  Array1d<float> fftData1;
  Array1d<float> fftData2;
  Array1d<float> fftData3;
  Array1d<float> cepstrumData;
  Array1d<float> detailedPitchData;
  Array1d<float> detailedPitchDataSmoothed;
  //std::vector<NoteData> noteData;
  large_vector<NoteData> noteData;
  Filter *highPassFilter;
  Filter *pitchSmallSmoothingFilter;
  Filter *pitchBigSmoothingFilter;
  //double filterStateX1, filterStateX2;
  //double filterStateY1, filterStateY2;
  double rmsFloor; //in dB
  double rmsCeiling; //in dB
  
  ZoomLookup summaryZoomLookup;
  ZoomLookup normalZoomLookup;
  ZoomLookup amplitudeZoomLookup;
  
  void lock() { mutex->lock(); isLocked = true; }
  void unlock() { isLocked = false; mutex->unlock(); }
  bool locked() { return isLocked; } //For same thread testing of asserts only

  //Channel();
  Channel(SoundFile *parent_, int size_, int k_=0);
  virtual ~Channel();
  float *begin() { return directInput.begin(); }
  float *end() { return directInput.end(); }
  int size() { return directInput.size(); }
  float &at(int pos) { return directInput.at(pos); }
  int rate() { return parent->rate(); }
  virtual void resize(int newSize, int k_=0);
  virtual void shift_left(int n);
  int framesPerChunk() { return parent->framesPerChunk(); }
  void setParent(SoundFile *parent_) { parent = parent_; }
  SoundFile* getParent() { return parent; }
  void setPitchMethod(int pitch_method) { _pitch_method = pitch_method; }
  int pitchMethod() { return _pitch_method; }
  void calc_last_n_coefficients(int n);
  void processNewChunk(FilterState *filterState);
  void processChunk(int chunk);
  bool isVisible() { return visible; }
  void setVisible(bool state=true) { visible = state; }
  void reset();
  double timePerChunk() { return parent->timePerChunk(); }
  double startTime() { return parent->startTime(); }
  void setStartTime(double newStartTime) { parent->setStartTime(newStartTime); }
  //int chunkNum() { return parent->chunkNum(); }
  int totalChunks() { return lookup.size(); }
  double finishTime() { return startTime() + totalTime(); }
  //double totalTime() { return double(lookup.size()) * timePerChunk(); }
  double totalTime() { return double(MAX(totalChunks()-1, 0)) * timePerChunk(); }
  void jumpToTime(double t) { parent->jumpToTime(t); }
  int chunkAtTime(double t) { return parent->chunkAtTime(t); }
  double chunkFractionAtTime(double t) { return parent->chunkFractionAtTime(t); }
  int chunkAtCurrentTime() { return parent->chunkAtCurrentTime(); }
  //int chunkOffset() { return parent->chunkOffset(); }
  //int currentChunk() { return chunkNum() - chunkOffset(); } //this one should be use to retrieve current info
  int currentChunk() { return parent->currentChunk(); } //this one should be use to retrieve current info
  double timeAtChunk(int chunk) { return parent->timeAtChunk(chunk); }

  //AnalysisData &dataAtChunk(int chunk) { myassert(chunk >= 0 && chunk < int(lookup.size())); return lookup[chunk]; }
  //AnalysisData &dataAtCurrentTime() { return dataAtChunk(chunkAtCurrentTime()); }

  AnalysisData *dataAtChunk(int chunk) { return (isValidChunk(chunk)) ? &lookup[chunk] : NULL; }
  AnalysisData *dataAtCurrentChunk() { return dataAtChunk(currentChunk()); }
  AnalysisData *dataAtTime(double t) { return dataAtChunk(chunkAtTime(t)); }
  large_vector<AnalysisData>::iterator dataIteratorAtChunk(int chunk) { return lookup.iterator_at(chunk); }
  static AnalysisData *getActiveChannelCurrentChunkData();
  
  bool hasAnalysisData() { return !lookup.empty(); }
  bool isValidChunk(int chunk) { return (chunk >= 0 && chunk < totalChunks()); }
  bool isValidTime(double t) { return isValidChunk(chunkAtTime(t)); }
  bool isValidCurrentTime() { return isValidChunk(chunkAtCurrentTime()); }
  
  float averagePitch(int begin, int end);
  float averageMaxCorrelation(int begin, int end);

  float threshold() { return _threshold; }
  //void setThreshold(float threshold) { _threshold = threshold; }
  void setIntThreshold(int thresholdPercentage) { _threshold = float(thresholdPercentage) / 100.0f; }
  void resetIntThreshold(int thresholdPercentage);
  void setColor(QColor c) { color = c; }
  //static QColor getNextColour();

  bool isNotePlaying() { return noteIsPlaying; }
  bool isVisibleNote(int noteIndex_);
  bool isVisibleChunk(int chunk_) { return isVisibleChunk(dataAtChunk(chunk_)); }
  bool isVisibleChunk(AnalysisData *data);
  bool isChangingChunk(AnalysisData *data);
  bool isNoteChanging(int chunk);
  bool isLabelNote(int noteIndex_);
  void clearFreqLookup();
  void clearAmplitudeLookup();
  void recalcScoreThresholds();

  QString getUniqueFilename();

  NoteData *getLastNote();
  NoteData *getCurrentNote();
  NoteData *getNote(int noteIndex);
  int getCurrentNoteIndex() { return int(noteData.size())-1; }
  void backTrackNoteChange(int chunk);
  void processNoteDecisions(int chunk, float periodDiff);
  void noteBeginning(int chunk);
  void noteEnding(int chunk);
  float calcOctaveEstimate();
  void recalcNotePitches(int chunk);
  void chooseCorrelationIndex1(int chunk);
  bool chooseCorrelationIndex(int chunk, float periodOctaveEstimate);
  void calcDeviation(int chunk);
  bool isFirstChunkInNote(int chunk);
  void resetNSDFAggregate(float period);
  void addToNSDFAggregate(const float scaler, float periodDiff);
  float calcDetailedPitch(float *input, double period, int chunk);
  bool firstTimeThrough() { return parent->firstTimeThrough; }
  bool doingDetailedPitch() { return parent->doingDetailedPitch(); }

  void calcVibratoData(int chunk);
  float periodOctaveEstimate(int chunk); /*< A estimate from over the whole duration of the note, to help get the correct octave */

  void exportChannel(int type, QString typeString);
  void doPronyFit(int chunk);
  int pronyDelay() { return pronyWindowSize/2; }
};

/** Create a ChannelLocker on the stack, the channel will be freed automaticly when
  the ChannelLocker goes out of scope
*/
class ChannelLocker
{
  Channel *channel;
  
public:
  ChannelLocker(Channel *channel_) {
    myassert(channel_);
    channel = channel_;
    channel->lock();
  }
  ~ChannelLocker() {
    channel->unlock();
  }
};

#endif
