/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TMEASURE_H
#define TMEASURE_H

#include "tmetrum.h"
#include "tchunk.h"


/** 
 * A musical measure
 */
class NOOTKACORE_EXPORT Tmeasure
{
	
public:
	Tmeasure(int nr);
	
	Tmetrum metrum() { return m_metrum; }
	
	int number() { return m_number; }
	
			/** Adds a note. For this moment it doesn't check duration (rhythm) */
	void addNote(const Tchunk& n);
	Tchunk& note(int index) { return m_notes[index]; } /** Returns given note in this measure */
	Tchunk& lastNote() { return m_notes.last(); }
	
	void removeLastNote();
	
	int conunt() { return m_notes.size(); }
	
private:
	int								m_number;
	Tmetrum						m_metrum;
	QList<Tchunk>			m_notes;
	
};

#endif // TMEASURE_H
