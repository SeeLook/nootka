/***************************************************************************
                          channel.cpp  -  description
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

#include "channel.h"

#include <math.h>
#include <algorithm>

#include "conversions.h"
#include "filters/FastSmoothedAveragingFilter.h"
#include "filters/FixedAveragingFilter.h"
#include "filters/GrowingAveragingFilter.h"
#include "bspline.h"
#include "mytransforms.h"
#include "useful.h"
#include <QDebug>


const double shortTime = 0.08; //0.08; //0.18;
const float shortBase = 0.1f;
const float shortStretch = 1.5; //1.0f; //3.5f;

const double longTime = 0.8;
const float longBase = 0.02f; //0.1f;
const float longStretch = 0.2f; //0.3f; //1.0f;


/*
This filter was created in Matlab using
[Bb,Ab]=butter(2,(150/(sampleRate/2)),'high');
num2str(Bb, '%1.52f\n')
num2str(Ab, '%1.52f\n')
*/
double highPassFilterCoeffB[6][3] = {
  { //96000 Hz
    0.9930820351754101604768720790161751210689544677734375,
    -1.9861640703508203209537441580323502421379089355468750,
    0.9930820351754101604768720790161751210689544677734375
  },  { //48000 Hz
    0.9862119246270822925382049106701742857694625854492188,
    -1.9724238492541645850764098213403485715389251708984375,
    0.9862119246270822925382049106701742857694625854492188
  },  { //44100 Hz
    0.9850017578724193922923291211191099137067794799804688,
    -1.9700035157448387845846582422382198274135589599609375,
    0.9850017578724193922923291211191099137067794799804688
  },  { //22050 Hz
    0.9702283766983297308428291216841898858547210693359375,
    -1.9404567533966594616856582433683797717094421386718750,
    0.9702283766983297308428291216841898858547210693359375
  },  { //11025 Hz
    0.9413417959923573441471944533986970782279968261718750,
    -1.8826835919847146882943889067973941564559936523437500,
    0.9413417959923573441471944533986970782279968261718750
  },  { //8000 Hz
    0.9200661584291680572533778104116208851337432861328125,
    -1.8401323168583361145067556208232417702674865722656250,
    0.9200661584291680572533778104116208851337432861328125
  }
};

double highPassFilterCoeffA[6][3] = {
  { //96000 Hz
    1.0,
    -1.9861162115408896866597387997899204492568969726562500,
    0.9862119291607510662700519787904340773820877075195312
  },  { //48000 Hz
    1.0,
    -1.9722337291952660720539824978914111852645874023437500,
    0.9726139693130629870765346822736319154500961303710938
  },  { //44100 Hz
    1.0,
    -1.9697785558261799998547303403029218316078186035156250,
    0.9702284756634975693145861441735178232192993164062500
  },  { //22050 Hz
    1.0,
    -1.9395702073516702945710221683839336037635803222656250,
    0.9413432994416484067556893933215178549289703369140625
  },  { //11025 Hz
    1.0,
    -1.8792398422342264652229459898080676794052124023437500,
    0.8861273417352029113658318237867206335067749023437500
  },  { //8000 Hz
    1.0,
    -1.8337326589246480956774121295893564820289611816406250,
    0.8465319747920239112914941870258189737796783447265625
  }
};

/*
This filter was created in matlab using
0  [Bb,Ab]=butter(2,1000/(44100/2),'low');
   num2str(Bb, '%1.52f\n')
   num2str(Ab, '%1.52f\n')

1  [Bb,Ab]=butter(2,0.25,'low');

2  [Bb,Ab]=butter(2,50/(44100/2),'low');
*/
double lowPassFilterCoeffB[3][3] = {
  { //44100 Hz
    0.0046039984750224638432314350211527198553085327148438,
    0.0092079969500449276864628700423054397106170654296875,
    0.0046039984750224638432314350211527198553085327148438
  }, {
    0.0976310729378175312653809214680222794413566589355469,
    0.1952621458756350625307618429360445588827133178710938,
    0.0976310729378175312653809214680222794413566589355469
  }, {
    0.0000126234651238732453748525585979223251342773437500,
    0.0000252469302477464907497051171958446502685546875000,
    0.0000126234651238732453748525585979223251342773437500
  }
};

