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


#ifndef TEXAMLEVEL_H
#define TEXAMLEVEL_H

#include "tqatype.h"
#include "tnote.h"
#include "tkeysignature.h"
#include <QString>
#include <QDataStream>

    /** This class describes exam's level.*/
class TexamLevel
{
public:
        /** default constructor creates "complex" level*/
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
  // RANGE
    Tnote loNote, hiNote;
    bool isNoteLo, isNoteHi;
    char loFret, hiFret;
    bool isFretHi;
    bool usedStrings[6];
    bool onlyLowPos;
    bool onlyCurrKey;

};

QDataStream &operator<< (QDataStream &out, TexamLevel &lev);
//QDataStream &operator>> (QDataStream &in, TexamLevel &lev);
bool getLevelFromStream(QDataStream &in, TexamLevel &lev);

#endif // TEXAMLEVEL_H
