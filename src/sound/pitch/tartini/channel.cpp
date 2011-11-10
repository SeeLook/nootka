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
 ***************************************************************************/

#include "channel.h"
//#include "gdata.h"
#include "soundfile.h"
//#include "fundamental.h"

#include <algorithm>
#include "myassert.h"
#include "mystring.h"
#include <QFileDialog>
#include <QFile>
#include <QTextStream>

#include "FastSmoothedAveragingFilter.h"
#include "FixedAveragingFilter.h"
#include "GrowingAveragingFilter.h"
#include "prony.h"
#include "musicnotes.h"
#include "bspline.h"

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

/*
Channel::Channel() : lookup(0, 512), filteredData(0, 8192)
{
  _pitch_method = 2;
  color = Qt::black;
  visible = true;
  //setThreshold(0.97f);
  setIntThreshold(gdata->settings.getInt("Analysis", "thresholdValue"));
  //estimate = 0.0;
  freq = 0.0;
  mutex = new QMutex(true);
  filterStateX1 = filterStateX2 = 0.0;
  filterStateY1 = filterStateY2 = 0.0;
}
*/

Channel::Channel(SoundFile *parent_, int size_, int k_) : lookup(0, 128)/*, filteredData(0, 8192)*/
{
  setParent(parent_);
  int sampleRate = parent->rate();
  if(k_ == 0) k_ = (size_ + 1) / 2;
  directInput.resize(size_, 0.0);
  filteredInput.resize(size_, 0.0);
  nsdfData.resize(k_, 0.0);
  nsdfAggregateData.resize(k_, 0.0);
  nsdfAggregateDataScaled.resize(k_, 0.0);
  nsdfAggregateRoof = 0.0;
  fftData1.resize(size_/2, 0.0);
  fftData2.resize(size_/2, 0.0);
  fftData3.resize(size_/2, 0.0);
  cepstrumData.resize(size_/2, 0.0);
  detailedPitchData.resize(size_/2, 0.0);
  detailedPitchDataSmoothed.resize(size_/2, 0.0);
  _pitch_method = 2;
  color = Qt::black;
  coefficients_table.resize(size_*4);
  rmsFloor = gdata->dBFloor();
  rmsCeiling = gdata->dBFloor();
  pronyWindowSize = int(ceil(0.4/timePerChunk()));
  pronyData.resize(pronyWindowSize);

  //freqLookup.resize(0);
  //noteLookup.resize(0);
  //errorLookup.resize(0);
  //lookup.resize(0);
  //lookup.clear();
  visible = true;
  noteIsPlaying = false;
  //setThreshold(0.97f);
  //setIntThreshold(gdata->settings.getInt("Analysis", "thresholdValue"));
  setIntThreshold(gdata->qsettings->value("Analysis/thresholdValue", 93).toInt());
  //estimate = 0.0;
  freq = 0.0;
  mutex = new QMutex(true);
  isLocked = false;
  int filterIndex = 2;
  //choose the closest filter index
  if(sampleRate > (48000 + 96000) / 2) filterIndex = 0; //96000 Hz
  else if(sampleRate > (44100 + 48000) / 2) filterIndex = 1; //48000 Hz
  else if(sampleRate > (22050 + 44100) / 2) filterIndex = 2; //44100 Hz
  else if(sampleRate > (11025 + 22050) / 2) filterIndex = 3; //22050 Hz
  else if(sampleRate > (8000 + 11025) / 2) filterIndex = 4; //11025 Hz
  else filterIndex = 5; //8000 Hz
 // printf("filterIndex = %d\n", filterIndex);
  //highPassFilter = new IIR_Filter();
  //lowPassFilter = new IIR_Filter();
  //highPassFilter->make_IIR(highPassFilterCoeffB[filterIndex], highPassFilterCoeffA[filterIndex], 3);
  //lowPassFilter->make_IIR(lowPassFilterCoeffB[2], lowPassFilterCoeffA[2], 3);
  highPassFilter = new IIR_Filter(highPassFilterCoeffB[filterIndex], highPassFilterCoeffA[filterIndex], 3);
  //lowPassFilter = new IIR_Filter(lowPassFilterCoeffB[2], lowPassFilterCoeffA[2], 3);
  pitchSmallSmoothingFilter = new GrowingAverageFilter(sampleRate/64);
  pitchBigSmoothingFilter = new FastSmoothedAveragingFilter(sampleRate/16);
  //pitchBigSmoothingFilter = new FixedAverageFilter(sampleRate/16);
  //pitchBigSmoothingFilter = new GrowingAverageFilter(sampleRate/16);

  //printf("typeid = %s\n", typeid(highPassFilter).name());
  //printf("typeid = %s\n", typeid(lowPassFilter).name());

  //highPassFilter->print();
  //lowPassFilter->print();
  //filterStateX1 = filterStateX2 = 0.0;
  //filterStateY1 = filterStateY2 = 0.0;

  fastSmooth = new fast_smooth(size_/8);
}

Channel::~Channel()
{
  delete fastSmooth;
  delete mutex;
  delete highPassFilter;
}

void Channel::resetIntThreshold(int thresholdPercentage)
{
  _threshold = float(thresholdPercentage) / 100.0f;
  uint j;
  for(j=0; j<lookup.size(); j++) {
    //parent->myTransforms.chooseCorrelationIndex(lookup.at(j), threshold(), lookup.at(j).periodOctaveEstimate);
    //chooseCorrelationIndex(lookup.at(j), threshold(), lookup.at(j).periodOctaveEstimate);
    //chooseCorrelationIndex(j, dataAtChunk(j)->periodOctaveEstimate);
    chooseCorrelationIndex(j, periodOctaveEstimate(j));
    calcDeviation(j);
  }
  clearFreqLookup();
}

