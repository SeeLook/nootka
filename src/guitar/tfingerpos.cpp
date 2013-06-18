/***************************************************************************
 *   Copyright (C) 2011 - 2012 by Tomasz Bojczuk                           *
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


#include "tfingerpos.h"

/* static */
const QString TfingerPos::fretsList[25] = { "0",
      "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X",
      "XI", "XII", "XIII", "XIV", "XV", "XVI", "XVII", "XVIII", "XIX", "XX",
      "XI", "XII", "XIII", "XIV"
    };
    
QString TfingerPos::romanFret(quint8 fret) {
    if (fret >= 0 && fret < 25)
      return fretsList[fret];
    else
      return QString("");
}

QString TfingerPos::toHtml() {
  //    return QString("<span style=\"font-size: 25px; font-family: nootka\">%1%2</span>").arg(pos.str()).arg(romanFret(pos.fret()));
    return QString("<span style=\"font-size: 25px; font-family: nootka\">%1</span><span style=\"font-size: 25px;\">%2</span>").arg(str()).arg(fret());
}


    
    
    
