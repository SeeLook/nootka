/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tqaunit.h"

TQAunit::TQAunit()
{
    qa.pos = TfingerPos();
    qa.note = Tnote(0,0,0);
    m_valid = 0; // correct a priori
}

void TQAunit::setMistake(Emistake mis) {
    switch (mis) {
    case e_correct : m_valid = 0; break;
    case e_wrongAccid : m_valid |= 1; break;
    case e_wrongKey : m_valid |= 2; break;
    case e_wrongOctave : m_valid |= 4; break;
    case e_wrongStyle : m_valid |= 8; break;
    case e_wrongPos : m_valid |= 16; break;
//    case e_wrong???? : m_valid |= 32; break;
    case e_wrongNote : m_valid |= 64; break;
    }
}
