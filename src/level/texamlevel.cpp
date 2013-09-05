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



#include "texamlevel.h"
#include "tglobals.h"
#include <ttune.h>
#include <QDebug>

extern Tglobals *gl;

TexamLevel::TexamLevel()
{
  // level paramrters
   name = QObject::tr("master of masters");
   desc = QObject::tr("All possible options are turned on");
   questionAs = TQAtype(true, true, true, true);
   answersAs[0] = TQAtype(true, true, true, true);
   answersAs[1] = TQAtype(true, true, true, true);
   answersAs[2] = TQAtype(true, true, false, false);
   answersAs[3] = TQAtype(true, true, true, true);
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
   showStrNr = true;
  // RANGE
   loNote = gl->loString();
   hiNote = Tnote(gl->hiString().getChromaticNrOfNote()+gl->GfretsNumber);
   /** variables isNoteLo, isNoteHi and isFretHi are not used - it has no sense.
		*  Since version 0.8.90 isNoteLo and isNoteHi are merged into Tclef.
		*  It can store multiple clefs (mayby somewhen it will be used)
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
}

QDataStream &operator << (QDataStream &out, TexamLevel &lev) {
    out << lev.name << lev.desc;
    out << lev.questionAs;
    out << lev.answersAs[0] << lev.answersAs[1] << lev.answersAs[2] << lev.answersAs[3];
    out << lev.withSharps << lev.withFlats << lev.withDblAcc;
    out << lev.useKeySign << lev.isSingleKey;
    out << lev.loKey << lev.hiKey;
    out << lev.manualKey << lev.forceAccids;
    out <<  lev.requireOctave << lev.requireStyle;
// RANGE
    out << lev.loNote << lev.hiNote;
//     out << lev.isNoteLo << lev.isNoteHi;
		out << (quint16)lev.clef.type();
    out << (qint8)lev.loFret << (qint8)lev.hiFret;
//     out << lev.isFretHi;
		quint8 instr;
		if (lev.instrument != e_noInstrument)
			instr = (quint8)lev.instrument;
		else // // because '0' is reserved for backword compability
			instr = 255;
		out << instr;
    out << lev.usedStrings[0] << lev.usedStrings[1] << lev.usedStrings[2]
            << lev.usedStrings[3] << lev.usedStrings[4] <<  lev.usedStrings[5];
    out << lev.onlyLowPos << lev.onlyCurrKey << lev.showStrNr;
    return out;
}


//QDataStream &operator >>(QDataStream &in, TexamLevel &lev) {
bool getLevelFromStream(QDataStream &in, TexamLevel &lev) {
    bool ok = true;
    in >> lev.name >> lev.desc;
    in >> lev.questionAs;
    in >> lev.answersAs[0] >> lev.answersAs[1] >> lev.answersAs[2] >> lev.answersAs[3];
    in >> lev.withSharps >> lev.withFlats >> lev.withDblAcc;
    in >> lev.useKeySign >> lev.isSingleKey;
    ok = getKeyFromStream(in, lev.loKey);
    ok = getKeyFromStream(in, lev.hiKey);
    in >> lev.manualKey >> lev.forceAccids;
    in >>  lev.requireOctave >> lev.requireStyle;
// RANGE
    ok = getNoteFromStream(in, lev.loNote);
    ok = getNoteFromStream(in, lev.hiNote);
//     in >> lev.isNoteLo >> lev.isNoteHi; 
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
//     in >> lev.isFretHi;
	/** Previously is was bool type */
		quint8 instr;
		in >> instr;
		
    in >> lev.usedStrings[0] >> lev.usedStrings[1] >> lev.usedStrings[2]
            >> lev.usedStrings[3] >> lev.usedStrings[4] >>  lev.usedStrings[5];
    in >> lev.onlyLowPos >> lev.onlyCurrKey >> lev.showStrNr;
	// determining/fixing a clef
		if (testClef == 0) // For backword compability - 'no clef' never occurs
				lev.clef = Tclef(Tclef::e_treble_G_8down); // and versions before 0.8.90 kept here 0
		else if (testClef == 1) {
			Tnote lowest(6, -2, 0);
			if (lev.canBeGuitar() || lev.loNote.getChromaticNrOfNote() < lowest.getChromaticNrOfNote() )
					lev.clef = Tclef(Tclef::e_treble_G_8down);  // surely - 1 = e_treble_G was not intended here
			else
					lev.clef = Tclef(Tclef::e_treble_G); 
		}	else if (testClef == 257) // some previous mess - untill levels won't support multiple clefs
				lev.clef = Tclef(Tclef::e_treble_G_8down); 
		else
				lev.clef = Tclef((Tclef::Etype)testClef);
// 		qDebug() << "detected clef is " << testClef << lev.clef.name();
	// determining/fixing an instrument in a level
		if (instr == 0 ||instr == 1) { // Those values occur in versions before 0.8.90 where an instrument doesn't exist
			if (lev.canBeGuitar() || lev.canBeSound()) // try to detect
					lev.instrument = e_classicalGuitar;
			else
					lev.instrument = e_noInstrument;
		} else if (instr < 4) // simple cast to detect an instrument
			lev.instrument = (Einstrument)instr;
		else if (instr == 255) // because '0' is reserved for backword compability
			lev.instrument = e_noInstrument;
		else {
			qDebug() << "TexamLevel::instrument has some stupid value. FIXED";
			lev.instrument = e_classicalGuitar;
		}
// 		qDebug() << "detected instrument is " << instr << instrumentToText(lev.instrument);
    return ok;
}

bool TexamLevel::canBeScore() {
  if (questionAs.isNote() || 
    (questionAs.isName() && answersAs[TQAtype::e_asName].isNote()) || 
    (questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isNote()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isNote())  )
    /** Checking is any question enabled first and then checking appropirate answer type.
     * Despite of level creator disables all questions with empty answers (set to false)
     * beter check this again to avoid further problems. */
      return true;
  else
      return false;
}

bool TexamLevel::canBeName() {
  if (questionAs.isName() || 
    (questionAs.isNote() && answersAs[TQAtype::e_asNote].isName()) || 
    (questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isName()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isName())  )
      return true;
  else
      return false;
}

bool TexamLevel::canBeGuitar() {
  if (questionAs.isFret() || 
    (questionAs.isName() && answersAs[TQAtype::e_asName].isFret()) || 
    (questionAs.isNote() && answersAs[TQAtype::e_asNote].isFret()) ||
    (questionAs.isSound() && answersAs[TQAtype::e_asSound].isFret())  )
      return true;
  else
      return false;
}

bool TexamLevel::canBeSound() {
  if (questionAs.isSound() || 
    (questionAs.isName() && answersAs[TQAtype::e_asName].isSound()) || 
    (questionAs.isFret() && answersAs[TQAtype::e_asFretPos].isSound()) ||
    (questionAs.isNote() && answersAs[TQAtype::e_asNote].isSound())  )
      return true;
  else
      return false;
}

