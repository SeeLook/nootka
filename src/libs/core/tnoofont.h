/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TNOOFONT_H
#define TNOOFONT_H

#include <nootkacoreglobal.h>
#include <QFont>
#include <QString>

/**
 * This is QFont with nootka.ttf initialized with size of 20 pixels
 * It has many helpers to quickly generate HTML spans with nootka as font and so
 */
class NOOTKACORE_EXPORT TnooFont : public QFont
{

public:
	TnooFont(int pointSize = 20);
	
			/** Returns given text wrapped with given HTML tag and:
			* - font size (if set)
			* - extra Style (if set)
			* like 
			* <tag style="font-family: nootka; extraStyle; font-size: XXpx;">text</tag>
			*/
	static QString tag(const QString& tag, const QString& text, int fontSize = 0, const QString& extraStyle = "");
	
			/** tag() method with span tag */
	static QString span(const QString& text, int fontSize = 0, const QString& extraStyle = "") { 
									return	tag("span", text, fontSize, extraStyle); }
									
	
			/** Overloaded method with current font size */
// 	QString span(const QString& text, const QString& extraStyle = "") { return span(text, pointSize(), extraStyle); }
	
	
	

};

#endif // TNOOFONT_H
