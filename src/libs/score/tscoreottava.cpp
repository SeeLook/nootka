/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include "tscoreottava.h"
#include "tscorestaff.h"

TscoreOttava::TscoreOttava(TscoreStaff* staff, int beginNote, int endNote) :
	TscoreItem(0)
{
	setStaff(staff);
	setParentItem(staff);
}


TscoreOttava::~TscoreOttava() {}



QRectF TscoreOttava::boundingRect() const {

}

void TscoreOttava::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

}


