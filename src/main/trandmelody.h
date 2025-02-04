/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
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

void getRandomMelodyNG(QList<TQAgroup> &qList, Tmelody *mel, int len, bool inKey, bool onTonic, int maxStep = 0);

TrhythmList getRandomRhythm(int meter, int barCount, quint32 basicMask, quint32 dotsMask, int rtmDiversity);

/**
 * Returns @p TRUE when merge was correct
 */
bool mergeRhythmAndMelody(const TrhythmList &rList, Tmelody *melody);

#endif // TRANDMELODY_H
