/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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
#include "ttune.h"

class QSettings;
class TexamParams;
class TaudioParams;

class Tglobals
{

public:

    Tglobals();
    ~Tglobals();

        /** This method return application install path.
        * Under Windows is empty,
        * in Linux is usually /usr/bin or /usr/local/bin */
    static QString getInstPath(QString appInstPath);

        /** Returns string with given color in HTML/CSS style.
        * It is needed in setStyleSheet method for Qwidget style*/
    static QString getBGcolorText(QColor C);

    static QColor invertColor(QColor C);

    void storeSettings();

    QString path;
    QString version;
        /** If @param true, hints of widgets are shown*/
    bool hintsEnabled;
        /** to show or skip first run wizzard*/
    bool isFirstRun;
        /** Application language. If empty - selected from system info*/
    QString lang;
        /** Pointer to QSettings instance of Nootka */
    QSettings *config;

        /** Let's have a convention:
        * globals settings for @class TnoteName will started from 'N' letter
        * for @class TscoreWidget and @class TscoreWidgetSimple
        * and for guitar (@class TfingerBoard) from 'G' letter */


//============ score widget settings =============================================================
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

//============= common with score widget and note name ==========================================
    bool doubleAccidentalsEnabled; //default true
    bool showEnharmNotes; // default true
        /** On the begining it is -1 and then it is set in TscoreWidget constructor
        * as inversion of highlight color from palette() and put to TnoteName,
        * otherwise is taken from saved settings. */
    QColor enharmNotesColor;
        /** To determine notes' names */
    bool seventhIs_B; //default true


//======== note name settings ===================================================================
    Tnote::EnameStyle NnameStyleInNoteName;
    bool NoctaveInNoteNameFormat; //default true
//    bool NoctaveNameInNoteName; //default true

//============ guitar settings =============================================================
    unsigned char GfretsNumber; //default 19
    bool GisRightHanded; //default true
        /** Shows other posibilities of note (sound) on the fretboard */
    bool GshowOtherPos; //default true
    QColor GfingerColor; // rules the same like in enharmNotesColor
    QColor GselectedColor;
//--- A Few variables and methods about tune -----
    //    Ttune Gtune; // default empty
    Ttune Gtune() { return m_tune; }
    void setTune(Ttune t);
        /** It returns real string number (0 - 5) when @param strNr
        * is sorted number from highest (0) to lowest (5) */
    char strOrder(char strNr) { return m_order[strNr]; }
        /** Returns the highest (usually first - 0) and
        * the lowest (usually last - 5) guitar strings. */
    Tnote hiString() { return m_tune[m_order[0] + 1]; }
    Tnote loString() { return m_tune[m_order[5] + 1]; }
//---------------------------------
        /** It says witch accidentals are prafered while user clicks guitar
        * and note is calculated. Default are sharps*/
    bool GpreferFlats; // default false


    TexamParams *E; /** Exam parameters */
    QColor EquestionColor;
    QColor EanswerColor;

    TaudioParams *A;  /** Audio parameters */
    
private:
  //--- for tune part ----------------
      /** current guitar tune */
    Ttune m_tune;
        /** Strings' order is determined in @param setTune() method */
    char m_order[6];

};
#endif // TGLOBALS_H

