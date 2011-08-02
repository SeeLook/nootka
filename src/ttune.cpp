/***************************************************************************
 *   Copyright (C) 2006-2011 by Tomasz Bojczuk   *
 *   tomaszbojczuk@gmail.com   *
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
#include "ttune.h"


/*static*/
Ttune Ttune::stdTune = Ttune("", Tnote(3,1,0), Tnote(7,0,0),
                                          Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(3,-1,0));

Ttune Ttune::tunes[4] = { {Ttune("", Tnote(3,1,0), Tnote(7,0,0), Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(2,-1,0)) },
      {Ttune("", Tnote(3,1,0), Tnote(7,0,0), Tnote(4,0,1), Tnote(2,0,0), Tnote(6,-1,0),Tnote(2,-1,0)) },
      {Ttune("", Tnote(2,1,0), Tnote(6,0,0), Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(2,-1,0)) },
      {Ttune("", Tnote(4,1,0), Tnote(2,1,0), Tnote(6,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(2,-1,0)) }
};

void Ttune::prepareDefinedTunes() {
    stdTune.name = QObject::tr("Standard: E A D G B E ");
    tunes[0].name = QObject::tr("Dropped D: D A D G B E ");
    tunes[1].name = QObject::tr("Dummy Lute: D A D F# B E ");
    tunes[2].name = QObject::tr("Open: D A D G A D ");
    tunes[3].name = QObject::tr("Kouyanbaba: D A D A D F ");
}



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

Ttune::~Ttune() {}

QDataStream &operator<< (QDataStream &out, const Ttune &t) {
    out << t.name;
    for (int i=0; i < 6; i++)
      out << t.m_S[i];
    return out;
}

QDataStream &operator>> (QDataStream &in, Ttune &t) {
    in >> t.name;
    for (int i=0; i < 6; i++)
        in >> t.m_S[i];
    return in;
}

