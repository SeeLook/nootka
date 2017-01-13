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


#include "tmelody.h"
#include "tnotestruct.h"
#include "tclef.h"
#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qdatetime.h>


/* local static */
/** Prints warring message and sets given clef reference to current default clef type. */
void unsupportedClef(Tclef::EclefType& clefType) {
  qDebug() << "Unsupported clef. Set to default" << Tclef(Tclef::defaultType).name();
  clefType = Tclef::defaultType;
}
/*******************************************************************************************/

Tmelody::Tmelody(const QString& title, const TkeySignature& k) :
  m_title(title),
  m_tempo(120),
  m_key(k),
  m_meter(0),
  m_clef(Tclef::defaultType)
{

}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void Tmelody::addNote(const Tchunk& n) {
  if (m_measures.isEmpty())
      m_measures << Tmeasure(1);

  lastMeasure().addNote(n);

  m_notes << &lastMeasure().lastNote();
}


void Tmelody::toXml(QXmlStreamWriter& xml) {
  for (int m = 0; m < m_measures.size(); ++m) {
    xml.writeStartElement(QStringLiteral("measure"));
      xml.writeAttribute(QStringLiteral("number"), QVariant(measure(m).number()).toString());
      if (measure(m).number() == 1) {
        xml.writeStartElement(QStringLiteral("attributes"));
          xml.writeTextElement(QStringLiteral("divisions"), QStringLiteral("1"));
          if (m_key.value() || m_key.isMinor())
            m_key.toXml(xml);
          // time signature (meter)
          if (m_clef == Tclef::PianoStaffClefs)
            xml.writeTextElement(QStringLiteral("staves"), QStringLiteral("2"));
          Tclef(m_clef).toXml(xml);
        xml.writeEndElement(); // attributes
      }
      int staffNr_1 = 1, staffNr_2 = 2;
      int *staffPtr = 0;
      for (int n = 0; n < measure(m).conunt(); ++n) {
        if (m_clef == Tclef::PianoStaffClefs) {
          if (measure(m).note(n).p().chromatic() > 12)
            staffPtr = & staffNr_1;
          else
            staffPtr = & staffNr_2;
        }
        measure(m).note(n).toXml(xml, staffPtr);
      }
    xml.writeEndElement(); // measure
  }
}


bool Tmelody::fromXml(QXmlStreamReader& xml) {
  bool ok = true;
  m_notes.clear();
  m_measures.clear();
  while (xml.readNextStartElement()) {
/** [measure] */
    if (xml.name() == QLatin1String("measure")) {
      int nr = xml.attributes().value(QStringLiteral("number")).toInt();
      m_measures << Tmeasure(nr);
      // TODO set melody meter for measure
      while (xml.readNextStartElement()) {
/** [attributes] */
        if (xml.name() == QLatin1String("attributes")) {
          if (nr == 1) {
            Tclef::EclefType clef1 = Tclef::NoClef, clef2 = Tclef::NoClef;
            int staffCnt = 1;
            while (xml.readNextStartElement()) {
              if (xml.name() == QLatin1String("staves")) {
                staffCnt = xml.readElementText().toInt();
                if (staffCnt > 2) {
                  qDebug() << "Read from more staves is unsupported";
                  staffCnt = 2;
                }
              } else if (xml.name() == QLatin1String("clef")) {
                Tclef cl;
                cl.fromXml(xml);
                Tclef::EclefType tmpClef = cl.type();
                if (tmpClef == Tclef::NoClef)
                    unsupportedClef(tmpClef);
                if (clef1 == Tclef::NoClef) // detecting piano staff
                    clef1 = tmpClef;
                else if (clef2 == Tclef::NoClef)
                    clef2 = tmpClef;
              } else if (xml.name() == QLatin1String("key"))
                  m_key.fromXml(xml);
              // TODO set meter for melody and for measure as well
              else
                xml.skipCurrentElement();
            }
            if (staffCnt == 2) {
              if (clef1 == Tclef::Treble_G && clef2 == Tclef::Bass_F)
                m_clef = Tclef::PianoStaffClefs;
              else
                unsupportedClef(m_clef);
            } else
                m_clef = clef1;
          } else
              qDebug() << "Changing any melody attributes in the middle of a melody are not supported!";
/** [note] */
        } else if (xml.name() == QLatin1String("note")) {
            int staffNr = 0;
            int *staffPtr = 0;
            if (m_clef == Tclef::PianoStaffClefs)
              staffPtr = & staffNr;
            addNote(Tchunk(Tnote(), Trhythm()));
            bool voiceOk = lastMeasure().lastNote().fromXml(xml, staffPtr);
            if (!voiceOk || (staffPtr && !lastMeasure().lastNote().p().isValid() && lastMeasure().lastNote().r().rhythm() == Trhythm::e_none)) {
              lastMeasure().removeLastNote(); // it is not real import from piano staves
              m_notes.removeLast(); // it will work properly only for MusicXMLs exported through Nootka
            }
        }
        else
            xml.skipCurrentElement();
      }
      if (lastMeasure().number() != m_measures.size()) {
        qDebug() << "Wrong measure number" << lastMeasure().number() << m_measures.size();
      }
    } else
        xml.skipCurrentElement();

  }
  return ok;
}


