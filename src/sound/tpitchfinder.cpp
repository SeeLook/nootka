/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
// #include <stdio.h>

#define MIN_SND_TIME (0.2) // minimal time of sound to detect its pitch

TpitchFinder::TpitchFinder(QObject* parent) :
  QObject(parent),
  m_chunkNum(0),
  m_channel(0),
  m_filteredChunk(0), m_prevChunk(0),
  m_minVolume(0.4),
  m_prevPitch(0), m_prevFreq(0),
  m_prevNoteIndex(-1), m_noticedIndex(-1),
  m_isVoice(false),
  m_doReset(false)
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
    m_aGl->loPitch = 35;
    
    m_aGl->ampThresholds[AMPLITUDE_RMS][0]           = -85.0; m_aGl->ampThresholds[AMPLITUDE_RMS][1]           = -0.0;
    m_aGl->ampThresholds[AMPLITUDE_MAX_INTENSITY][0] = -30.0; m_aGl->ampThresholds[AMPLITUDE_MAX_INTENSITY][1] = -20.0;
    m_aGl->ampThresholds[AMPLITUDE_CORRELATION][0]   =  0.40; m_aGl->ampThresholds[AMPLITUDE_CORRELATION][1]   =  1.00;
    m_aGl->ampThresholds[FREQ_CHANGENESS][0]         =  0.50; m_aGl->ampThresholds[FREQ_CHANGENESS][1]         =  0.02;
    m_aGl->ampThresholds[DELTA_FREQ_CENTROID][0]     =  0.00; m_aGl->ampThresholds[DELTA_FREQ_CENTROID][1]     =  0.10;
    m_aGl->ampThresholds[NOTE_SCORE][0]              =  0.03; m_aGl->ampThresholds[NOTE_SCORE][1]              =  0.20;
    m_aGl->ampThresholds[NOTE_CHANGE_SCORE][0]       =  0.12; m_aGl->ampThresholds[NOTE_CHANGE_SCORE][1]       =  0.30;
    
    
    m_isBussy = false;
//     m_channel = new Channel(this, aGl()->windowSize);
//     myTransforms.init(m_aGl, aGl()->windowSize, 0, aGl()->rate, aGl()->equalLoudness);
//     if (aGl()->equalLoudness)
//       m_filteredChunk = new float[aGl()->framesPerChunk];
//     m_prevChunk = new float[aGl()->framesPerChunk];
    setSampleRate(m_aGl->rate);
    m_channel = new Channel(this, aGl()->windowSize);
    myTransforms.init(m_aGl, aGl()->windowSize, 0, aGl()->rate, aGl()->equalLoudness);
}

TpitchFinder::~TpitchFinder()
{
    if (m_filteredChunk)
        delete m_filteredChunk;
    delete m_prevChunk;
    myTransforms.uninit();
    if(m_channel)
      delete m_channel;
    delete m_aGl;
}

void TpitchFinder::setIsVoice(bool voice) {
  if (m_isBussy) {
      qDebug("finder is bussy");
      return;
  }    
  m_isVoice = voice;
  if (voice)
    m_aGl->analysisType = e_MPM_MODIFIED_CEPSTRUM;
  else
    m_aGl->analysisType = e_MPM /*e_AUTOCORRELATION*/;
}


void TpitchFinder::setSampleRate(unsigned int sRate) {
    m_aGl->rate = sRate;
    if (sRate > 48000) {
      m_aGl->framesPerChunk = 2048;
      m_aGl->windowSize = 4096;
    } else if (sRate > 96000) {
      m_aGl->framesPerChunk = 4096;
      m_aGl->windowSize = 8192;
    } else {
      m_aGl->framesPerChunk = 1024;
      m_aGl->windowSize = 2048;
    }
    delete m_prevChunk;
    delete m_filteredChunk;
    if (aGl()->equalLoudness)
      m_filteredChunk = new float[aGl()->framesPerChunk];
    m_prevChunk = new float[aGl()->framesPerChunk];
    resetFinder();
}


