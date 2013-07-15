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


#ifndef TEXAMLEVEL_H
#define TEXAMLEVEL_H

#include "tqatype.h"
#include "tnote.h"
#include "tkeysignature.h"
#include "tclef.h"
#include "tinstrument.h"
#include <QString>
#include <QDataStream>

    /** This class describes exam's level.*/
class TexamLevel
{
public:
        /** default constructor creates a "complex" level*/
    TexamLevel();

  // level paramrters
    QString name;
    QString desc; /** description */
    TQAtype questionAs;
    TQAtype answersAs[4];
  // QUESTIONS - as note
    bool withSharps, withFlats, withDblAcc;
    bool useKeySign;
    bool isSingleKey;
    TkeySignature loKey, hiKey; /** range of key signature */
    bool manualKey; /** manually selecting key in answers */
    bool forceAccids;
  // QUESTIONS - as name
    bool requireOctave;
    bool requireStyle;
  // ANSWERS - as position on fingerboard
    bool showStrNr; /** Shows a string number in questions*/
  // RANGE
    Tnote loNote, hiNote;
//     bool isNoteLo, isNoteHi; 
		// since 0.8.90 version those values are changed to Tclef
		Tclef clef;
    char loFret, hiFret;
//     bool isFretHi;
		// since 0.8.90 version this value is changed to Einstrument
		Einstrument instrument;
    bool usedStrings[6];
    bool onlyLowPos;
    bool onlyCurrKey;
    
  // some helpers
    bool canBeScore(); // True if answer or question is note on a score
    bool canBeName(); // True if answer or question is note name
    bool canBeGuitar(); // True if answer or question is position on a guitar
    bool canBeSound(); // True if answer or question is played or sang sound

};

QDataStream &operator<< (QDataStream &out, TexamLevel &lev);
//QDataStream &operator>> (QDataStream &in, TexamLevel &lev);
bool getLevelFromStream(QDataStream &in, TexamLevel &lev);

#endif // TEXAMLEVEL_H