void Channel::resize(int newSize, int k_)
{
  //Array1d<float>::resize(newSize);
  coefficients_table.resize(newSize*4);
  if(k_ == 0) k_ = (newSize + 1) / 2;
  directInput.resize(newSize, 0.0);
  filteredInput.resize(newSize, 0.0);
  nsdfData.resize(k_, 0.0);
  nsdfAggregateData.resize(k_, 0.0);
  nsdfAggregateDataScaled.resize(k_, 0.0);
  fftData1.resize(newSize/2, 0.0);
  fftData2.resize(newSize/2, 0.0);
  fftData3.resize(newSize/2, 0.0);
  cepstrumData.resize(newSize/2, 0.0);
  detailedPitchData.resize(newSize/2, 0.0);
  detailedPitchDataSmoothed.resize(newSize/2, 0.0);
  //freqLookup.resize(0);
  //noteLookup.resize(0);
  //errorLookup.resize(0);
  //lookup.resize(0);
  lookup.clear();
}

void Channel::shift_left(int n)
{
  //Array1d<float>::shift_left(n);
  directInput.shift_left(n);
  filteredInput.shift_left(n);
  coefficients_table.shift_left(n*4);
  //frame_num += n;
}

void Channel::calc_last_n_coefficients(int n)
{
  //build a table of coefficients for calculating interpolation
  //gdata->coefficients_table.shift_left(n*4);
	int start_pos = MAX(size() - n, 3);
	float *buf_pos = begin() + start_pos;
	float *coeff_pos = coefficients_table.begin()+start_pos*4;
	//printf("toRead=%d, length()=%d, w=%d, start_pos=%d\n", toRead, gdata->mainBuffer->length(), gdata->coefficients_table.w(), start_pos);
	for(;buf_pos < end(); buf_pos++) {
    const float &xm1 = buf_pos[-3];
    const float &x0 = buf_pos[-2];
    const float &x1 = buf_pos[-1];
    const float &x2 = buf_pos[0];
    *coeff_pos++ = (3 * (x0-x1) - xm1 + x2) / 2;
    *coeff_pos++ = 2*x1 + xm1 - (5*x0 + x2) / 2;
    *coeff_pos++ = (x1 - xm1) / 2;
    *coeff_pos++ = x0;
  }
}

/** Analysis the current data and add it to the end of the lookup table
  Note: The Channel shoud be locked before calling this.
*/
void Channel::processNewChunk(FilterState *filterState)
{
  myassert(locked());
/*
  int n = framesPerChunk();
  if(!coefficients_table.isEmpty()) {
    calc_last_n_coefficients(n);
    //std::cout << coefficients_table;
  }
*/
  
  //printf("%d, %d, %d, %d\n", currentChunk(), parent->currentRawChunk(), parent->currentStreamChunk(), lookup.size());
  //fflush(stdout);
  //lock();
  myassert(parent->currentRawChunk() == MAX(0, parent->currentStreamChunk()-1) ||
           parent->currentRawChunk() == MAX(0, parent->currentStreamChunk()));
  lookup.push_back(AnalysisData());
  lookup.back().filterState = *filterState;
  //printf("lookup=%d\n", lookup.capacity());
  //AnalysisData &analysisData = lookup.back();
  //parent->myTransforms.calculateAnalysisData(begin(), analysisData, nsdfData.begin(), threshold());
  //parent->myTransforms.calculateAnalysisData(begin(), lookup.back(), nsdfData.begin(), threshold());
  //parent->myTransforms.calculateAnalysisData(begin(), lookup.back(), this, threshold());
  parent->myTransforms.calculateAnalysisData(/*begin(), */int(lookup.size())-1, this/*, threshold()*/);
  //unlock();
}

/** Analysis the current data and put it at chunk position in the lookup table
  Note: The Channel shoud be locked before calling this.
  @param chunk The chunk number to store the data at
*/
void Channel::processChunk(int chunk)
{
  //myassert(chunk >= 0 && chunk < totalChunks());
  //lock();
  myassert(locked());
  if(chunk >= 0 && chunk < totalChunks())
    //parent->myTransforms.calculateAnalysisData(begin(), *dataAtChunk(chunk), nsdfData.begin(), threshold());
    //parent->myTransforms.calculateAnalysisData(begin(), *dataAtChunk(chunk), this, threshold());
    parent->myTransforms.calculateAnalysisData(/*begin(), */chunk, this/*, threshold()*/);
  //unlock();
}

void Channel::reset()
{
  std::fill(begin(), end(), 0.0f);
  std::fill(filteredInput.begin(), filteredInput.end(), 0.0f);
  std::fill(coefficients_table.begin(), coefficients_table.end(), 0.0f);
  //estimate = 0.0;
}

/*
QColor Channel::getNextColour()
{
	return colours[nextColour++ % 12];
}
*/

QString Channel::getUniqueFilename()
{
	QString endingStar = (parent->saved()) ? QString("") : QString("*");

	if (getParent()->channels.size() == 1) {
		return QString(getFilenamePart(getParent()->filename)) + endingStar;
		//return QString(getParent()->filename) + endingStar;
	} else {
		for (int i = 0; i < getParent()->channels.size(); i++) {
			if ( getParent()->channels.at(i) == this ) {
				return QString(getFilenamePart(getParent()->filename)) + " (" + QString::number(i+1) + ")" + endingStar;
				//return QString(getParent()->filename) + " (" + QString::number(i+1) + ")" + endingStar;
			}
		}
	}

	// If we're here, we didn't find the channel in the parent's channels array.
	// This should never happen!
	myassert(false);
	return QString(getFilenamePart(getParent()->filename)) + endingStar;
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
  //myassert(mutex->locked());
  myassert(locked());
  myassert(isValidChunk(begin) && isValidChunk(end-1));

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
  //if (goodCount < 1.0) return -1;
  return float(total / goodCount);
}

