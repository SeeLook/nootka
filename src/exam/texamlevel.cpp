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
   name = "complex";
   desc = "All possible options turned on";
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

QDataStream& TexamLevel::operator <<(QDataStream &out) {
    out << name << desc;
    out << questionAs;
    out << answersAs[0] << answersAs[1] << answersAs[2] << answersAs[3];
    out << withSharps << withFlats << withDblAcc;
    out << useKeySign << isSingleKey;
    out << (qint8)loKey << (qint8)hiKey;
    out << manualKey << forceAccids;
    out <<  requireOctave << requireStyle;
// RANGE
    out << (qint8)loNote.note << (qint8)loNote.octave << (qint8)loNote.acidental;
    out << (qint8)hiNote.note << (qint8)hiNote.octave << (qint8)hiNote.acidental;
    out << isNoteLo << isNoteHi;
    out << (qint8)loFret << (qint8)hiFret;
    out << isFretHi;
    out << usedStrings[0] << usedStrings[1] << usedStrings[2] << usedStrings[3] <<
            usedStrings[4] <<  usedStrings[5];
    out << onlyLowPos << onlyCurrKey;
    return out;
}

QDataStream& TexamLevel::operator >>(QDataStream &in) {
    in >> name >> desc;
    in >> questionAs;
    in >> answersAs[0] >> answersAs[1] >> answersAs[2] >> answersAs[3];
    in >> withSharps >> withFlats >> withDblAcc;
    in >> useKeySign >> isSingleKey;
    in >> (char)loKey >> (char)hiKey;
    in >> manualKey >> forceAccids;
    in >>  requireOctave >> requireStyle;
// RANGE
    in >> (char)loNote.note >> (char)loNote.octave >> (char)loNote.acidental;
    in >> (char)hiNote.note >> (char)hiNote.octave >> (char)hiNote.acidental;
    in >> isNoteLo >> isNoteHi;
    in >> (char)loFret >> (char)hiFret;
    in >> isFretHi;
    in >> usedStrings[0] >> usedStrings[1] >> usedStrings[2] >> usedStrings[3] >>
            usedStrings[4] >>  usedStrings[5];
    in >> onlyLowPos >> onlyCurrKey;
    return in;
}
