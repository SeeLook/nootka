/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TATTEMPT_H
#define TATTEMPT_H

#include "nootkacoreglobal.h"
#include "tqaunit.h"


/** 
 * This class describes an attempt to 'resolve' an exercise/exam question
 * which is a note sequence (melody)
 * Not all members has usage for given question/answer type
 */
class NOOTKACORE_EXPORT Tattempt
{

public:
  Tattempt();
  
  virtual ~Tattempt();
	
	QList<quint32> times; /** List of answer times of every note in a melody */
	
	QList<TQAunit::Emistake> mistakes; /** Type of mistake of every note */
	
	quint16 playedCount() { return m_playedCounter; } /** How many times user playbacks a question melody */
	void questionWasPlayed() { m_playedCounter++; } /** Increases playback counter */
  
private:
	
	quint16							m_playedCounter;
  
};

#endif // TATTEMPT_H
