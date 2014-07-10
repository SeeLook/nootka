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

#ifndef TCHUNK_H
#define TCHUNK_H

class QXmlStreamReader;
class QXmlStreamWriter;
class Trhythm;
class Tnote;


/** 
 * This class represent a note: 
 * a pitch described by @p Tnote 
 * and its value (relative duration) described by @p Trhythm
 */
class Tchunk
{

public:
	Tchunk(const Tnote& pitch, const Trhythm& rhythm);
	~Tchunk();
	
	Tnote* p() { return m_pitch; }
	void setPitch(const Tnote& pitch) { m_pitch = pitch; }
	
	Trhythm* r() { return m_rhythm; }
	void setRhythm(const Trhythm& rhythm) { m_rhythm = rhythm; }

	
	void toXml(QXmlStreamWriter& xml);
	bool fromXml(QXmlStreamReader& xml);
	
	
private:
	Tnote				*m_pitch;
	Trhythm			*m_rhythm;
	
};

#endif // TCHUNK_H
