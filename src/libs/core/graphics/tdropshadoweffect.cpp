/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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


#include "tdropshadoweffect.h"
// #include <tcolor.h>
#include <QApplication>
#include <QPalette>

TdropShadowEffect::TdropShadowEffect(QColor color)
{
	setBlurRadius(10);
	setOffset(4, 4);
// 		setColor(qApp->palette().text().color());
	if (color == -1)
		setColor(qApp->palette().text().color());
// 				setColor(Tcolor::shadow);
	else
		setColor(color);
}

