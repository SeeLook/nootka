/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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


#include "tqatype.h"
#include <QtCore/qxmlstream.h>
#include <QtCore/qvariant.h>
#include <QtCore/qmath.h>


TQAtype::TQAtype(bool _asNote, bool _asName, bool _asFretPos, bool _asSound)
{
  m_value = (_asNote ? 1 : 0) + (_asName ? 2 : 0) + (_asFretPos ? 4 : 0) + (_asSound ? 8 : 0);
}


TQAtype::TQAtype(int val) {
  m_value = static_cast<quint16>(val);
}


TQAtype::Etype TQAtype::next() {
  do {
      m_index++;
      if (m_index == 4)
        m_index = 0;
  } while (!(m_value & static_cast<quint8>(qPow(2.0, static_cast<qreal>(m_index)))));

  return static_cast<Etype>(m_index);
}


TQAtype::Etype TQAtype::randNext() {
  m_index = (qrand() % 4) - 1;
  return next();
}


void TQAtype::toXml(int id, QXmlStreamWriter& xml) {
  xml.writeStartElement(QStringLiteral("qaType"));
    xml.writeAttribute(QStringLiteral("id"), QVariant(id).toString());
    xml.writeAttribute(QStringLiteral("score"), QVariant(isNote()).toString());
    xml.writeAttribute(QStringLiteral("name"), QVariant(isName()).toString());
    xml.writeAttribute(QStringLiteral("guitar"), QVariant(isFret()).toString());
    xml.writeAttribute(QStringLiteral("sound"), QVariant(isSound()).toString());
  xml.writeEndElement(); // qaType
}


int TQAtype::fromXml(QXmlStreamReader& xml) {
  int id = QVariant(xml.attributes().value(QStringLiteral("id")).toString()).toInt();
  setAsNote(QVariant(xml.attributes().value(QStringLiteral("score")).toString()).toBool());
  setAsName(QVariant(xml.attributes().value(QStringLiteral("name")).toString()).toBool());
  setAsFret(QVariant(xml.attributes().value(QStringLiteral("guitar")).toString()).toBool());
  setAsSound(QVariant(xml.attributes().value(QStringLiteral("sound")).toString()).toBool());
  xml.skipCurrentElement();
  return id;
}


QDataStream &operator << (QDataStream &out,TQAtype &qatype) {
  out << qatype.isNote() << qatype.isName() << qatype.isFret() << qatype.isSound();
  return out;
}


QDataStream &operator >> (QDataStream &in, TQAtype &qatype) {
  bool b[4];
  in >> b[0] >> b[1] >> b[2] >> b[3];
  qatype = TQAtype(b[0], b[1], b[2], b[3]);
  return in;
}