/*
float Channel::averagePitch(Channel *ch, int begin, int end)
{
  ChannelLocker channelLocker(ch);

  //myassert(begin >= 0 && begin < int(ch->lookup.size()));
  //myassert(end >= 0 && end < int(ch->lookup.size()));
  if(ch->totalChunks() == 0) return -1;
  if(!ch->hasAnalysisData()) return -1;
  begin = bound(begin, 0, ch->totalChunks()-1);
  end = bound(end, 0, ch->totalChunks());
  //if (begin > end || begin >= ch->lookup.size() || end >= ch->lookup.size()) return -1;

  // Init the total to be the first item if certain enough or zero if not
  float goodCount = 0.0f;
  float total = 0.0f;

  for (int i = begin; i < end; i++) {
    if (ch->dataAtChunk(i)->correlation() >= 0.8) {
    //if (ch->isVisibleNote(ch->dataAtChunk(i)->noteIndex)) {
      total += ch->dataAtChunk(i)->pitch * ch->dataAtChunk(i)->correlation();
      //printf("Note is %f\n", ch->lookup.at(i).note);
      goodCount += ch->dataAtChunk(i)->correlation();
    }
  }
  //printf("begin = %d, end = %d\n", begin, end);
  //printf("total = %f, goodCount = %f\n", total, goodCount);
  if (goodCount == 0.0f) return -1;
  return (total / goodCount);
}
*/

float Channel::averageMaxCorrelation(int begin, int end)
{
  //myassert(mutex->locked());
  myassert(locked());
  //ChannelLocker channelLocker(ch);
  //myassert(begin >= 0 && begin < int(ch->lookup.size()));
  //myassert(end >= 0 && end < int(ch->lookup.size()));
  if(!hasAnalysisData()) return -1;
  begin = bound(begin, 0, totalChunks()-1);
  end = bound(end, 0, totalChunks()-1);
  //if (begin > end || begin >= ch->lookup.size() || end >= ch->lookup.size()) return -1;

  // Init the total to be the first item
  float totalCorrelation = dataAtChunk(begin)->correlation();

  for (int i = begin + 1; i < end; i++) {
     //total += ch->dataAtChunk(i)->volumeValue;
     totalCorrelation += dataAtChunk(i)->correlation();
  }
  //return (total / (end - begin + 1));
  return (totalCorrelation / (end - begin + 1));
}

AnalysisData *Channel::getActiveChannelCurrentChunkData()
{
  Channel *active = gdata->getActiveChannel();
  return (active) ? active->dataAtCurrentChunk() : NULL;
}

void Channel::clearFreqLookup()
{
  ChannelLocker channelLocker(this);
  summaryZoomLookup.clear();
  normalZoomLookup.clear();
}

void Channel::clearAmplitudeLookup()
{
  ChannelLocker channelLocker(this);
  amplitudeZoomLookup.clear();
}

void Channel::recalcScoreThresholds()
{
  AnalysisData *d;
  ChannelLocker channelLocker(this);
  for(int j=0; j<totalChunks(); j++)
    if((d = dataAtChunk(j)) != NULL) d->calcScores();
}

/**
  @param noteIndex_ the index of the note to inquire about
  @return true if the loudest part of the note is above the noiseThreshold
*/
bool Channel::isVisibleNote(int noteIndex_)
{
  myassert(noteIndex_ < (int)noteData.size());
  //if(!(noteIndex_ >= 0)) return false;
  if(noteIndex_ == NO_NOTE) return false;
/*
  if(gdata->amplitudeMode() == AMPLITUDE_RMS) {
    //if(noteData[noteIndex_].maxRms >= gdata->noiseThreshold()) return true;
    //if(noteData[noteIndex_].maxLogRMS >= gdata->noiseThresholdDB()) return true;
    if(noteData[noteIndex_].maxLogRMS >= gdata->ampThreshold(AMPLITUDE_RMS, 0)) return true;
  } else if(gdata->amplitudeMode() == AMPLITUDE_MAX_INTENSITY) {
    //if(noteData[noteIndex_].maxIntensityDB >= gdata->noiseThresholdDB()) return true;
    if(noteData[noteIndex_].maxIntensityDB >= gdata->ampThreshold(AMPLITUDE_MAX_INTENSITY, 0)) return true;
  } else if(gdata->amplitudeMode() == AMPLITUDE_CORRELATION) {
    //if((noteData[noteIndex_].maxCorrelation-1.0f) * 160.0f >= gdata->noiseThresholdDB()) return true;
    if(noteData[noteIndex_].maxCorrelation >= gdata->ampThreshold(AMPLITUDE_CORRELATION, 0)) return true;
  //} else if(gdata->amplitudeMode() == AMPLITUDE_PURITY) {
  //  if(noteData[noteIndex_].maxPurity >= gdata->noiseThresholdDB()) return true;
  }
  return false;
*/
  return true;
}

bool Channel::isVisibleChunk(AnalysisData *data)
{
  myassert(data);
  if(data->noteScore() >= gdata->ampThreshold(NOTE_SCORE,0)) return true;
/*  if(gdata->pitchContourMode() == 0) {
    if(gdata->amplitudeMode() == AMPLITUDE_RMS) {
      //if(data->logrms() >= gdata->noiseThresholdDB()) return true;
      if(data->logrms() >= gdata->ampThreshold(AMPLITUDE_RMS, 0)) return true;
    } else if(gdata->amplitudeMode() == AMPLITUDE_MAX_INTENSITY) {
      //if(data->maxIntensityDB() >= gdata->noiseThresholdDB()) return true;
      if(data->maxIntensityDB() >= gdata->ampThreshold(AMPLITUDE_MAX_INTENSITY, 0)) return true;
    } else if(gdata->amplitudeMode() == AMPLITUDE_CORRELATION) {
      //if((data->correlation()-1.0f) * 160.0f >= gdata->noiseThresholdDB()) return true;
      if(data->correlation() >= gdata->ampThreshold(AMPLITUDE_CORRELATION, 0)) return true;
    //} else if(gdata->amplitudeMode() == AMPLITUDE_PURITY) {
    //  if(data->volumeValue >= gdata->noiseThresholdDB()) return true;
    } else {
      return true;
    }
  } else {
    return isVisibleNote(data->noteIndex);
  }
*/
  return false;
}

