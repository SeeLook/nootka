/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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
#include "tartini/mytransforms.h"
#include <QtWidgets/QApplication>
#include <QThread>
#include <QDebug>


TpitchFinder::TpitchFinder(QObject* parent) :
  QObject(parent),
  m_thread(new QThread),
  m_chunkNum(0),
  m_channel(0),
  m_filteredChunk(0), m_prevChunk(0),
  m_buffer_1(0), m_buffer_2(0),
  m_posInBuffer(0),
  m_minVolume(0.4),
  m_prevNoteIndex(-1),
  m_minDuration(0.09),
  m_doReset(false), m_isOffline(false),
  m_rateRatio(1.0),
  m_volume(0),
  m_state(e_silence), m_prevState(e_silence),
  m_pcmVolume(0.0f), m_workVol(0.0f),
  m_splitByVol(true), m_minVolToSplit(0.1),
  m_skipStillerVal(0.0), m_averVolume(0.0)
{
	m_aGl = new TartiniParams();
	m_aGl->chanells = 1;
	m_aGl->rate = 44100;
	m_aGl->windowSize = 2048;
	m_aGl->framesPerChunk = 1; // A trick - setSampleRate() will set it soon
	m_aGl->dBFloor = -150.0; // it is unchanged but if it will in conversions.cpp it is hard coded
	m_aGl->equalLoudness = true;
	m_aGl->doingFreqAnalysis = true;
	m_aGl->doingAutoNoiseFloor = true;
	m_aGl->doingHarmonicAnalysis = false;
	m_aGl->firstTimeThrough = true;
	m_aGl->doingDetailedPitch = true;
	m_aGl->threshold = 93;
	m_aGl->analysisType = e_AUTOCORRELATION;
	m_aGl->topPitch = 140.0;
	m_aGl->loPitch = 15;
	
	m_aGl->ampThresholds[AMPLITUDE_RMS][0]           = -85.0; m_aGl->ampThresholds[AMPLITUDE_RMS][1]           = -0.0;
	m_aGl->ampThresholds[AMPLITUDE_MAX_INTENSITY][0] = -30.0; m_aGl->ampThresholds[AMPLITUDE_MAX_INTENSITY][1] = -20.0;
	m_aGl->ampThresholds[AMPLITUDE_CORRELATION][0]   =  0.40; m_aGl->ampThresholds[AMPLITUDE_CORRELATION][1]   =  1.00;
	m_aGl->ampThresholds[FREQ_CHANGENESS][0]         =  0.50; m_aGl->ampThresholds[FREQ_CHANGENESS][1]         =  0.02;
	m_aGl->ampThresholds[DELTA_FREQ_CENTROID][0]     =  0.00; m_aGl->ampThresholds[DELTA_FREQ_CENTROID][1]     =  0.10;
	m_aGl->ampThresholds[NOTE_SCORE][0]              =  0.03; m_aGl->ampThresholds[NOTE_SCORE][1]              =  0.20;
	m_aGl->ampThresholds[NOTE_CHANGE_SCORE][0]       =  0.12; m_aGl->ampThresholds[NOTE_CHANGE_SCORE][1]       =  0.30;
	
	m_averVolume = 0.0;
  m_currentNote.init(0, 0, 0);
	setSampleRate(m_aGl->rate);
	m_channel = new Channel(this, aGl()->windowSize);
  m_transforms = new MyTransforms();
  m_transforms->init(m_aGl, aGl()->windowSize, 0, aGl()->rate);
	moveToThread(m_thread);
	connect(m_thread, &QThread::started, this, &TpitchFinder::startPitchDetection);
	connect(m_thread, &QThread::finished, this, &TpitchFinder::processed);
	
	m_isBussy = false;
}


TpitchFinder::~TpitchFinder()
{
	if (m_thread->isRunning()) {
			m_thread->terminate();
			m_thread->quit();  
	}
	if (m_filteredChunk)
			delete m_filteredChunk;
	delete m_prevChunk;
	delete m_buffer_1;
	delete m_buffer_2;
  delete m_transforms;
	if (m_channel)
		delete m_channel;
	delete m_aGl;
	m_thread->deleteLater();
}

//##########################################################################################################
//########################################## PUBLIC ########################################################
//##########################################################################################################
void TpitchFinder::setOffLine(bool off) {
  if (off != m_isOffline) {
    m_isOffline = off;
  }
}


