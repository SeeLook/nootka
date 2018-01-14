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


#include "tchunk.h"

#include <QtCore/qxmlstream.h>
#include <QtCore/qdebug.h>


Tchunk::Tchunk(const Tnote& pitch, const TfingerPos& fretPos) :
  m_pitch(pitch)
{
  m_technical.setFingerPos(fretPos);
}


Tchunk::Tchunk(const Tnote& pitch, const Ttechnical& technical) :
  m_pitch(pitch),
  m_technical(technical)
{
}


void Tchunk::toXml(QXmlStreamWriter& xml, int* staffNr) {
  xml.writeStartElement(QLatin1String("note"));
    if (m_pitch.isRest() || !m_pitch.isValid())
      xml.writeEmptyElement(QLatin1String("rest"));
    else
      m_pitch.toXml(xml);
    xml.writeTextElement(QLatin1String("duration"), QString::number(m_pitch.rtm.isValid() ? m_pitch.duration() : Trhythm(Trhythm::Quarter).duration()));
    if (m_pitch.rhythm() == Trhythm::NoRhythm) {
        if (!m_pitch.isRest() && m_pitch.isValid())
          xml.writeTextElement(QLatin1String("stem"), QLatin1String("none"));
    } else {
        if (m_pitch.rtm.tie())
          tieToXml(xml, m_pitch.rtm.tie(), e_tie);
        xml.writeTextElement(QLatin1String("type"), m_pitch.rtm.xmlType());
        if (m_pitch.hasDot())
          xml.writeEmptyElement(QLatin1String("dot"));
        if (m_pitch.rtm.beam()) {
          xml.writeStartElement(QStringLiteral("beam"));
            xml.writeAttribute(QStringLiteral("number"), QStringLiteral("1"));
            xml.writeCharacters(beamToString(m_pitch.rtm.beam()));
          xml.writeEndElement(); // beam
        }
    }
//     if (m_pitch.alter && !m_pitch.isRest() && m_pitch.isValid()) {
//       QString accidentalTag;
//       switch (m_pitch.alter) {
//         case -2: accidentalTag = QStringLiteral("flat-flat"); break;
//         case -1: accidentalTag = QStringLiteral("flat"); break;
//         case 1: accidentalTag = QStringLiteral("sharp"); break;
//         case 2: accidentalTag = QStringLiteral("double-sharp"); break;
//         default: break;
//       }
//       if (!accidentalTag.isEmpty())
//         xml.writeTextElement(QLatin1String("accidental"), accidentalTag);
//     }
    if (m_pitch.rtm.tie() || !m_technical.isEmpty()) {
      xml.writeStartElement(QLatin1String("notations"));
        if (!m_technical.isEmpty())
          m_technical.toXml(xml);
        if (m_pitch.rtm.tie())
          tieToXml(xml, m_pitch.rtm.tie(), e_tied);
      xml.writeEndElement();
    }
    if (staffNr)
      xml.writeTextElement(QLatin1String("staff"), QString::number(*staffNr));
  xml.writeEndElement(); // note
}


/** 
 * So far, returned @p FALSE value is used to inform that chunk (a note) was in other voice than 'first'
 * More voices are not (and never be) supported...
 * Tie is recognized by <notations><tied type="" ></tied></notations> tag, <tie /> itself is ignored,
 * but during save, both tags are writing.
 */
bool Tchunk::fromXml(QXmlStreamReader& xml, int* staffNr) {
  bool ok = true;
  int stNr = 1;
  m_pitch.setRhythm(Trhythm(Trhythm::NoRhythm));
  while (xml.readNextStartElement()) {
      if (xml.name() == QLatin1String("grace") || xml.name() == QLatin1String("chord")) {
          ok = false;
          xml.skipCurrentElement();
      } else if (xml.name() == QLatin1String("pitch"))
          m_pitch.fromXml(xml);
      else if (xml.name() == QLatin1String("type"))
          m_pitch.setRhythmValue(xml.readElementText().toStdString());
      else if (xml.name() == QLatin1String("rest")) {
          m_pitch.setRest(true);
          xml.skipCurrentElement();
      } else if (xml.name() == QLatin1String("dot")) {
          m_pitch.setDot(true);
          xml.skipCurrentElement();
      } else if (xml.name() == QLatin1String("notations")) {
          while (xml.readNextStartElement()) {
            if (xml.name() == QLatin1String("technical"))
                m_technical.fromXml(xml);
            else if (xml.name() == QLatin1String("tied")) {
              auto type = xml.attributes().value(QStringLiteral("type"));
              Trhythm::Etie tie = Trhythm::e_noTie;
              if (type == QLatin1String("start"))
                  tie = m_pitch.rtm.tie() ? Trhythm::e_tieCont : Trhythm::e_tieStart; // tie was set already - means tie starts and stops on this note
              else if (type == QLatin1String("stop"))
                  tie = m_pitch.rtm.tie() ? Trhythm::e_tieCont : Trhythm::e_tieEnd; // tie was set already - means tie starts and stops on this note
              m_pitch.rtm.setTie(tie);
              xml.skipCurrentElement();
            } else
                xml.skipCurrentElement();
          }
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


QString Tchunk::beamToString(Trhythm::Ebeam b) {
  switch (b) {
    case Trhythm::e_beamStart: return QStringLiteral("begin");
    case Trhythm::e_beamCont: return QStringLiteral("continue");
    case Trhythm::e_beamEnd: return QStringLiteral("end");
    default: return QString();
  }
}


void Tchunk::tieToXml(QXmlStreamWriter& xml, Trhythm::Etie t, Tchunk::EtieTag tag) {
  QString tieTag = tag == e_tie ? QLatin1String("tie") : QLatin1String("tied");
  xml.writeStartElement(tieTag);
    xml.writeAttribute(QLatin1String("type"), t == Trhythm::e_tieStart ? QLatin1String("start") : QLatin1String("stop"));
  xml.writeEndElement();
  if (t == Trhythm::e_tieCont)
    tieToXml(xml, Trhythm::e_tieStart, tag);
}




