/***************************************************************************
 *   Copyright (C) 2018-2019 by Tomasz Bojczuk                             *
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


#include "ttechnical.h"

#include <QtCore/qdebug.h>


Ttechnical::Ttechnical(quint32 fromData)
{
  setData(fromData);
}


/**
 * Upper staff id default state so it is for 0 value
 */
void Ttechnical::setOnUpperStaff(bool onUpper) {
  if (onUpper)
    m_otherData &= ~ON_UPPER;
  else
    m_otherData |= ON_UPPER;
}


void Ttechnical::setBowing(Ttechnical::EbowDirection b) {
  m_otherData &= ~BOW_DIRECTION; // reset it first
  m_otherData |= b;
}


void Ttechnical::setFinger(int fingerNr) {
  m_otherData &= ~FINGERING;
  if (fingerNr >= -1 && fingerNr < 6)
    m_otherData |= (static_cast<quint16>(fingerNr) + 1) << 3;
  else
    qDebug() << "[TnoteData] wrong finger number to store" << fingerNr << " --> Ignoring.";
}


quint32 Ttechnical::data() const {
  quint32 d = m_otherData;
  d <<= 8;
  d += m_fingerPos.data();
  return d;
}


void Ttechnical::setData(quint32 d) {
  m_fingerPos.setData(static_cast<quint8>(d));
  m_otherData = static_cast<quint16>(d >> 8);
}


void Ttechnical::fromXml(QXmlStreamReader& xml) {
  int s = 0, f = 50;
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("string"))
        s = xml.readElementText().toInt();
    else if (xml.name() == QLatin1String("fret"))
        f = xml.readElementText().toInt();
    else if (xml.name() == QLatin1String("down-bow")) {
        xml.skipCurrentElement();
        setBowing(BowDown);
    } else if (xml.name() == QLatin1String("up-bow")) {
        xml.skipCurrentElement();
        setBowing(BowUp);
    } else
        xml.skipCurrentElement();
  }
  if (s > 0 && s < 7)
    m_fingerPos.setPos(s, f == 50 ? 0 : f); // set fret to 0 if its value was not set
  else
    m_fingerPos.setData(NO_TECHNICALS); // invalid
}


void Ttechnical::toXml(QXmlStreamWriter& xml, const QString& tag) const {
  if (!tag.isEmpty())
    xml.writeStartElement(tag);
  if (m_fingerPos.isValid()) {
    xml.writeTextElement(QLatin1String("string"), QString("%1").arg(m_fingerPos.str()));
    xml.writeTextElement(QLatin1String("fret"), QString("%1").arg(m_fingerPos.fret()));
  }
  if (bowing())
    xml.writeEmptyElement(bowing() == BowDown ? QLatin1String("down-bow") : QLatin1String("up-bow"));
  if (!tag.isEmpty())
    xml.writeEndElement(); // tag
}
