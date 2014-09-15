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

#include "tmelody.h"
#include "tnotestruct.h"
#include "tclef.h"
#include <QVariant>
#include <QDebug>


Tmelody::Tmelody(const QString& title, const TkeySignature& k) :
	m_title(title),
	m_tempo(120),
	m_key(k),
	m_metrum(0),
	m_clef(Tclef::e_treble_G_8down)
{

}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void Tmelody::addNote(const Tchunk& n) {
	if (m_measures.isEmpty())
		m_measures << Tmeasure(1);
	lastMeasure().addNote(n);
	
	m_notes << &lastMeasure().lastNote();
}


void Tmelody::toXml(QXmlStreamWriter& xml) {
	for (int m = 0; m < m_measures.size(); ++m) {
		xml.writeStartElement("measure");
			xml.writeAttribute("number", QVariant(measure(m).number()).toString());
			if (measure(m).number() == 1) {
				xml.writeStartElement("attributes");
					xml.writeTextElement("divisions", "1");
					if (m_key.value() || m_key.isMinor())
						m_key.toXml(xml);
					// time signature (metrum)
					Tclef(m_clef).toXml(xml);
				xml.writeEndElement(); // attributes
			}
			for (int n = 0; n < measure(m).conunt(); ++n)
				measure(m).note(n).toXml(xml);
		xml.writeEndElement(); // measure
	}
}


bool Tmelody::fromXml(QXmlStreamReader& xml) {
	bool ok = true;
	m_notes.clear();
	m_measures.clear();
	while (xml.readNextStartElement()) {
		if (xml.name() == "measure") {
			int nr = xml.attributes().value("number").toInt();
			m_measures << Tmeasure(nr);
			// TODO set melody metrum for measure
			while (xml.readNextStartElement()) {
				if (xml.name() == "attributes") {
					if (nr == 1) {
						while (xml.readNextStartElement()) {
							if (xml.name() == "clef") {
								Tclef cl; 
								cl.fromXml(xml);
								m_clef = cl.type();
								if (m_clef == Tclef::e_none) {
									qDebug() << "Unsupported clef. Set to 'G dropped'";
									m_clef = Tclef::e_treble_G_8down; // TODO it has to be app default clef here
								}
							} else if (xml.name() == "key")
									m_key.fromXml(xml);
							// TODO set metrum for melody and for measure as well
							else
								xml.skipCurrentElement();
						}
					} else
							qDebug() << "Changing any melody attributes in the middle are not supported!";
				} else if (xml.name() == "note") {
						addNote(Tchunk(Tnote(), Trhythm()));
						lastMeasure().lastNote().fromXml(xml);
				}
				else
						xml.skipCurrentElement();
			}
			if (lastMeasure().number() != m_measures.size()) {
				qDebug() << "Wrong measure number" << lastMeasure().number() << m_measures.size();
			}
		} else
				xml.skipCurrentElement();
		
	}
	return ok;
}


bool Tmelody::saveToMusicXml(const QString& xmlFileName)
{

}


bool Tmelody::grabFromMusicXml(const QString& xmlFileName)
{

}


void Tmelody::fromNoteStruct(QList<TnoteStruct>& ns) {
	for (int i = 0; i < ns.size(); ++i)
		addNote(Tchunk(ns[i].pitch, Trhythm()));
	// TODO convert rhythm as well
}



