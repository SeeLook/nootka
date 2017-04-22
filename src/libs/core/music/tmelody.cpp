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
#include "tmeter.h"
#include "tchunk.h"
#include "nootkaconfig.h"

#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtCore/qdatetime.h>


/* local static */
/** Prints warning message and sets given clef reference to current default clef type. */
void unsupportedClef(Tclef::EclefType& clefType) {
  qDebug() << "[Tmelody] Unsupported clef. Set to default" << Tclef(Tclef::defaultType).name();
  clefType = Tclef::defaultType;
}
/*******************************************************************************************/

Tmelody::Tmelody(const QString& title, const TkeySignature& k) :
  m_title(title),
  m_tempo(120),
  m_key(k),
  m_meter(new Tmeter),
  m_clef(Tclef::defaultType)
{

}

Tmelody::~Tmelody()
{
  delete m_meter;
}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

void Tmelody::setMeter(int m) {
  m_meter->setMeter(static_cast<Tmeter::Emeter>(m));
}


/**
 * IT DOESN'T CHECK NOTES DURATION!!!!
 * all notes added to melody have to be nicely divided to fit every measure duration exactly.
 */
void Tmelody::addNote(const Tchunk& n) {
  if (m_measures.isEmpty() || lastMeasure().isFull())
      m_measures << Tmeasure(m_measures.count() + 1, m_meter->meter());

  lastMeasure().addNote(n);
  m_notes << &lastMeasure().lastNote();
}


void Tmelody::toXml(QXmlStreamWriter& xml) {
  for (int m = 0; m < m_measures.size(); ++m) {
    xml.writeStartElement(QStringLiteral("measure"));
      Tmeasure& meas = measure(m);
      xml.writeAttribute(QStringLiteral("number"), QVariant(meas.number()).toString());
      if (meas.number() == 1) {
        xml.writeStartElement(QStringLiteral("attributes"));
          xml.writeTextElement(QStringLiteral("divisions"), QString("%1").arg(Trhythm(Trhythm::Quarter).duration()));
          if (m_key.value() || m_key.isMinor())
            m_key.toXml(xml);
          m_meter->toXml(xml);
          if (m_clef == Tclef::PianoStaffClefs)
            xml.writeTextElement(QStringLiteral("staves"), QStringLiteral("2"));
          Tclef(m_clef).toXml(xml);
        xml.writeEndElement(); // attributes
      }
      int staffNr_1 = 1, staffNr_2 = 2;
      int *staffPtr = 0;
      for (int n = 0; n < meas.conunt(); ++n) {
        if (m_clef == Tclef::PianoStaffClefs) {
          if (meas.note(n).p().chromatic() > 12)
            staffPtr = & staffNr_1;
          else
            staffPtr = & staffNr_2;
        }
        meas.note(n).toXml(xml, staffPtr);
      }
    xml.writeEndElement(); // measure
  }
}


