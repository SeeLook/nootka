/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#ifndef TSELECTINSTRUMENT_H
#define TSELECTINSTRUMENT_H

#include <QWidget>

class QGridLayout;
class QLabel;
class QPushButton;
class QRadioButton;

#define BUTTON_COUNT (4)

class TselectInstrument : public QWidget
{
    Q_OBJECT

public:
			/** Constructor takes kind of layout as a parameter. */
	explicit TselectInstrument(Elayout buttonLayout = e_buttonAndTextV, QWidget* parent = 0);
	
			/** Returns currently selected instrument */
	int instrument() { return m_instr; }
	
			/** The enumerator defining layout of selecting buttons:
			 * ------ e_textAndButtonV - vertical layout with description on the right
			 *	   |  g  |    classical guitar
			 *     |  e  |    electric guitar
			 * --------------------------------------------------
			 * ------ e_textUnderButton - horizontal layout with description under the button
			 * ----------|  g  | --------------- |  e  |              
			 * ---------classical --------------electric
			 * -----------guitar------------------guitar
			 * --------------------------------------------------
			 * ------ e_buttonsOnlyGrid - 2x2 grid with buttons only and texts in status tips
			 *    |  g  |     |  e  |    
			 *    |  b  |     |  n  |
			 * --------------------------------------------------
			 */
	enum Elayout {
		e_buttonAndTextV = 0,
		e_textUnderButtonH,
		e_buttonsOnlyGrid
	};
	
	void setLay(Elayout l);
	void setGlyphSize(int siz); /** Sets the size of instrument glyph on the button. */
	
	
	signals:
		void instrumentChanged(int);
		
private slots:
		void buttonPressed();


private:
	Elayout					m_currentLay;
	int							m_instr, m_glypshSize;
	QPushButton 		*m_buttons[BUTTON_COUNT];
	QLabel					*m_labels[BUTTON_COUNT];
	QGridLayout			*m_gridLay;
	
};

#endif // TSELECTINSTRUMENT_H