double lowPassFilterCoeffA[3][3] = {
  { //44100 Hz
    1.0,
    -1.7990964094846682019834815946524031460285186767578125,
    0.8175124033847580573564073347370140254497528076171875
  }, {
    1.0,
    -0.9428090415820633563015462641487829387187957763671875,
    0.3333333333333333703407674875052180141210556030273438
  }, {
    1.0,
    -1.9899255200849252922523646702757105231285095214843750,
    0.9899760139454206742115616179944481700658798217773438
  }
};

Channel::Channel(TpitchFinder *parent_, int size_, int k_) :
	m_lookup(0, 128),
	m_parent(parent_)
{
  int sampleRate = m_parent->aGl()->rate;
  if(k_ == 0) k_ = (size_ + 1) / 2;
  directInput.resize(size_, 0.0);
  nsdfData.resize(k_, 0.0);
  nsdfAggregateData.resize(k_, 0.0);
  nsdfAggregateDataScaled.resize(k_, 0.0);
  nsdfAggregateRoof = 0.0;
  fftData1.resize(size_ / 2, 0.0);
  fftData2.resize(size_ / 2, 0.0);
  fftData3.resize(size_ / 2, 0.0);
  cepstrumData.resize(size_ / 2, 0.0);
  rmsFloor = m_parent->aGl()->dBFloor;
  rmsCeiling = m_parent->aGl()->dBFloor;

  m_visible = true;
  m_noteIsPlaying = false;
  setIntThreshold(m_parent->aGl()->threshold);
//   mutex = new QMutex(true);
  mutex = new QMutex();
  isLocked = false;
  int filterIndex = 2;
  
 //choose the closest filter index
  if(sampleRate > (48000 + 96000) / 2) filterIndex = 0; //96000 Hz
  else if(sampleRate > (44100 + 48000) / 2) filterIndex = 1; //48000 Hz
  else if(sampleRate > (22050 + 44100) / 2) filterIndex = 2; //44100 Hz
  else if(sampleRate > (11025 + 22050) / 2) filterIndex = 3; //22050 Hz
  else if(sampleRate > (8000 + 11025) / 2) filterIndex = 4; //11025 Hz
  else filterIndex = 5; //8000 Hz
  highPassFilter = new IIR_Filter(highPassFilterCoeffB[filterIndex], highPassFilterCoeffA[filterIndex], 3);

  fastSmooth = new fast_smooth(size_ / 8);
}

Channel::~Channel()
{
  delete fastSmooth;
	mutex->lock(); // wait for performed routines
	mutex->unlock();
  delete mutex;
  delete highPassFilter;
}


void Channel::resize(int newSize, int k_) {
  if(k_ == 0) k_ = (newSize + 1) / 2;
  directInput.resize(newSize, 0.0);
  nsdfData.resize(k_, 0.0);
  nsdfAggregateData.resize(k_, 0.0);
  nsdfAggregateDataScaled.resize(k_, 0.0);
  fftData1.resize(newSize / 2, 0.0);
  fftData2.resize(newSize / 2, 0.0);
  fftData3.resize(newSize / 2, 0.0);
  cepstrumData.resize(newSize / 2, 0.0);
  m_lookup.clear();
}


void Channel::shift_left(int n) {
  directInput.shift_left(n);
}


void Channel::processNewChunk(FilterState *filterState) {
  lock();
  m_lookup.push_back(AnalysisData());
  m_lookup.back().filterState = *filterState;
  m_parent->transforms()->calculateAnalysisData(int(m_lookup.size())-1, this);
  unlock();
}


void Channel::reset() {
  std::fill(begin(), end(), 0.0f);
}

/**
 * Returns the average pitch for a given channel between two frames.
 * It weights using a -ve cos shaped window
 *
 * @param begin the starting frame number.
 * @param end   the ending frame number.
 *
 * @return the average pitch, or -1 if there were no valid pitches.
 **/
float Channel::averagePitch(int begin, int end)
{
  if(begin < 0) begin = 0;
  if (begin >= end) return -1;
  // Init the total to be the first item if certain enough or zero if not

  //do a weighted sum (using cosine window smoothing) to find the average note
  double goodCount = 0.0;
  double total = 0.0;
  double size = double(end - begin);
  double window_pos, window_weight, weight;
  AnalysisData *data;
  for (int i = begin; i < end; i++) {
    window_pos = double(i - begin) / size;
    window_weight = 0.5 - 0.5 * cos(window_pos * (2 * PI));
    data = dataAtChunk(i);
    //weight = window_weight * data->correlation * data->rms;
    weight = window_weight * data->correlation() * dB2Linear(data->logrms());
    total += data->pitch * weight;
    goodCount += weight;
  }
  return float(total / goodCount);
}