bool Channel::isChangingChunk(AnalysisData *data)
{
  myassert(data);
  if(data->noteChangeScore() >= gdata->ampThreshold(NOTE_CHANGE_SCORE,0)) return true;
  else return false;
}

void Channel::backTrackNoteChange(int chunk) {
  int first = MAX(chunk - (int)ceil(longTime/timePerChunk()), getLastNote()->startChunk()/*+5*/);
  //printf("ceil = %d, %d\n", (int)ceil(longTime/timePerChunk()), chunk-first);
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
    //dataAtChunk(curChunk)->noteIndex = NO_NOTE;
    //dataAtChunk(curChunk)->notePlaying = false;
    curChunk++;
  }
  while((curChunk < last) && !isVisibleChunk(dataAtChunk(curChunk))) {
    //dataAtChunk(curChunk)->noteIndex = NO_NOTE;
    //dataAtChunk(curChunk)->notePlaying = false;
    curChunk++;
  }
  if((curChunk < last) && isVisibleChunk(dataAtChunk(curChunk))) {
    noteIsPlaying = true;
    noteBeginning(curChunk);
    NoteData *currentNote = getLastNote();
    myassert(currentNote);
    //periodDiff = 0.0f;
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
  myassert(dataAtChunk(chunk));
  myassert(noteData.size() > 0);
  AnalysisData *analysisData = dataAtChunk(chunk);
  //Note: analysisData.noteIndex is still undefined here
  int numChunks = getLastNote()->numChunks();

  float diff = fabs(analysisData->pitch - analysisData->shortTermMean);
  double spread = fabs(analysisData->shortTermMean - analysisData->longTermMean) -
    (analysisData->shortTermDeviation + analysisData->longTermDeviation);
  if(numChunks >= 5 && spread > 0.0) {
    analysisData->reason = 1;
    //backTrackNoteChange(chunk);
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
    //backTrackNoteChange(chunk);
    return true;
  }
  //if(getCurrentNote()->numChunks() >= 2 && diff > 2/*1.5*/) { //if jumping to fast anywhere then note is changing
  if(numChunks > 1 && diff > 2/*1.5*/) { //if jumping to fast anywhere then note is changing
    //printf("numChunks=%d\n", getCurrentNote()->numChunks());
    //printf("analysisData->pitch=%f, ", analysisData->pitch);
    //printf("prevData->shortTermMean=%f\n", prevData->shortTermMean);
    analysisData->reason = 2;
    //backTrackNoteChange(chunk);
    return true;
  }
/*
  if(numChunks > 1 && fabs(prevData->shortTermMean - prevData->longTermMean) > prevData->longTermDeviation) {
    analysisData->reason = 3;
    return true;
  }*/
  return false;
}

/**
  @param noteIndex_ the index of the note to inquire about
  @return true if the note is long enough
*/
bool Channel::isLabelNote(int noteIndex_)
{
  myassert(noteIndex_ < (int)noteData.size());
  if(noteIndex_ >= 0 && noteData[noteIndex_].isValid()) return true;
  else return false;
}

void Channel::processNoteDecisions(int chunk, float periodDiff)
{
  myassert(locked());
  myassert(dataAtChunk(chunk));
  AnalysisData &analysisData = *dataAtChunk(chunk);
  //AnalysisData *prevAnalysisData = dataAtChunk(chunk-1);

  analysisData.reason = 0;
  //look for note transitions
  if(noteIsPlaying) {
    if(isVisibleChunk(&analysisData) /*&& !isChangingChunk(&analysisData)*/ && !isNoteChanging(chunk)) {
      //noteData.back().addData(&analysisData, float(framesPerChunk()) / float(analysisData.period));
    } else {
      //printf("ending chunk %d, ", chunk);
      //printf("%s, %s\n", isVisibleChunk(&analysisData)?"T":"F", isNoteChanging(chunk)?"T":"F");
      noteIsPlaying = false;
      noteEnding(chunk);
    }
  } else { //if(!noteIsPlaying)
    if(isVisibleChunk(&analysisData) /*&& !isChangingChunk(&analysisData)*/) {
      noteBeginning(chunk);
      periodDiff = 0.0f;
      noteIsPlaying = true;
    }
  }

  analysisData.notePlaying = noteIsPlaying;

  if(noteIsPlaying) {
      //addTo(nsdfData.begin(), nsdfData.end(), nsdfAggregateData.begin());
      //addElements(nsdfAggregateData.begin(), nsdfAggregateData.end(), nsdfData.begin(), dB2Normalised(analysisData.logrms()));
      //addToNSDFAggregate(dB2Normalised(analysisData.logrms()));
      //addToNSDFAggregate(dB2Linear(analysisData.logrms()), analysisData.period);
      addToNSDFAggregate(dB2Linear(analysisData.logrms()), periodDiff);
      //analysisData.periodOctaveEstimate = calcOctaveEstimate();
      NoteData *currentNote = getLastNote();
      myassert(currentNote);

      analysisData.noteIndex = getCurrentNoteIndex();
      currentNote->setEndChunk(chunk+1);

      currentNote->addData(&analysisData, float(framesPerChunk()) / float(analysisData.period));
      currentNote->setPeriodOctaveEstimate(calcOctaveEstimate());
      //if(!gdata->doingActiveCepstrum()) {
      //if(!gdata->analysisType() == MPM_MODIFIED_CEPSTRUM) {
      if(gdata->analysisType() != MPM_MODIFIED_CEPSTRUM) {
        recalcNotePitches(chunk);
      }
  } else { //if(!noteIsPlaying)
/*
      analysisData.noteIndex = NO_NOTE;
      analysisData.periodOctaveEstimate = -1.0f;
*/
  }
}

