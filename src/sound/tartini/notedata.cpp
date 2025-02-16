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

#include <vector>

#include "notedata.h"
#include "channel.h"
#include "useful.h"
#include "../tartiniparams.h"


NoteData::NoteData(Channel* channel_, TartiniParams* tParams) :
  m_params(tParams)
{
  channel = channel_;
  m_maxLogRMS = m_params->dBFloor;
  maxima = new Array1d<int>();
  minima = new Array1d<int>();
  m_periodOctaveEstimate = 1.0f;
  m_numPeriods = 0;
}


NoteData::NoteData(Channel* channel_, int startChunk_, AnalysisData* analysisData, TartiniParams* tParams) :
  m_params(tParams)
{
  channel = channel_;
  m_startChunk = startChunk_;
  m_endChunk = startChunk_ + 1;
  m_maxLogRMS = analysisData->logrms();
  m_maxIntensityDB = analysisData->maxIntensityDB();
  m_maxCorrelation = analysisData->correlation();
  m_maxPurity = analysisData->volumeValue();
  m_volume = 0.0f;
  m_numPeriods = 0.0f;
  m_periodOctaveEstimate = 1.0f;
  m_prevExtremumTime = -1;
  m_prevExtremumPitch = -1;
  maxima = new Array1d<int>();
  minima = new Array1d<int>();
  nsdfAggregateData.resize(channel_->nsdfData.size(), 0.0f);
  nsdfAggregateDataScaled.resize(channel_->nsdfData.size(), 0.0f);
  nsdfAggregateRoof = 0.0;
  firstNsdfPeriod = 0.0f;
  currentNsdfPeriod = 0.0f;
  m_avgPitch = 0.0f;
}

NoteData::~NoteData()
{
}


void NoteData::resetData()
{
  m_numPeriods = 0;
}

//TODO Remove this class or make m_params more smart. Method below can be invoked without m_params
void NoteData::addData(AnalysisData *analysisData, float periods)
{
  double topP = 140.0;
  if (m_params != 0) // to avoid seg fault if no m_params
        topP = m_params->topPitch;
  m_maxLogRMS = MAX(m_maxLogRMS, analysisData->logrms());
  m_maxIntensityDB = MAX(m_maxIntensityDB, analysisData->maxIntensityDB());
  m_maxCorrelation = MAX(m_maxCorrelation, analysisData->correlation());
  m_maxPurity = MAX(m_maxPurity, analysisData->volumeValue());
  m_volume = MAX(m_volume, dB2Normalised(analysisData->logrms()));
  m_numPeriods += periods; //sum up the periods
  m_avgPitch = bound(freq2pitch(avgFreq()), 0.0, topP);
}


double NoteData::noteLength() {
  return double(numChunks()) * double(channel->framesPerChunk()) / double(channel->rate());
}


void NoteData::recalcAvgPitch() {
  double topP = 140.0;
  if (m_params != 0) // to avoid seg fault if no m_params
    topP = m_params->topPitch;
  m_numPeriods = 0.0f;
  for(int j=startChunk(); j<endChunk(); j++) {
      m_numPeriods += float(channel->framesPerChunk()) / float(channel->dataAtChunk(j)->period);
  }
  m_avgPitch = bound(freq2pitch(avgFreq()), 0.0, topP);
}