float Channel::averageMaxCorrelation(int begin, int end)
{
  if(!hasAnalysisData()) return -1;
  begin = bound(begin, 0, totalChunks()-1);
  end = bound(end, 0, totalChunks()-1);
  // Init the total to be the first item
  float totalCorrelation = dataAtChunk(begin)->correlation();

  for (int i = begin + 1; i < end; i++) {
     totalCorrelation += dataAtChunk(i)->correlation();
  }
  return (totalCorrelation / (end - begin + 1));
}


bool Channel::isVisibleChunk(AnalysisData *data)
{
  if(data->noteScore() >= m_parent->aGl()->ampThresholds[NOTE_SCORE][0]) return true;
	else return false;
}

bool Channel::isChangingChunk(AnalysisData *data)
{
  if(data->noteChangeScore() >= m_parent->aGl()->ampThresholds[NOTE_CHANGE_SCORE][0]) return true;
  else return false;
}

void Channel::backTrackNoteChange(int chunk) {
  int first = MAX(chunk - (int)ceil(longTime/timePerChunk()), getLastNote()->startChunk());
  int last = chunk; //currentNote->endChunk();
  if(first >= last) return;
  float largestWeightedDiff = 0.0f; //fabs(dataAtChunk(first)->pitch - dataAtChunk(first)->shortTermMean);
  int largestDiffChunk = first;
  int maxJ = last - first;
  for(int curChunk=first+1, j=1; curChunk<=last; curChunk++, j++) {
    float weightedDiff = fabs(dataAtChunk(curChunk)->pitch - dataAtChunk(curChunk)->shortTermMean) /** float(maxJ-j)*/;
    if(weightedDiff > largestWeightedDiff) {
      largestWeightedDiff = weightedDiff;
      largestDiffChunk = curChunk;
    }
  }
  getLastNote()->setEndChunk(largestDiffChunk);
  getLastNote()->recalcAvgPitch();
  dataAtChunk(largestDiffChunk)->reason = 5;

  //start on next note
  for(int curChunk = largestDiffChunk; curChunk <= last; curChunk++) {
    dataAtChunk(curChunk)->noteIndex = NO_NOTE;
    dataAtChunk(curChunk)->notePlaying = false;
    dataAtChunk(curChunk)->shortTermMean = dataAtChunk(curChunk)->pitch;
    dataAtChunk(curChunk)->longTermMean = dataAtChunk(curChunk)->pitch;
    dataAtChunk(curChunk)->shortTermDeviation = 0.2f;
    dataAtChunk(curChunk)->longTermDeviation = 0.05f;
    dataAtChunk(curChunk)->periodRatio = 1.0f;
  }

  int curChunk = largestDiffChunk;
  if(curChunk < last) {
    curChunk++;
  }
  while((curChunk < last) && !isVisibleChunk(dataAtChunk(curChunk))) {
    curChunk++;
  }
  if((curChunk < last) && isVisibleChunk(dataAtChunk(curChunk))) {
    m_noteIsPlaying = true;
    noteBeginning(curChunk);
    NoteData *currentNote = getLastNote();

    dataAtChunk(curChunk)->noteIndex = getCurrentNoteIndex();
    dataAtChunk(curChunk)->notePlaying = true;
    curChunk++;
    while((curChunk < last) && isVisibleChunk(dataAtChunk(curChunk))) {
      dataAtChunk(curChunk)->noteIndex = getCurrentNoteIndex();
      dataAtChunk(curChunk)->notePlaying = true;
      currentNote->addData(dataAtChunk(curChunk), float(framesPerChunk()) / float(dataAtChunk(curChunk)->period));
      curChunk++;
    }
    resetNSDFAggregate(dataAtChunk(last-1)->period); //just start the NSDF Aggregate from where we are now
    //just start with the octave estimate from the last note
    currentNote->setPeriodOctaveEstimate(getNote(getCurrentNoteIndex()-1)->periodOctaveEstimate());
  }
}

