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
}

QDataStream TQAtype::operator << (QDataStream &out) {
    out << isNote() << isName() << isFret() << isSound();
    return out;
}

QDataStream TQAtype::operator >>(QDataStream &in) {
    in >> m_typeArr[0] >> m_typeArr[1] >> m_typeArr[2] >> m_typeArr[3];
    return in;
}
