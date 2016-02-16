/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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


#include "tnoofont.h"

TnooFont::TnooFont(int pointSize) :
	QFont("nootka", pointSize)
{
	setPixelSize(pointSize);
  setBold(false);
  setWeight(50); // Normal
}


QString TnooFont::tag(const QString& tag, const QString& text, int fontSize, const QString& extraStyle) {
	QString fSize = "";
	if (fontSize)
			fSize = QString("font-size: %1px;").arg(fontSize);
	QString ex = extraStyle;
	if (!extraStyle.isEmpty() && !extraStyle.endsWith(";"))
			ex = extraStyle + ";";
	return "<" + tag + " style=\"font-family: nootka;" + fSize + ex + "\">" + text + "</" + tag + ">";
}





