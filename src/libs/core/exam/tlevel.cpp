/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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

#include "tlevel.h"
#include "tglobals.h"
#include <music/ttune.h>
#include <taudioparams.h>
#include <tscoreparams.h>

#include <QtCore/qfile.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qxmlstream.h>
#include <QtCore/qvariant.h>
#include <QtCore/qdebug.h>


/*static-------------------------------------------------------------------------------------------*/
/** Versions history:
 * 1. 0x95121701
 *
 * 2. 0x95121703 (02.12.2013)
 *     - support for instrument types and guessing an instrument from previous version
 *     - instrument enum is casting directly to quint8: Tinstrument::NoInstrument is 0
 *
 * 3. 0x95121705 (22.06.2014) - XML stream - universal version
 *
 * 4. 0x95121707 (05.02.2018) - rhythms, new instruments, melodies in Music XML
 */

const qint32 Tlevel::levelVersion = 0x95121701;
const qint32 Tlevel::currentVersion = 0x95121707;


int Tlevel::levelVersionNr(qint32 ver) {
  if ((ver - levelVersion) % 2)
      return -1; // invalid when rest of division is 1
  return ((ver - levelVersion) / 2) + 1 ;
}


bool Tlevel::isLevelVersion(quint32 ver) {
  if (levelVersionNr(ver) <= levelVersionNr(currentVersion))
    return true;
  else
    return false;
}


bool Tlevel::couldBeLevel(qint32 ver) {
  int givenVersion = levelVersionNr(ver);
  if (givenVersion >= 1 && givenVersion <= 127)
    return true;
  else
    return false;
}


/**
 * TlevelSelector context of translate() is used for backward compatibility with translations
 */
void Tlevel::fileIOerrorMsg(QFile& f) {
  if (!f.fileName().isEmpty())
      QMessageBox::critical(nullptr, QLatin1String(" "), QApplication::translate("TlevelSelector",
                                      "Cannot open file\n %1 \n for reading").arg(f.fileName()));
  else
      QMessageBox::critical(nullptr, QLatin1String(" "), QApplication::translate("TlevelSelector", "No file name specified"));
}


void Tlevel::fretFromXml(QXmlStreamReader& xml, char& fr, Tlevel::EerrorType& err) {
  fr = (char)QVariant(xml.readElementText()).toInt();
  if (fr < 0 || fr > 24) { // max frets number
    fr = 0;
    qDebug() << "[Tlevel] Fret number in" << xml.name() << "was wrong but fixed";
    err = Tlevel::e_levelFixed;
  }
}


void Tlevel::skipCurrentXmlKey(QXmlStreamReader& xml) {
  qDebug() << "[Tlevel] Unrecognized key:" << xml.name();
  xml.skipCurrentElement();
}
/*end static--------------------------------------------------------------------------------------*/

Tlevel::Tlevel() :
  hasInstrToFix(false)
{
// level parameters
  name = QObject::tr("master of masters");
  desc = QObject::tr("All possible options are turned on");
  bool hasGuitar = GLOB->instrument().isGuitar();
// QUESTIONS
  questionAs = TQAtype(true, true, GLOB->instrument().type() != Tinstrument::NoInstrument, true);
  answersAs[0] = TQAtype(true, true, GLOB->instrument().type() != Tinstrument::NoInstrument, true);
  answersAs[1] = TQAtype(true, true, GLOB->instrument().type() != Tinstrument::NoInstrument, true);
  answersAs[2] = TQAtype(true, true, GLOB->instrument().isGuitar(), false);
  answersAs[3] = TQAtype(true, true, GLOB->instrument().type() != Tinstrument::NoInstrument, true);
  requireOctave = true;
  requireStyle = true;
    /**
     * variables isNoteLo, isNoteHi and isFretHi are not used - it has no sense.
     *  Since version 0.8.90 isNoteLo and isNoteHi are merged into Tclef.
     *  It can store multiple clefs (maybe in unknown future it will be used)
     *  0 - no clef and up to 15 different clefs.
     */
  clef = Tclef(GLOB->S->clef);

  instrument = GLOB->instrument().type();
  onlyLowPos = false;
  onlyCurrKey = false;
  intonation = GLOB->A->intonation;
// ACCIDENTALS
  withSharps = true;
  withFlats = true;
  withDblAcc = true;
  useKeySign = true;
  isSingleKey  = false;
  loKey = TkeySignature(-7);
  hiKey = TkeySignature(7); // key range (7b to 7#)
  manualKey = true;
  forceAccids = true;
  showStrNr = hasGuitar;
// MELODIES
  melodyLen = 1;
  endsOnTonic = true;
  requireInTempo = true;
  randMelody = e_randFromRange;
  randOrderInSet = true;
  repeatNrInSet = 1;
  //   notesList is clean here
// RHYTHMS
  basicRhythms = 0;
  dotsRhythms = 0;
  meters = 0;
  rhythmDiversity = 5;
  barNumber = 4;
  variableBarNr = true;
  useRests = false;
  useTies = false;
// RANGE - for non guitar Tglobals will returns scale determined by clef
  loNote = GLOB->loString();
  hiNote = Tnote(GLOB->hiString().chromatic() + GLOB->GfretsNumber);
  loFret = 0;
  hiFret = GLOB->GfretsNumber;
  for (int i = 0; i < 6; i++) {
    if (i <= GLOB->Gtune()->stringNr())
      usedStrings[i] = true;
    else
      usedStrings[i] = false;
  }
}


