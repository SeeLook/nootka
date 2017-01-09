/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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

#ifndef TSCORELINES_H
#define TSCORELINES_H

#include <nootkacoreglobal.h>
#include <QGraphicsObject>

class TscoreItem;
class TscoreNote;


typedef QList<QGraphicsLineItem*> TaddLines; /** List of graphics lines  */

/** 
 * There are additional lines appearing above or below a staff (or between piano staves)
 * Every @class TscoreNote has it and note cursor as well.
 * It has method to determine which lines has to be shown depending on note position:
 * @p checkLines()
 */
class NOOTKACORE_EXPORT TscoreLines
{

public:
	TscoreLines(TscoreNote* note);
	
	TaddLines& up() { return m_upper; }
	TaddLines& mid() { return m_middle; }
	TaddLines& lo() { return m_lower; }
	
	void setColor(const QColor& lineColor);
	
			/** Recreates lines adjusting them to new staff (clef) */
	void adjustLines(TscoreNote* scoreNote) { m_parentNote = scoreNote; createLines(); }
	
	void hideAllLines();
	
	void checkLines(int curPos); /** Checks whose lines show and hide. @p curPos is current position of note those lines belong to. */
	
	void setParent(TscoreNote* sn);
	
private:
	QGraphicsLineItem* createNoteLine(int yPos);
	
					/** Common method creating upper and lower staff lines.
				 * It appends new lines to list 
				 * so do not forget to clear list before every next call. */
	void createLines();
	void hideLines(TaddLines &linesList);	
	void deleteLines(TaddLines &linesList); /** Deletes lines in the list and clears the list */
	
private:
	TaddLines 				m_upper, m_middle, m_lower;
	TscoreNote			 *m_parentNote;
	
};

#endif // TSCORELINES_H
