/***************************************************************************
 *   Copyright (C) 2006-2019 by Tomasz Bojczuk                             *
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

#include "tnote.h"
#include "tnamestylefilter.h"

#include <sstream>
#include <string>
#include <iostream>
#include <unistd.h>

#include <QtCore/qvariant.h>
#include <QtGui/qguiapplication.h>


const std::string Letters [7] = { "C", "D", "E", "F", "G", "A", "H"};
const std::string signsAcid[5] = { "bb", "b", "", "#", "x", };


std::string Tnote::alterSymbol(int id) {
  if (id > -1 && id < 5)
    return signsAcid[id];
  return "";
}


std::string IntToString(int num) {
  std::ostringstream myStream;
  myStream << num << std::flush;
  return(myStream.str());
}


std::string CharToString(char chr) {
  std::ostringstream myStream;
  myStream << (int) chr << std::flush;
  return(myStream.str());
}


QString accidInSpan(char accid) {
  QString accTxt;
  switch (accid) {
    case -2: accTxt = QStringLiteral("B"); break;
    case -1: accTxt = QStringLiteral("b"); break;
    case 1: accTxt = QStringLiteral("#"); break;
    case 2: accTxt = QStringLiteral("x"); break;
    default: break;
  }
  return  QString("<span style=\"font-family: nootka;\">%1</span>").arg(accTxt);
}

/**
 * Quick map (array) to get chromatic note number - first filed is invalid
 */
static qint8 chromaticMap[8] { 0, 1, 3, 5, 6, 8, 10, 12 };

static char setChromaticMap[12][2] { {1, 0}, {1, 1}, {2, 0}, {2, 1}, {3, 0}, {4, 0}, {4, 1}, {5, 0}, {5, 1}, {6, 0}, {6, 1}, {7, 0} };

const char* const fullOctaveNames[8] = { QT_TRANSLATE_NOOP("TnoteName", "Subcontra octave"), QT_TRANSLATE_NOOP("TnoteName", "Contra octave"),
  QT_TRANSLATE_NOOP("TnoteName", "Great octave"), QT_TRANSLATE_NOOP("TnoteName", "Small octave"),
  QT_TRANSLATE_NOOP("TnoteName", "One-line octave"), QT_TRANSLATE_NOOP("TnoteName", "Two-line octave"),
  QT_TRANSLATE_NOOP("TnoteName", "Three-line octave"), QT_TRANSLATE_NOOP("TnoteName", "Four-line octave")
};

static const char* const shortOctaveNames[8] = { QT_TRANSLATE_NOOP("TnoteName", "Sub"), QT_TRANSLATE_NOOP("TnoteName", "Contra"),
  QT_TRANSLATE_NOOP("TnoteName", "Great"), QT_TRANSLATE_NOOP("TnoteName", "Small"),
  QT_TRANSLATE_NOOP("TnoteName", "1-line"), QT_TRANSLATE_NOOP("TnoteName", "2-line"),
  QT_TRANSLATE_NOOP("TnoteName", "3-line"), QT_TRANSLATE_NOOP("TnoteName", "4-line")
};


/*static*/
std::string Tnote::m_solmization[7] = {"Do", "Re", "Mi", "Fa", "Sol", "La", "Si"};
std::string Tnote::m_solmizationRu[7] = {"До", "Ре", "Ми", "Фа", "Соль", "Ля", "Си"};

Tnote::EnameStyle Tnote::defaultStyle = Tnote::e_norsk_Hb;
bool Tnote::scientificOctaves = false;


//#############################################################################################
//################################ PUBLIC #####################################################
//#############################################################################################

void Tnote::setChromatic(short int noteNr) {
  int nr = (noteNr + 143) % 12;
  setNote(setChromaticMap[nr][0]);
  setAlter(setChromaticMap[nr][1]);
  setOctave((noteNr + 143) / 12 - 12);
}


short Tnote::chromatic() const {
  return chromaticMap[static_cast<int>(Tnote::note())] + Tnote::octave() * 12 + Tnote::alter();
}


Tnote Tnote::showAsNatural() const {
  Tnote n(chromatic());
  n.setRhythm(rtm);
  n.setOnUpperStaff(onUpperStaff());
  return n;
}


