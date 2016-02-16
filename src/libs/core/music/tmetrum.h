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

#ifndef TMETRUM_H
#define TMETRUM_H

#include <nootkacoreglobal.h>

class NOOTKACORE_EXPORT Tmetrum
{

public:
	enum Emeter {
		e_none = 0,
		e_2_4 = 1,
		e_3_4 = 2,
		e_4_4 = 4,
		e_5_4 = 8,
		e_6_4 = 16
	};
	
	Tmetrum(Emeter meter = e_none) : m_meter(meter) {}
	
	Emeter meter() { return m_meter; }
	
private:
	Emeter				m_meter;
};

#endif // TMETRUM_H
