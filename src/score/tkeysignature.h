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


#ifndef TKEYSIGNATURE_H
#define TKEYSIGNATURE_H

#include <QString>
#include "tnote.h"


    /** This class describes a key signatute.
    * It keeps it in char type  */
class TkeySignature
{
public:
    TkeySignature();
    TkeySignature(char keyS, bool isMinor = false);
        /** Lists of keys names*/
    static QString majorNames[15], minorNames[15];
        /** Array with definitions of each scale.
        * @param scalesDefArr[7] is C-major/a-minor and has 7 times 0 (no accids).
        * The order os from 7b [0] to 7# [15]*/
    static const char scalesDefArr[15][7];
        /** Those smart arrays keep which note in @p scalesDefArr is 1-st in key scale.
        * F.e. For G-major is @p majorKeys[8] = 4,
        * like @p scalesDefArr[8][4] point on accid of a G note.
        * This is faster way then calculate it and takes less memory.*/
    static const char majorKeys[15], minorKeys[15];
        /** This method fulfills majorNames and minorNames arrays
        * appropirate keys names depends on nameing style
        * and suffixes.*/
    static void setNameStyle(Tnote::EnameStyle style, QString majSuf = "", QString minSuf = "");

    static QString getMajorName(char key) { return majorNames[key+7]; }
    static QString getMinorName(char key) { return minorNames[key+7]; }
    QString getMajorName() { return majorNames[m_key+7]; }
    QString getMinorName() { return minorNames[m_key+7]; }
        /** Return QString with accidentals number and their symbol.
         * f.e.: 7# for @param m_key = 7 (Cis-major).
         * @param inHtml points when HTML syntax is used. If true # and b symbols
         * are formated to better view. */
    QString accidNumber(bool inHtml = false);
        /** Checks is given @class Tnote @param n in given @class TkeySignature @param k.
        * In the method it checks all posibilities of the note n (flats, sharps, natural,
        * and returns eighter founded note or Tnote(0,0,0) if didn't find.*/
    static Tnote inKey(TkeySignature k, Tnote n);
    Tnote inKey(Tnote n);
    bool isMinor() { return m_isMinor; }
    void setMinor(bool isMinor) { m_isMinor = isMinor; };

    char value() { return m_key; }

private:
    char m_key;
    bool m_isMinor;
        /** Common part for static and no static inKey() methods.*/
    static Tnote m_inKey(char val, Tnote n);
};

QDataStream &operator<< (QDataStream &out, TkeySignature &key);
//QDataStream &operator>> (QDataStream &in, TkeySignature &key);
    /** This function is substitute of >> operator for @class TkeySignature.
    * It checks is Tnote valid, and return @value bool about it. */
bool getKeyFromStream(QDataStream &in, TkeySignature &k);


#endif // TKEYSIGNATURE_H
