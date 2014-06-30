/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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
#include "tinitcorelib.h"
#include <music/ttune.h>
#include <taudioparams.h>
#include <tscoreparams.h>
#include <QFile>
#include <QMessageBox>
#include <QApplication>
#include <QXmlStreamWriter>
#include <QXmlStreamWriter>
#include <QVariant>
#include <QDebug>


/*static-------------------------------------------------------------------------------------------*/
/** Versions history:
 * 1. 0x95121701 
 * 
 * 2. 0x95121703 (02.12.2013)
 * 		- support for instrument types and guessing an instrument from previous version
 * 		- instrument enum is casting directly to quint8: e_noInstrument is 0 
 * 
 * 3. 0x95121705 (22.06.2014) - XML stream - universal version
 */

const qint32 Tlevel::levelVersion = 0x95121701;
const qint32 Tlevel::currentVersion = 0x95121705;

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


/** TlevelSelector context of translate() is used for backward compatibility with translations */
void Tlevel::fileIOerrorMsg(QFile& f, QWidget* parent) {
	if (f.fileName() != "") {
			QMessageBox::critical(parent, "", QApplication::translate("TlevelSelector",
																			"Cannot open file\n %1 \n for reading").arg(f.fileName()));
	} else
			QMessageBox::critical(parent, "", QApplication::translate("TlevelSelector", "No file name specified"));
}


void Tlevel::tnoteToXml(const QString& tag, Tnote& n, QXmlStreamWriter& xml) {
	xml.writeStartElement(tag);
		n.toXml(xml);
	xml.writeEndElement(); // tag
}


Tnote Tlevel::tnoteFromXml(QXmlStreamReader& xml) {
	xml.readNextStartElement();
	Tnote note(0, 0, 0);
	note.fromXml(xml);
	return note;
}


/*end static--------------------------------------------------------------------------------------*/

void newerNootkaMessage(const QString &fileName, QWidget* parent) {
	QMessageBox::warning(parent, "Update Nootka!", QString("File: <b>%1</b><br>was created in newer Nootka version that you have.<br>To open it, visit program site to obtain the newest version.").arg(fileName));
}


//-----------------------------------------------------------------------------------------------------
Tlevel::Tlevel() :
	hasInstrToFix(false)
{
  // level parameters
   name = QObject::tr("master of masters");
   desc = QObject::tr("All possible options are turned on");
	 bool hasGuitar = true;
	 if (Tglob::glob()->instrument == e_noInstrument)
		 hasGuitar = false;
   questionAs = TQAtype(true, true, hasGuitar, true);
   answersAs[0] = TQAtype(true, true, hasGuitar, true);
   answersAs[1] = TQAtype(true, true, hasGuitar, true);
   answersAs[2] = TQAtype(true, true, false, false);
   answersAs[3] = TQAtype(true, true, hasGuitar, true);
  // QUESTIONS - as note
   withSharps = true;
   withFlats = true;
   withDblAcc = true;
   useKeySign = true;
   isSingleKey  = false;
   loKey = -7;
   hiKey = 7; // key range (7b to 7#)
   manualKey = true;
   forceAccids = true;
  // QUESTIONS - as name
   requireOctave = true;
   requireStyle = true;
 // ANSWERS - as position on fingerboard
   showStrNr = hasGuitar;
  // RANGE - for non guitar Tglobals will returns scale determined by clef
   loNote = Tglob::glob()->loString();
   hiNote = Tnote(Tglob::glob()->hiString().getChromaticNrOfNote() + Tglob::glob()->GfretsNumber);
   /** variables isNoteLo, isNoteHi and isFretHi are not used - it has no sense.
		*  Since version 0.8.90 isNoteLo and isNoteHi are merged into Tclef.
		*  It can store multiple clefs (maybe in unknown future it will be used)
		*  0 - no clef and up to 15 different clefs	  */
	 clef = Tclef(Tglob::glob()->S->clef);
//    isNoteLo = false;   isNoteHi = false;
//    isFretHi = false; 
	 instrument = Tglob::glob()->instrument;
   //-------------------
   loFret = 0;
   hiFret = Tglob::glob()->GfretsNumber;
	 for (int i = 0; i < 6; i++) {
		 if (i <= Tglob::glob()->Gtune()->stringNr())
				usedStrings[i] = true; 
		 else
				usedStrings[i] = false; 
	 }
   onlyLowPos = false;
   onlyCurrKey = false;
	 intonation = Tglob::glob()->A->intonation;
}


