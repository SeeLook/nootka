/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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


#include "tchunk.h"
#include "trhythm.h"
#include "tnote.h"
#include <QtCore/qxmlstream.h>


Tchunk::Tchunk(const Tnote& pitch, const Trhythm& rhythm, const TfingerPos& fretPos) :
  m_pitch(pitch),
  m_rhythm(rhythm),
  m_fretPos(fretPos)
{
}


Tchunk::~Tchunk()
{}


void Tchunk::toXml(QXmlStreamWriter& xml, int* staffNr) {
  xml.writeStartElement(QLatin1String("note"));
    if (m_rhythm.isRest() || !m_pitch.isValid())
      xml.writeEmptyElement(QLatin1String("rest"));
    else
      m_pitch.toXml(xml);
    if (m_rhythm.rhythm() == Trhythm::NoRhythm) {
      if (!m_rhythm.isRest() && m_pitch.isValid())
        xml.writeTextElement(QLatin1String("stem"), QLatin1String("none"));
    } else {
      xml.writeTextElement(QLatin1String("type"), m_rhythm.xmlType());
      if (m_rhythm.hasDot())
        xml.writeEmptyElement(QLatin1String("dot"));
    }
    xml.writeTextElement(QLatin1String("duration"), QLatin1String("1"));
    if (validPos()) {
      xml.writeStartElement(QLatin1String("notations"));
        g().toXml(xml);
      xml.writeEndElement();
    }
    if (staffNr)
      xml.writeTextElement(QLatin1String("staff"), QString("%1").arg(*staffNr));
  xml.writeEndElement(); // note
}


/** So far, returned @p FALSE value is used to inform that chunk (a note) was in other voice than 'first'
 * More voices are not (and never be) supported... */
bool Tchunk::fromXml(QXmlStreamReader& xml, int* staffNr) {
  bool ok = true;
  int stNr = 1;
  m_rhythm.setRhythmValue(Trhythm::NoRhythm);
  while (xml.readNextStartElement()) {
      if (xml.name() == QLatin1String("pitch"))
          m_pitch.fromXml(xml);
      else if (xml.name() == QLatin1String("rest")) {
          m_rhythm.setRest(true);
          xml.skipCurrentElement();
      } else if (xml.name() == QLatin1String("type"))
          m_rhythm.setRhythmValue(xml.readElementText().toStdString());
      else if (xml.name() == QLatin1String("notations")) {
          xml.readNextStartElement();
          if (xml.name() == QLatin1String("technical"))
            m_fretPos.fromXml(xml);
          xml.skipCurrentElement();
      } else if (xml.name() == QLatin1String("voice")) {
          if (xml.readElementText().toInt() != 1)
            ok = false;
      } else if (xml.name() == QLatin1String("staff"))
          stNr = xml.readElementText().toInt();
      else
        xml.skipCurrentElement();
  }
  if (staffNr)
      *staffNr = stNr;
  return ok;
}






