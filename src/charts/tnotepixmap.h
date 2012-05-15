/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#ifndef TNOTEPIXMAP_H
#define TNOTEPIXMAP_H

#include <QPixmap>
#include <tnote.h>
#include <tkeysignature.h>

/**  returns QPixmap with given note. 
 * @param factor determines its size
 */
QPixmap getNotePixmap(Tnote note, bool clef = true, 
                      TkeySignature key = TkeySignature(0), double factor = 4);



/** This class creates pixmap with given note i ngiven key and so on.
 * Default height is 100 px. Width is calculated by contents (clef, key, accidentals) 
 */
class TnotePixmap : public QPixmap
{

public:
    TnotePixmap(Tnote note, bool clef = true, int height = 100, int width = 150, TkeySignature key = TkeySignature(0));
    virtual ~TnotePixmap();

    static TnotePixmap pix(Tnote note, bool clef = true, TkeySignature key = TkeySignature(0));
        /** Returns true if note has accidental or neutral in given key signature. */
    static bool hasAccid(Tnote note, TkeySignature key = TkeySignature(0));
//         /** Returns html formated QString <img src... */
//     static QString getPixHtmlString(Tnote note, bool clef = true, TkeySignature key = TkeySignature(0));

protected:
//    static int hiLinePos;
};

#endif // TNOTEPIXMAP_H