bool Tmelody::saveToMusicXml(const QString& xmlFileName) {
  QFile file(xmlFileName);
  if (file.open(QIODevice::WriteOnly)) {
    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);
    xml.setAutoFormattingIndent(2);
    xml.writeStartDocument();
    xml.writeDTD("<!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\">");
    xml.writeStartElement("score-partwise");
      xml.writeStartElement("work");
        xml.writeTextElement("work-title", title());
      xml.writeEndElement(); // work
      xml.writeStartElement("identification");
        xml.writeStartElement("creator");
          xml.writeAttribute("type", "composer");
          xml.writeCharacters("Nootka Composer");
        xml.writeEndElement(); // creator composer
        xml.writeStartElement("encoding");
          xml.writeTextElement("software", "Nootka 1.2"); // TODO grab current version
          xml.writeTextElement("encoding-date", QDate::currentDate().toString(Qt::ISODate));
        xml.writeEndElement(); // encoding
      xml.writeEndElement(); // identification
      xml.writeStartElement("part-list");
        xml.writeStartElement("score-part");
        xml.writeAttribute("id", "P1");
//           xml.writeTextElement("part-name", "instrument");
        xml.writeEndElement(); // score-part
      xml.writeEndElement(); //part-list
      xml.writeStartElement("part");
      xml.writeAttribute("id", "P1");
        toXml(xml);
      xml.writeEndElement(); // part
    xml.writeEndElement(); // score-partwise
    xml.writeEndDocument();
    file.close();
    return true;
  } else
      return false;

}


bool Tmelody::grabFromMusicXml(const QString& xmlFileName) {
  QFile file(xmlFileName);
  bool ok = true;
  if (file.open(QIODevice::ReadOnly)) {
    QXmlStreamReader xml(&file);
  // DTD is ignored, only <score-partwise> key is required as main
    if (xml.readNextStartElement()) {
      if (xml.name() != "score-partwise") {
        qDebug() << "File" << xmlFileName << "is not MusicXML format.";
        ok = false;
      }
    }
    while (xml.readNextStartElement()) {
      qDebug() << "partwise" << xml.name();
      // TODO - grab melody info (title and so...)
      if (xml.name() == "part") {
          if (!fromXml(xml)) {
            ok = false;
          }
      } else
          xml.skipCurrentElement();
    }
    file.close();
  }
    return ok;
}


void Tmelody::fromNoteStruct(QList<TnoteStruct>& ns) {
  for (int i = 0; i < ns.size(); ++i)
    addNote(Tchunk(ns[i].pitch, Trhythm()));
  // TODO convert rhythm as well
}



