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
#include <QColor>
#include "tnote.h"
#include "ttune.h"


class Tglobals
{

public:

    Tglobals();
    ~Tglobals();

        /** This method return application install path.
        * Under Windows is empty,
        * in Linux is usually /usr/bin or /usr/local/bin */
    static QString getInstPath(QString appInstPath);
    static QString getBGcolorText(QColor C);
    QString path;
    QString version;


        /** Let's have a convention:
        * globals settings for @class TnoteName will started from 'N' letter
	* for @class TscoreWidget and @class TscoreWidgetSimple
        * and for guitar (@class TfingerBoard) from 'G' letter*/
//score widget settings
        /** if true shows other similar (enharmonic) notes on the staff:
        * for C# - Db, for G - Fx and Abb. In Tnotename widget also. */
    bool SkeySignatureEnabled;
    bool SshowKeySignName; // default true
    Tnote::EnameStyle SnameStyleInKeySign;
        /** Convention is:
        * if keyNameSuffix == " " constructor of Tglobals sets its default and
        * constructor of TkeySignatureView sets translateable value "major" and "minor"
        * otherwise it is overvrites by loading settings
        * if keyNameSuffix == "" user prefers without suffix.
        * If keyNameSuffix has some text -  is translated by user himself */
    QString SmajKeyNameSufix;
    QString SminKeyNameSufix;
    QColor SpointerColor;

//common with score widget and note name
    bool doubleAccidentalsEnabled; //default true
    bool showEnharmNotes; // default true
        /** On the begining it is -1 and then it is set in TscoreWidget constructor
        * as inversion of highlight color from palette() and put to TnoteName,
        * otherwise is taken from saved settings. */
    QColor enharmNotesColor;
        /** To determine notes' names */
    bool seventhIs_B; //default true


//note name settings
    Tnote::EnameStyle NnameStyleInNoteName;
    bool NoctaveInNoteNameFormat; //default true
//    bool NoctaveNameInNoteName; //default true

// guitar settings
    unsigned char GfretsNumber; //default 19
    bool GisRightHanded; //default true
        /** Shows other posibilities of note (sound) on the fretboard */
    bool GshowOtherPos; //default true
    QColor GfingerColor; // rules the same like in enharmNotesColor
    QColor GselectedColor;
        /** If empty TfingerBoard constructor set it to standard tune,
        * otherwise user tune is loaded*/
    Ttune Gtune; // default empty
        /** It says witch accidentals are prafered while user clicks guitar
        * and note is calculated. Default are sharps*/
    bool GpreferFlats; // default false

// Exam settings
    QColor EquestionColor;
    QColor EanswerColor;
        /** Next quetion in en exam is given automatically after
         * correct answer */
    bool EautoNextQuest;
        /** If EautoNextQuest is true incorrect questions are asked
        * untill correct answer will be given. */
    bool ErepeatIncorrect;

};
#endif // TGLOBALS_H
