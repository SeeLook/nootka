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

#include "tchunk.h"
#include "trhythm.h"
#include "tnote.h"
#include <QXmlStreamWriter>

Tchunk::Tchunk(const Tnote& pitch, const Trhythm& rhythm) :
	m_pitch(new Tnote(pitch)),
	m_rhythm(new Trhythm(rhythm))
{}


Tchunk::~Tchunk()
{
	delete m_pitch;
	delete m_rhythm;
}



void Tchunk::toXml(QXmlStreamWriter& xml) {
	xml.writeStartElement("note");
		if (m_rhythm->isRest() || m_pitch->note == 0)
			xml.writeEmptyElement("rest");
		else 
			m_pitch->toXml(xml);
		xml.writeTextElement("type", m_rhythm->xmlType());
		if (m_rhythm->hasDot())
			xml.writeEmptyElement("dot");
	xml.writeEndElement(); // note
}


bool Tchunk::fromXml(QXmlStreamReader& xml) {

}






