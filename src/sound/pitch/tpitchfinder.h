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

#ifndef TPITCHFINDER_H
#define TPITCHFINDER_H



#include <QThread>

class Channel;


/** The main purpose of this class is to recognize pitch
 * of aduio data flowing throught it. 
 * Finding pitch method(s) are taken from Tartini project
 * writen by Philip McLeod.
 */
class TpitchFinder : public QThread
{
	Q_OBJECT
	
public:
    TpitchFinder();
    virtual ~TpitchFinder();
	
	struct audioSetts {
	  quint32 rate;
	  quint8 chanells;
	  quint32 windowSize;
	  quint32 framesPerChunk; // in mono signal frames are the same as samples
	  double dBFloor;
	  bool equalLoudness;
	  
	};
	  /** global settings for pitch recognizeing. */
	audioSetts aGl() { return m_aGl; }
	
	  /** Starts thread searching in @param chunk,
	   * whitch is pointer to array of floats of audio data. 
	   * First copy it to channel obiect. */
	void searchIn(float *chunk);
	
private:
	audioSetts m_aGl;
	Channel *m_channel;
	float *m_filteredChunk;
	
};

#endif // TPITCHFINDER_H