bool getLevelFromStream(QDataStream& in, Tlevel& lev, qint32 ver) {
  bool ok = true;
  in >> lev.name >> lev.desc;
  in >> lev.questionAs;
  in >> lev.answersAs[0] >> lev.answersAs[1] >> lev.answersAs[2] >> lev.answersAs[3];
  in >> lev.withSharps >> lev.withFlats >> lev.withDblAcc;
  quint8 sharedByte;
  in >> lev.useKeySign >> sharedByte;
  lev.isSingleKey = (bool)(sharedByte % 2);
  lev.intonation = sharedByte / 2;
  ok = getKeyFromStream(in, lev.loKey);
  ok = getKeyFromStream(in, lev.hiKey);
  in >> lev.manualKey >> lev.forceAccids;
  in >>  lev.requireOctave >> lev.requireStyle;
// RANGE
  ok = getNoteFromStream(in, lev.loNote);
  ok = getNoteFromStream(in, lev.hiNote);
/** Merged to quint16 since version 0.8.90 */
  quint16 testClef;
  in >> testClef;
  qint8 lo, hi;
  in >> lo >> hi;
  if (lo < 0 || lo > 24) { // max frets number
      lo = 0;
      ok = false;
  }
  if (hi < 0 || hi > 24) { // max frets number
      hi = GLOB->GfretsNumber;
      ok = false;
  }
  lev.loFret = char(lo);
  lev.hiFret = char(hi);
/** Previously is was bool type */
  quint8 instr;
  in >> instr;
  in >> lev.usedStrings[0] >> lev.usedStrings[1] >> lev.usedStrings[2]
          >> lev.usedStrings[3] >> lev.usedStrings[4] >>  lev.usedStrings[5];
  in >> lev.onlyLowPos >> lev.onlyCurrKey >> lev.showStrNr;
  if (ver == lev.levelVersion) { // first version of level file structure
      lev.clef = lev.fixClef(testClef); // determining/fixing a clef from first version
      lev.instrument = lev.fixInstrument(instr); // determining/fixing an instrument type
  } else {
      lev.clef = Tclef((Tclef::EclefType)testClef);
      lev.instrument = (Tinstrument::Etype)instr;
  }
  lev.melodyLen = 1; // Those parameters was deployed in XML files
  lev.endsOnTonic = false; // By settings those values their will be ignored
  lev.requireInTempo = false;
  return ok;
}


Tlevel::EerrorType Tlevel::qaTypeFromXml(QXmlStreamReader& xml) {
  TQAtype qa;
  EerrorType er = e_level_OK;
  int id = qa.fromXml(xml);
  if (id == -1) {
      questionAs = qa;
      if (!questionAs.isOnScore() && !questionAs.isName() && !questionAs.isOnInstr() && !questionAs.isSound()) {
        qDebug() << "There are not any questions in a level. It makes no sense.";
        return e_otherError;
      }
  } else if (id >= 0 && id < 4) {
      answersAs[id] = qa;
      // verify every answersAs context and set corresponding questionAs to false when all were unset (false)
      if (questionAs.isOnScore() &&
        (!answersAs[0].isOnScore() && !answersAs[0].isName() && !answersAs[0].isOnInstr() && !answersAs[0].isSound())) {
          er = e_levelFixed;
          questionAs.setOnScore(false);
      }
      if (questionAs.isName() &&
        (!answersAs[1].isOnScore() && !answersAs[1].isName() && !answersAs[1].isOnInstr() && !answersAs[1].isSound())) {
          er = e_levelFixed;
          questionAs.setAsName(false);
      }
      if (questionAs.isOnInstr() &&
        (!answersAs[2].isOnScore() && !answersAs[2].isName() && !answersAs[2].isOnInstr() && !answersAs[2].isSound())) {
          er = e_levelFixed;
          questionAs.setOnInstr(false);
      }
      if (questionAs.isSound() &&
        (!answersAs[3].isOnScore() && !answersAs[3].isName() && !answersAs[3].isOnInstr() && !answersAs[3].isSound())) {
          er = e_levelFixed;
          questionAs.setOnScore(false);
      }
  }
  return er;
}


