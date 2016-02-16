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

#ifndef TSCLAEPREVIEWLABEL_H
#define TSCLAEPREVIEWLABEL_H

#include "nootkamiscglobal.h"
#include <music/tnote.h>
#include <widgets/troundedlabel.h>


/** 
 * This is a QLabel subclass that displays note names of C-major scale in given style.
 * If withAccids is set to true additional line with some notes with accidentals is displayed. 
 */
class NOOTKAMISC_EXPORT TscalePreviewLabel : public TroundedLabel
{

public:
		explicit TscalePreviewLabel(Tnote::EnameStyle style, bool withAccids = true, QWidget* parent = 0);
		
		void changeStyle(Tnote::EnameStyle style);
				/** Enables/disables line with accidentals. changeStyle() has to be called after to refresh. */
		void enableAccidsLabel(bool enable) { m_withAccids = enable; }
		
private:
		bool m_withAccids;


};

#endif // TSCLAEPREVIEWLABEL_H

