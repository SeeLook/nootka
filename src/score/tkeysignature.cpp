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


#include "tkeysignature.h"
//#include <QDebug>

/*static*/
const char TkeySignature::scalesDefArr[15][7] = {
    {-1,-1,-1,-1,-1,-1,-1},       // Cb/ab
    {-1,-1,-1, 0,-1,-1,-1},       // Gb/eb
    { 0,-1,-1, 0,-1,-1,-1},       // Db/bb
    { 0,-1,-1, 0, 0,-1,-1},       // Ab/f
    { 0, 0,-1, 0, 0,-1,-1},       // Eb/c
    { 0, 0,-1, 0, 0, 0,-1},       // B/g
    { 0, 0, 0, 0, 0, 0,-1},       // F/d
    { 0, 0, 0, 0, 0, 0, 0},       // C/a
    { 0, 0, 0, 1, 0, 0, 0},       // G/e
    { 1, 0, 0, 1, 0, 0, 0},       // D/h
    { 1, 0, 0, 1, 1, 0, 0},       // A/f#
    { 1, 1, 0, 1, 1, 0, 0},       // E/c#
    { 1, 1, 0, 1, 1, 1, 0},       // B/g#
    { 1, 1, 1, 1, 1, 1, 0},       // F#/d#
    { 1, 1, 1, 1, 1, 1, 1},       // C#/a#
};

const char TkeySignature::majorKeys[15] = { 0, 4, 1, 5, 2, 6, 3, 0, 4, 1, 5, 2, 6, 3, 0 };
const char TkeySignature::minorKeys[15] = { 5, 2, 6, 3, 0, 4, 1, 5, 2, 6, 3, 0, 4, 1, 5 };

QString TkeySignature::majorNames[15] = { "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};
QString TkeySignature::minorNames[15] = { "", "", "", "", "", "", "", "", "", "", "", "", "", "", ""};

void TkeySignature::setNameStyle(Tnote::EnameStyle style, QString majSuf, QString minSuf) {
    Tnote n;
    for (int i=0; i<15; i++) {
        n = Tnote(majorKeys[i]+1, 0, scalesDefArr[i][majorKeys[i]]);
        majorNames[i] = QString::fromStdString(n.getName(style, false));
        if (majSuf != "") majorNames[i] += "-" + majSuf;
        n = Tnote(minorKeys[i]+1, 0, scalesDefArr[i][minorKeys[i]]);
        minorNames[i] = QString::fromStdString(n.getName(style, false )).toLower();
        if (minSuf != "") minorNames[i] += "-" + minSuf;
    }
}

/*end satic */

TkeySignature::TkeySignature()
{
    m_key = 0;
}

TkeySignature::TkeySignature(char keyS)
{
    if (keyS > -8 && keyS <8)
        m_key = keyS;
    else
        m_key = 0;
}

QString TkeySignature::accidNumber(bool inHtml) {
    QString a = "";
    if (m_key < 0) a = "b";
    if (m_key > 0) a = "#";
    QString S = QString("%1").arg(int(qAbs(m_key)));
    if (inHtml)
        S += "<sub><i>";
    S += a;
    if (inHtml)
        S += "</i></sub>";
    return S;
}

QDataStream &operator << (QDataStream &out, TkeySignature &key) {
    out << qint8(key.value());
    return out;
}

QDataStream &operator >> (QDataStream &in, TkeySignature &key) {
//    qint8 ky;
//    in >> ky;
//    key = TkeySignature(char(ky));
    return in;

}