void Channel::noteBeginning(int chunk)
{
  AnalysisData *analysisData = dataAtChunk(chunk);
  //noteData.push_back(NoteData(chunk, chunk+1, analysisData.logrms(), analysisData.maxIntensityDB(), analysisData.correlation(), analysisData.volumeValue())); 
  //noteData.push_back(NoteData(this, chunk, analysisData, float(framesPerChunk()) / float(analysisData->period)));
  noteData.push_back(NoteData(this, chunk, analysisData));
  //initalise the aggregate NSDF data with the current NSDF data
  //std::copy(nsdfData.begin(), nsdfData.end(), nsdfAggregateData.begin());
  //addElements(nsdfAggregateData.begin(), nsdfAggregateData.end(), nsdfData.begin(), dB2Normalised(analysisData->logrms()));
  resetNSDFAggregate(analysisData->period);
  //addToNSDFAggregate(dB2Linear(analysisData->logrms()), 0.0f);
  //printf("Note Beginning: period = %f\n", analysisData->period);

  pitchSmallSmoothingFilter->reset();
  pitchBigSmoothingFilter->reset();
}

void Channel::noteEnding(int chunk)
{
  AnalysisData &analysisData = *dataAtChunk(chunk);
  //recalcNotePitches(chunk);
  /*
  NoteData *currentNote = getCurrentNote();
  myassert(currentNote);
  currentNote->nsdfAggregateData = nsdfAggregateData;
  currentNote->nsdfAggregateRoof = nsdfAggregateRoof;
  */
  if(analysisData.reason > 0) {
    backTrackNoteChange(chunk);
  }

  //getLastNote()->recalcAvgPitch();
}

/** Uses the nsdfAggregateData to get an estimate of
    the period, over a whole note so far.
    This can be used to help determine the correct octave throughout the note
    @return the estimated note period, or -1.0 if none found
*/
float Channel::calcOctaveEstimate() {
  //Array1d<float> agData = nsdfAggregateData;
  Array1d<float> agData = nsdfAggregateDataScaled;
  std::vector<int> nsdfAggregateMaxPositions;
  MyTransforms::findNSDFMaxima(agData.begin(), agData.size(), nsdfAggregateMaxPositions);
  if(nsdfAggregateMaxPositions.empty()) return -1.0;
  //myassert(!nsdfAggregateMaxPositions.empty());
  
  //get the highest nsdfAggregateMaxPosition
  uint j;
  int nsdfAggregateMaxIndex = 0;
  for(j=1; j<nsdfAggregateMaxPositions.size(); j++) {
    if(agData[nsdfAggregateMaxPositions[j]] > agData[nsdfAggregateMaxPositions[nsdfAggregateMaxIndex]]) nsdfAggregateMaxIndex = j;
  }
  //get the choosen nsdfAggregateMaxPosition
  double highest = agData[nsdfAggregateMaxPositions[nsdfAggregateMaxIndex]];
  //double nsdfAggregateCutoff = bound(nsdfAggregateRoof - (nsdfAggregateRoof - highest) * 2.0, 0.0, highest);
  double nsdfAggregateCutoff = highest * threshold();
  /*Allow for overide by the threshold value*/
  //nsdfAggregateCutoff = std::max(highest*threshold(), nsdfAggregateCutoff);
  int nsdfAggregateChoosenIndex = 0;
  for(j=0; j<nsdfAggregateMaxPositions.size(); j++) {
    if(agData[nsdfAggregateMaxPositions[j]] >= nsdfAggregateCutoff) { nsdfAggregateChoosenIndex = j; break; }
  }
  float periodEstimate = float(nsdfAggregateMaxPositions[nsdfAggregateChoosenIndex]+1); //add 1 for index offset, ie position 0 = 1 period
  //NoteData *currentNote = getLastNote();
  //myassert(currentNote);
  //float periodRatio = currentNote->currentNsdfPeriod / currentNote->firstNsdfPeriod;
  //return periodEstimate * periodRatio;
  return periodEstimate;
}

void Channel::recalcNotePitches(int chunk)
{
  if(!isValidChunk(chunk)) return;
  //AnalysisData *data = dataAtChunk(chunk);

  //noteData.back().setEndChunk(chunk+1);
  //recalculate the values for the note using the overall periodOctaveEstimate
  NoteData *currentNote = getLastNote();
  if(currentNote == NULL) return;
  myassert(currentNote);
  //float periodOctaveEstimate = currentNote->periodOctaveEstimate();
  int first = currentNote->startChunk();
  int last = chunk; //currentNote->endChunk();
  currentNote->resetData();
  int numNotesChangedIndex = 0;
  for(int curChunk=first; curChunk<=last; curChunk++) {
    //dataAtChunk(curChunk)->periodOctaveEstimate = periodOctaveEstimate;
    //if(chooseCorrelationIndex(curChunk, periodOctaveEstimate)) numNotesChangedIndex++;
    if(chooseCorrelationIndex(curChunk, periodOctaveEstimate(curChunk))) numNotesChangedIndex++;
    calcDeviation(curChunk);
    currentNote->addData(dataAtChunk(curChunk), float(framesPerChunk()) / float(dataAtChunk(curChunk)->period));
  }
  //printf("numNotesChangedIndex=%d/%d\n", numNotesChangedIndex, last-first+1);
}

NoteData *Channel::getLastNote()
{
  return (noteData.empty()) ? NULL : &noteData.back();
}

NoteData *Channel::getCurrentNote()
{
  AnalysisData *analysisData = dataAtCurrentChunk();
  if(analysisData) {
    int noteIndex = analysisData->noteIndex;
    if(noteIndex >= 0 && noteIndex < (int)noteData.size()) return &noteData[noteIndex];
  }
  return NULL;
}

NoteData *Channel::getNote(int noteIndex)
{
  if(noteIndex >= 0 && noteIndex < (int)noteData.size()) return &noteData[noteIndex];
  else return NULL;
}

/** Choose the correlation index (with no starting octave estimate)
  * For use with at the start of the note
  */