void TpitchFinder::setSampleRate(unsigned int sRate, int range) {
	m_mutex.lock();
	int oldRate = m_aGl->rate, oldFramesPerChunk = m_aGl->framesPerChunk;
	m_aGl->rate = sRate;
	switch (range) {
		case 0: 
			m_rateRatio = 0.5; break; // e_high - lowest pitch is F small
		case 2:
			m_rateRatio = 2.0; break; // e_low - lowest pitch is ... very low
		default:
			m_rateRatio = 1.0; break; // e_middle - lowest pitch is F contra
	}
// 		qDebug() << "m_rateRatio is " << m_rateRatio;
	if (sRate > 96000)
		m_aGl->framesPerChunk = 4096 * m_rateRatio;
	else if (sRate > 48000)
		m_aGl->framesPerChunk = 2048 * m_rateRatio;
	else
		m_aGl->framesPerChunk = 1024 * m_rateRatio;
	bool doReset = false;
	if (oldRate != m_aGl->rate || oldFramesPerChunk != m_aGl->framesPerChunk) {
		m_aGl->windowSize = 2 * m_aGl->framesPerChunk;
		delete m_prevChunk;
		delete m_filteredChunk;
    m_filteredChunk = 0;
		delete m_buffer_1;
		delete m_buffer_2;
		if (aGl()->equalLoudness)
			m_filteredChunk = new float[aGl()->framesPerChunk];
		m_prevChunk = new float[aGl()->framesPerChunk];
		m_buffer_1 = new float[aGl()->framesPerChunk];
		m_buffer_2 = new float[aGl()->framesPerChunk];
		m_currentBuff = m_buffer_1;
		doReset = true;
    m_chunkTime = (qreal)aGl()->framesPerChunk / (qreal)aGl()->rate;
    setMinimalDuration(m_minDuration); // recalculate minimum chunks number
// 		qDebug() << "framesPerChunk" << m_aGl->framesPerChunk << "windowSize" << m_aGl->windowSize
//              << "min chunks" << m_minChunks << "chunk time" << m_chunkTime;
	}
	m_mutex.unlock();
	if (doReset)
		resetFinder();
}


void TpitchFinder::resetFinder() {
	if (!m_mutex.tryLock()) {
// 		qDebug() << "Pitch detection in progress, have to wait for reset...";
		m_mutex.lock();
	}
  m_doReset = false;
  if (m_channel) {
      delete m_channel;
      m_chunkNum = 0;
      m_averVolume = 0.0;
      m_transforms->uninit();
      m_channel = new Channel(this, aGl()->windowSize);
      m_transforms->init(m_aGl, aGl()->windowSize, 0, aGl()->rate);
//       qDebug() << "reset channel";
  }
//   qDebug() << "framesPerChunk" << m_aGl->framesPerChunk << "windowSize" << m_aGl->windowSize
//           << "min chunks" << m_minChunks << "chunk time" << m_chunkTime << "noise filter" << aGl()->equalLoudness;
  m_mutex.unlock();
}

//##########################################################################################################
//##########################       PROTECTED SLOTS    ######################################################
//##########################################################################################################

void TpitchFinder::startPitchDetection() {
	m_mutex.lock();
  m_isBussy = true;
	if (m_doReset) { // copy last chunk to keep capturing data continuous
		if (aGl()->equalLoudness)
				std::copy(m_filteredChunk, m_filteredChunk + aGl()->framesPerChunk - 1, m_prevChunk);
		else
				std::copy(m_workChunk, m_workChunk + aGl()->framesPerChunk - 1, m_prevChunk);
		m_mutex.unlock();
		resetFinder();
		m_mutex.lock();
		std::copy(m_prevChunk, m_prevChunk + aGl()->framesPerChunk - 1, m_channel->end() - aGl()->framesPerChunk);
	}
	m_workChunk = m_filledBuff;
	m_channel->shift_left(aGl()->framesPerChunk); // make room in channel for new audio data
	if (aGl()->equalLoudness) { // filter it and copy to channel
		m_channel->highPassFilter->filter(m_workChunk, m_filteredChunk, aGl()->framesPerChunk);
		for(int i = 0; i < aGl()->framesPerChunk; i++)
				m_filteredChunk[i] = bound(m_filteredChunk[i], -1.0f, 1.0f);
		std::copy(m_filteredChunk, m_filteredChunk + aGl()->framesPerChunk - 1, m_channel->end() - aGl()->framesPerChunk);
	} else // copy without filtering
			std::copy(m_workChunk, m_workChunk + aGl()->framesPerChunk - 1, m_channel->end() - aGl()->framesPerChunk);
	detect();
  if (!m_isOffline)
    m_thread->quit();
	m_mutex.unlock();
}


