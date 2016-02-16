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

#include "tqagroup.h"

void qaGroupToXml(TQAgroup& gr, QXmlStreamWriter& xml, const QString& tag) {
	xml.writeStartElement(tag);
		if (gr.note.isValid())
			gr.note.toXml(xml, "n"); // n like note
		if (gr.pos.str() > 0)
			gr.pos.toXml(xml, "p"); // p like position
	xml.writeEndElement();
}


bool qaGroupFromXml(TQAgroup& gr, QXmlStreamReader& xml) {
  bool ok = true;
	while (xml.readNextStartElement()) {
		if (xml.name() == "n")
			gr.note.fromXml(xml);
		else if (xml.name() == "p")
			gr.pos.fromXml(xml);
		else
			xml.skipCurrentElement();
	}
	return ok;
}