void Channel::chooseCorrelationIndex1(int chunk)
{
  myassert(dataAtChunk(chunk));
  AnalysisData &analysisData = *dataAtChunk(chunk);
  uint iterPos;
  int choosenMaxIndex = 0;
  if(analysisData.periodEstimates.empty()) { //no period found
/*
    analysisData.fundamentalFreq = 0.0f;
    analysisData.note = 0.0f;
    analysisData.chosenCorrelationIndex = -1;
    analysisData.correlation() = 0.0f;
*/
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
  
  //double period = analysisData.periodEstimates[choosenMaxIndex];
  analysisData.period = analysisData.periodEstimates[choosenMaxIndex];
  //float clarityMin = get_fine_clarity_measure(period);
  //double freq = rate / period;
  double freq = rate() / analysisData.period;
  analysisData.fundamentalFreq = float(freq);
  analysisData.pitch = bound(freq2pitch(freq), 0.0, gdata->topPitch());
  //if(isnan(analysisData.note)) analysisData.note = 0.0f;
  analysisData.pitchSum = (double)analysisData.pitch;
  analysisData.pitch2Sum = sq((double)analysisData.pitch);
}

/** This uses an octave extimate to help chose the correct correlation index
  * Returns true if the new correlation index is different from the old one
  */
bool Channel::chooseCorrelationIndex(int chunk, float periodOctaveEstimate)
{
  myassert(dataAtChunk(chunk));
  AnalysisData &analysisData = *dataAtChunk(chunk);
  uint iterPos;
  int choosenMaxIndex = 0;
  bool isDifferentIndex = false;
  if(analysisData.periodEstimates.empty()) { //no period found
/*
    analysisData.fundamentalFreq = 0.0f;
    analysisData.note = 0.0f;
    analysisData.chosenCorrelationIndex = -1;
    analysisData.correlation() = 0.0f;
*/
    return false;
  }
  if(gdata->analysisType() == MPM || gdata->analysisType() == MPM_MODIFIED_CEPSTRUM) {
    /*
    //choose a cutoff value based on the highest value and a relative threshold
    float cutoff = analysisData.periodEstimatesAmp[analysisData.highestCorrelationIndex] * threshold;
    //find the first of the maxPositions which is above the cutoff
    for(iterPos = 0; iterPos < analysisData.periodEstimatesAmp.size(); iterPos++) {
      if(analysisData.periodEstimatesAmp[iterPos] >= cutoff) { choosenMaxIndex = iterPos; break; }
    }
    */
  //choose the periodEstimate which is closest to the periodOctaveEstimate
    float minDist = fabs(analysisData.periodEstimates[0] - periodOctaveEstimate);
    float dist;
    for(iterPos = 1; iterPos < analysisData.periodEstimates.size(); iterPos++) {
      dist = fabs(analysisData.periodEstimates[iterPos] - periodOctaveEstimate);
      if(dist < minDist) { minDist = dist; choosenMaxIndex = iterPos; }
    }

    //myassert(wantedMaxPos > 0);
  } else {
    choosenMaxIndex = analysisData.highestCorrelationIndex;
  }
  if(choosenMaxIndex != analysisData.chosenCorrelationIndex) isDifferentIndex = true;
  analysisData.chosenCorrelationIndex = choosenMaxIndex;
  analysisData.correlation() = analysisData.periodEstimatesAmp[choosenMaxIndex];
  
  //double period = analysisData.periodEstimates[choosenMaxIndex];
  analysisData.period = analysisData.periodEstimates[choosenMaxIndex];
  //float clarityMin = get_fine_clarity_measure(period);
  //double freq = rate / period;
  double freq = rate() / analysisData.period;
  analysisData.fundamentalFreq = float(freq);
  analysisData.pitch = bound(freq2pitch(freq), 0.0, gdata->topPitch());
  if(chunk > 0 && !isFirstChunkInNote(chunk)) {
    analysisData.pitchSum = dataAtChunk(chunk-1)->pitchSum + (double)analysisData.pitch;
    analysisData.pitch2Sum = dataAtChunk(chunk-1)->pitch2Sum + sq((double)analysisData.pitch);
  } else {
    analysisData.pitchSum = (double)analysisData.pitch;
    analysisData.pitch2Sum = sq((double)analysisData.pitch);
  }
  //if(isnan(analysisData.note)) analysisData.note = 0.0f;
  return isDifferentIndex;
}

void Channel::calcDeviation(int chunk) {
  //int j;
  int lastChunk = chunk;
  int currentNoteIndex = getCurrentNoteIndex();
  myassert(dataAtChunk(chunk));
  AnalysisData &lastChunkData = *dataAtChunk(lastChunk);
  //assert(currentNoteIndex >= 0);
  if(currentNoteIndex < 0) return;

  //Do long term calculation
  int firstChunk = MAX(lastChunk - (int)ceil(longTime/timePerChunk()), noteData[currentNoteIndex].startChunk());
  AnalysisData *firstChunkData = dataAtChunk(firstChunk);
  //int numChunks = (lastChunk - firstChunk) + 1; //lastChunk is inclusive
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
  myassert(currentNote);
  currentNote->nsdfAggregateRoof = 0.0;
  currentNote->currentNsdfPeriod = currentNote->firstNsdfPeriod = period;
  //currentNote->resize(nsdfData.size());
  //std::fill(currentNote->nsdfAggregateData.begin(), currentNote->nsdfAggregateData.end(), 0.0f);
}

void Channel::addToNSDFAggregate(const float scaler, float periodDiff)
{
  AnalysisData &analysisData = *dataAtCurrentChunk();

  nsdfAggregateRoof += scaler;
  addElements(nsdfAggregateData.begin(), nsdfAggregateData.end(), nsdfData.begin(), scaler);

  NoteData *currentNote = getLastNote();
  myassert(currentNote);
  currentNote->nsdfAggregateRoof += scaler;
  currentNote->currentNsdfPeriod += periodDiff;
  float periodRatio = currentNote->currentNsdfPeriod / currentNote->firstNsdfPeriod;
  analysisData.periodRatio = periodRatio;
  //printf("%f, periodDiff = %f\n", currentNote->currentNsdfPeriod, periodDiff);
  //printf("currentNsdfPeriod = %f\n", currentNote->currentNsdfPeriod);
  //printf("ratio=%f\n", periodRatio);
  //addElements(currentNote->nsdfAggregateData.begin(), currentNote->nsdfAggregateData.end(), nsdfData.begin(), scaler);
  int len = nsdfData.size();
  float stretch_len = float(len) * periodRatio;
  Array1d<float> stretch_data(len);

  //the scaled version
  stretch_array(len, nsdfData.begin(), len, stretch_data.begin(), 0.0f, stretch_len, LINEAR);
  addElements(nsdfAggregateDataScaled.begin(), nsdfAggregateDataScaled.end(), stretch_data.begin(), scaler);
  copyElementsDivide(nsdfAggregateDataScaled.begin(), nsdfAggregateDataScaled.end(), currentNote->nsdfAggregateDataScaled.begin(), currentNote->nsdfAggregateRoof);

  //the unscaled version
  copyElementsDivide(nsdfAggregateData.begin(), nsdfAggregateData.end(), currentNote->nsdfAggregateData.begin(), currentNote->nsdfAggregateRoof);
/*
  printf("chunk = %d, ", currentChunk());
  printf("scaler = %f, ", scaler);
  printf("periodDiff = %f, ", periodDiff);
  printf("currentNote->currentNsdfPeriod = %f, ", currentNote->currentNsdfPeriod);
  printf("currentNote->firstNsdfPeriod = %f, ", currentNote->firstNsdfPeriod);
  printf("periodRatio = %f\n", periodRatio);
*/
}

/** Calculate (the middle half of) pitches within the current window of the input
    Calculates pitches for positions 1/4 of size() to 3/4 of size()
    e.g. for size() == 1024, does indexs 256 through < 768
    @param period The period estimate
    @return The change in period size
*/
float Channel::calcDetailedPitch(float *input, double period, int /*chunk*/)
{
  //printf("begin calcDetailedPitch\n"); fflush(stdout);
  const int pitchSamplesRange = 4; //look 4 samples either side of the period. Total of 9 different subwindows.

  int n = size();
  int i, j, j2;
  //int preferd_subwindow_size = n / 4;
  int subwindow_size;
  //if(period < 1.0 || period > n/2 - pitchSamplesRange) {
  if(period < pitchSamplesRange || period > double(n)*(3.0/8.0) - pitchSamplesRange) {
    std::fill(detailedPitchData.begin(), detailedPitchData.end(), 0.0f); //invalid period
    std::fill(detailedPitchDataSmoothed.begin(), detailedPitchDataSmoothed.end(), 0.0f); //invalid period
    //printf("end calcDetailedPitch\n"); fflush(stdout);
    return 0.0f;
  }
  int iPeriod = int(floor(period));
  //if(period < double(max_subwindow)) subwindow_size = int(floor(period * (double(max_subwindow) / period)));
  //if(iPeriod < preferd_subwindow_size) subwindow_size = preferd_subwindow_size;
  //else subwindow_size = iPeriod;
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
  //int num = n-subwindow_size-iPeriod;

  //printf("n=%d, subwindow_size=%d, iPeriod=%d\n", n, subwindow_size, iPeriod);
  /*std::vector<int> offsets;
  for(j=-4; j<=4; j++) offsets.push_back(j); //do a total of 9 subwindows at once. 4 either side.
  int ln = offsets.size();*/

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
  //float *unsmoothed = new float[num];
  //Array1d<float> smoothed(num);
  //std::vector<float> smoothed_diff(num);

  //calc the values of pow and err for the first in each row.
  for(i=0; i<ln; i++) {
    left_pow[i] = right_pow[i] = pow[i] = err[i] = 0;
    int offset = periods[i]-iPeriod;
    //for(j=0, j2=iPeriod+offsets[i]; j<subwindow_size-offsets[i]; j++, j2++) {
    for(j=0, j2=periods[i]; j<subwindow_size-offset; j++, j2++) {
      left_pow[i] += squareTable[j]; //sq(input[j]);
      right_pow[i] += squareTable[j2]; //sq(input[j2]);
      err[i] += sq(input[j] - input[j2]);
    }
  }
  //printf("subwindow_size=%d, num=%d, period=%lf\n", subwindow_size, num, period);
  //TODO: speed up this for loop
  //for(j=0; j<num-1; j++) {
  int left1 = 0;
  int left2;
  int right1;
  int right2 = left1 + subwindow_size + iPeriod;

  for(; left1<num; left1++, right2++) {
    for(i=0; i<ln; i++) {
      //left1 = j;
      right1 = left1+periods[i];
      //right2 = j+subwindow_size + iPeriod;
      left2 = right2 - periods[i];

      pow[i] = left_pow[i] + right_pow[i];
      result[i] = 1.0 - (err[i] / pow[i]);

      err[i] += sq(input[left2] - input[right2]) - sq(input[left1] - input[right1]);
      //left_pow[i]  += sq(input[left2])  - sq(input[left1]);
      //right_pow[i] += sq(input[right2]) - sq(input[right1]);
      left_pow[i]  += squareTable[left2]  - squareTable[left1];
      right_pow[i] += squareTable[right2] - squareTable[right1];
    }

    int pos = int(std::max_element(result.begin(), result.begin()+ln) - result.begin());
    myassert(pos >= 0 && pos < ln);
    if(pos > 0 && pos < ln-1)
      unsmoothed[left1] = double(periods[pos]) + parabolaTurningPoint(result[pos-1], result[pos], result[pos+1]);
    else
      unsmoothed[left1] = double(periods[pos]);
  }

  //fastSmooth->fast_smoothB(&(unsmoothed[0]), &(smoothed[0]), num-1);
  //fastSmooth->fast_smoothB(&(unsmoothed[0]), detailedPitchData.begin(), num);
  float periodDiff = unsmoothed.back() - unsmoothed.front();

  //float avgPitch = bound(freq2pitch(rate() / period), 0.0, gdata->topPitch());
  for(j=0; j<num; j++) {
    //detailedPitchData[j] = bound(freq2pitch(rate() / smoothed[j]), 0.0, gdata->topPitch()) - avgPitch;
    //detailedPitchData[j] = bound(freq2pitch(rate() / unsmoothed[j]), 0.0, gdata->topPitch());
    //unsmoothed[j] = bound(freq2pitch(rate() / unsmoothed[j]), 0.0, gdata->topPitch());
    unsmoothed[j] = freq2pitch(rate() / unsmoothed[j]);
  }

  //for(j=0; j<num; j++) detailedPitchData[j] = detailedPitchData[j] - period;
  pitchBigSmoothingFilter->filter(unsmoothed.begin(), detailedPitchDataSmoothed.begin(), num);
  //printf("big filter size = %d\n", pitchBigSmoothingFilter->delay());
  for(j=0; j<num; j++) detailedPitchDataSmoothed[j] = bound(detailedPitchDataSmoothed[j], 0.0f, (float)gdata->topPitch());

  //for(j=0; j<num; j++) printf("%f, %f\n", detailedPitchData[j], unsmoothed[j]);
  //std::copy(unsmoothed, unsmoothed+num, detailedPitchData.begin());
  //std::copy(unsmoothed.begin(), unsmoothed.end(), detailedPitchData.begin());
  pitchSmallSmoothingFilter->filter(unsmoothed.begin(), detailedPitchData.begin(), num);
  //printf("small filter size = %d\n", pitchSmallSmoothingFilter->delay());
  for(j=0; j<num; j++) detailedPitchData[j] = bound(detailedPitchData[j], 0.0f, (float)gdata->topPitch());
/*
  int max_pos = 0;
  for(j=0; j<num-2; j++) {
    smoothed_diff[j] = fabs(smoothed[j+1] - smoothed[j]);
    //printf("%f ", smoothed[j]);
    if(smoothed_diff[j] > smoothed_diff[max_pos]) max_pos = j;
  }
  return;
*/
  //delete unsmoothed;
  //printf("end calcDetailedPitch\n"); fflush(stdout);
  return periodDiff;
}

void Channel::calcVibratoData(int chunk)
{
  NoteData *currentNote = getLastNote();
  if (currentNote && (dataAtChunk(chunk)->noteIndex >=0)) {
    currentNote->addVibratoData(chunk);
  }
}

float Channel::periodOctaveEstimate(int chunk)
{
  //return (float)rate() / 440.0f;

  AnalysisData *analysisData = dataAtChunk(chunk);
  if(analysisData && analysisData->noteIndex >= 0) {
    return noteData[analysisData->noteIndex].periodOctaveEstimate() * analysisData->periodRatio;
  }
  else return -1.0f;
}

void Channel::exportChannel(int type, QString typeString)
{
  QString s = QFileDialog::getSaveFileName(mainWindow, "Choose a filename to save under", ".", typeString);
  //printf("file = %s\n", s.latin1());
  if(s.isEmpty()) return;

  QFile f(s);
  //f.open(QIODevice::WriteOnly & QIODevice::Text);
  f.open(QIODevice::WriteOnly);
  QTextStream out(&f);
  if(type == 0) { //plain text
    out << "        Time(secs) Pitch(semi-tones)       Volume(rms)" << endl;
    out << qSetFieldWidth(18);
    for(int j=0; j<totalChunks(); j++) {
      out << timeAtChunk(j) <<  dataAtChunk(j)->pitch << dataAtChunk(j)->logrms() << endl;
    }
  } else if(type == 1) { //matlab file
    out << "t = [";
    for(int j=0; j<totalChunks(); j++) {
      if(j>0) out << ", ";
      out << timeAtChunk(j);
    }
    out << "];" << endl;

    out << "pitch = [";
    for(int j=0; j<totalChunks(); j++) {
      if(j>0) out << ", ";
      out << dataAtChunk(j)->pitch;
    }
    out << "];" << endl;

    out << "volume = [";
    for(int j=0; j<totalChunks(); j++) {
      if(j>0) out << ", ";
      out << dataAtChunk(j)->logrms();
    }
    out << "];" << endl;
  }
  f.close();
}

void Channel::doPronyFit(int chunk)
{
  if(chunk < pronyWindowSize) return;

  int start = chunk - pronyWindowSize;
  int center = chunk - pronyDelay();
  AnalysisData *data = dataAtChunk(center);
  //AnalysisData *data = dataAtChunk(chunk);
  for(int j=0; j<pronyWindowSize; j++) {
    pronyData[j] = dataAtChunk(start + j)->pitch;
  }
  //PronyData p = pronyFit(pronyData.begin(), pronyWindowSize, 2, timePerChunk(), true);
  PronyData p;
  if(pronyFit(&p, pronyData.begin(), pronyWindowSize, 2, true)) {
    data->vibratoError = p.error;
    data->vibratoPitch = p.yOffset;
    if(p.error < 1.0) {
      data->vibratoSpeed = p.freqHz(timePerChunk());
      if(p.omega * pronyWindowSize < TwoPi) {
        data->vibratoPitch = data->pitch;
      } else {
        data->vibratoWidth = p.amp;
        data->vibratoPhase = p.phase;
        //float xCenter = pronyData[pronyWindowSize/2-1];
        //float fitCenter = p.amp * sin((pronyWindowSize/2-1)*p.omega + p.phase);
        //dataAtChunk(center)->vibratoPitch += fitCenter;
        //dataAtChunk(center)->vibratoPitch = xCenter;
        //dataAtChunk(center)->vibratoWidthAdjust = xCenter / fitCenter;
        data->vibratoWidthAdjust = 0.0f;
      }
    } else {
      data->vibratoPitch = data->pitch;
    }
  } else {
    data->vibratoPitch = data->pitch;
  }
}