/** If the current note has shifted far enough away from the mean of the current Note
    then the note is changing.
*/
bool Channel::isNoteChanging(int chunk)
{
  AnalysisData *prevData = dataAtChunk(chunk-1);
  if(!prevData) return false;
  AnalysisData *analysisData = dataAtChunk(chunk);
  //Note: analysisData.noteIndex is still undefined here
  int numChunks = getLastNote()->numChunks();

  float diff = fabs(analysisData->pitch - analysisData->shortTermMean);
  double spread = fabs(analysisData->shortTermMean - analysisData->longTermMean) -
    (analysisData->shortTermDeviation + analysisData->longTermDeviation);
  if(numChunks >= 5 && spread > 0.0) {
    analysisData->reason = 1;
    return true;
  }

  int firstShortChunk = MAX(chunk - (int)ceil(shortTime/timePerChunk()), getLastNote()->startChunk());
  AnalysisData *firstShortData = dataAtChunk(firstShortChunk);
  double spread2 = fabs(analysisData->shortTermMean - firstShortData->longTermMean) -
    (analysisData->shortTermDeviation + firstShortData->longTermDeviation);
  analysisData->spread = spread;
  analysisData->spread2 = spread2;

  if(numChunks >= (int)(ceil(longTime/timePerChunk()) / 2.0) && spread2 > 0.0) {
    analysisData->reason = 4;
    return true;
  }
  if(numChunks > 1 && diff > 2) { //if jumping to fast anywhere then note is changing
    analysisData->reason = 2;
    return true;
  }
  return false;
}


void Channel::processNoteDecisions(int chunk, float periodDiff)
{
  AnalysisData &analysisData = *dataAtChunk(chunk);

  analysisData.reason = 0;
  //look for note transitions
  if(m_noteIsPlaying) {
    if(isVisibleChunk(&analysisData) && !isNoteChanging(chunk)) {
    } else {
      m_noteIsPlaying = false;
      noteEnding(chunk);
    }
  } else { //if(!noteIsPlaying)
    if(isVisibleChunk(&analysisData)) {
      noteBeginning(chunk);
      periodDiff = 0.0f;
      m_noteIsPlaying = true;
    }
  }

  analysisData.notePlaying = m_noteIsPlaying;

  if(m_noteIsPlaying) {
      addToNSDFAggregate(dB2Linear(analysisData.logrms()), periodDiff);
      NoteData *currentNote = getLastNote();

      analysisData.noteIndex = getCurrentNoteIndex();
      currentNote->setEndChunk(chunk+1);

      currentNote->addData(&analysisData, float(framesPerChunk()) / float(analysisData.period));
      currentNote->setPeriodOctaveEstimate(calcOctaveEstimate());
	  if(m_parent->aGl()->analysisType != e_MPM_MODIFIED_CEPSTRUM) {
        recalcNotePitches(chunk);
      }
  }
}


void Channel::noteBeginning(int chunk) {
  AnalysisData *analysisData = dataAtChunk(chunk);
  noteData.push_back(NoteData(this, chunk, analysisData, m_parent->aGl()));
  resetNSDFAggregate(analysisData->period);
}


void Channel::noteEnding(int chunk)
{
  AnalysisData &analysisData = *dataAtChunk(chunk);
  if(analysisData.reason > 0) {
    backTrackNoteChange(chunk);
  }
}

/** Uses the nsdfAggregateData to get an estimate of
    the period, over a whole note so far.
    This can be used to help determine the correct octave throughout the note
    @return the estimated note period, or -1.0 if none found
*/
float Channel::calcOctaveEstimate() {
  Array1d<float> agData = nsdfAggregateDataScaled;
  std::vector<int> nsdfAggregateMaxPositions;
  MyTransforms::findNSDFMaxima(agData.begin(), agData.size(), nsdfAggregateMaxPositions);
  if(nsdfAggregateMaxPositions.empty()) return -1.0;
  
  //get the highest nsdfAggregateMaxPosition
  uint j;
  int nsdfAggregateMaxIndex = 0;
  for(j=1; j<nsdfAggregateMaxPositions.size(); j++) {
    if(agData[nsdfAggregateMaxPositions[j]] > agData[nsdfAggregateMaxPositions[nsdfAggregateMaxIndex]]) nsdfAggregateMaxIndex = j;
  }
  //get the choosen nsdfAggregateMaxPosition
  double highest = agData[nsdfAggregateMaxPositions[nsdfAggregateMaxIndex]];
  double nsdfAggregateCutoff = highest * threshold();
  /*Allow for overide by the threshold value*/
  int nsdfAggregateChoosenIndex = 0;
  for(j=0; j<nsdfAggregateMaxPositions.size(); j++) {
    if(agData[nsdfAggregateMaxPositions[j]] >= nsdfAggregateCutoff) { nsdfAggregateChoosenIndex = j; break; }
  }
  //add 1 for index offset, ie position 0 = 1 period
  float periodEstimate = float(nsdfAggregateMaxPositions[nsdfAggregateChoosenIndex]+1); 
  return periodEstimate;
}


