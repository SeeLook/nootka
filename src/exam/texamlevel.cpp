/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "texamlevel.h"
#include "tglobals.h"

extern Tglobals *gl;


TexamLevel::TexamLevel()
{
  // level paramrters
   name = QT_TR_NOOP("master of masters");
   desc = QT_TR_NOOP("All possible options turned on");
   questionAs = TQAtype(true, true, true, false);
   answersAs[0] = TQAtype(true, true, true, false);
   answersAs[1] = TQAtype(true, true, true, false);
   answersAs[2] = TQAtype(true, true, true, false);
   answersAs[3] = TQAtype(true, true, true, false);
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
  // RANGE
   loNote = gl->Gtune.lowest();
   hiNote = Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber);
   isNoteLo = true;
   isNoteHi = true;
   loFret = 0;
   hiFret = gl->GfretsNumber;
   isFretHi = true;
   usedStrings[0] = true; usedStrings[1] = true; usedStrings[2] = true;
   usedStrings[3] = true; usedStrings[4] = true; usedStrings[5] = true;
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
    out << (qint8)lev.loNote.note << (qint8)lev.loNote.octave << (qint8)lev.loNote.acidental;
    out << (qint8)lev.hiNote.note << (qint8)lev.hiNote.octave << (qint8)lev.hiNote.acidental;
    out << lev.isNoteLo << lev.isNoteHi;
    out << (qint8)lev.loFret << (qint8)lev.hiFret;
    out << lev.isFretHi;
    out << lev.usedStrings[0] << lev.usedStrings[1] << lev.usedStrings[2]
            << lev.usedStrings[3] << lev.usedStrings[4] <<  lev.usedStrings[5];
    out << lev.onlyLowPos << lev.onlyCurrKey;
    return out;
}


    /** @todo check all no-bool values to avoid corrupted files*/
QDataStream &operator >>(QDataStream &in, TexamLevel &lev) {
    in >> lev.name >> lev.desc;
    in >> lev.questionAs;
    in >> lev.answersAs[0] >> lev.answersAs[1] >> lev.answersAs[2] >> lev.answersAs[3];
    in >> lev.withSharps >> lev.withFlats >> lev.withDblAcc;
    in >> lev.useKeySign >> lev.isSingleKey;
//    TkeySignature loK, hiK;
    in >> lev.loKey >> lev.hiKey;
//    lev.loKey = char(lo);
//    lev.hiKey = char(hi);
    in >> lev.manualKey >> lev.forceAccids;
    in >>  lev.requireOctave >> lev.requireStyle;
// RANGE
    qint8 nnote, ooctave, aaccid;
    in >> nnote >> ooctave >> aaccid; // loNote
    lev.loNote.note = char(nnote);
    lev.loNote.octave = char(ooctave);
    lev.loNote.acidental = char(aaccid);
    in >> nnote >> ooctave >> aaccid;
    lev.hiNote.note = char(nnote);
    lev.hiNote.octave = char(ooctave);
    lev.hiNote.acidental = char(aaccid);
    in >> lev.isNoteLo >> lev.isNoteHi;
    qint8 lo,hi;
    in >> lo >> hi;
    lev.loFret = char(lo);
    lev.hiFret = char(hi);
    in >> lev.isFretHi;
    in >> lev.usedStrings[0] >> lev.usedStrings[1] >> lev.usedStrings[2]
            >> lev.usedStrings[3] >> lev.usedStrings[4] >>  lev.usedStrings[5];
    in >> lev.onlyLowPos >> lev.onlyCurrKey;
    return in;
}