Tnote Tnote::showWithFlat() const {
  if (alter() != Tnote::e_Flat)  {
    Tnote outputNote = Tnote::showAsNatural();
// Tnote::showAsNatural() always returns notes with sharps or neutral, and never returns H# or E#, but C or F.
    if (outputNote.alter() == Tnote::e_Sharp) {
        outputNote.setNote(outputNote.note() + 1);
        outputNote.setAlter(Tnote::e_Flat);
    } else if (outputNote.note() == 7 || outputNote.note() == 3) { // so only for H changed to Cb we have to increment octave,
        if (outputNote.note() == 7) {
            outputNote.setOctave(outputNote.octave() + 1);
            outputNote.setNote(1);
        }
        if (outputNote.note() == 3) //and for E simply convert to Fb
          outputNote.setNote(4);
        outputNote.setAlter(Tnote::e_Flat);
    }
    outputNote.setOnUpperStaff(onUpperStaff());
    return outputNote;
  }
  return Tnote(note(), octave(), alter());
}


Tnote Tnote::showWithSharp() const {
  if (alter() != Tnote::e_Sharp)  {
    Tnote outputNote = Tnote::showAsNatural();
    if (outputNote.alter() == Tnote::e_Natural) {
      if (outputNote.note() == 4) {
          outputNote.setNote(3);
          outputNote.setAlter(Tnote::e_Sharp);
      } else if (outputNote.note() == 1) {
          outputNote.setOctave(outputNote.octave() - 1);
          outputNote.setNote(7);
          outputNote.setAlter(Tnote::e_Sharp);
      }
    }
    outputNote.setOnUpperStaff(onUpperStaff());
    return outputNote;
  }
  return Tnote(note(), octave(), alter());
}


Tnote Tnote::showWithDoubleSharp() const {
  if (alter() != Tnote::e_DoubleSharp)  {
    Tnote outputNote = Tnote::showAsNatural();
    if (outputNote.alter() == Tnote::e_Natural)  {
      if (outputNote.note() == 4) {
          outputNote.setAlter(Tnote::e_Sharp);
          outputNote.setNote(3);
      } else if (outputNote.note() == 1) {
          outputNote.setAlter(Tnote::e_Sharp);
          outputNote.setNote(7);
          outputNote.setOctave(outputNote.octave() - 1);
      } else {
          outputNote.setAlter(Tnote::e_DoubleSharp);
          outputNote.setNote(note() - 1);
      }
    }
    outputNote.setOnUpperStaff(onUpperStaff());
    return outputNote;
  }
  return Tnote(note(), octave(), alter());
}


Tnote Tnote::showWithDoubleFlat() const {
  if (alter() != Tnote::e_DoubleFlat)  {
    Tnote outputNote = Tnote(note(), octave(), alter());
    if (outputNote.alter() == Tnote::e_Flat && (outputNote.note() == 3 || outputNote.note() == 7)) {
        if (outputNote.note() == 3) {
            outputNote.setAlter(Tnote::e_DoubleFlat);
            outputNote.setNote(4);
        } else {
            outputNote.setAlter(Tnote::e_DoubleFlat);
            outputNote.setNote(1);
            outputNote.setOctave(outputNote.octave() + 1);
        }
    } else {
        outputNote = outputNote.showAsNatural();
        if (outputNote.alter() == Tnote::e_Natural) {
            if (outputNote.note() == 3) {
                outputNote.setAlter(Tnote::e_Flat);
                outputNote.setNote(4);
            } else if (outputNote.note() == 7) {
                outputNote.setAlter(Tnote::e_Flat);
                outputNote.setNote(1);
                outputNote.setOctave(outputNote.octave() + 1);
            } else {
                outputNote.setAlter(Tnote::e_DoubleFlat);
                outputNote.setNote(outputNote.note() + 1);
            }
        } else if (outputNote.alter() == Tnote::e_Sharp) {
            if (outputNote.note() == 2) {
                outputNote.setAlter(Tnote::e_DoubleFlat);
                outputNote.setNote(4);
            } else if (outputNote.note() == 6) {
                outputNote.setAlter(Tnote::e_DoubleFlat);
                outputNote.setNote(1);
                outputNote.setOctave(outputNote.octave() + 1);
            } else
                outputNote = outputNote.showWithFlat();
          }
    }
    outputNote.setRhythm(rtm);
    outputNote.setOnUpperStaff(onUpperStaff());
    return outputNote;
  }
  return Tnote(note(), octave(), alter());
}


