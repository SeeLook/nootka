/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#ifndef TNOTEPIXMAP_H
#define TNOTEPIXMAP_H

#include <QPixmap>
#include <tnote.h>
#include <tkeysignature.h>
#include <tclef.h>

class TQAunit;


			/**  Returns QPixmap with given note. 
			* @param factor determines its size	*/
QPixmap getNotePixmap(Tnote note, Tclef::Etype clef = Tclef::e_treble_G, 
                      TkeySignature key = TkeySignature(0), qreal factor = 4.0);


/** Returns HTML formated text with answer details. 
 * If @param fontSize remains default - default fontSize is taken. */
QString wasAnswerOKtext(TQAunit *answer, QColor textColor, int fontSize = -1);





#endif // TNOTEPIXMAP_H