Tlevel::EerrorType Tlevel::loadFromXml(QXmlStreamReader& xml) {
  EerrorType er = e_level_OK;

  if (xml.name() != QLatin1String("level")) {
    qDebug() << "[Tlevel] There is no 'level' key in that XML";
    return e_noLevelInXml;
  }
  name = xml.attributes().value(QLatin1String("name")).toString();
  if (name.isEmpty()) {
      qDebug() << "[Tlevel] Level key has empty 'name' attribute";
      return e_otherError;
  }
  melodySet.clear();
  randOrderInSet = true;
  repeatNrInSet = 1;

  while (xml.readNextStartElement()) {
    if (xml.name() == QLatin1String("description"))
      desc = xml.readElementText();
    else if (xml.name() == QLatin1String("nameTR"))
      name = QApplication::translate("Levels", xml.readElementText()
                                    .replace(QLatin1String("translate(\"Levels\", \""), QString())
                                    .replace(QLatin1String("\")"), QString()).toLocal8Bit());
    else if (xml.name() == QLatin1String("descriptionTR"))
      desc = QApplication::translate("Levels", xml.readElementText()
                                    .replace(QLatin1String("translate(\"Levels\", \""), QString())
                                    .replace(QLatin1String("\")"), QString()).toLocal8Bit());
  // QUESTIONS
    else if (xml.name() == QLatin1String("questions")) {
      while (xml.readNextStartElement()) {
        if (xml.name() == QLatin1String("qaType")) {
          er = qaTypeFromXml(xml);
          if (er == e_otherError)
            return er;
        } else if (xml.name() == QLatin1String("requireOctave"))
            requireOctave = QVariant(xml.readElementText()).toBool();
        else if (xml.name() == QLatin1String("requireStyle"))
            requireStyle = QVariant(xml.readElementText()).toBool();
        else if (xml.name() == QLatin1String("showStrNr"))
            showStrNr = QVariant(xml.readElementText()).toBool();
        else if (xml.name() == QLatin1String("clef")) {
            clef.setClef(Tclef::EclefType(QVariant(xml.readElementText()).toInt()));
            if (clef.name().isEmpty()) { // when clef has improper/unsupported value its name returns empty string
              qDebug() << "[Tlevel] Level had wrong/undefined clef. It was fixed to treble dropped.";
              clef.setClef(Tclef::Treble_G_8down);
              er = e_levelFixed;
            }
        } else if (xml.name() == QLatin1String("instrument")) {
            instrument = Tinstrument::Etype(QVariant(xml.readElementText()).toInt());
            if (Tinstrument::staticName(instrument).isEmpty()) {
              qDebug() << "[Tlevel] Level had wrong instrument type. It was fixed to classical guitar.";
              instrument = Tinstrument::ClassicalGuitar;
              er = e_levelFixed;
            }
        } else if (xml.name() == QLatin1String("onlyLowPos"))
            onlyLowPos = QVariant(xml.readElementText()).toBool();
        else if (xml.name() == QLatin1String("onlyCurrKey"))
            onlyCurrKey = QVariant(xml.readElementText()).toBool();
        else if (xml.name() == QLatin1String("intonation"))
            intonation = QVariant(xml.readElementText()).toInt();
        else
            skipCurrentXmlKey(xml);
      }
    } else if (xml.name() == QLatin1String("melodies")) {
      // Melodies
        while (xml.readNextStartElement()) {
          if (xml.name() == QLatin1String("melodyLength"))
              melodyLen = qBound(1, QVariant(xml.readElementText()).toInt(), 100);
          else if (xml.name() == ("endsOnTonic"))
              endsOnTonic = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("requireInTempo"))
              requireInTempo = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("randType"))
              randMelody = static_cast<ErandMelody>(xml.readElementText().toInt());
          else if (xml.name() == QLatin1String("keyOfrandList")) {
              xml.readNextStartElement();
              keyOfrandList.fromXml(xml);
              xml.skipCurrentElement();
          } else if (xml.name() == QLatin1String("noteList")) {
              notesList.clear();
              while (xml.readNextStartElement()) {
                if (xml.name() == QLatin1String("n")) {
                    notesList << Tnote();
                    notesList.last().fromXml(xml);
                    if (!notesList.last().isValid()) // skip empty notes
                      notesList.removeLast();
                } else
                    skipCurrentXmlKey(xml);
              }
          } else if (xml.name() == QLatin1String("randOrderInSet"))
              randOrderInSet = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("repeatNrInSet")) {
              auto ris = QVariant(xml.readElementText()).toInt();
              if (ris < 1 || ris > 15) {
                ris = qBound(1, ris, 15);
                qDebug() << "[Tlevel] value of melody repeats was wrong and fixed to" << ris;
              }
              repeatNrInSet = static_cast<quint8>(ris);
          } else if (xml.name() == QLatin1String("melody")) {
              auto t = xml.attributes().value(QLatin1String("title")).toString();
              auto c = xml.attributes().value(QLatin1String("composer")).toString();
              melodySet << Tmelody();
              melodySet.last().fromXml(xml); // TODO: no validation here, could be vulnerable
              melodySet.last().setTitle(t);
              melodySet.last().setComposer(c);
          } else
              skipCurrentXmlKey(xml);
        }
    } else if (xml.name() == QLatin1String("accidentals")) {
        while (xml.readNextStartElement()) {
//           qDebug() << "accidentals->" << xml.name();
          if (xml.name() == QLatin1String("withSharps"))
              withSharps = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("withFlats"))
              withFlats = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("withDblAcc"))
              withDblAcc = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("useKeySign"))
              useKeySign = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("loKey")) {
              xml.readNextStartElement();
              loKey.fromXml(xml);
              xml.skipCurrentElement();
          } else if (xml.name() == QLatin1String("hiKey")) {
              xml.readNextStartElement();
              hiKey.fromXml(xml);
              xml.skipCurrentElement();
          }
          else if (xml.name() == QLatin1String("isSingleKey"))
              isSingleKey = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("manualKey"))
              manualKey = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("forceAccids"))
              forceAccids = QVariant(xml.readElementText()).toBool();
          else
            skipCurrentXmlKey(xml);
        }
    } else if (xml.name() == QLatin1String("rhythms")) {
  // RHYTHMS
        while (xml.readNextStartElement()) {
          if (xml.name() == QLatin1String("meters"))
              meters = static_cast<quint16>(QVariant(xml.readElementText()).toInt());
          else if (xml.name() == QLatin1String("basic"))
              basicRhythms = static_cast<quint32>(QVariant(xml.readElementText()).toUInt());
          else if (xml.name() == QLatin1String("dots"))
              dotsRhythms = static_cast<quint32>(QVariant(xml.readElementText()).toUInt());
          else if (xml.name() == QLatin1String("diversity"))
              rhythmDiversity = static_cast<quint8>(QVariant(xml.readElementText()).toInt());
          else if (xml.name() == QLatin1String("bars")) {
              variableBarNr = xml.attributes().value(QLatin1String("variable")) == QLatin1String("true");
              barNumber = static_cast<quint8>(QVariant(xml.readElementText()).toInt());
          } else if (xml.name() == QLatin1String("randomBars"))
              variableBarNr = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("rests"))
              useRests = QVariant(xml.readElementText()).toBool();
          else if (xml.name() == QLatin1String("ties"))
              useTies = QVariant(xml.readElementText()).toBool();
          else
              skipCurrentXmlKey(xml);
        }
    } else if (xml.name() == QLatin1String("range")) {
  // RANGE
        while (xml.readNextStartElement()) {
          if (xml.name() == QLatin1String("loFret"))
              fretFromXml(xml, loFret, er);
          else if (xml.name() == QLatin1String("hiFret"))
              fretFromXml(xml, hiFret, er);
          else if (xml.name() == QLatin1String("loNote"))
              loNote.fromXml(xml);
          else if (xml.name() == QLatin1String("hiNote"))
              hiNote.fromXml(xml);
          else if (xml.name() == QLatin1String("useString")) {
              int id = xml.attributes().value(QLatin1String("number")).toInt();
              if (id > 0 && id < 7)
                usedStrings[id - 1] = QVariant(xml.readElementText()).toBool();
          } else
              skipCurrentXmlKey(xml);
        }
    } else
        skipCurrentXmlKey(xml);
  }
  if (name.size() > 29) {
    name = name.left(29);
    qDebug() << "[Tlevel] Name of a level was reduced to 29 characters:" << name;
  }
  if (canBeGuitar() && fixFretRange() == e_levelFixed) {
    er = e_levelFixed;
    qDebug() << "[Tlevel] Lowest fret in the range was bigger than the highest one. Fixed";
  }
  if (useKeySign && !isSingleKey && fixKeyRange() == e_levelFixed) {
    er = e_levelFixed;
    qDebug() << "[Tlevel] Lowest key in the range was higher than the highest one. Fixed";
  }
  if (loNote.note() == 0 || hiNote.note() == 0) {
      qDebug() << "[Tlevel] Note range is wrong.";
      return e_otherError;
  } else if (fixNoteRange() == e_levelFixed) {
      er = e_levelFixed;
      qDebug() << "[Tlevel] Lowest note in the range was higher than the highest one. Fixed";
  }
  if (notesList.isEmpty()) {
      if (randMelody == e_randFromList) {
        qDebug() << "[Tlevel] list of notes is empty but e_randFromList is set";
        randMelody = e_randFromRange;
      }
  } else {
      if (randMelody == e_randFromRange) {
        qDebug() << "[Tlevel] has list of notes but e_randFromRange is set. List will be cleaned";
        notesList.clear();
      }
  }
  if (randMelody == e_melodyFromSet) {
      if (melodySet.isEmpty()) {
        qDebug() << "[Tlevel] is melody set but list of melodies is empty. Level corrupted!!!";
        er = e_otherError;
      }
  } else {
      if (!randOrderInSet) {
        qDebug() << "[Tlevel] is not a melody set but question order is set. Back to random.";
        randOrderInSet = true;
        er = e_levelFixed;
      }
      if (repeatNrInSet > 1) {
        qDebug() << "[Tlevel] is not a melody set but number of repeats was set. Fixed!";
        repeatNrInSet = 1;
        er = e_levelFixed;
      }
  }
  if (xml.hasError()) {
    qDebug() << "[Tlevel] level has error" << xml.errorString() << xml.lineNumber();
    return e_otherError;
  }
  return er;
}