bool Tmelody::fromXml(QXmlStreamReader& xml) {
  bool ok = true;
  int prevTie = -1; // -1 -> no tie, otherwise this number points to previous note that had a tie
  m_notes.clear();
  m_measures.clear();
  m_meter->setMeter(Tmeter::NoMeter);
  while (xml.readNextStartElement()) {
/** [measure] */
    if (xml.name() == QLatin1String("measure")) {
      int nr = xml.attributes().value(QStringLiteral("number")).toInt();
      m_measures << Tmeasure(nr);
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
                    qDebug() << "[Tmelody] Read from more staves is unsupported";
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
/** [meter (time signature)] */
              else if (xml.name() == QLatin1String("time"))
                  m_meter->fromXml(xml);
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
              qDebug() << "[Tmelody] Change of any melody attributes (clef, meter, key) in the middle of a melody is not supported!";
/** [note] */
        } else if (xml.name() == QLatin1String("note")) {
            int staffNr = 0;
            int *staffPtr = nullptr;
            if (m_clef == Tclef::PianoStaffClefs)
              staffPtr = & staffNr;
            Tchunk ch;
            bool voiceOk = ch.fromXml(xml, staffPtr);
            if (voiceOk) {
              if (ch.p().isRest() && ch.p().rhythm() == Trhythm::NoRhythm) { // Fix rest duration - if it is undefined - means entire measure
                ch.p().setRhythm(m_meter->duration()); // it will reset 'rest' attribute
                ch.p().setRest(true);
              }
              if (!(staffPtr && !ch.p().isValid() && ch.p().rhythm() == Trhythm::NoRhythm)) {
                // Nootka is not able to import from grand staff of real score (XML)
                // and above condition avoids it, but allows to import piano staves created by Nootka itself

                if (prevTie > -1) {
                  // check and fix tie, Nootka supports them only between the same notes
                  // scoring app may set tie between different ones, but seems like in such case there is no 'stop' tag used (musescore)
                  Tnote& prevNote = m_notes[prevTie]->p();
                  short prevChromatic = prevNote.chromatic(), currChromatic = ch.p().chromatic();
                  if ((prevChromatic == currChromatic && ch.p().rtm.tie() == Trhythm::e_noTie) || prevChromatic != currChromatic) {
                    if (prevNote.rtm.tie() == Trhythm::e_tieCont)
                      prevNote.rtm.setTie(Trhythm::e_tieEnd);
                    else if (prevNote.rtm.tie() == Trhythm::e_tieStart)
                      prevNote.rtm.setTie(Trhythm::e_noTie);
                  }
                }
                prevTie = ch.p().rtm.tie() ? m_notes.count() : -1;
                addNote(ch);
              }
            }
        }
        else
            xml.skipCurrentElement();
      }
      if (lastMeasure().number() != m_measures.size()) {
        qDebug() << "[Tmelody] Wrong measure number" << lastMeasure().number() << m_measures.size();
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
    xml.writeDTD(QStringLiteral("<!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 Partwise//EN\" \"http://www.musicxml.org/dtds/partwise.dtd\">"));
    xml.writeStartElement(QStringLiteral("score-partwise"));
      xml.writeStartElement(QStringLiteral("work"));
        xml.writeTextElement(QStringLiteral("work-title"), title());
      xml.writeEndElement(); // work
      xml.writeStartElement(QStringLiteral("identification"));
        xml.writeStartElement(QStringLiteral("creator"));
          xml.writeAttribute(QStringLiteral("type"), QStringLiteral("composer"));
          xml.writeCharacters(QStringLiteral("Nootka The Composer"));
        xml.writeEndElement(); // creator composer
        xml.writeStartElement(QStringLiteral("encoding"));
          xml.writeTextElement(QStringLiteral("software"), QLatin1String("Nootka ") + QString(NOOTKA_VERSION));
          xml.writeTextElement(QStringLiteral("encoding-date"), QDate::currentDate().toString(Qt::ISODate));
        xml.writeEndElement(); // encoding
      xml.writeEndElement(); // identification
      xml.writeStartElement(QStringLiteral("part-list"));
        xml.writeStartElement(QStringLiteral("score-part"));
          xml.writeAttribute(QStringLiteral("id"), QStringLiteral("P1"));
          xml.writeTextElement(QStringLiteral("part-name"), QStringLiteral("Nootka"));
//           xml.writeTextElement("part-name", "instrument");
        xml.writeEndElement(); // score-part
      xml.writeEndElement(); //part-list
      xml.writeStartElement(QStringLiteral("part"));
        xml.writeAttribute(QStringLiteral("id"), QStringLiteral("P1"));
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
      if (xml.name() != QLatin1String("score-partwise")) {
        qDebug() << "[Tmelody] File" << xmlFileName << "is not MusicXML format.";
        ok = false;
      }
    }
    while (xml.readNextStartElement()) {
      qDebug() << "partwise" << xml.name();
      // TODO - grab melody info (title and so...)
      if (xml.name() == QLatin1String("part")) {
          if (!fromXml(xml))
            ok = false;
      } else
          xml.skipCurrentElement();
    }
    file.close();
  }
    return ok;
}


void Tmelody::fromNoteStruct(QList<TnoteStruct>& ns) {
  for (int i = 0; i < ns.size(); ++i)
    addNote(Tchunk(ns[i].pitch));
  // TODO convert rhythm as well
}