TnotesList Tnote::getTheSameNotes(bool enableDbAccids) const {
  TnotesList notesList;
  short cnt; // counter of notes. With double accidentals is 5 (4) without 3 (2)
  notesList.push_back(Tnote(note(),octave(),alter()));
  notesList[0].setOnUpperStaff(onUpperStaff());
  if (notesList[0].alter() != Tnote::e_Natural)
    notesList.push_back(notesList[0].showAsNatural());
  if (notesList[0].alter() != Tnote::e_Sharp)
    notesList.push_back(notesList[0].showWithSharp());
  if (notesList[0].alter() != Tnote::e_Flat)
    notesList.push_back(notesList[0].showWithFlat());
  if (enableDbAccids)  {
      if (notesList[0].alter() != Tnote::e_DoubleSharp)
        notesList.push_back(notesList[0].showWithDoubleSharp());
      if (notesList[0].alter() != Tnote::e_DoubleFlat)
        notesList.push_back(notesList[0].showWithDoubleFlat());
      cnt = 4;
  } else
      cnt = 2;
  for(int m = 0; m < cnt; m++)  {
    for( int n = cnt; n > m; n--) {
      if ((notesList[m].note() != 0) && (notesList[n].note() != 0)) {
        if (notesList[m].compareNotes(notesList[n]))
          notesList[n].setNote(0);
      }
    }
  }
  TnotesList::iterator m = notesList.end();
  while(m != notesList.begin()){
    --m;
    if (!(*m).isValid())
      notesList.erase(m);
  }
  return notesList;
}


std::string Tnote::getName(Tnote::EnameStyle notation, bool showOctave) const {
  std::string noteStr;
  if (!isValid())
      return "none";

  switch (TnameStyleFilter::get(notation)) {
      case e_italiano_Si:
          noteStr = m_solmization[note() - 1] + signsAcid[alter() + 2];
          break;
      case e_russian_Ci:
          noteStr = m_solmizationRu[note() - 1] + signsAcid[alter() + 2];
          break;
      case e_deutsch_His:
          noteStr = Letters[note() - 1];
          switch (alter())  {
            case e_Natural: break;
            case e_DoubleSharp: noteStr = noteStr + "isis"; break;
            case e_Sharp: noteStr = noteStr + "is"; break;
            case e_DoubleFlat: switch (note())  {
              case 3: noteStr = noteStr + "ses"; break;
              case 6: noteStr = noteStr + "sas"; break;
              default: noteStr = noteStr + "eses"; break;
              }
              break;
            case e_Flat: switch (note())  {
              case 3: noteStr = noteStr + "s"; break;
              case 6: noteStr = noteStr + "s"; break;
              case 7: noteStr = "B"; break;
              default: noteStr = noteStr + "es"; break;
                }
                break;
          }
          break;
    case e_nederl_Bis:
        noteStr = Letters[note() - 1];
        if (note() == 7) noteStr = "B";
        switch( alter() ){
          case e_Natural: break;
          case e_DoubleSharp: noteStr = noteStr + "isis"; break;
          case e_Sharp: noteStr = noteStr + "is"; break;
          case e_DoubleFlat:
            switch (note())  {
            case 3: noteStr = noteStr + "ses"; break;
            case 6: noteStr = noteStr + "ses"; break;
            default: noteStr = noteStr + "eses"; break;
            }
            break;
          case e_Flat:
            switch (note())  {
            case 3: noteStr = noteStr + "s"; break;
            case 6: noteStr = noteStr + "s"; break;
            default: noteStr = noteStr + "es"; break;
            }
            break;
        }
        break;
    default:
        noteStr = Letters[note() - 1];
        if ((notation == e_english_Bb) && (note() == 7)) noteStr = "B";
            noteStr = noteStr + signsAcid[alter() + 2];
        break;
  }
  if (showOctave)
      noteStr = noteStr + CharToString(octave() + (scientificOctaves ? 3 : 0));
  return noteStr;
}


QString Tnote::toRichText(Tnote::EnameStyle notation, bool showOctave) const {
  QString richText = toText(notation, false);
    if (notation == Tnote::e_italiano_Si ||
        notation == Tnote::e_russian_Ci ||
        notation == Tnote::e_english_Bb ||
        notation == Tnote::e_norsk_Hb ) {
        if (alter())
          richText.replace(QString::fromStdString(signsAcid[alter() + 2]), QString("<sub>%1</sub>").arg(accidInSpan(alter())));
    }
    if (alter() == -2)
        richText.replace(QLatin1String("B"), QLatin1String("!")); // store capital B otherwise toLower() make it lower
    richText = richText.toLower(); // it converts double flat (B) to single flat (b)
    if (alter() == -2)
        richText.replace(QLatin1String("!"), QLatin1String("B")); // bring back capital B
    if (showOctave) {
      if (scientificOctaves) {
          QString l1 = richText.mid(0, 1).toUpper();
          richText.replace(0, 1, l1);
          richText += QString("<sub>%1</sub>").arg(static_cast<int>(octave() + 3));
      } else {
          if (octave() < 0) { //first letter capitalize
            QString l1 = richText.mid(0, 1).toUpper();
            richText.replace(0, 1, l1);
            if (octave() < -1)
                richText += QString("<sub>%1</sub>").arg(static_cast<int>(octave() * (-1) - 1));
          }
          if (octave() > 0)
              richText += QString("<sup>%1</sup>").arg(static_cast<int>(octave()));
      }
    }
    return richText;
}


