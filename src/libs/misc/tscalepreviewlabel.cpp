/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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

#include "tscalepreviewlabel.h"
#include <music/tnamestylefilter.h>


TscalePreviewLabel::TscalePreviewLabel(Tnote::EnameStyle style, bool withAccids, QWidget* parent) :
	TroundedLabel(parent),
	m_withAccids(withAccids)
{
	setAlignment(Qt::AlignCenter);
	changeStyle(style);
}

void TscalePreviewLabel::changeStyle(Tnote::EnameStyle style) {
	// Tnote::toRichText() method returns only names in user preferred according to settings
	// To cheat it and force note name in any given style we are resetting pointer of is7th_B 
	// then Tnote skips filtering of a style during name generation.
		bool* tmpPtr = TnameStyleFilter::is7th_B();
		TnameStyleFilter::setStyleFilter(0, TnameStyleFilter::solfegeStyle());
		QString c_majorScale = "";
		QString accidsLine = "";
		for (int i = 1; i < 8; i++) {
			Tnote tmpNote(i, 0, 0);
			c_majorScale += tmpNote.toRichText(style) + " &nbsp;&nbsp;";
		}
		if (m_withAccids) {
				accidsLine = "<br>";
				Tnote tmpNote(1, 0, 1); // Ces
				accidsLine += tmpNote.toRichText(style) + "&nbsp;&nbsp;&nbsp;";
				tmpNote = Tnote(2, 0, -1); // Des
				accidsLine += tmpNote.toRichText(style) + "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;";
				tmpNote = Tnote(6, 0, -1); // Ais
				accidsLine += tmpNote.toRichText(style) + "&nbsp;&nbsp;&nbsp;";
				tmpNote = Tnote(7, 0, -1); // Bb or Hb
				accidsLine += tmpNote.toRichText(style);
		}
		setText(QString("<p style=\"font-size: x-large;\">") + c_majorScale + accidsLine + "</p>");
		TnameStyleFilter::setStyleFilter(tmpPtr, TnameStyleFilter::solfegeStyle()); // restore is7th_B settings
}




