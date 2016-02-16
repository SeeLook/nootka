/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tclef.h"
#include <QApplication>
#include <QXmlStreamWriter>

Tclef::Etype Tclef::defaultType = Tclef::e_treble_G;


Tclef::Tclef(Tclef::Etype type)
{
    m_type = type;
}


QString Tclef::name() {
  switch(m_type) {
    case e_treble_G:
      return QApplication::translate("Tclef", "treble");
    case e_treble_G_8down:
      return QApplication::translate("Tclef", "treble dropped");
    case e_bass_F:
      return QApplication::translate("Tclef", "bass");
    case e_bass_F_8down:
      return QApplication::translate("Tclef", "bass dropped");
    case e_alto_C:
      return QApplication::translate("Tclef", "alto");
    case e_tenor_C:
      return QApplication::translate("Tclef", "tenor");
		case e_pianoStaff:
      return QApplication::translate("Tclef", "grand staff");
		default :
			return "";
  }
}


QString Tclef::desc() {
	switch(m_type) {
    case e_treble_G:
      return QApplication::translate("Tclef", "clef G");
    case e_treble_G_8down:
      return QApplication::translate("Tclef", "clef G dropped octave down");
    case e_bass_F:
      return QApplication::translate("Tclef", "clef F");
    case e_bass_F_8down:
      return QApplication::translate("Tclef", "clef F dropped octave down");
    case e_alto_C:
      return QApplication::translate("Tclef", "clef C");
    case e_tenor_C:
      return QApplication::translate("Tclef", "clef C");
		case e_pianoStaff:
      return QApplication::translate("Tclef", "treble and bass clefs");
		default :
			return "";
  }
}


void Tclef::toXml(QXmlStreamWriter& xml) {
	QString sign, line;
	if (type() == e_treble_G || type() == e_treble_G_8down || type() == e_pianoStaff) {
			sign = "G"; line = "2";
	} else if (type() == e_bass_F || type() == e_bass_F_8down) {
			sign = "F"; line = "4";
	} else if (type() == e_alto_C) {
			sign = "C"; line = "3";
	} else if (type() == e_tenor_C) {
			sign = "C"; line = "4";
	} else {
			return;
	}
	
	xml.writeStartElement("clef");
		if (type() == e_pianoStaff)
			xml.writeAttribute("number" , "1");
		xml.writeTextElement("sign", sign);
		xml.writeTextElement("line", line);
		if (type() == e_bass_F_8down || type() == e_treble_G_8down)
			xml.writeTextElement("clef-octave-change", "-1");
	xml.writeEndElement(); // clef
	if (type() == e_pianoStaff) {
		xml.writeStartElement("clef");
			xml.writeAttribute("number" , "2");
		xml.writeTextElement("sign", "F");
		xml.writeTextElement("line", "4");
	xml.writeEndElement(); // clef
	}
}


void Tclef::fromXml(QXmlStreamReader& xml) {
	QString sign;
	int line, oc = 0;
	while (xml.readNextStartElement()) {
		if (xml.name() == "sign")
			sign = xml.readElementText();
		else if (xml.name() == "line")
			line = xml.readElementText().toInt();
		else if (xml.name() == "clef-octave-change")
			oc = xml.readElementText().toInt();
		else 
			xml.skipCurrentElement();		
	}
	m_type = e_none; // clef unsupported
	if (sign == "G") {
		if (line == 2) {
				if (oc == -1)
					m_type = e_treble_G_8down;
				else if (oc == 0)
					m_type = e_treble_G;
		}
	} else if (sign == "F") {
			if (line == 4) {
				if (oc == -1)
					m_type = e_bass_F_8down;
				else if (oc == 0)
					m_type = e_bass_F;
			}
	} else if (sign == "C") {
			if (line == 3)
				m_type = e_alto_C;
			else if (line == 4)
				m_type = e_tenor_C;
	}
}



