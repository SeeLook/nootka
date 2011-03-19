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
#include <QObject>

/*static*/
const Ttune Ttune::stdTune = Ttune(QObject::tr("Standard: E A D G B E "), Tnote(3,1,0), Tnote(7,0,0),
                                          Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(3,-1,0));

const Ttune Ttune::tunes[4] = { {Ttune(QObject::tr("Dropped D: D A D G B E "), Tnote(3,1,0), Tnote(7,0,0), Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(2,-1,0)) },
      {Ttune(QObject::tr("Fake Lute: D A D F# B E "), Tnote(3,1,0), Tnote(7,0,0), Tnote(4,0,1), Tnote(2,0,0), Tnote(6,-1,0),Tnote(2,-1,0)) },
      {Ttune(QObject::tr("Open: D A D G A D "), Tnote(2,1,0), Tnote(6,0,0), Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(2,-1,0)) },
      {Ttune(QObject::tr("Kouyanbaba: D A D A D F "), Tnote(4,1,0), Tnote(2,1,0), Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(2,-1,0)) }
};

Ttune::Ttune( const QString _name, Tnote S1, Tnote S2, Tnote S3, Tnote S4, Tnote S5, Tnote S6 )
{
        name = _name;
        m_S[0] = S1;
        m_S[1] = S2;
        m_S[2] = S3;
        m_S[3] = S4;
        m_S[4] = S5;
        m_S[5] = S6;
}


Ttune::~Ttune()
{
}

