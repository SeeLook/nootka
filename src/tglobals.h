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


#ifndef TGLOBALS_H
#define TGLOBALS_H

#include <QString>
#include <QColor>
#include "tnote.h"
#include <tclef.h>
#include <tinstrument.h>

class Ttune;
class QSettings;
class TexamParams;
class TaudioParams;

class Tglobals
{

public:

    Tglobals();
    ~Tglobals();

        /** This method return application install path - path from whwre Nootka was started. */
    static QString getInstPath(QString appInstPath);

        /** Returns string with given color in HTML/CSS style.
        * It is needed in setStyleSheet method for Qwidget style*/
    static QString getBGcolorText(QColor C);

    static QColor invertColor(QColor C);
    static qreal iV(int ch) { return ch / 255.0; } // inverts value (0 - 255) to 0.0 - 1.0
    static QColor mergeColors(QColor C1, QColor C2);

    void storeSettings();

    QString path;
    QString version;
        /** If @param true, hints of widgets are shown*/
    bool hintsEnabled;
        /** to show or skip first run wizard*/
    bool isFirstRun;
        /** Application language. If empty - selected from system info*/
    QString lang;
        /** Pointer to QSettings instance of Nootka */
    QSettings *config;
        /** to show GUI animations. */
    bool useAnimations;

        /** Let's have a convention:
        * globals settings for @class TnoteName will started from 'N' letter
        * for @class TscoreWidget and @class TscoreWidgetSimple
        * and for guitar (@class TfingerBoard) from 'G' letter.
				* For sound and exam there are pointers to appropirate classes with theirs parameters. */


//============ score widget settings =============================================================
        /** if true shows other similar (enharmonic) notes on the staff:
        * for C# - Db, for G - Fx and Abb. In Tnotename widget also. */
    bool SkeySignatureEnabled;
    bool SshowKeySignName; // default true
    Tnote::EnameStyle SnameStyleInKeySign;
        /** Convention is:
        * if keyNameSuffix == " " constructor of Tglobals sets its default and
        * constructor of TkeySignatureView sets translateable value "major" and "minor"
        * otherwise it is overrides by loading settings
        * if keyNameSuffix == "" user prefers without suffix.
        * If keyNameSuffix has some text -  is translated by user himself */
    QString SmajKeyNameSufix;
    QString SminKeyNameSufix;
    QColor SpointerColor;
		Tclef::Etype Sclef; // prefered clef - treble by default

//============= common with score widget and note name ==========================================
    bool doubleAccidentalsEnabled; //default true
    bool showEnharmNotes; // default true
        /** On the beginning it is -1 and then it is set in TscoreWidget constructor
        * as inversion of highlight color from palette() and put to TnoteName,
        * otherwise is taken from saved settings. */
    QColor enharmNotesColor;
        /** To determine notes' names */
    bool seventhIs_B; //default true


//======== note name settings ===================================================================
    Tnote::EnameStyle NnameStyleInNoteName;
    bool NoctaveInNoteNameFormat; //default true
    Tnote::EnameStyle NsolfegeStyle; // e_italiano_Si is default
				/** Guessing solfege name style from current locale setting. F.e.: ru is e_russian_Ci */
    Tnote::EnameStyle getSolfegeStyle();
//    bool NoctaveNameInNoteName; //default true

//============ guitar settings =============================================================
				/** Type of instrument - classical guitar default */
		Einstrument instrument;
    unsigned char GfretsNumber; //default 19
    bool GisRightHanded; //default true
        /** Shows other possibilities of note (sound) on the fretboard */
    bool GshowOtherPos; //default true
    QColor GfingerColor; // rules the same like in enharmNotesColor
    QColor GselectedColor;
//--- A Few variables and methods about tune -----
		
				/** Acctual tune of the guitar also with information about strings number
				 * available by Ttune::stringsNr() */
    Ttune *Gtune() { return m_tune; }
    void setTune(Ttune &t);
        /** It returns real string number (0 - 5) when @param strNr
        * is sorted number from highest (0) to lowest (5) */
    char strOrder(char strNr) { return m_order[strNr]; }
        /** Returns the highest (usually first - 0) string. */
    Tnote hiString();
				/** Returns the lowest (usually last) string */
    Tnote loString();
//---------------------------------
        /** It says witch accidentals are prafered while user clicks guitar
        * and note is calculated. Default are sharps*/
    bool GpreferFlats; // default false


    TexamParams *E; /** Exam parameters */
    QColor EquestionColor; // red
    QColor EanswerColor; // green
    QColor EnotBadColor; // #ff8000 (orange)

    TaudioParams *A;  /** Audio parameters */
    
private:
  //--- for tune part ----------------
      /** current guitar tune */
    Ttune *m_tune;
        /** Strings' order is determined in @param setTune() method */
    char m_order[6];

};
#endif // TGLOBALS_H

