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
 ***************************************************************************/

#include <vector>

#include "notedata.h"
#include "gdata.h"
#include "channel.h"
#include "useful.h"
#include "musicnotes.h"


NoteData::NoteData(Channel *channel_)
{
  channel = channel_;
  maxLogRMS = gdata->dBFloor();
  maxima = new Array1d<int>();
  minima = new Array1d<int>();
  _periodOctaveEstimate = 1.0f;
  _numPeriods = 0;
}

/*
NoteData::NoteData(int startChunk_, int endChunk_, float logRMS_, float intensityDB_, float correlation_, float purity_)
{
  startChunk = startChunk_;
  endChunk = endChunk_;
  maxLogRMS = logRMS_;
  maxIntensityDB = intensityDB_;
  maxCorrelation = correlation_;
  maxPurity = purity_;
}*/

NoteData::NoteData(Channel *channel_, int startChunk_, AnalysisData *analysisData)
{
  channel = channel_;
  _startChunk = startChunk_;
  _endChunk = startChunk_ + 1;
  maxLogRMS = analysisData->logrms();
  maxIntensityDB = analysisData->maxIntensityDB();
  maxCorrelation = analysisData->correlation();
  maxPurity = analysisData->volumeValue();
  _volume = 0.0f;
  _numPeriods = 0.0f; //periods;
  //_periodOctaveEstimate = analysisData->periodOctaveEstimate;
  _periodOctaveEstimate = 1.0f;
  loopStep = channel->rate() / 1000;  // stepsize = 0.001 seconds
  loopStart = _startChunk * channel->framesPerChunk() + loopStep;
  prevExtremumTime = -1;
  prevExtremumPitch = -1;
  prevExtremum = NONE;
  maxima = new Array1d<int>();
  minima = new Array1d<int>();
  nsdfAggregateData.resize(channel_->nsdfData.size(), 0.0f);
  nsdfAggregateDataScaled.resize(channel_->nsdfData.size(), 0.0f);
  nsdfAggregateRoof = 0.0;
  firstNsdfPeriod = 0.0f;
  currentNsdfPeriod = 0.0f;
  _avgPitch = 0.0f;
}

NoteData::~NoteData()
{
}
/*
void NoteData::addValues(float logRMS_, float intensityDB_, float correlation_, float purity_)
{
  if(logRMS_ > maxLogRMS) maxLogRMS = logRMS_;
  if(intensityDB_ > maxIntensityDB) maxIntensityDB = intensityDB_;
  if(correlation_ > maxCorrelation) maxCorrelation = correlation_;
  if(purity_ > maxPurity) maxPurity = purity_;
}
*/
void NoteData::resetData()
{
  _numPeriods = 0;
}

void NoteData::addData(AnalysisData *analysisData, float periods)
{
  maxLogRMS = MAX(maxLogRMS, analysisData->logrms());
  maxIntensityDB = MAX(maxIntensityDB, analysisData->maxIntensityDB());
  maxCorrelation = MAX(maxCorrelation, analysisData->correlation());
  maxPurity = MAX(maxPurity, analysisData->volumeValue());
  _volume = MAX(_volume, dB2Normalised(analysisData->logrms()));
  _numPeriods += periods; //sum up the periods
  //_periodOctaveEstimate = analysisData->periodOctaveEstimate; //overwrite the old estimate
  _avgPitch = bound(freq2pitch(avgFreq()), 0.0, gdata->topPitch());
}

/** @return The length of the note (in seconds)
*/
double NoteData::noteLength()
{
  return double(numChunks()) * double(channel->framesPerChunk()) / double(channel->rate());
}

/** @return The average of this note, in fractions of semi-tones.
*/
double NoteData::avgPitch()
{
  //return bound(freq2pitch(avgFreq()), 0.0, gdata->topPitch());
  return _avgPitch;
}