QDataStream &operator << (QDataStream &out, Tlevel &lev) {
    out << lev.name << lev.desc;
    out << lev.questionAs;
    out << lev.answersAs[0] << lev.answersAs[1] << lev.answersAs[2] << lev.answersAs[3];
    out << lev.withSharps << lev.withFlats << lev.withDblAcc;
		quint8 sharedByte = (int)lev.isSingleKey + (2 * lev.intonation);
    out << lev.useKeySign << sharedByte;
    out << lev.loKey << lev.hiKey;
    out << lev.manualKey << lev.forceAccids;
    out << lev.requireOctave << lev.requireStyle;
// RANGE
    out << lev.loNote << lev.hiNote;
//     out << lev.isNoteLo << lev.isNoteHi;
		out << (quint16)lev.clef.type();
    out << (qint8)lev.loFret << (qint8)lev.hiFret;
//     out << lev.isFretHi;
		quint8 instr;
// 		if (lev.instrument != e_noInstrument)
			instr = (quint8)lev.instrument;
// 		else // // because '0' is reserved for backward compatibility
// 			instr = 255;
		out << instr;
    out << lev.usedStrings[0] << lev.usedStrings[1] << lev.usedStrings[2]
            << lev.usedStrings[3] << lev.usedStrings[4] <<  lev.usedStrings[5];
    out << lev.onlyLowPos << lev.onlyCurrKey << lev.showStrNr;
    return out;
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
        hi = Tglob::glob()->GfretsNumber;
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
				lev.clef = Tclef((Tclef::Etype)testClef);
				lev.instrument = (Einstrument)instr;
		}
// 		qDebug() << lev.name << "ver:" << lev.levelVersionNr(ver) << "instrument:" << 
// 						instr << "saved as" << instrumentToText(lev.instrument);
    return ok;
}


void Tlevel::qaTypeFromXml(QXmlStreamReader& xml) {
	TQAtype qa;
	int id = qa.fromXml(xml);
	if (id == -1)
		questionAs = qa;
	else if (id >= 0 && id < 4) {
		answersAs[id] = qa;
		// TODO: verify answersAs context and set corresponding questionAs to false if missing
	}
}