void Tlevel::writeToXml(QXmlStreamWriter& xml) {
  xml.writeStartElement(QLatin1String("level"));
    xml.writeAttribute(QLatin1String("name"), name);
    xml.writeTextElement(QLatin1String("description"), desc);
  // QUESTIONS
    xml.writeStartElement(QLatin1String("questions"));
      questionAs.toXml(-1, xml);
      for (int i = 0; i < 4; i++)
        answersAs[i].toXml(i, xml);
      xml.writeTextElement(QLatin1String("requireOctave"), QVariant(requireOctave).toString());
      xml.writeTextElement(QLatin1String("requireStyle"), QVariant(requireStyle).toString());
      xml.writeTextElement(QLatin1String("showStrNr"), QVariant(showStrNr).toString());
      xml.writeTextElement(QLatin1String("clef"), QVariant(static_cast<int>(clef.type())).toString());
      xml.writeTextElement(QLatin1String("instrument"), QVariant(static_cast<int>(instrument)).toString());
      xml.writeTextElement(QLatin1String("onlyLowPos"), QVariant(onlyLowPos).toString());
      xml.writeTextElement(QLatin1String("onlyCurrKey"), QVariant(onlyCurrKey).toString());
      xml.writeTextElement(QLatin1String("intonation"), QVariant(intonation).toString());
    xml.writeEndElement();
  // ACCIDENTALS
    xml.writeStartElement(QLatin1String("accidentals"));
      xml.writeTextElement(QLatin1String("withSharps"), QVariant(withSharps).toString());
      xml.writeTextElement(QLatin1String("withFlats"), QVariant(withFlats).toString());
      xml.writeTextElement(QLatin1String("withDblAcc"), QVariant(withDblAcc).toString());
      xml.writeTextElement(QLatin1String("useKeySign"), QVariant(useKeySign).toString());
      xml.writeStartElement(QLatin1String("loKey"));
        loKey.toXml(xml);
      xml.writeEndElement(); // loKey
      xml.writeStartElement(QLatin1String("hiKey"));
        hiKey.toXml(xml);
      xml.writeEndElement(); // hiKey
      xml.writeTextElement(QLatin1String("isSingleKey"), QVariant(isSingleKey).toString());
      xml.writeTextElement(QLatin1String("manualKey"), QVariant(manualKey).toString());
      xml.writeTextElement(QLatin1String("forceAccids"), QVariant(forceAccids).toString());
    xml.writeEndElement(); // accidentals
  // MELODIES
    xml.writeStartElement(QLatin1String("melodies"));
      xml.writeTextElement(QLatin1String("melodyLength"), QVariant(melodyLen).toString());
      xml.writeTextElement(QLatin1String("endsOnTonic"), QVariant(endsOnTonic).toString());
      xml.writeTextElement(QLatin1String("requireInTempo"), QVariant(requireInTempo).toString());
      if (randMelody != e_randFromRange) { // write it only when needed
        xml.writeTextElement(QLatin1String("randType"), QVariant(static_cast<quint8>(randMelody)).toString());
        if (randMelody == e_randFromList) {
            xml.writeStartElement(QLatin1String("keyOfrandList"));
              keyOfrandList.toXml(xml);
            xml.writeEndElement(); // keyOfrandList
            xml.writeStartElement(QLatin1String("noteList"));
            for (int n = 0; n < notesList.count(); ++n)
              notesList[n].toXml(xml, QLatin1String("n")); // XML note wrapped into <n> tag
            xml.writeEndElement(); // noteList
        } else if (randMelody == e_melodyFromSet) {
            xml.writeTextElement(QLatin1String("randOrderInSet"), QVariant(randOrderInSet).toString());
            xml.writeTextElement(QLatin1String("repeatNrInSet"), QVariant(repeatNrInSet).toString());
            for (int m = 0; m < melodySet.count(); ++m) {
              xml.writeStartElement(QLatin1String("melody"));
              Tmelody& mel = melodySet[m];
              if (!mel.title().isEmpty())
                xml.writeAttribute(QLatin1String("title"), mel.title());
              if (!mel.composer().isEmpty())
                xml.writeAttribute(QLatin1String("composer"), mel.composer());
              melodySet[m].toXml(xml);
              xml.writeEndElement(); // melody
            }
        }
      }
    xml.writeEndElement(); // melodies
  // RHYTHMS
    if (useRhythms()) { // store only when enabled
      xml.writeStartElement(QLatin1String("rhythms"));
        xml.writeTextElement(QLatin1String("meters"), QVariant(meters).toString());
        xml.writeTextElement(QLatin1String("basic"), QVariant(basicRhythms).toString());
        xml.writeTextElement(QLatin1String("dots"), QVariant(dotsRhythms).toString());
        xml.writeTextElement(QLatin1String("diversity"), QVariant(rhythmDiversity).toString());
        xml.writeStartElement(QLatin1String("bars"));
          xml.writeAttribute(QLatin1String("variable"), QVariant(variableBarNr).toString());
          xml.writeCharacters(QVariant(barNumber).toString());
        xml.writeEndElement(); // bars
        xml.writeTextElement(QLatin1String("rests"), QVariant(useRests).toString());
        xml.writeTextElement(QLatin1String("ties"), QVariant(useTies).toString());
      xml.writeEndElement(); // rhythms
    }
  // RANGE
    xml.writeStartElement(QLatin1String("range"));
      xml.writeTextElement(QLatin1String("loFret"), QVariant(static_cast<qint8>(loFret)).toString());
      xml.writeTextElement(QLatin1String("hiFret"), QVariant(static_cast<qint8>(hiFret)).toString());
      loNote.toXml(xml, QLatin1String("loNote"));
      hiNote.toXml(xml, QLatin1String("hiNote"));
      for (int i = 0; i < 6; i++) {
        xml.writeStartElement(QLatin1String("useString"));
        xml.writeAttribute(QLatin1String("number"), QVariant(i + 1).toString());
        xml.writeCharacters(QVariant(usedStrings[i]).toString());
        xml.writeEndElement(); // useString
      }
    xml.writeEndElement(); // range

  xml.writeEndElement(); // level
}


