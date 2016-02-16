/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TRANDMELODY_H
#define TRANDMELODY_H

#include <exam/tqagroup.h>

class TkeySignature;
class Tmelody;

/** 
 * Generates randomized melody into given reference of @p Tmelody.
 * Length is determined by @p len.
 * Notes are taken form given question list 
 * and key signature is respected if @inKey is set to @p true
 * Melody is finished on tonic note of the given key signature
 * when @p onTonic is set to @p true
 */
void getRandomMelody(QList<TQAgroup>& qList, Tmelody* mel, int len, bool inKey, bool onTonic);


#endif // TRANDMELODY_H