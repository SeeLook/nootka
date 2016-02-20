/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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

/** Glyphs (some)
 * Rests:
 * 0xe102 - whole rest
 * 0xe103 - half rest
 * 0xe108 - quarter rest
 * 0xe10A - eight rest
 * 0xe10B - sixteen rest
 * Flags
 * 0xe21C - eight Up
 * 0xe21D - sixteen Up
 * 0xe221 - eight Down
 * 0xe221 - sixteen Down
 *
 * digits [0-9] starts from 0x0180
 */

#include "tnoofont.h"

TnooFont::TnooFont(int pointSize) :
	QFont("nootka", pointSize)
{
	setPixelSize(pointSize);
  setBold(false);
  setWeight(50); // Normal
}


QString TnooFont::tag(const QString& tag, const QString& text, int fontSize, const QString& extraStyle) {
	QString fSize;
	if (fontSize)
			fSize = QString("font-size: %1px;").arg(fontSize);
	QString ex = extraStyle;
	if (!extraStyle.isEmpty() && !extraStyle.endsWith(";"))
			ex = extraStyle + QLatin1String(";");
	return QLatin1String("<") + tag + QLatin1String(" style=\"font-family: nootka;") + fSize + ex + QLatin1String("\">")
          + text + QLatin1String("</") + tag + QLatin1String(">");
}