void Channel::recalcNotePitches(int chunk) {
  if(!isValidChunk(chunk))
    return;

  //recalculate the values for the note using the overall periodOctaveEstimate
  NoteData *currentNote = getLastNote();
  if(currentNote == NULL) return;
  int first = currentNote->startChunk();
  int last = chunk; //currentNote->endChunk();
  currentNote->resetData();
  int numNotesChangedIndex = 0;
  for(int curChunk=first; curChunk<=last; curChunk++) {
    if(chooseCorrelationIndex(curChunk, periodOctaveEstimate(curChunk))) numNotesChangedIndex++;
    calcDeviation(curChunk);
    currentNote->addData(dataAtChunk(curChunk), float(framesPerChunk()) / float(dataAtChunk(curChunk)->period));
  }
}


NoteData *Channel::getCurrentNote() {
  AnalysisData *analysisData = dataAtCurrentChunk();
  if(analysisData) {
    int noteIndex = analysisData->noteIndex;
    if(noteIndex >= 0 && noteIndex < (int)noteData.size()) return &noteData[noteIndex];
  }
  return NULL;
}


void Channel::chooseCorrelationIndex1(int chunk) {
  AnalysisData &analysisData = *dataAtChunk(chunk);
  uint iterPos;
  int choosenMaxIndex = 0;
  if(analysisData.periodEstimates.empty()) { //no period found
    return;
  }
  //choose a cutoff value based on the highest value and a relative threshold
  float highest = analysisData.periodEstimatesAmp[analysisData.highestCorrelationIndex];
  float cutoff = highest * threshold();
  //find the first of the maxPositions which is above the cutoff
  for(iterPos = 0; iterPos < analysisData.periodEstimatesAmp.size(); iterPos++) {
    if(analysisData.periodEstimatesAmp[iterPos] >= cutoff) { choosenMaxIndex = iterPos; break; }
  }
  analysisData.chosenCorrelationIndex = choosenMaxIndex;
  analysisData.correlation() = analysisData.periodEstimatesAmp[choosenMaxIndex];
  
  analysisData.period = analysisData.periodEstimates[choosenMaxIndex];
  double freq = rate() / analysisData.period;
  analysisData.fundamentalFreq = float(freq);
  analysisData.pitch = bound(freq2pitch(freq), 0.0, m_parent->aGl()->topPitch);
  analysisData.pitchSum = (double)analysisData.pitch;
  analysisData.pitch2Sum = sq((double)analysisData.pitch);
}


bool Channel::chooseCorrelationIndex(int chunk, float periodOctaveEstimate) {
  AnalysisData &analysisData = *dataAtChunk(chunk);
  uint iterPos;
  int choosenMaxIndex = 0;
  bool isDifferentIndex = false;
  if(analysisData.periodEstimates.empty()) { //no period found
    return false;
  }
  if(m_parent->aGl()->analysisType == e_MPM || m_parent->aGl()->analysisType == e_MPM_MODIFIED_CEPSTRUM) {
  //choose the periodEstimate which is closest to the periodOctaveEstimate
    float minDist = fabs(analysisData.periodEstimates[0] - periodOctaveEstimate);
    float dist;
    for(iterPos = 1; iterPos < analysisData.periodEstimates.size(); iterPos++) {
      dist = fabs(analysisData.periodEstimates[iterPos] - periodOctaveEstimate);
      if(dist < minDist) { minDist = dist; choosenMaxIndex = iterPos; }
    }
  } else {
    choosenMaxIndex = analysisData.highestCorrelationIndex;
  }
  if(choosenMaxIndex != analysisData.chosenCorrelationIndex) isDifferentIndex = true;
  analysisData.chosenCorrelationIndex = choosenMaxIndex;
  analysisData.correlation() = analysisData.periodEstimatesAmp[choosenMaxIndex];
  
  analysisData.period = analysisData.periodEstimates[choosenMaxIndex];
  double freq = rate() / analysisData.period;
  analysisData.fundamentalFreq = float(freq);
  analysisData.pitch = bound(freq2pitch(freq), 0.0, m_parent->aGl()->topPitch );
  if(chunk > 0 && !isFirstChunkInNote(chunk)) {
    analysisData.pitchSum = dataAtChunk(chunk-1)->pitchSum + (double)analysisData.pitch;
    analysisData.pitch2Sum = dataAtChunk(chunk-1)->pitch2Sum + sq((double)analysisData.pitch);
  } else {
    analysisData.pitchSum = (double)analysisData.pitch;
    analysisData.pitch2Sum = sq((double)analysisData.pitch);
  }
  return isDifferentIndex;
}


