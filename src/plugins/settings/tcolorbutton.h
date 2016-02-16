/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk  				                   *
 *   seelook@gmail.com        						                                 *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TCOLORBUTTON_H
#define TCOLORBUTTON_H

#include <QPushButton>


class TcolorButton : public QPushButton
{
	Q_OBJECT
	
public:
	
    explicit TcolorButton(QColor col = QColor(0, 0, 0, 0), QWidget* parent = 0);
	
	QColor getColor() { return m_color; }
	void setColor(QColor col);
	
	
protected:
	void paintEvent(QPaintEvent *event);
	
private:
	QColor m_color;
	
private slots:
	void whenClicked();
};

#endif // TCOLORBUTTON_H