bool Tlevel::saveToFile(Tlevel& level, const QString& levelFile) {
  QFile file(levelFile);
  if (file.open(QIODevice::WriteOnly)) {
      QDataStream out(&file);
      out.setVersion(QDataStream::Qt_5_2);
      out << currentVersion;
      QXmlStreamWriter xml(&file);

      xml.setAutoFormatting(true);
      xml.setAutoFormattingIndent(2);
      xml.writeStartDocument();
      xml.writeComment(QStringLiteral("\nXML file of Nootka exam level.\n"
                       "https://nootka.sourceforge.io\n"
                       "It is strongly recommended to do not edit this file manually.\n"
                       "Use Nootka level creator instead!\n"));
      level.writeToXml(xml);
      xml.writeEndDocument();

      file.close();
      return true;
  } else
      return false;
}

//#################################################################################################
//###################      FIXES FOR LEVEL CONTENT     ############################################
//#################################################################################################

void Tlevel::convFromDropedBass() {
  if (clef.type() == Tclef::Bass_F_8down)
    clef.setClef(Tclef::Bass_F);

  loNote.riseOctaveUp();
  hiNote.riseOctaveUp();
  if (!notesList.isEmpty()) {
    for (int n = 0; n < notesList.count(); ++n)
      notesList[n].riseOctaveUp();
  }
  if (!melodySet.isEmpty()) {
    for (int m = 0; m < melodySet.count(); ++m) {
      auto mel = &melodySet[m];
      if (mel->clef() == Tclef::Bass_F_8down)
        mel->setClef(Tclef::Bass_F);
      for (int n = 0; n < mel->length(); ++n)
        mel->note(n)->p().riseOctaveUp();
    }
  }
}


