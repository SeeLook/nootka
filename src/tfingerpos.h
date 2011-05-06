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

    unsigned char str() { return (m_pos / 40) +1; }
    unsigned char fret() { return m_pos % 40; }

    void setPos(unsigned char realStr, unsigned char fret) {
        m_pos = (realStr-1)*40 + fret;
    }

private:
    unsigned char m_pos;
};

#endif // TFINGERPOS_H