bool Tlevel::loadFromXml(QXmlStreamReader& xml) {
	if (xml.readNextStartElement()) {
		if (xml.name() != "level") {
			qDebug() << "There is no 'level' element in that XML";
			return false;
		}
		name = xml.attributes().value("name").toString();
		if (name == "")
			return false;
	}
	while (xml.readNextStartElement()) {
// 		qDebug() << "readLevelFromXml" << xml.name();
		if (xml.name() == "description")
			desc = xml.readElementText();
	// QUESTIONS
		else if (xml.name() == "questions") {
			while (xml.readNextStartElement()) {
// 				qDebug() << "questions->" << xml.name();
				if (xml.name() == "qaType")
					qaTypeFromXml(xml);
				else if (xml.name() == "requireOctave")
					requireOctave = QVariant(xml.readElementText()).toBool();
				else if (xml.name() == "requireStyle")
					requireStyle = QVariant(xml.readElementText()).toBool();
				else if (xml.name() == "showStrNr")
					showStrNr = QVariant(xml.readElementText()).toBool();
				else if (xml.name() == "clef")
					clef.setClef(Tclef::Etype(QVariant(xml.readElementText()).toInt()));
				else if (xml.name() == "instrument")
					instrument = Einstrument(QVariant(xml.readElementText()).toInt());
				else if (xml.name() == "onlyLowPos")
					onlyLowPos = QVariant(xml.readElementText()).toBool();
				else if (xml.name() == "onlyCurrKey")
					onlyCurrKey = QVariant(xml.readElementText()).toBool();
				else if (xml.name() == "intonation")
					intonation = QVariant(xml.readElementText()).toInt();		
				else
					xml.skipCurrentElement();
			}
		} else if (xml.name() == "accidentals") {
				while (xml.readNextStartElement()) {
// 					qDebug() << "accidentals->" << xml.name();
					if (xml.name() == "withSharps")
						withSharps = QVariant(xml.readElementText()).toBool();
					else if (xml.name() == "withFlats")
						withFlats = QVariant(xml.readElementText()).toBool();
					else if (xml.name() == "withDblAcc")
						withDblAcc = QVariant(xml.readElementText()).toBool();
					else if (xml.name() == "useKeySign")
						useKeySign = QVariant(xml.readElementText()).toBool();
					else if (xml.name() == "loKey")
						loKey = TkeySignature(QVariant(xml.readElementText()).toInt());
					else if (xml.name() == "hiKey")
						hiKey = TkeySignature(QVariant(xml.readElementText()).toInt());
					else if (xml.name() == "isSingleKey")
						isSingleKey = QVariant(xml.readElementText()).toBool();
					else if (xml.name() == "manualKey")
						manualKey = QVariant(xml.readElementText()).toBool();
					else if (xml.name() == "forceAccids")
						forceAccids = QVariant(xml.readElementText()).toBool();
					else
						xml.skipCurrentElement();
				}
		} else if (xml.name() == "range") {
	// RANGE
			while (xml.readNextStartElement()) {
// 				qDebug() << "range->" << xml.name();
				if (xml.name() == "loFret")
					loFret = (char)QVariant(xml.readElementText()).toInt();
				else if (xml.name() == "hiFret")
					hiFret = (char)QVariant(xml.readElementText()).toInt();
				else if (xml.name() == "loNote")
					loNote = tnoteFromXml(xml);
				else if (xml.name() == "hiNote")
					hiNote = tnoteFromXml(xml);
				else if (xml.name() == "useString") {
					int id = QVariant(xml.attributes().value("number").toString()).toInt();
					if (id > 0 && id < 7)
						usedStrings[id - 1] = QVariant(xml.readElementText()).toBool();
				} else
					xml.skipCurrentElement();
			}
		} else
				xml.skipCurrentElement();
	}	
  if (xml.hasError()) {
		qDebug() << "level has error" << xml.errorString() << xml.lineNumber();
// 		return false;
  }
	return true;
}


void Tlevel::writeToXml(QXmlStreamWriter& xml) {
	xml.writeStartElement("level");
		xml.writeAttribute("name", name);
		xml.writeTextElement("description", desc);
	// QUESTIONS
		xml.writeStartElement("questions");
			questionAs.toXml(-1, xml);
			for (int i = 0; i < 4; i++)
				answersAs[i].toXml(i, xml);
			xml.writeTextElement("requireOctave", QVariant(requireOctave).toString());
			xml.writeTextElement("requireStyle", QVariant(requireStyle).toString());
			xml.writeTextElement("showStrNr", QVariant(showStrNr).toString());
			xml.writeTextElement("clef", QVariant((int)clef.type()).toString());
			xml.writeTextElement("instrument", QVariant((int)instrument).toString());
			xml.writeTextElement("onlyLowPos", QVariant(onlyLowPos).toString());
			xml.writeTextElement("onlyCurrKey", QVariant(onlyCurrKey).toString());
			xml.writeTextElement("intonation", QVariant(intonation).toString());
		xml.writeEndElement();
	// ACCIDENTALS
		xml.writeStartElement("accidentals");
			xml.writeTextElement("withSharps", QVariant(withSharps).toString());
			xml.writeTextElement("withFlats", QVariant(withFlats).toString());   
			xml.writeTextElement("withDblAcc", QVariant(withDblAcc).toString());
			xml.writeTextElement("useKeySign", QVariant(useKeySign).toString());
			xml.writeTextElement("loKey", QVariant(loKey.value()).toString());
			xml.writeTextElement("hiKey", QVariant(hiKey.value()).toString());
			xml.writeTextElement("isSingleKey", QVariant(isSingleKey).toString());
			xml.writeTextElement("manualKey", QVariant(manualKey).toString());
			xml.writeTextElement("forceAccids", QVariant(forceAccids).toString());
		xml.writeEndElement(); // accidentals
	// RANGE
		xml.writeStartElement("range");
			xml.writeTextElement("loFret", QVariant((qint8)loFret).toString());
			xml.writeTextElement("hiFret", QVariant((qint8)hiFret).toString());
			tnoteToXml("loNote", loNote, xml);
			tnoteToXml("hiNote", hiNote, xml);
			for (int i = 0; i < 6; i++) {
				xml.writeStartElement("useString");
				xml.writeAttribute("number", QVariant(i + 1).toString());
				xml.writeCharacters(QVariant(usedStrings[i]).toString());
				xml.writeEndElement(); // string
			}
		xml.writeEndElement(); // range
		
	xml.writeEndElement(); // level
}


