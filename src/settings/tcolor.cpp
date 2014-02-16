/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#include "tcolor.h"


Tcolor::Tcolor() : QColor() 
{}

Tcolor::Tcolor(Qt::GlobalColor color) : QColor(color) 
{}

Tcolor::Tcolor(int r, int g, int b, int a) : QColor(r, g, b, a) 
{}

Tcolor::Tcolor(QRgb rgb) : QColor(rgb) 
{}

Tcolor::Tcolor(QColor::Spec spec) : QColor(spec) 
{}

Tcolor::Tcolor(const QString& name) : QColor(name) 
{}

Tcolor::Tcolor(const char* name) : QColor(name) 
{}

Tcolor::Tcolor(const QColor& color) : QColor(color) 
{}

Tcolor::Tcolor(const QString& name, int a) : QColor(name) 
{ 
	setAlpha(a); 
}


Tcolor::Tcolor(const QColor& color, int a) : QColor(color) 
{
	setAlpha(a);
}


void Tcolor::merge(const QColor& otherColor) {
	qreal al = iV(alpha()) + iV(otherColor.alpha() * (1 - iV(alpha())));
	setRgba(qRgba(((iV(red()) * iV(alpha()) + iV(otherColor.red()) * iV(otherColor.alpha()) * (1 - iV(alpha()))) / al) * 255,
								((iV(green()) * iV(alpha()) + iV(otherColor.green()) * iV(otherColor.alpha()) * (1 - iV(alpha()))) / al) * 255,
								((iV(blue()) * iV(alpha()) + iV(otherColor.blue()) * iV(otherColor.alpha()) * (1 - iV(alpha()))) / al) * 255,
								qMin(255, (int)(255 * al))));
}
