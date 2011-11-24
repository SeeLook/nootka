/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tpitchfinder.h"
#include "tartini/channel.h"
#include "tartini/filters/Filter.h"
#include "tartini/filters/IIR_Filter.h"
#include "tartini/analysisdata.h"

#include <QDebug>
#include <stdio.h>
// #include "tnote.h"


TpitchFinder::audioSetts *glAsett;

float *filteredChunk = 0;
bool shown = true;
bool noteNoticed = false;
int noticedChunk = 0; // chunk nr where note was started
const quint8 lowestMidiNote = 35; // a lowest detectable note

TpitchFinder::TpitchFinder(QObject* parent) :
  QThread(parent),
  m_chunkNum(0),
  m_channel(0)
{
	m_aGl.chanells = 1;
	m_aGl.rate = 44100;
	m_aGl.windowSize = 2048;
	m_aGl.framesPerChunk = 1024;
	m_aGl.dBFloor = -150.0;
	m_aGl.equalLoudness = true;
	m_aGl.doingFreqAnalysis = true;
	m_aGl.doingAutoNoiseFloor = true;
	m_aGl.doingHarmonicAnalysis = false;
	m_aGl.firstTimeThrough = true;
	m_aGl.doingDetailedPitch = true;
	m_aGl.isVoice = true;
	m_aGl.threshold = 93;
	m_aGl.analysisType = e_MPM_MODIFIED_CEPSTRUM;
	m_aGl.topPitch = 140.0;
	
	m_aGl.ampThresholds[AMPLITUDE_RMS][0]           = -85.0; m_aGl.ampThresholds[AMPLITUDE_RMS][1]           = -0.0;
	m_aGl.ampThresholds[AMPLITUDE_MAX_INTENSITY][0] = -30.0; m_aGl.ampThresholds[AMPLITUDE_MAX_INTENSITY][1] = -20.0;
	m_aGl.ampThresholds[AMPLITUDE_CORRELATION][0]   =  0.40; m_aGl.ampThresholds[AMPLITUDE_CORRELATION][1]   =  1.00;
	m_aGl.ampThresholds[FREQ_CHANGENESS][0]         =  0.50; m_aGl.ampThresholds[FREQ_CHANGENESS][1]         =  0.02;
	m_aGl.ampThresholds[DELTA_FREQ_CENTROID][0]     =  0.00; m_aGl.ampThresholds[DELTA_FREQ_CENTROID][1]     =  0.10;
	m_aGl.ampThresholds[NOTE_SCORE][0]              =  0.03; m_aGl.ampThresholds[NOTE_SCORE][1]              =  0.20;
	m_aGl.ampThresholds[NOTE_CHANGE_SCORE][0]       =  0.12; m_aGl.ampThresholds[NOTE_CHANGE_SCORE][1]       =  0.30;
	
	
	glAsett = &m_aGl;
	m_isBussy = false;
	
	m_channel = new Channel(this, aGl().windowSize);
	myTransforms.init(aGl().windowSize, 0, aGl().rate, aGl().equalLoudness);
	if (aGl().equalLoudness)
	  filteredChunk = new float[aGl().framesPerChunk+16] + 16;
}

TpitchFinder::~TpitchFinder()
{	
  	if (filteredChunk)
	  delete[] (filteredChunk - 16);
	myTransforms.uninit();
	if(m_channel)
	  delete m_channel;	
}

void TpitchFinder::setParameters(SaudioInParams& params) {
  m_aGl.analysisType = params.analysisType;
  m_aGl.equalLoudness = params.equalLoudness;
  m_aGl.doingAutoNoiseFloor = params.doingAutoNoiseFloor;
  m_aGl.isVoice = params.isVoice;
}


void TpitchFinder::searchIn(float* chunk) {
	if (chunk) {
		m_workChunk = chunk;
		run();
	} else {
	  delete m_channel;
	  m_chunkNum = 0;
	  myTransforms.uninit();
	  m_channel = new Channel(this, aGl().windowSize);
	  myTransforms.init(aGl().windowSize, 0, aGl().rate, aGl().equalLoudness);
	}
}


void TpitchFinder::run() {
	m_isBussy = true;
	if (aGl().equalLoudness) {
	  m_channel->highPassFilter->filter(m_workChunk, filteredChunk, aGl().framesPerChunk);
	  for(int i = 0; i < aGl().framesPerChunk; i++)
		  filteredChunk[i] = bound(filteredChunk[i], -1.0f, 1.0f);
	}
	m_channel->shift_left(aGl().framesPerChunk);
	std::copy(m_workChunk, m_workChunk+aGl().framesPerChunk, m_channel->end() - aGl().framesPerChunk);
	if (aGl().equalLoudness)
	  std::copy(filteredChunk, filteredChunk+aGl().framesPerChunk, m_channel->filteredInput.end() - aGl().framesPerChunk);
	
	FilterState filterState;
	m_channel->processNewChunk(&filterState);
	AnalysisData *data = m_channel->dataAtCurrentChunk();
	if (data) {
	  if (m_channel->isVisibleNote(data->noteIndex) && m_channel->isLabelNote(data->noteIndex)) {
		if (m_aGl.isVoice) { // average pitch
		  if (!noteNoticed) {
			noteNoticed = true;
			noticedChunk = currentChunk();
		  }
		} else { // pitch in single chunk
			if (shown && data->pitch > lowestMidiNote) {
// 			  qDebug() << currentChunk() << data->noteIndex << data->fundamentalFreq;
			  shown = false;
			  emit pitchFound(data->pitch);
			}
		}
	  } else {
		if (m_aGl.isVoice) { // average pitch - shown when note is stoped
		  if(noteNoticed) {
			noteNoticed = false;
			float nn = m_channel->averagePitch(noticedChunk, currentChunk());
			if (nn > lowestMidiNote) {
// 			  qDebug() << noticedChunk << currentChunk();
			  emit pitchFound(nn);
			}
		  }
		} else { // pitch in single chunk 
			shown = true;
		}
		emit noteStoped();
	  }
	}
	incrementChunk();
	m_isBussy = false;
}	

// void TpitchFinder::run() {
// QThread::run();
// }

	