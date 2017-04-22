/***************************************************************************
 *   Copyright (C) 2016-2017 by Tomasz Bojczuk                             *
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

#include "tmeter.h"
#include "trhythm.h"

#include <QtCore/qdebug.h>


int Tmeter::upper() const {
  switch (m_meter) {
    case Meter_2_4: return 2;
    case Meter_3_4:
    case Meter_3_8: return 3;
    case Meter_4_4: return 4;
    case Meter_5_4:
    case Meter_5_8: return 5;
    case Meter_6_4:
    case Meter_6_8: return 6;
    case Meter_7_4:
    case Meter_7_8: return 7;
    case Meter_9_8: return 9;
    case Meter_12_8: return 12;
    default: return 0;
  }
}


int Tmeter::lower() const {
  int v = static_cast<int>(m_meter);
  if (v > 0) {
    if (v <= 32)
      return 4;
    else
      return 8;
  } else
      return 0;
}


int Tmeter::duration() const {
  if (m_meter == NoMeter)
    return 1;
  else
    return (RVALUE / lower()) * upper();
}


// \ue09f  Control character for denominator (lower) digit
// \ue09e) Control character for numerator (upper) digit
#define ZERO (0xe080) // first digit


QString Tmeter::symbol() const {
  if (m_meter == NoMeter)
    return QString();

  QString s = QString::fromUtf8("\ue09e");
  int u = upper();
  if (u == 12)
    s += QString::fromUtf8("\ue081\ue09e\ue082\ue09f\ue088");
  else
    s += QString(QChar(ZERO + u)) + QString::fromUtf8("\ue09f") + QString(QChar(ZERO + lower()));
  return s;
}


void Tmeter::debug(const QString& text) {
  qDebug() << text << "Meter" << QString("%1/%2").arg(upper()).arg(lower());
}


void Tmeter::toXml(QXmlStreamWriter& xml) const {
  if (m_meter != NoMeter) {
    xml.writeStartElement(QLatin1String("time"));
      xml.writeTextElement(QLatin1String("beats"), QString::number(upper()));
      xml.writeTextElement(QLatin1String("beat-type"), QString::number(lower()));
    xml.writeEndElement(); // time
  }
}


bool Tmeter::fromXml(QXmlStreamReader& xml) {
  bool ok = true;
  int up = 0, lo = 0; 
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("beats"))
      up = xml.readElementText().toInt();
    else if (xml.name() == QLatin1String("beat-type"))
      lo = xml.readElementText().toInt();
    else
      xml.skipCurrentElement();
  }
  m_meter = valueToMeter(up, lo);
  if ((up || lo) && m_meter == NoMeter) {
    qDebug() << "[Tmeter] XML unsupported meter" << up << "/" << lo << "revert to 4/4";
    m_meter = Meter_4_4;
    ok = false;
  }
  return ok;
}


Tmeter::Emeter Tmeter::valueToMeter(int up, int lo) {
  if (lo == 8) {
      switch (up) {
        case 3: return Meter_3_8;
        case 5: return Meter_5_8;
        case 6: return Meter_6_8;
        case 7: return Meter_7_8;
        case 9: return Meter_9_8;
        case 12: return Meter_12_8;
        default: return NoMeter;
      }
  } else if (lo == 4) {
      switch (up) {
        case 2: return Meter_2_4;
        case 3: return Meter_3_4;
        case 4: return Meter_4_4;
        case 5: return Meter_5_4;
        case 6: return Meter_6_4;
        case 7: return Meter_7_4;
        default: return NoMeter;
      }
  }
  return NoMeter;
}