void Channel::calcDeviation(int chunk) {
  int lastChunk = chunk;
  int currentNoteIndex = getCurrentNoteIndex();
  AnalysisData &lastChunkData = *dataAtChunk(lastChunk);
  if(currentNoteIndex < 0) return;

  //Do long term calculation
  int firstChunk = MAX(lastChunk - (int)ceil(longTime/timePerChunk()), noteData[currentNoteIndex].startChunk());
  AnalysisData *firstChunkData = dataAtChunk(firstChunk);
  int numChunks = (lastChunk - firstChunk);
  double mean_sum, mean, sumX2, variance, standard_deviation;
  if(numChunks > 0) {
    mean_sum = (lastChunkData.pitchSum - firstChunkData->pitchSum);
    mean = mean_sum / double(numChunks);
    lastChunkData.longTermMean = mean;
    sumX2 = (lastChunkData.pitch2Sum - firstChunkData->pitch2Sum);
    variance = sumX2 / double(numChunks) - sq(mean);
    standard_deviation = sqrt(fabs(variance));
    lastChunkData.longTermDeviation = longBase + sqrt(standard_deviation)*longStretch;
  } else {
    lastChunkData.longTermMean = firstChunkData->pitch;
    lastChunkData.longTermDeviation = longBase;
  }

  //Do short term calculation
  firstChunk = MAX(lastChunk - (int)ceil(shortTime/timePerChunk()), noteData[currentNoteIndex].startChunk());
  firstChunkData = dataAtChunk(firstChunk);
  numChunks = (lastChunk - firstChunk);
  if(numChunks > 0) {
    mean_sum = (lastChunkData.pitchSum - firstChunkData->pitchSum);
    mean = mean_sum / double(numChunks);
    lastChunkData.shortTermMean = mean;
    sumX2 = (lastChunkData.pitch2Sum - firstChunkData->pitch2Sum);
    variance = sumX2 / double(numChunks) - sq(mean);
    standard_deviation = sqrt(fabs(variance));
    lastChunkData.shortTermDeviation = shortBase + sqrt(standard_deviation)*shortStretch;
  } else {
    lastChunkData.shortTermMean = firstChunkData->pitch;
    lastChunkData.shortTermDeviation = shortBase;
  }
}


bool Channel::isFirstChunkInNote(int chunk)
{
  AnalysisData *analysisData = dataAtChunk(chunk);
  if(analysisData && analysisData->noteIndex >= 0 && noteData[analysisData->noteIndex].startChunk() == chunk) return true;
  else return false;
}


void Channel::resetNSDFAggregate(float period)
{
  nsdfAggregateRoof = 0.0;
  std::fill(nsdfAggregateData.begin(), nsdfAggregateData.end(), 0.0f);
  std::fill(nsdfAggregateDataScaled.begin(), nsdfAggregateDataScaled.end(), 0.0f);

  NoteData *currentNote = getLastNote();
  currentNote->nsdfAggregateRoof = 0.0;
  currentNote->currentNsdfPeriod = currentNote->firstNsdfPeriod = period;
}

