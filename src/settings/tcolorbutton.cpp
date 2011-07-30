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

#include "tcolorbutton.h"
#include <QColorDialog>

TcolorButton::TcolorButton(QColor col, QWidget* parent): 
	QPushButton(col, parent)
{
	m_color = col;
	connect(this, SIGNAL(clicked()), this, SLOT(whenClicked()));
}

void TcolorButton::paint() {

}

void TcolorButton::whenClicked() {
	setColor(QColorDialog::getColor(m_color, this));
}

void TcolorButton::setColor(QColor col) {

}





