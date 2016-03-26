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

#ifndef TINSTRUMENT_H
#define TINSTRUMENT_H

#include <nootkacoreglobal.h>
#include <QString>



  /** Some instruments used in Nootka */
enum Einstrument {
  e_noInstrument = 0, // 0, however level and exam save it as 255 for backward comparability
  e_classicalGuitar = 1,
  e_electricGuitar = 2,
  e_bassGuitar = 3
};


    /** Converts Einstrument enum to translated text. */
NOOTKACORE_EXPORT QString instrumentToText(Einstrument instr);

    /** Converts Einstrument enum to instrument symbol (singer for e_noInstrument). */
NOOTKACORE_EXPORT QString instrumentToGlyph(Einstrument instr);


#endif //TINSTRUMENT_H

