/***************************************************************************
 *   Copyright (C) 2014-2018 by Tomasz Bojczuk                             *
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


void TQAgroup::toXml(QXmlStreamWriter& xml, const QString& tag) {
  xml.writeStartElement(tag);
    if (note.isValid())
      note.toXml(xml, QLatin1String("n")); // n like note
    if (!technical.isEmpty())
      technical.toXml(xml, QLatin1String("t")); // t like technical (string, finger, bowing, etc.)
//     if (gr.pos().str() > 0) // DEPRECATED: Nootka 1.X used only TfingerPos
//       gr.pos().toXml(xml, QLatin1String("p")); // p like position
  xml.writeEndElement();
}


bool TQAgroup::fromXml(QXmlStreamReader& xml) {
  bool ok = true;
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("n"))
      note.fromXml(xml);
    else if (xml.name() == QLatin1String("p")) // to properly read older *.noo versions from Nootka 1.X
      pos().fromXml(xml);
    else if (xml.name() == QLatin1String("t"))
      technical.fromXml(xml);
    else
      xml.skipCurrentElement();
  }
  return ok;
}