Tclef Tlevel::fixClef(quint16 cl) {
    if (cl == 0) // For backward compatibility - 'no clef' never occurs
        return Tclef(Tclef::Treble_G_8down); // and versions before 0.8.90 kept here 0
    if (cl == 1) {
      Tnote lowest(6, -2, 0);
      if (canBeGuitar() || loNote.chromatic() < lowest.chromatic() )
          return Tclef(Tclef::Treble_G_8down);  // surely: 1 = e_treble_G was not intended here
      else
          return Tclef(Tclef::Treble_G);
    }
    if (cl != 2 && cl != 4 && cl != 8 && cl != 16 && cl != 32 && cl != 64 && cl != 128) {
        qDebug() << "[Tlevel] Fixed clef type. Previous value was:" << cl;
        return Tclef(Tclef::Treble_G_8down); // some previous mess - when levels didn't' support clefs
    }

    return Tclef((Tclef::EclefType)cl);
}


Tinstrument::Etype Tlevel::fixInstrument(quint8 instr) {
  // Value 255 comes from transition version 0.8.90 - 0.8.95 and means no instrument,
  // however it is invalid because it ignores guitarists and doesn't play exams/exercises on proper instrument
    if (instr == 255) {
        if (canBeGuitar() || canBeSound()) {
            hasInstrToFix = true;
            return GLOB->instrument().type();
        } else // instrument has no matter
            return Tinstrument::NoInstrument;
    } else if (instr == 0 || instr == 1) {
        // Values 0 and 1 occur in versions before 0.8.90 where an instrument doesn't exist
        if (canBeGuitar() || canBeSound())
            return Tinstrument::ClassicalGuitar;
        else
            return Tinstrument::NoInstrument;
    } else if (instr < 4) { // simple cast to detect an instrument
          return (Tinstrument::Etype)instr;
    } else {
        qDebug() << "[Tlevel]  Tlevel::instrument had some stupid value. FIXED";
        return GLOB->instrument().type();
    }
}


