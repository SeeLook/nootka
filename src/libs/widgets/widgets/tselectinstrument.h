/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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


#ifndef TSELECTINSTRUMENT_H
#define TSELECTINSTRUMENT_H


#include "nootkacoreglobal.h"
#include <QWidget>

class TpushButton;
class QLabel;
class QVBoxLayout;
class QGridLayout;


#define BUTTON_COUNT (4)

/**
 * This is a widget to selecting among instruments available in Nootka
 * It has three kinds of layout (@see setButtonLayout() method)
 * Glyph size of instrument can be manipulated with @p setGlyphSize()
 * The widget can contain a header label manipulated with @p setHeadLabel()
 */
class NOOTKACORE_EXPORT TselectInstrument : public QWidget
{
    Q_OBJECT

public:
	
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
       * ------ e_buttonsOnlyHorizontal - horizontal layout without text under buttons
       *    | n  |  g  |  e  |  b  |
			 * -------------------------------------------------- */
	enum Elayout {
		e_buttonAndTextV = 0,
		e_textUnderButtonH,
		e_buttonsOnlyGrid,
		e_buttonsOnlyHorizontal
	};
	
	
			/** Constructor takes kind of layout as a parameter. */
	explicit TselectInstrument(QWidget* parent = 0, Elayout buttonLayout = e_buttonAndTextV);
	
			/** Returns currently selected instrument */
	int instrument() { return m_instr; }
	void setInstrument(int instr);
	
	void setButtonLayout(Elayout l);
	Elayout buttonLayout() { return m_buttonLay; }
	
	void setGlyphSize(int siz); /** Sets the size of instrument glyph on the button. */
	int glyphSize() { return m_glypshSize; }
	
			/** Adds a header label with given text above buttons. If text is "" - removes the label */
	void setHeadLabel(QString text);
	
	
	signals:
		void instrumentChanged(int);
		
private slots:
		void buttonPressed();


private:
	Elayout					m_buttonLay;
	int							m_instr, m_glypshSize;
	TpushButton 		*m_buttons[BUTTON_COUNT];
	QLabel					*m_labels[BUTTON_COUNT];
	QGridLayout			*m_gridLay;
	QVBoxLayout			*m_mainLay;
	QLabel					*m_header;
	
};

#endif // TSELECTINSTRUMENT_H
