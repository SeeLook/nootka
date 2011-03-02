/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef TGLOBALS_H
#define TGLOBALS_H

#include <QString>
#include "tnote.h"


class Tglobals
{

public:

    Tglobals();
    ~Tglobals();

    QString version;

//score widget settings
        /** if true shows other similar (enharmonic) notes on the staff:
        * for C# - Db, for G - Fx and Abb. In Tnotename widget also. */
    bool keySignatureEnabled;
    bool showKeySignName; // default true
    Tnote::Enotation nameStyleInKeySign;
        /** Convention is:
        * if keyNameSuffix == " " constructor of Tglobals sets its default and
        * constructor of TkeySignatureView sets translateable value "major" and "minor"
        * otherwise it is overvrites by loading settings
        * if keyNameSuffix == "" user prefers without suffix.
        * If keyNameSuffix has some text -  is translated by user himself */
    QString majKeyNameSufix;
    QString minKeyNameSufix;
//common with score widget and note name
    bool doubleAccidentalsEnabled; //default true
    bool showEnharmNotes; // default true

//note name settings
    Tnote::Enotation nameStyleInNoteName;

};
#endif // TGLOBALS_H