void NoteData::addVibratoData(int chunk)
{
  if ((channel->doingDetailedPitch()) && (channel->pitchLookupSmoothed.size() > 0)) {
    // Detailed pitch information available, calculate maxima and minima
    int loopLimit = ((chunk+1) * channel->framesPerChunk()) - loopStep;
    for (int currentTime = loopStart; currentTime < loopLimit; currentTime += loopStep) {
      myassert(currentTime + loopStep < (int)channel->pitchLookupSmoothed.size());
      myassert(currentTime - loopStep >= 0);
      float prevPitch = channel->pitchLookupSmoothed.at(currentTime - loopStep);
      float currentPitch = channel->pitchLookupSmoothed.at(currentTime);
      float nextPitch = channel->pitchLookupSmoothed.at(currentTime + loopStep);

      if ((prevPitch < currentPitch) && (currentPitch >= nextPitch)) {  // Maximum
        if (prevExtremum == NONE) {
          maxima->push_back(currentTime);
          prevExtremumTime = currentTime;
          prevExtremumPitch = currentPitch;
          prevExtremum = FIRST_MAXIMUM;
          continue;
        }
        if ((prevExtremum == FIRST_MAXIMUM) || (prevExtremum == MAXIMUM)) {
          if (currentPitch >= prevExtremumPitch) {
            myassert(!maxima->isEmpty());
            maxima->pop_back();
            maxima->push_back(currentTime);
            prevExtremumTime = currentTime;
            prevExtremumPitch = currentPitch;
          }
          continue;
        }
        if ((fabs(currentPitch - prevExtremumPitch) > 0.04) &&
            (currentTime - prevExtremumTime > 42 * loopStep)) {
          if (prevExtremum == MINIMUM) {
            maxima->push_back(currentTime);
            prevExtremumTime = currentTime;
            prevExtremumPitch = currentPitch;
            prevExtremum = MAXIMUM;
            continue;
          } else {
            // prevExtremum == FIRST_MINIMUM
            if (currentTime - minima->at(0) < loopStep * 500) {
              // Good
              maxima->push_back(currentTime);
              prevExtremumTime = currentTime;
              prevExtremumPitch = currentPitch;
              prevExtremum = MAXIMUM;
              continue;
            } else {
              // Not good
              myassert(!minima->isEmpty());
              minima->pop_back();
              maxima->push_back(currentTime);
              prevExtremumTime = currentTime;
              prevExtremumPitch = currentPitch;
              prevExtremum = FIRST_MAXIMUM;
              continue;
            }
          }
        }
      } else if ((prevPitch > currentPitch) && (currentPitch <= nextPitch)) {  // Minimum
        if (prevExtremum == NONE) {
          minima->push_back(currentTime);
          prevExtremumTime = currentTime;
          prevExtremumPitch = currentPitch;
          prevExtremum = FIRST_MINIMUM;
          continue;
        }
        if ((prevExtremum == FIRST_MINIMUM) || (prevExtremum == MINIMUM)) {
          if (currentPitch <= prevExtremumPitch) {
            myassert(!minima->isEmpty());
            minima->pop_back();
            minima->push_back(currentTime);
            prevExtremumTime = currentTime;
            prevExtremumPitch = currentPitch;
          }
          continue;
        }
        if ((fabs(currentPitch - prevExtremumPitch) > 0.04) &&
            (currentTime - prevExtremumTime > 42 * loopStep)) {
          if (prevExtremum == MAXIMUM) {
            minima->push_back(currentTime);
            prevExtremumTime = currentTime;
            prevExtremumPitch = currentPitch;
            prevExtremum = MINIMUM;
            continue;
          } else {
            // prevExtremum == FIRST_MAXIMUM
            if (currentTime - maxima->at(0) < loopStep * 500) {
              // Good
              minima->push_back(currentTime);
              prevExtremumTime = currentTime;
              prevExtremumPitch = currentPitch;
              prevExtremum = MINIMUM;
              continue;
            } else {
              // Not good
              myassert(!maxima->isEmpty());
              maxima->pop_back();
              minima->push_back(currentTime);
              prevExtremumTime = currentTime;
              prevExtremumPitch = currentPitch;
              prevExtremum = FIRST_MINIMUM;
              continue;
            }
          }
        }
      }
    }
    // Calculate start of next loop
    loopStart = loopStart + ((loopLimit - loopStart) / loopStep) * loopStep + loopStep;
  }
}

void NoteData::recalcAvgPitch() {
  _numPeriods = 0.0f;
  for(int j=startChunk(); j<endChunk(); j++) {
      _numPeriods += float(channel->framesPerChunk()) / float(channel->dataAtChunk(j)->period);
  }
  _avgPitch = bound(freq2pitch(avgFreq()), 0.0, gdata->topPitch());
}
