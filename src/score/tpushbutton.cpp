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

#include "tpushbutton.h"

/*static*/
QColor TpushButton::m_backColor = Qt::black;
QColor TpushButton::m_textColor = Qt::white;
void TpushButton::setCheckColor(QColor background, QColor text) {
	m_backColor = background;
	m_textColor = text;
}



TpushButton::TpushButton(QString text, QWidget* parent):
	 QPushButton(text, parent),
	 m_Ichecked(false)
{
// 	connect(this, SIGNAL(clicked()), this, SLOT(wasClicked()));
}

void TpushButton::setChecked(bool isChecked) {
	if (isChecked)
		setStyleSheet(QString("background-color: %1; color: %2; ")
			.arg(m_backColor.name()).arg(m_textColor.name()));
	else
		setStyleSheet("background-color: native; color: native");
	m_Ichecked = isChecked;
}

void TpushButton::wasClicked() {
// 	setChecked(!m_Ichecked);
}

