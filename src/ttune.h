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
#ifndef TTUNE_H
#define TTUNE_H

#include "tnote.h"
#include <QString>
#include <QMetaType>

/** It stores six notes in Tnote objects, which can be set
 * in constructor, or get by overloaded [] operator.
 * @short This class describes a tune of the guitar
 * @author Tomasz Bojczuk <tomaszbojczuk@gmail.com> */
class Ttune{
public:
        /** The constructor.
        * @p _name is the name,
        * @p S(1-6) are Tnote notes */

    Ttune(const QString _name = "", Tnote S1 = Tnote(0,0,0) , Tnote S2 = Tnote(0,0,0),
                    Tnote S3 = Tnote(0,0,0), Tnote S4 = Tnote(0,0,0), Tnote S5 = Tnote(0,0,0), Tnote S6 = Tnote(0,0,0));
    ~Ttune();
                /** It is a name of the tune*/
    QString name;
            /** Overloaded operator [] allows to use statment
            * @li Ttune your_variable[number of a string]
            * @p stringNr is real string number (1 to 6) */
    static const Ttune stdTune;
    static const Ttune tunes[4];
        /** This methods return highest and lowest note in the tune.
        * It is nessesery for tunes without string order f.e "Neshvile". */

//    const Tnote &operator[] (unsigned char stringNr) const { return m_S[stringNr-1]; }
    Tnote &operator[] (unsigned char stringNr) { return m_S[stringNr-1]; }
    bool operator==(Ttune T2) {
        return ( m_S[0]==T2[1] && m_S[1]==T2[2] && m_S[2]==T2[3] &&
                 m_S[3]==T2[4] && m_S[4]==T2[5] && m_S[5]==T2[6] );
    }
    bool operator!=(Ttune T2) {
        return ( m_S[0]!=T2[1] || m_S[1]!=T2[2] || m_S[2]!=T2[3] ||
                m_S[3]!=T2[4] || m_S[4]!=T2[5] || m_S[5]!=T2[6] );
    }

private:
            /** Array of Tnotes that represents six strings */
        Tnote m_S[6];
};
Q_DECLARE_METATYPE(Ttune)


QDataStream &operator<< (QDataStream &out, const Ttune &t);
QDataStream &operator<< (QDataStream &out, Ttune &t);
QDataStream &operator>> (QDataStream &in, Ttune &t);

#endif
