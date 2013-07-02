/***************************************************************************
 *   Copyright (C) 2006-2013 by Tomasz Bojczuk  				                   *
 *   tomaszbojczuk@gmail.com   						                                 *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "ttune.h"


/*static*/
Ttune Ttune::stdTune = Ttune("", Tnote(3,1,0), Tnote(7,0,0),
                                          Tnote(5,0,0), Tnote(2,0,0), Tnote(6,-1,0),Tnote(3,-1,0));

Ttune Ttune::tunes[4];
Ttune Ttune::bassTunes[];

void Ttune::prepareDefinedTunes() {
	stdTune = Ttune(QObject::tr("Standard: E A D G B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
					Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(3, -1, 0));
	
	tunes[0] = Ttune(QObject::tr("Dropped D: D A D G B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
									 Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0));
	tunes[1] = Ttune(QObject::tr("Dummy Lute: D A D F# B E "), Tnote(3, 1, 0), Tnote(7, 0, 0),
									 Tnote(4, 0, 1), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0));
	tunes[2] = Ttune(QObject::tr("Open: D A D G A D "), Tnote(2, 1, 0), Tnote(6, 0, 0), 
									 Tnote(5, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0));
	tunes[3] = Ttune(QObject::tr("Kouyanbaba: D A D A D F "), Tnote(4, 1, 0), Tnote(2, 1, 0), 
									 Tnote(6, 0, 0), Tnote(2, 0, 0), Tnote(6, -1, 0),Tnote(2, -1, 0));
	
	bassTunes[0] = Ttune(QObject::tr("Standard bass: E A D G"), Tnote(5, -1, 0), Tnote(2, -1, 0),
												Tnote(6, -2, 0), Tnote(3, -2, 0), Tnote(0, 0, 0), Tnote(0, 0, 0));
	bassTunes[0] = Ttune(QObject::tr("Bass in fifths: C G D A"), Tnote(6, -1, 0), Tnote(2, -1, 0),
												Tnote(5, -2, 0), Tnote(1, -2, 0), Tnote(0, 0, 0), Tnote(0, 0, 0));
	bassTunes[1] = Ttune(QObject::tr("5-str. bass: B E A D G"), Tnote(5, -1, 0), Tnote(2, -1, 0),
												Tnote(6, -2, 0), Tnote(3, -2, 0), Tnote(7, -3, 0), Tnote(0, 0, 0));
	bassTunes[2] = Ttune(QObject::tr("6-str. bass: B E A D G C"), Tnote(1, 0, 0), Tnote(5, -1, 0), Tnote(2, -1, 0),
												Tnote(6, -2, 0), Tnote(3, -2, 0), Tnote(7, -3, 0));
}



Ttune::Ttune(const QString tuneName, Tnote S1, Tnote S2, Tnote S3, Tnote S4, Tnote S5, Tnote S6 ) :
	name(tuneName),
	m_strNumber(0)
{
		stringsArray[0] = S1;
		stringsArray[1] = S2;
		stringsArray[2] = S3;
		stringsArray[3] = S4;
		stringsArray[4] = S5;
		stringsArray[5] = S6;
		
		determineStringsNumber();
}


void Ttune::determineStringsNumber() {
		Tnote tmpStrings[6];
		quint8 strCnt = 0;
		for (int i = 0; i < 6; i++) {
			if (stringsArray[i].note != 0) { // put all defined notes to tmpStrings
				tmpStrings[strCnt] = stringsArray[i];
				strCnt++;
			}
		}
		if (strCnt < 6) // if not all were defined fill the rest of tmpStrings with empty notes
			for (int i = strCnt; i < 6; i++)
				tmpStrings[i] = Tnote(0, 0, 0);
		for (int i = 0; i < 6; i++) // copy all to main stringsArray
			stringsArray[i] = tmpStrings[i];
		m_strNumber = strCnt; // define number of strings
}


//##################################################################################################
//###################### I/O OPERATORS #############################################################
//##################################################################################################

QDataStream &operator<< (QDataStream &out, const Ttune &t) {
    out << t.name;
    for (int i=0; i < 6; i++)
      out << t.stringsArray[i];
    return out;
}

QDataStream &operator>> (QDataStream &in, Ttune &t) {
    in >> t.name;
    for (int i=0; i < 6; i++)
        in >> t.stringsArray[i];
		t.determineStringsNumber();
    return in;
}

