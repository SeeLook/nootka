/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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
#include "tglobals.h"
#include <ttune.h>
#include <taudioparams.h>
#include <QDebug>
#include <QMessageBox>

extern Tglobals *gl;


/*static*/
/** Versions history:
 * 1. 0x95121701 
 * 
 * 2. 0x95121703 (02.12.2013)
 * 		- support for instrument types and guessing an instrument from previous version
 * 		- instrument enum is casting directly to quint8: e_noInstrument is 0 
 */

const qint32 Tlevel::levelVersion = 0x95121701;
const qint32 Tlevel::currentVersion = 0x95121703;

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

/*end static*/

Tlevel::Tlevel() :
	hasInstrToFix(false)
{
  // level parameters
   name = QObject::tr("master of masters");
   desc = QObject::tr("All possible options are turned on");
	 bool hasGuitar = true;
	 if (gl->instrument == e_noInstrument)
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
   loNote = gl->loString();
   hiNote = Tnote(gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber);
   /** variables isNoteLo, isNoteHi and isFretHi are not used - it has no sense.
		*  Since version 0.8.90 isNoteLo and isNoteHi are merged into Tclef.
		*  It can store multiple clefs (maybe in unknown future it will be used)
		*  0 - no clef and up to 15 different clefs	  */
	 clef = Tclef(gl->Sclef);
//    isNoteLo = false;   isNoteHi = false;
//    isFretHi = false; 
	 instrument = gl->instrument;
   //-------------------
   loFret = 0;
   hiFret = gl->GfretsNumber;
	 for (int i = 0; i < 6; i++) {
		 if (i <= gl->Gtune()->stringNr())
				usedStrings[i] = true; 
		 else
				usedStrings[i] = false; 
	 }
   onlyLowPos = false;
   onlyCurrKey = false;
	 intonation = gl->A->intonation;
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
        hi = gl->GfretsNumber;
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
		qDebug() << lev.name << "ver:" << lev.levelVersionNr(ver) << "instrument:" << 
						instr << "saved as" << instrumentToText(lev.instrument);
    return ok;
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
		if (cl == 257) // some previous mess - when levels didn't' support multiple clefs
				return Tclef(Tclef::e_treble_G_8down); 
		
		return Tclef((Tclef::Etype)cl);
}


Einstrument Tlevel::fixInstrument(quint8 instr) {
	// Value 255 comes from transition version 0.8.90 - 0.8.95 and means no instrument,
	// however it is invalid because it ignores guitarists and doesn't play exams/exercises on proper instrument
		if (instr == 255) {
			if (canBeGuitar() || canBeSound()) {
					hasInstrToFix = true;
					return gl->instrument;
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
			return gl->instrument;
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


bool Tlevel::inScaleOf(char loNoteNr, char hiNoteNr) {
	if (loNote.getChromaticNrOfNote() >= loNoteNr && loNote.getChromaticNrOfNote() <= hiNoteNr
		&& hiNote.getChromaticNrOfNote() >= loNoteNr && hiNote.getChromaticNrOfNote() <= hiNoteNr)
				return true;
	else
				return false;
}


bool Tlevel::inScaleOf() {
	inScaleOf(gl->loString().getChromaticNrOfNote(), gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber);
}


void newerNootkaMessage(const QString &fileName, QWidget* parent) {
	QMessageBox::warning(parent, "Update Nootka!", QString("File: <b>%1</b><br>was created in newer Nootka version that you have.<br>To open it, visit program site to obtain the newest version.").arg(fileName));
}





