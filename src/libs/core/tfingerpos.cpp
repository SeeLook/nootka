/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#include "tfingerpos.h"


/* static */
const QString TfingerPos::fretsList[25] = { "0",
      "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X",
      "XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX",
      "XI", "XII", "XIII", "XIV"
    };

    
QString TfingerPos::romanFret(quint8 fret) {
    if (fret >= 0 && fret < 25)
      return fretsList[fret];
    else
      return QString("");
}


QString TfingerPos::toHtml() {
  //    return QString("<span style=\"font-size: 25px; font-family: nootka\">%1%2</span>").arg(pos.str()).arg(romanFret(pos.fret()));
    return QString("<span style=\"font-size: xx-large; font-family: nootka\">%1</span><span style=\"font-size: xx-large;\">%2</span>").arg(str()).arg(fret());
}


void TfingerPos::toXml(QXmlStreamWriter& xml, const QString& tag) {
	if (!tag.isEmpty())
		xml.writeStartElement(tag);
	xml.writeTextElement("string", QString("%1").arg(str()));
	xml.writeTextElement("fret", QString("%1").arg(fret()));	
	if (!tag.isEmpty())
		xml.writeEndElement(); // tag
}


void TfingerPos::fromXml(QXmlStreamReader& xml) {
	int s = 0, f = 50;
	while (xml.readNextStartElement()) {
		if (xml.name() == "string")
			s = xml.readElementText().toInt();
		else if (xml.name() == "fret")
			f = xml.readElementText().toInt();
		else
			xml.skipCurrentElement();
	}
	if (s == 0 || f == 50)
		m_pos = 255; // invalid
	else
		setPos(s, f);
}






    
    