bool Tlevel::saveToFile(Tlevel& level, const QString& levelFile) {
		QFile file(levelFile);
    if (file.open(QIODevice::WriteOnly)) {
			QDataStream out(&file);
// 			out.setVersion(QDataStream::Qt_5_2);
			out << currentVersion /*<< level*/;
			QXmlStreamWriter xml(&file);

			xml.setAutoFormatting(true);
			xml.writeStartDocument();
			xml.writeComment("\nXML file of Nootka exam level.\nhttp://nootka.sf.net\nIt is strongly recommended to do not edit this file manually.\nUse Nootka level creator instead!\n");
			level.writeToXml(xml);
			xml.writeEndDocument();
			
			file.close();
			return true;
    }
    else
        return false;
}


Tclef Tlevel::fixClef(quint16 cl) {
		if (cl == 0) // For backward compatibility - 'no clef' never occurs
				return Tclef(Tclef::e_treble_G_8down); // and versions before 0.8.90 kept here 0
		if (cl == 1) {
			Tnote lowest(6, -2, 0);
			if (canBeGuitar() || loNote.getChromaticNrOfNote() < lowest.getChromaticNrOfNote() )
					return Tclef(Tclef::e_treble_G_8down);  // surely: 1 = e_treble_G was not intended here
			else
					return Tclef(Tclef::e_treble_G); 
		}	
		if (cl != 2 && cl != 4 && cl != 8 && cl != 16 && cl != 32 && cl != 64 && cl != 128) {
        qDebug() << "Fixed clef type. Previous value was:" << cl;
				return Tclef(Tclef::e_treble_G_8down); // some previous mess - when levels didn't' support clefs
    }
		
		return Tclef((Tclef::Etype)cl);
}


Einstrument Tlevel::fixInstrument(quint8 instr) {
	// Value 255 comes from transition version 0.8.90 - 0.8.95 and means no instrument,
	// however it is invalid because it ignores guitarists and doesn't play exams/exercises on proper instrument
		if (instr == 255) {
			if (canBeGuitar() || canBeSound()) {
					hasInstrToFix = true;
					return Tglob::glob()->instrument;
			} else // instrument has no matter
					return e_noInstrument;
		} else if (instr == 0 || instr == 1) {
			// Values 0 and 1 occur in versions before 0.8.90 where an instrument doesn't exist
			if (canBeGuitar() || canBeSound())
					return e_classicalGuitar;
			else 
					return e_noInstrument;
		} else if (instr < 4) // simple cast to detect an instrument
					return (Einstrument)instr;
		else {
			qDebug() << "Tlevel::instrument has some stupid value. FIXED";
			return Tglob::glob()->instrument;
		}
}


Einstrument Tlevel::detectInstrument(Einstrument currInstr) {
	if (canBeGuitar()) { // it has to be some kind of guitar
			if (currInstr != e_noInstrument)
					return currInstr;
			else // if current instrument isn't guitar force classical 
					return e_classicalGuitar;
	} else if (canBeSound()) // prefer current instrument for it
				return currInstr;
	else // no guitar & no sound - instrument type really has no matter
				return e_noInstrument;
}


//###################### HELPERS ################################################################
/** Checking is any question enabled first and then checking appropriate answer type.
     * Despite of level creator disables all questions with empty answers (set to false)
     * better check this again to avoid further problems. */
bool Tlevel::canBeScore() {
  if (questionAs.isNote() || 
    (questionAs.isName() && answersAs[TQAtype::e_asName].isNote()) || 
    (questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isNote()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isNote())  )
      return true;
  else
      return false;
}