QString Tnote::styledName(bool showOctave) const {
  QString name;
  if (isValid()) {
    EnameStyle notation = defaultStyle;
    if (notation == Tnote::e_italiano_Si || notation == Tnote::e_russian_Ci ||
        notation == Tnote::e_english_Bb || notation == Tnote::e_norsk_Hb ) {
          name = Tnote(note(), octave(), 0).toText(notation, false).toLower();
          if (alter())
            name += QString(QChar(379 + alter())); // 377 (0x179) is glyph number of double flat (-2)
    } else // e_deutsch_His & e_nederl_Bis (full name)
          name = toText(notation, false).toLower();
    if (showOctave) {
      if (scientificOctaves) {
          QString firstLetter = name.mid(0, 1).toUpper();
          name.replace(0, 1, firstLetter);
          name += QString(QChar(435 + octave()));
      } else {
          if (octave() > 0)
              name += QString(QChar(390 + octave())); // 391 is fist glyph of sup script digit
          else if (octave() < 0) {
              QString firstLetter = name.mid(0, 1).toUpper();
              name.replace(0, 1, firstLetter);
              if (octave() < -1) // 397 is fist glyph of sub script digit,
                name += QString(QChar(395 - octave())); // and -1 octave is just with capital letter but without digit, only -2 has 1 digit and etc
          }
      }
    }
  }
  return name;
}


void Tnote::toXml(QXmlStreamWriter& xml, const QString& tag, const QString& prefix,
                  const QString& attr, const QString& val) const {
  if (!tag.isEmpty()) {
    xml.writeStartElement(tag);
    if (!attr.isEmpty())
      xml.writeAttribute(attr, val);
  }
  if (note() !=0) { // write <pitch> context only if note is valid
    Tnote bareNote = Tnote(note(), octave(), 0);
    xml.writeTextElement(prefix + QLatin1String("step"), bareNote.toText(Tnote::e_english_Bb, false));
    if (alter())
      xml.writeTextElement(prefix + QLatin1String("alter"), QString::number(static_cast<int>(alter())));
    xml.writeTextElement(prefix + QLatin1String("octave"), QString::number(static_cast<int>(octave() + 3)));
  }
  if (!tag.isEmpty())
    xml.writeEndElement(); // pitch
}


void Tnote::fromXml(QXmlStreamReader& xml, const QString& prefix) {
//   if (xml.name() == "pitch") {
    setNote(0); setOctave(0); setAlter(0); // reset this note
    while (xml.readNextStartElement()) {
      if (xml.name() == (prefix + QLatin1String("step"))) {
        QString step = xml.readElementText().toUpper();
        for (char i = 1; i < 8; i++) {
          Tnote n(i, 0, 0);
          if (n.toText(Tnote::e_english_Bb, false) == step) {
            setNote(i);
            break;
          }
        }
      } else if (xml.name() == (prefix + QLatin1String("octave")))
        setOctave(static_cast<char>(xml.readElementText().toInt() - 3));
      else if (xml.name() == (prefix + QLatin1String("alter")))
        setAlter(static_cast<char>(xml.readElementText().toInt()));
      else
        xml.skipCurrentElement();
    }
//   }
}


void Tnote::transpose(int interval) {
  if (isValid() && interval != 0)
    setChromatic(chromatic() + static_cast<short>(interval));
}


QString Tnote::shortOctaveName(int o) {
  return o > -4 && o < 5 ? QGuiApplication::translate("TnoteName", shortOctaveNames[o + 3]) : QString();
}


QString Tnote::fullOctaveName(int o) {
  return o > -4 && o < 5 ? QGuiApplication::translate("TnoteName", fullOctaveNames[o + 3]) : QString();
}


bool getNoteFromStream(QDataStream &in, Tnote &n) {
    bool ok = true;
    qint8 nn, oo, aa;
    in >> nn >> oo >> aa;
    if (nn < 1 || nn > 8 || aa < -2 || aa > 2) {
        nn = 1; aa = 0; oo = 0;
        ok = false;
    }
    n = Tnote(nn, oo, aa);
    return ok;
}


QDataStream &operator << (QDataStream &out, const Tnote &n) {
  out << static_cast<qint8>(n.note()) << static_cast<qint8>(n.octave()) << static_cast<qint8>(n.alter());
  return out;
}


QDataStream &operator >> (QDataStream &in, Tnote &n) {
  qint8 nn, oo, aa;
  in >> nn >> oo >> aa;
  n = Tnote(nn, oo, aa);
  return in;
}
