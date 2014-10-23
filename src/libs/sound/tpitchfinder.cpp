/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include <QThread>

#include <QDebug>



#define MIN_SND_TIME (0.2) // minimal time of sound to detect its pitch in voice mode



TpitchFinder::TpitchFinder(QObject* parent) :
  QObject(parent),
  m_thread(new QThread),
  m_chunkNum(0),
  m_channel(0),
  m_filteredChunk(0), m_prevChunk(0),
  m_buffer_1(0), m_buffer_2(0),
  m_posInBuffer(0),
  m_minVolume(0.4),
  m_prevPitch(0), m_prevFreq(0), m_prevDuration(0),
  m_prevNoteIndex(-1), m_noticedIndex(-1),
  m_isVoice(false),
  m_doReset(false),
  m_rateRatio(1.0)
{
    m_aGl = new TartiniParams();
    m_aGl->chanells = 1;
    m_aGl->rate = 44100;
    m_aGl->windowSize = 2048;
    m_aGl->framesPerChunk = 1024;
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
    
    
    m_isBussy = false;
    setSampleRate(m_aGl->rate);
    m_channel = new Channel(this, aGl()->windowSize);
    myTransforms.init(m_aGl, aGl()->windowSize, 0, aGl()->rate, aGl()->equalLoudness);
		moveToThread(m_thread);
		connect(m_thread, SIGNAL(started()), this, SLOT(startPitchDetection()));
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
	myTransforms.uninit();
	if(m_channel)
		delete m_channel;
	delete m_aGl;
	m_thread->deleteLater();
}

//##########################################################################################################
//########################################## PUBLIC ########################################################
//##########################################################################################################


void TpitchFinder::setIsVoice(bool voice) {
  m_isVoice = voice;
  if (voice)
    m_aGl->analysisType = e_MPM_MODIFIED_CEPSTRUM;
  else
    m_aGl->analysisType = e_MPM /*e_AUTOCORRELATION*/;
}


void TpitchFinder::setSampleRate(unsigned int sRate, int range) {
	m_mutex.lock();
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
	if (sRate > 48000) {
		m_aGl->framesPerChunk = 2048 * m_rateRatio;
		m_aGl->windowSize = 4096 * m_rateRatio;
	} else if (sRate > 96000) {
		m_aGl->framesPerChunk = 4096 * m_rateRatio;
		m_aGl->windowSize = 8192 * m_rateRatio;
	} else {
		m_aGl->framesPerChunk = 1024 * m_rateRatio;
		m_aGl->windowSize = 2048 * m_rateRatio;
	}
// 	qDebug() << "framesPerChunk" << m_aGl->framesPerChunk << "windowSize" << m_aGl->windowSize;
	delete m_prevChunk;
	delete m_filteredChunk;
	delete m_buffer_1;
	delete m_buffer_2;
	if (aGl()->equalLoudness)
		m_filteredChunk = new float[aGl()->framesPerChunk];
	m_prevChunk = new float[aGl()->framesPerChunk];
	m_buffer_1 = new float[aGl()->framesPerChunk];
	m_buffer_2 = new float[aGl()->framesPerChunk];
	m_currentBuff = m_buffer_1;
	m_mutex.unlock();
	resetFinder();
}


void TpitchFinder::fillBuffer(float sample) {
	if (m_posInBuffer < m_aGl->framesPerChunk) {
		*(m_currentBuff + m_posInBuffer) = sample;
		m_posInBuffer++;
	} else { // buffer is full
		m_filledBuff = m_currentBuff;
		m_thread->start(QThread::NormalPriority);
		m_posInBuffer = 0;
		if (m_currentBuff == m_buffer_1) // swap buffers
			m_currentBuff = m_buffer_2;
		else
			m_currentBuff = m_buffer_1;
		fillBuffer(sample); // save sample to new buffer
	}
}


void TpitchFinder::startPitchDetection() {
	m_mutex.lock();
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
	m_thread->quit();
	m_mutex.unlock();
}


void TpitchFinder::resetFinder() {
	if (!m_mutex.tryLock()) {
		qDebug() << "Pitch detection in progress, have to wait for reset...";
		m_mutex.lock();
	}
  m_doReset = false;
  if (m_channel) {
      delete m_channel;
      m_chunkNum = 0;
      myTransforms.uninit();
      m_channel = new Channel(this, aGl()->windowSize);
      myTransforms.init(aGl(), aGl()->windowSize, 0, aGl()->rate, aGl()->equalLoudness);
//       qDebug() << "reset channel";
  }
  m_mutex.unlock();
}

//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################

void TpitchFinder::emitFound() {
  if (m_prevPitch) {
    emit found(m_prevPitch, m_prevFreq, m_prevDuration);
    m_prevPitch = 0;
    m_prevFreq = 0;
		m_prevDuration = 0;
  }
}


void TpitchFinder::detect() {
	m_isBussy = true;
	FilterState filterState;
  m_channel->processNewChunk(&filterState);
	AnalysisData *data = m_channel->dataAtCurrentChunk();
    if (data) {
      if (m_channel->isVisibleNote(data->noteIndex) && m_channel->isLabelNote(data->noteIndex)) {
          NoteData *curNote = m_channel->getCurrentNote();
          bool watchNoote = true;
          emit volume(curNote->volume());
        if (data->noteIndex != m_noticedIndex) {
          if (curNote->volume() >= m_minVolume) {
            m_noticedIndex = data->noteIndex;
            watchNoote = true;
          } else
            watchNoote = false;
        }
        if (watchNoote) {
            if (m_isVoice) {
//                 if (curNote->noteLength() > MIN_SND_TIME) {
								if (curNote->noteLength() >= m_minDuration) {
										if (!m_prevPitch) { // emit only once per note
											emit newNote(curNote->avgPitch());
// 											qDebug() << "->" << data->noteIndex << curNote->avgPitch() << curNote->noteLength() * 1000 << "ms";
										}
                    m_prevPitch = curNote->avgPitch();
                    m_prevFreq = curNote->avgFreq();
										m_prevDuration = curNote->noteLength();
                }
                emit pichInChunk(data->pitch);
              } else {
                  if (data->noteIndex != m_prevNoteIndex) {
// 										qDebug() << data->noteIndex << data->pitch << curNote->noteLength() << curNote->volume();
										if (curNote->noteLength() >= m_minDuration) {
                      m_prevNoteIndex = data->noteIndex; 
// 											qDebug() << data->noteIndex << data->pitch << curNote->noteLength() << curNote->volume() << m_minDuration;
// 											qDebug() << curNote->avgPitch() << aGl()->loPitch << aGl()->topPitch;
                      emit found(/*data->pitch*/curNote->avgPitch(), data->fundamentalFreq, curNote->noteLength());
										}
                  }
                  emit pichInChunk(curNote->avgPitch());
            }
        }
      } else {
          m_noticedIndex = -1;
          if (m_isVoice) {
// 						if (m_prevPitch)
// 								qDebug() << "end" << data->noteIndex << m_prevPitch << m_prevDuration * 1000 << "ms";
            emitFound();
					}
          else {
            m_prevNoteIndex = -1;
          }
          if (m_chunkNum > 1000 && !m_channel->isVisibleNote(data->noteIndex)) {
            m_doReset = true;
//               qDebug() << m_channel->isVisibleNote(data->noteIndex) << m_channel->isLabelNote(data->noteIndex);
          }
          emit pichInChunk(0.0);
          emit volume(0.0);
      }
    }
	incrementChunk();
	m_isBussy = false;
}	