Tinstrument::Etype Tlevel::detectInstrument(Tinstrument::Etype currInstr) {
  if (canBeGuitar()) { // it has to be some kind of guitar
      if (currInstr != Tinstrument::NoInstrument)
          return currInstr;
      else // if current instrument isn't guitar force classical
          return Tinstrument::ClassicalGuitar;
  } else if (canBeSound()) // prefer current instrument for it
        return currInstr;
  else // no guitar & no sound - instrument type really has no matter
        return Tinstrument::NoInstrument;
}


//###################### HELPERS ################################################################
/**
 * Checking is any question enabled first and then checking appropriate answer type.
 * Despite of level creator disables all questions with empty answers (set to false)
 * better check this again to avoid further problems. 
 */
bool Tlevel::canBeScore() const {
  if (questionAs.isOnScore() ||
    (questionAs.isName() && answersAs[TQAtype::e_asName].isOnScore()) ||
    (questionAs.isOnInstr() && answersAs[TQAtype::e_onInstr].isOnScore()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isOnScore())  )
      return true;
  else
      return false;
}

bool Tlevel::canBeName() const {
  if (questionAs.isName() ||
    (questionAs.isOnScore() && answersAs[TQAtype::e_onScore].isName()) ||
    (questionAs.isOnInstr() && answersAs[TQAtype::e_onInstr].isName()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isName())  )
      return true;
  else
      return false;
}

bool Tlevel::canBeGuitar() const {
  if (questionAs.isOnInstr() ||
    (questionAs.isName() && answersAs[TQAtype::e_asName].isOnInstr()) ||
    (questionAs.isOnScore() && answersAs[TQAtype::e_onScore].isOnInstr()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isOnInstr())  )
      return true;
  else
      return false;
}

bool Tlevel::canBeSound() const {
  if (questionAs.isSound() ||
    (questionAs.isName() && answersAs[TQAtype::e_asName].isSound()) ||
    (questionAs.isOnInstr() && answersAs[TQAtype::e_onInstr].isSound()) ||
    (questionAs.isOnScore() && answersAs[TQAtype::e_onScore].isSound())  )
      return true;
  else
      return false;
}

/**
 * To be sure, a melody is possible we checking not only notes number
 * but question-answer types as well, even if creator doesn't allow for wrong sets.
 */
bool Tlevel::canBeMelody() const {
  if (melodyLen > 1 &&
      ((questionAs.isOnScore() && answersAs[TQAtype::e_onScore].isSound()) ||
      (questionAs.isSound() && answersAs[TQAtype::e_asSound].isOnScore()) ||
      (questionAs.isSound() && answersAs[TQAtype::e_asSound].isSound())) )
    return true;
  else
    return false;
}


/**
 * Checking questions would be skipped because Level creator avoids selecting answer without question.
 * Unfortunately built-in levels are not so perfect.
 */
bool Tlevel::answerIsNote() const {
  if ((questionAs.isOnScore() && answersAs[TQAtype::e_onScore].isOnScore()) ||
      (questionAs.isName() && answersAs[TQAtype::e_asName].isOnScore()) ||
      (questionAs.isOnInstr() && answersAs[TQAtype::e_onInstr].isOnScore()) ||
      (questionAs.isSound() && answersAs[TQAtype::e_asSound].isOnScore()) )
        return true;
  else
        return false;
}