void TpitchFinder::processed() {
	emit pitchInChunk(m_chunkPitch);
	if (m_state != m_prevState) {
		if (m_prevState == e_noticed) {
			if (m_state == e_playing) {
        emit noteStarted(m_currentNote.getAverage(3, minChunksNumber()), m_currentNote.freq, m_currentNote.duration); // new note started
// 				qDebug() << "started" << m_currentNote.index << "pitch:" << m_currentNote.pitchF
// 									 << "freq:" << m_currentNote.freq << "time:" << m_currentNote.duration;
			}
		} else if (m_prevState == e_playing) {
				if (m_state == e_silence || m_state == e_noticed) {
          emit noteFinished(&m_currentNote); // previous note was finished
          if (m_averVolume == 0.0)
            m_averVolume = m_currentNote.maxVol;
          else
            m_averVolume = (m_averVolume + m_currentNote.maxVol) / 2.0;
//           qDebug() << "started" << m_currentNote.index << "pitch:" << m_currentNote.pitchF
//                    << "freq:" << m_currentNote.freq << "time:" << m_currentNote.duration;
				}
		}
	}
	m_prevState = m_state;
	emit volume(m_volume);
}


//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################
void TpitchFinder::detect() {
  FilterState filterState;
  m_channel->processNewChunk(&filterState);
  AnalysisData *data = m_channel->dataAtCurrentChunk();
  if (data == 0) {
    qDebug() << "Uh-uh! There is no Analysis data in processed chunk!";
    incrementChunk();
    m_isBussy = false;
    return;
  }
  data->pcmVolume = m_pcmVolume;
  if (data->noteIndex == NO_NOTE) {
    m_chunkPitch = 0;
    m_volume = 0;
  } else {
    m_chunkPitch = data->pitch;
    m_volume = data->normalVolume();
  }

  if (data->noteIndex != m_prevNoteIndex) { // note changed
    if (m_prevNoteIndex != NO_NOTE && m_newNote.numChunks() >= m_minChunks && m_newNote.maxVol >= m_averVolume * m_skipStillerVal) {
      m_currentNote = m_newNote; // summarize previous note if it was long enough
      m_currentNote.sumarize(m_chunkTime);
    }
    if (data->noteIndex != NO_NOTE) { // initialize a new note when it is valid
      m_newNote.init(data->noteIndex, m_chunkNum, data->pitch);
      m_state = e_noticed;
    } else
      m_state = e_silence;
  } else { // note is still playing
    if (data->noteIndex != NO_NOTE) {
      m_newNote.update(m_chunkNum, data->pitch, m_volume);
      if (m_newNote.maxVol >= m_minVolume && m_newNote.maxVol >= m_averVolume * m_skipStillerVal) { // note was loud enough
        if (m_newNote.numChunks() == m_minChunks) { // note is accepted by Nootka
          m_currentNote = m_newNote;
          m_currentNote.sumarize(m_chunkTime);
          m_state = e_playing;
        } else if (m_splitByVol && m_newNote.numChunks() > m_minChunks) { // the same note for Tartini can be split by Nootka
          if (m_volume - m_newNote.minVol >= m_minVolToSplit && m_volume >= m_averVolume * m_skipStillerVal) {
            m_currentNote = m_newNote;
            m_currentNote.endChunk--;
            m_currentNote.sumarize(m_chunkTime);
            m_newNote.init(data->noteIndex, m_chunkNum, data->pitch);
            m_state = e_noticed;
          }
        }
      }
    }
  }
  m_prevNoteIndex = data->noteIndex;
  if (!m_isOffline && m_chunkNum > 1000 && data->noteIndex == NO_NOTE)
    m_doReset = true;

  incrementChunk();
  m_isBussy = false;
}


void TpitchFinder::bufferReady() {
  m_filledBuff = m_currentBuff;
  m_pcmVolume = m_workVol;
  m_workVol = 0.0;
  m_posInBuffer = 0;
  if (m_currentBuff == m_buffer_1) // swap buffers
    m_currentBuff = m_buffer_2;
  else
    m_currentBuff = m_buffer_1;
//   if (m_isOffline) {
//     startPitchDetection();
//     processed();
//   } else {
    m_thread->start(QThread::HighestPriority);
//   }
}






