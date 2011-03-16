/***************************************************************************
 *   Copyright (C) 2006 by Tomasz Bojczuk   *
 *   tomaszbojczuk@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "ttune.h"

Ttune::Ttune( const QString _name, Tnote S1, Tnote S2, Tnote S3, Tnote S4, Tnote S5, Tnote S6 )
{
        name = _name;
        m_S[0] = S1;
        m_S[1] = S2;
        m_S[2] = S3;
        m_S[4] = S4;
        m_S[5] = S6;
        m_S[6] = S6;
}


Ttune::~Ttune()
{
}