bool Tlevel::canBeName() {
  if (questionAs.isName() || 
    (questionAs.isNote() && answersAs[TQAtype::e_asNote].isName()) || 
    (questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isName()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isName())  )
      return true;
  else
      return false;
}

bool Tlevel::canBeGuitar() {
  if (questionAs.isFret() || 
    (questionAs.isName() && answersAs[TQAtype::e_asName].isFret()) || 
    (questionAs.isNote() && answersAs[TQAtype::e_asNote].isFret()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isFret())  )
      return true;
  else
      return false;
}

bool Tlevel::canBeSound() {
  if (questionAs.isSound() || 
    (questionAs.isName() && answersAs[TQAtype::e_asName].isSound()) || 
    (questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isSound()) ||
    (questionAs.isNote() && answersAs[TQAtype::e_asNote].isSound())  )
      return true;
  else
      return false;
}

/** Checking questions would be skipped because Level creator avoids selecting answer without question.
* Unfortunately built-in levels are not so perfect.*/
bool Tlevel::answerIsNote() {
	if ((questionAs.isNote() && answersAs[TQAtype::e_asNote].isNote()) || 
			(questionAs.isName() && answersAs[TQAtype::e_asName].isNote()) ||
			(questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isNote()) ||
			(questionAs.isSound() && answersAs[TQAtype::e_asSound].isNote()) )
				return true;
	else
				return false;
}

bool Tlevel::answerIsName() {
	if ((questionAs.isNote() && answersAs[TQAtype::e_asNote].isName()) || 
			(questionAs.isName() && answersAs[TQAtype::e_asName].isName()) ||
			(questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isName()) ||
			(questionAs.isSound() && answersAs[TQAtype::e_asSound].isName()) )
				return true;
	else
				return false;
}

bool Tlevel::answerIsGuitar() {
	if ((questionAs.isNote() && answersAs[TQAtype::e_asNote].isFret()) || 
			(questionAs.isName() && answersAs[TQAtype::e_asName].isFret()) ||
			(questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isFret()) ||
			(questionAs.isSound() && answersAs[TQAtype::e_asSound].isFret()) )
				return true;
	else
				return false;
}

bool Tlevel::answerIsSound() {
	if ((questionAs.isNote() && answersAs[TQAtype::e_asNote].isSound()) || 
			(questionAs.isName() && answersAs[TQAtype::e_asName].isSound()) ||
			(questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isSound()) ||
			(questionAs.isSound() && answersAs[TQAtype::e_asSound].isSound()) )
				return true;
	else
				return false;
}


bool Tlevel::inScaleOf(int loNoteNr, int hiNoteNr) {
	int loNr = loNote.getChromaticNrOfNote();
	int hiNr = hiNote.getChromaticNrOfNote();
	if (loNr >= loNoteNr && loNr <= hiNoteNr && hiNr >= loNoteNr && hiNr <= hiNoteNr)
		return true;
	else
		return false;
}


bool Tlevel::inScaleOf() {
	return inScaleOf(Tglob::glob()->loString().getChromaticNrOfNote(), Tglob::glob()->hiNote().getChromaticNrOfNote());
}


bool Tlevel::adjustFretsToScale(char& loF, char& hiF) {
	if (!inScaleOf()) // when note range is not in an instrument scale
		return false; // get rid - makes no sense to further check
	
	int lowest = Tglob::glob()->GfretsNumber, highest = 0;
	for (int no = loNote.getChromaticNrOfNote(); no <= hiNote.getChromaticNrOfNote(); no++) {
		if (!withFlats && !withSharps)
			if (Tnote(no).acidental) // skip note with accidental when not available in the level
					continue;
		int tmpLow = Tglob::glob()->GfretsNumber;
		for(int st = 0 ; st < Tglob::glob()->Gtune()->stringNr(); st++) {
			if (!usedStrings[st]) 
					continue;
			int diff = no - Tglob::glob()->Gtune()->str(Tglob::glob()->strOrder(st) + 1).getChromaticNrOfNote();
			if (diff >= 0 && diff <= Tglob::glob()->GfretsNumber) { // found
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