bool Tlevel::answerIsName() const {
  if ((questionAs.isOnScore() && answersAs[TQAtype::e_onScore].isName()) ||
      (questionAs.isName() && answersAs[TQAtype::e_asName].isName()) ||
      (questionAs.isOnInstr() && answersAs[TQAtype::e_onInstr].isName()) ||
      (questionAs.isSound() && answersAs[TQAtype::e_asSound].isName()) )
        return true;
  else
        return false;
}

bool Tlevel::answerIsGuitar() const {
  if ((questionAs.isOnScore() && answersAs[TQAtype::e_onScore].isOnInstr()) ||
      (questionAs.isName() && answersAs[TQAtype::e_asName].isOnInstr()) ||
      (questionAs.isOnInstr() && answersAs[TQAtype::e_onInstr].isOnInstr()) ||
      (questionAs.isSound() && answersAs[TQAtype::e_asSound].isOnInstr()) )
        return true;
  else
        return false;
}

bool Tlevel::answerIsSound() const {
  if ((questionAs.isOnScore() && answersAs[TQAtype::e_onScore].isSound()) ||
      (questionAs.isName() && answersAs[TQAtype::e_asName].isSound()) ||
      (questionAs.isOnInstr() && answersAs[TQAtype::e_onInstr].isSound()) ||
      (questionAs.isSound() && answersAs[TQAtype::e_asSound].isSound()) )
        return true;
  else
        return false;
}


bool Tlevel::inScaleOf(int loNoteNr, int hiNoteNr) const {
  int loNr = loNote.chromatic();
  int hiNr = hiNote.chromatic();
  if (loNr >= loNoteNr && loNr <= hiNoteNr && hiNr >= loNoteNr && hiNr <= hiNoteNr)
    return true;
  else
    return false;
}


bool Tlevel::inScaleOf() {
  return inScaleOf(GLOB->loString().chromatic(), GLOB->hiNote().chromatic());
}


bool Tlevel::adjustFretsToScale(char& loF, char& hiF) {
  if (!inScaleOf()) // when note range is not in an instrument scale
    return false; // get rid - makes no sense to further check

  int lowest = GLOB->GfretsNumber, highest = 0;
  for (int no = loNote.chromatic(); no <= hiNote.chromatic(); no++) {
    if (!withFlats && !withSharps)
      if (Tnote(no).alter()) // skip note with accidental when not available in the level
          continue;
    int tmpLow = GLOB->GfretsNumber;
    for(int st = 0 ; st < GLOB->Gtune()->stringNr(); st++) {
      if (!usedStrings[st])
          continue;
      int diff = no - GLOB->Gtune()->str(GLOB->strOrder(st) + 1).chromatic();
      if (diff >= 0 && diff <= static_cast<int>(GLOB->GfretsNumber)) { // found
          lowest = qMin<int>(lowest, diff);
          tmpLow = qMin<int>(tmpLow, diff);
      }
    }
    highest = qMax<int>(highest, tmpLow);
  }
  loF = (char)lowest;
  hiF = (char)highest;
  return true;
}


/**
 * FIXME
 * It is possible that first melody in the list will be without meter whether another one will have one
 */
bool Tlevel::useRhythms() const {
  return canBeMelody() && ((meters && (dotsRhythms || basicRhythms)) || (isMelodySet() && melodySet.first().meter()->meter() != Tmeter::NoMeter));
}


int Tlevel::keysInRange() const {
  if (!useKeySign || isSingleKey)
    return 1;
  if (hiKey.value() - loKey.value() < 0) {
    qDebug() << "[Tlevel] FIXME! Key range is invalid!";
    return 1;
  }
  return hiKey.value() - loKey.value() + 1;
}


Tlevel::EerrorType Tlevel::fixFretRange() {
  if (loFret > hiFret) {
    char tmpFret = loFret;
    loFret = hiFret;
    hiFret = tmpFret;
    return e_levelFixed;
  }
  return e_level_OK;
}


Tlevel::EerrorType Tlevel::fixNoteRange() {
  if (loNote.chromatic() > hiNote.chromatic()) {
    Tnote tmpNote = loNote;
    loNote = hiNote;
    hiNote = tmpNote;
    return e_levelFixed;
  }
  return e_level_OK;
}


Tlevel::EerrorType Tlevel::fixKeyRange() {
  if (loKey.value() > hiKey.value()) {
    char tmpKey = loKey.value();
    loKey = hiKey;
    hiKey = TkeySignature(tmpKey);
    return e_levelFixed;
  }
  return e_level_OK;
}