void Channel::addToNSDFAggregate(const float scaler, float periodDiff)
{
  AnalysisData &analysisData = *dataAtCurrentChunk();

  nsdfAggregateRoof += scaler;
  addElements(nsdfAggregateData.begin(), nsdfAggregateData.end(), nsdfData.begin(), scaler);

  NoteData *currentNote = getLastNote();
  currentNote->nsdfAggregateRoof += scaler;
  currentNote->currentNsdfPeriod += periodDiff;
  float periodRatio = currentNote->currentNsdfPeriod / currentNote->firstNsdfPeriod;
  analysisData.periodRatio = periodRatio;
  int len = nsdfData.size();
  float stretch_len = float(len) * periodRatio;
  Array1d<float> stretch_data(len);

  //the scaled version
  stretch_array(len, nsdfData.begin(), len, stretch_data.begin(), 0.0f, stretch_len, LINEAR);
  addElements(nsdfAggregateDataScaled.begin(), nsdfAggregateDataScaled.end(), stretch_data.begin(), scaler);
  copyElementsDivide(nsdfAggregateDataScaled.begin(), nsdfAggregateDataScaled.end(), currentNote->nsdfAggregateDataScaled.begin(), currentNote->nsdfAggregateRoof);

  //the unscaled version
  copyElementsDivide(nsdfAggregateData.begin(), nsdfAggregateData.end(), currentNote->nsdfAggregateData.begin(), currentNote->nsdfAggregateRoof);
}


float Channel::calcDetailedPitch(float *input, double period, int /*chunk*/) {
  const int pitchSamplesRange = 4; //look 4 samples either side of the period. Total of 9 different subwindows.

  int n = size();
  int i, j, j2;
  int subwindow_size;
  if(period < pitchSamplesRange || period > double(n)*(3.0/8.0) - pitchSamplesRange)
    return 0.0f;

  int iPeriod = int(floor(period));
  subwindow_size = n/4;
  int num = n/2;
  int left_start = n/4 - iPeriod/2;
  int left_finsh = left_start + num;
  int right_start = n/4 + iPeriod/2;
  int right_finsh = right_start + num;
  if(iPeriod > subwindow_size) {
    subwindow_size = n/4 - (iPeriod - n/4);
    left_start = 0;
    left_finsh = left_start + num;
    right_start = iPeriod;
    right_finsh = right_start + num;
  }

  std::vector<int> periods;
  for(j=-pitchSamplesRange; j<=pitchSamplesRange; j++) periods.push_back(iPeriod+j);
  int ln = periods.size();

  std::vector<float> squareTable(n);
  for(j=0; j<n; j++) squareTable[j] = sq(input[j]);

  std::vector<float> left(ln);
  std::vector<float> right(ln);
  std::vector<float> left_pow(ln);
  std::vector<float> right_pow(ln);
  std::vector<float> pow(ln);
  std::vector<float> err(ln);
  std::vector<float> result(ln);
  Array1d<float> unsmoothed(num);

  //calc the values of pow and err for the first in each row.
  for(i=0; i<ln; i++) {
    left_pow[i] = right_pow[i] = pow[i] = err[i] = 0;
    int offset = periods[i]-iPeriod;
    for(j=0, j2=periods[i]; j<subwindow_size-offset; j++, j2++) {
      left_pow[i] += squareTable[j]; //sq(input[j]);
      right_pow[i] += squareTable[j2]; //sq(input[j2]);
      err[i] += sq(input[j] - input[j2]);
    }
  }
  int left1 = 0;
  int left2;
  int right1;
  int right2 = left1 + subwindow_size + iPeriod;

  for(; left1 < num; left1++, right2++) {
    for(i = 0; i < ln; i++) {
      right1 = left1+periods[i];
      left2 = right2 - periods[i];

      pow[i] = left_pow[i] + right_pow[i];
      result[i] = 1.0 - (err[i] / pow[i]);

      err[i] += sq(input[left2] - input[right2]) - sq(input[left1] - input[right1]);
      left_pow[i]  += squareTable[left2]  - squareTable[left1];
      right_pow[i] += squareTable[right2] - squareTable[right1];
    }

    int pos = int(std::max_element(result.begin(), result.begin()+ln) - result.begin());
    if(pos > 0 && pos < ln-1)
      unsmoothed[left1] = double(periods[pos]) + parabolaTurningPoint(result[pos-1], result[pos], result[pos+1]);
    else
      unsmoothed[left1] = double(periods[pos]);
  }
  float periodDiff = unsmoothed.back() - unsmoothed.front();
  return periodDiff;
}


float Channel::periodOctaveEstimate(int chunk) {
  AnalysisData *analysisData = dataAtChunk(chunk);
  if(analysisData && analysisData->noteIndex >= 0) {
    return noteData[analysisData->noteIndex].periodOctaveEstimate() * analysisData->periodRatio;
  }
  else return -1.0f;
}
