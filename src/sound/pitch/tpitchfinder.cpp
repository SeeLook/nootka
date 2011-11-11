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
#include "tartini/sound/filters/IIR_Filter.h"




TpitchFinder::TpitchFinder() :
  m_chunkNum(0)
{
	m_aGl.chanells = 1;
	m_aGl.rate = 44100;
	m_aGl.windowSize = 2048;
	m_aGl.framesPerChunk = 1024;
	m_aGl.dBFloor = -150.0;
	m_aGl.equalLoudness = true;
	
	m_channel = new Channel(this, aGl().windowSize);
}

TpitchFinder::~TpitchFinder()
{

}


void TpitchFinder::searchIn(float* chunk) {
	// copy chunk to channel
	float *filteredChunk;
	m_channel->lock();
	if (aGl().equalLoudness) {
	  filteredChunk = new float[aGl().framesPerChunk];
	  m_channel->highPassFilter->filter(chunk, filteredChunk, aGl().framesPerChunk);
	  for(int i = 0; i < aGl().framesPerChunk; i++)
		  filteredChunk[i] = qBound(filteredChunk[i], -1.0f, 1.0f);
	  }
	m_channel->shift_left(aGl().framesPerChunk);
	std::copy(chunk, chunk+aGl().framesPerChunk, m_channel->end() - aGl().framesPerChunk);
	if (aGl().equalLoudness)
	  std::copy(filteredChunk, filteredChunk+aGl().framesPerChunk, m_channel->filteredInput.end() - aGl().framesPerChunk);
	run();
	
}

void TpitchFinder::start() {
	FilterState filterState;
	m_channel->processNewChunk(&filterState);	
	m_channel->unlock();
	incrementChunk();	
	
}

