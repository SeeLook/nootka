/***************************************************************************
 *   Copyright (C) 2013-2020 by Tomasz Bojczuk                             *
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
#include <QtCore/qmath.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qxmlstream.h>
#include <QtCore/qdebug.h>


Tclef::EclefType Tclef::defaultType = Tclef::Treble_G;


QString Tclef::name() const {
  switch(m_type) {
    case Treble_G:
      return QApplication::translate("Tclef", "treble");
    case Treble_G_8down:
      return QApplication::translate("Tclef", "treble dropped");
    case Bass_F:
      return QApplication::translate("Tclef", "bass");
    case Bass_F_8down:
      return QStringLiteral("bass dropped"); // just in case it would appear somewhere - but it shouldn't - not used any more
    case Alto_C:
      return QApplication::translate("Tclef", "alto");
    case Tenor_C:
      return QApplication::translate("Tclef", "tenor");
    case PianoStaffClefs:
      return QApplication::translate("Tclef", "grand staff");
    case NoClef:
      return QApplication::translate("Tclef", "neutral");
    default :
      return QString();
  }
}


QString Tclef::desc() const {
  QString br = QStringLiteral("<br>");
  switch(m_type) {
    case Treble_G:
      return QApplication::translate("Tclef", "clef G") + br +
             QApplication::translate("Tclef", "Common used clef (for violin, flute, saxophones, etc.)");
    case Treble_G_8down:
      return QApplication::translate("Tclef", "clef G dropped octave down") + br +
             QApplication::translate("Tclef", "Clef for guitars (classical, electric and so)");
    case Bass_F:
      return QApplication::translate("Tclef", "clef F") + br +
             QApplication::translate("Tclef", "Clef for bass guitar and double bass, also used for cello and trombone.");
    case Bass_F_8down:
      return QString();// QApplication::translate("Tclef", "clef F dropped octave down"); used no more
    case Alto_C:
      return QApplication::translate("Tclef", "clef C") + br +
             QApplication::translate("Tclef", "Sometimes it is called clef for viola and mostly used for this instrument.");
    case Tenor_C:
      return QApplication::translate("Tclef", "clef C");
    case PianoStaffClefs:
      return QApplication::translate("Tclef", "treble and bass clefs");
    case NoClef:
      return QApplication::translate("Tclef", "percussion clef") + br +
             QApplication::translate("Tclef", "For rhythms only, note pitch is undefined.");
    default :
      return QString();
  }
}


QString Tclef::glyph() const {
  switch(m_type) {
    case Treble_G:
      return QStringLiteral("\ue050");
    case Treble_G_8down:
      return QStringLiteral("\ue052");
    case Bass_F:
      return QStringLiteral("\ue062");
    case Bass_F_8down:
      return QStringLiteral("\ue064");
    case Alto_C:
      return QStringLiteral("\ue05c");
    case Tenor_C:
      return QStringLiteral("\ue05c");
    case PianoStaffClefs:
      return QStringLiteral("\ue050");
    case NoClef:
      return QStringLiteral("\ue069");
    default :
      return QString();
  }
}


QString Tclef::glyphOnStaff() const {
  switch(m_type) {
    case NoClef:
      return QStringLiteral("\ue041");
    case Treble_G:
      return QStringLiteral("\ue042");
    case Treble_G_8down:
      return QStringLiteral("\ue045");
    case Bass_F:
      return QStringLiteral("\ue043");
    case Alto_C:
      return QStringLiteral("\ue044");
    case Tenor_C:
      return QStringLiteral("\ue046");
    case PianoStaffClefs:
      return QStringLiteral("\ue047");
    default:
      return QString();
  }
}



void Tclef::toXml(QXmlStreamWriter& xml) {
  QString sign, line;
  if (type() == Treble_G || type() == Treble_G_8down || type() == PianoStaffClefs) {
      sign =  QStringLiteral("G"); line = QStringLiteral("2");
  } else if (type() == Bass_F || type() == Bass_F_8down) {
      sign = QStringLiteral("F"); line = QStringLiteral("4");
  } else if (type() == Alto_C) {
      sign = QStringLiteral("C"); line = QStringLiteral("3");
  } else if (type() == Tenor_C) {
      sign = QStringLiteral("C"); line = QStringLiteral("4");
  } else {
      return;
  }

  xml.writeStartElement(QLatin1String("clef"));
    if (type() == PianoStaffClefs)
      xml.writeAttribute(QLatin1String("number") , QLatin1String("1"));
    xml.writeTextElement(QLatin1String("sign"), sign);
    xml.writeTextElement(QLatin1String("line"), line);
    if (type() == Bass_F_8down || type() == Treble_G_8down)
      xml.writeTextElement(QLatin1String("clef-octave-change"), QLatin1String("-1"));
  xml.writeEndElement(); // clef
  if (type() == PianoStaffClefs) { // another 'clef' element
    xml.writeStartElement(QLatin1String("clef"));
      xml.writeAttribute(QLatin1String("number") , QLatin1String("2"));
    xml.writeTextElement(QLatin1String("sign"), QLatin1String("F"));
    xml.writeTextElement(QLatin1String("line"), QLatin1String("4"));
    xml.writeEndElement(); // clef
  }
}


void Tclef::fromXml(QXmlStreamReader& xml) {
  QString sign;
  int line = 0, oc = 0;
  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("sign"))
        sign = xml.readElementText();
    else if (xml.name() == QLatin1String("line"))
        line = xml.readElementText().toInt();
    else if (xml.name() == QLatin1String("clef-octave-change"))
        oc = xml.readElementText().toInt();
    else
      xml.skipCurrentElement();
  }
  m_type = NoClef; // clef unsupported
  if (sign == QLatin1String("G")) {
      if (line == 2) {
        if (oc == -1)
            m_type = Treble_G_8down;
        else if (oc == 0)
            m_type = Treble_G;
      }
  } else if (sign == QLatin1String("F")) {
      if (line == 4) {
        if (oc == -1)
            m_type = Bass_F_8down;
        else if (oc == 0)
            m_type = Bass_F;
      }
  } else if (sign == QLatin1String("C")) {
      if (line == 3)
          m_type = Alto_C;
      else if (line == 4)
          m_type = Tenor_C;
  }
}



