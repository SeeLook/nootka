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


#ifndef TFINGERPOS_H
#define TFINGERPOS_H

#include <QDataStream>


/**
*  @short A class describes finger's position on the fingerboard.
*  @author Tomasz Bojczuk <tomaszbojczuk@gmail.com>
*/

class TfingerPos
{
public:
    TfingerPos() { m_pos = 255; }
    TfingerPos(unsigned char realStr, unsigned char fret) {
        setPos(realStr, fret);
    }

    quint8 str() { return (m_pos / 40) +1; }
    quint8 fret() { return m_pos % 40; }

    void setPos(unsigned char realStr, unsigned char fret) {
        m_pos = (realStr-1)*40 + fret;
    }

//    bool operator==( TfingerPos f2) { return str() == f2.str() && fret() == f2.fret(); }
    bool operator==( TfingerPos f2) { return m_pos == f2.m_pos; }
//    bool operator!=( TfingerPos f2) { return str() != f2.str() || fret() != f2.fret(); }
    bool operator!=( TfingerPos f2) { return m_pos != f2.m_pos; }

    friend QDataStream &operator<< (QDataStream &out, const TfingerPos &fPos) {
        out << fPos.m_pos;
        return out;
    }

    friend QDataStream &operator>> (QDataStream &in, TfingerPos &fPos) {
        in >> fPos.m_pos;
        return in;
    }

protected:
    quint8 m_pos;
};



#endif // TFINGERPOS_H