void TpitchFinder::searchIn(float* chunk) {
  if (chunk) {
      if (m_doReset) { // copy last chunk to keep capturing data continous
        if (aGl()->equalLoudness)
            std::copy(m_filteredChunk, m_filteredChunk + aGl()->framesPerChunk - 1, m_prevChunk);
        else
            std::copy(m_workChunk, m_workChunk + aGl()->framesPerChunk - 1, m_prevChunk);
        resetFinder();
        std::copy(m_prevChunk, m_prevChunk + aGl()->framesPerChunk - 1, m_channel->end() - aGl()->framesPerChunk);
      }
      m_workChunk = chunk;
      m_channel->shift_left(aGl()->framesPerChunk); // make palce in channel for new audio data
      if (aGl()->equalLoudness) { // filter it and copy  too channel
        m_channel->highPassFilter->filter(m_workChunk, m_filteredChunk, aGl()->framesPerChunk);
        for(int i = 0; i < aGl()->framesPerChunk; i++)
            m_filteredChunk[i] = bound(m_filteredChunk[i], -1.0f, 1.0f);
        std::copy(m_filteredChunk, m_filteredChunk + aGl()->framesPerChunk - 1, m_channel->end() - aGl()->framesPerChunk);
      } else // copy without filtering
          std::copy(m_workChunk, m_workChunk + aGl()->framesPerChunk - 1, m_channel->end() - aGl()->framesPerChunk);
      run();
  } else {
      emitFound();
      resetFinder();
  }
}


void TpitchFinder::resetFinder() {
  m_doReset = false;
  if (m_channel) {
      delete m_channel;
      m_chunkNum = 0;
      myTransforms.uninit();
      m_channel = new Channel(this, aGl()->windowSize);
      myTransforms.init(aGl(), aGl()->windowSize, 0, aGl()->rate, aGl()->equalLoudness);
      qDebug() << "reset channel" << m_minVolume;
  }
}

void TpitchFinder::emitFound() {
  if (m_prevPitch) {
    emit found(m_prevPitch, m_prevFreq);
    m_prevPitch = 0;
    m_prevFreq = 0;
  }
}


void TpitchFinder::run() {
	m_isBussy = true;
	FilterState filterState;
  m_channel->processNewChunk(&filterState);
	AnalysisData *data = m_channel->dataAtCurrentChunk();
    if (data) {
      if (m_channel->isVisibleNote(data->noteIndex) && m_channel->isLabelNote(data->noteIndex)) {
          NoteData *curNote = m_channel->getCurrentNote();
          bool watchNoote = true;
        if (data->noteIndex != m_noticedIndex) {
          if (curNote->volume() >= m_minVolume) {
            m_noticedIndex = data->noteIndex;
            watchNoote = true;
          } else
            watchNoote = false;
        }
        if (watchNoote) {
            if (m_isVoice) {
                if (curNote->noteLength() > MIN_SND_TIME) {
                    m_prevPitch = curNote->avgPitch();
                    m_prevFreq = curNote->avgFreq();
                }
                emit pichInChunk(data->pitch);
              } else {
                  if (data->noteIndex != m_prevNoteIndex) {
                      m_prevNoteIndex = data->noteIndex;
                      qDebug() << data->noteIndex << data->pitch << curNote->noteLength() << curNote->volume();
    //                  qDebug() << data->noteIndex << data->pitch << curNote->volume() << "aver:" << averVol;
                      emit found(/*data->pitch*/curNote->avgPitch(), data->fundamentalFreq);
                  }
                  emit pichInChunk(curNote->avgPitch());
            }
        }
      } else {
          m_noticedIndex = -1;
          if (m_isVoice)
            emitFound();
          else {
            m_prevNoteIndex = -1;
          }
          if (m_chunkNum > 100 && !m_channel->isVisibleNote(data->noteIndex)) {
            m_doReset = true;
//               qDebug() << m_channel->isVisibleNote(data->noteIndex) << m_channel->isLabelNote(data->noteIndex);
          }
          emit pichInChunk(0.0);
      }
    }
	incrementChunk();
	m_isBussy = false;
}	

