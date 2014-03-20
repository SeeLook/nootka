/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com                                               *
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


#include "tqatype.h"

TQAtype::TQAtype()
{
    TQAtype(false, false, false, false);
}

TQAtype::TQAtype(bool _asNote, bool _asName, bool _asFretPos, bool _asSound)
{
    m_typeArr[0] = _asNote;
    m_typeArr[1] = _asName;
    m_typeArr[2] = _asFretPos;
    m_typeArr[3] = _asSound;
    m_index = 0;
}

TQAtype::Etype TQAtype::next() {
    do {
        m_index++;
        if (m_index == 4) m_index = 0;
    } while ( !m_typeArr[m_index] );
    return (Etype)m_index;
}

TQAtype::Etype TQAtype::randNext() {
    m_index = (qrand() % 4) - 1;
    return next();
}

QDataStream &operator << (QDataStream &out,TQAtype &qatype) {
    out << qatype.isNote() << qatype.isName() << qatype.isFret() << qatype.isSound();
    return out;
}

QDataStream &operator >> (QDataStream &in, TQAtype &qatype) {
    bool b[4];
    in >> b[0] >> b[1] >> b[2] >> b[3];
    qatype = TQAtype(b[0], b[1], b[2], b[3]);
    return in;
}
