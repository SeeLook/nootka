/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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

#include <nootkacoreglobal.h>
#include <QPixmap>
#include <music/tkeysignature.h>
#include <music/tclef.h>

class Tmelody;
class Tnote;


/**
 * This is static "container" for Tclef.
 * It is shared among TexamExecutor and charts to display current exam/exercise clef
 */
class NOOTKACORE_EXPORT TnotePixmap 
{
public:
	TnotePixmap() {}
	static void setDefaultClef(const Tclef& cl) { m_clef = cl; }
	static Tclef& clef() { return m_clef; }
	
private:
	static Tclef m_clef;
};

			/**  Returns QPixmap with given note. @p factor determines its size	*/
NOOTKACORE_EXPORT QPixmap getNotePixmap(const Tnote& note, Tclef::Etype clef = Tclef::e_treble_G, 
											TkeySignature key = TkeySignature(0), qreal factor = 4.0, int strNr = 0);

			/** Creates pixmap from a melody. Line are not breaking. */
NOOTKACORE_EXPORT QPixmap getMelodyPixmap(Tmelody* mel, bool showStrings = false, qreal factor = 4.0);

		/** Returns HTML <img> tag with given pixmap. */
NOOTKACORE_EXPORT QString pixToHtml(const QPixmap& pix);

		/** Returns HTML sting with note pixmap generated according to given params. */
NOOTKACORE_EXPORT QString wrapPixToHtml(const Tnote& note, Tclef::Etype clef, TkeySignature key, qreal factor = 4.0, int strNr = 0);

		/** Overloaded method - clef is taken from static  */
NOOTKACORE_EXPORT QString wrapPixToHtml(const Tnote& note, bool defClef, TkeySignature key, qreal factor = 4.0, int strNr = 0);

		/** Changes given sting to pixmap in given font size. 
     * Nootka font is used and current palette colors. */
NOOTKACORE_EXPORT QPixmap pixFromString(const QString& glif, const QFont& font, const QColor& color = -1);

    /** Returns HTML <img> statements with image scaled to given @p width.*/
NOOTKACORE_EXPORT QString pixToHtml(const QString& imageFile, int width = 0);



#endif // TNOTEPIXMAP_H
