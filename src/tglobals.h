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

    void storeSettings();

    QString path;
    QString version;
    bool hintsEnabled; /** If @param true, hints of widgets are shown */
    bool isFirstRun; /** to show or skip first run wizard*/
    QString lang; /** Application language. If empty - selected from system info*/
    QSettings *config; /** Pointer to QSettings instance of Nootka */
    bool useAnimations; /** to show GUI animations. */
    
				/** Due to bug in version 0.8.95 it keeps value of user declared instrument to fix incorrect levels
				 * or -1 to display a dialog for user to declare it.  */
    int instrumentToFix;

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
		Tclef::Etype Sclef; // preferred clef - treble by default

//============= common with score widget and note name ==========================================
    bool doubleAccidentalsEnabled; //default true
    bool showEnharmNotes; // default true
    
        /** On the beginning it is -1 and then it is set in TscoreWidget constructor
        * as inversion of highlight color from palette() and put to TnoteName,
        * otherwise is taken from saved settings. */
    QColor enharmNotesColor;
    bool seventhIs_B; /** To determine note names - default true */


//======== note name settings ===================================================================
    Tnote::EnameStyle NnameStyleInNoteName;
    bool NoctaveInNoteNameFormat; //default true
    Tnote::EnameStyle NsolfegeStyle; // e_italiano_Si is default
    
				/** Guessing solfege name style from current locale setting. F.e.: ru is e_russian_Ci */
    Tnote::EnameStyle getSolfegeStyle();

//============ guitar settings =============================================================
		Einstrument instrument; /** Type of instrument - classical guitar default */
    unsigned char GfretsNumber; /** default 19 */
    bool GisRightHanded; /** default true */
    bool GshowOtherPos; /** Shows other possibilities of note (sound) on the fretboard (default true) */
    QColor GfingerColor; /** rules the same like in enharmNotesColor */
    QColor GselectedColor;
		
				/** Actual tune of the guitar also with information about strings number
				 * available by Ttune::stringsNr() */
    Ttune *Gtune() { return m_tune; }
    void setTune(Ttune &t);
		
        /** It returns real string number (0 - 5) when @param strNr
        * is sorted number from highest (0) to lowest (5) */
    char strOrder(char strNr) { return m_order[strNr]; }
    Tnote hiString(); /** Returns the highest (usually first - 0) string. */
    Tnote loString(); /** Returns the lowest (usually last) string */
		
				/** The highest available note in current tune with current fret number */
		Tnote hiNote() { return Tnote(hiString().getChromaticNrOfNote() + GfretsNumber); }
		Tnote loNote() { return loString(); } // the same as loString()

        /** It says witch accidentals are preferred while user clicks guitar
        * and note is calculated. Default are sharps*/
    bool GpreferFlats; // default false


    TexamParams *E; /** Exam parameters */
    QColor EquestionColor; // red
    QColor EanswerColor; // green
    QColor EnotBadColor; // #ff8000 (orange)

    TaudioParams *A;  /** Audio parameters */
    
private:
    Ttune *m_tune; /** current guitar tune */
    char m_order[6]; /** Strings' order is determined in @param setTune() method */

};
#endif // TGLOBALS_H

