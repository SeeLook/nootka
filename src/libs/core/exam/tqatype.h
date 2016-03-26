/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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


#ifndef TQATYPE_H
#define TQATYPE_H

#include <nootkacoreglobal.h>
#include <QDataStream>

class QXmlStreamReader;
class QXmlStreamWriter;


/**
* Question-Answer type. It keeps array of four bool elements
* witch say is appropriate type enabled or not.
* Also it randomize possible type by calling @param randNext(),
* or returns next possible type by calling next().
*/
class NOOTKACORE_EXPORT TQAtype
{

public:
    TQAtype();
    TQAtype(bool _asNote, bool _asName, bool _asFretPos, bool _asSound);

    enum Etype { e_asNote = 0, e_asName = 1, e_asFretPos = 2, e_asSound = 3};

    void setAsNote(bool isNote) {m_typeArr[0] = isNote; }
    void setAsName(bool isName) {m_typeArr[1] = isName; }
    void setAsFret(bool isFret) {m_typeArr[2] = isFret; }
    void setAsSound(bool isSound) {m_typeArr[3] = isSound; }
    bool isNote() { return m_typeArr[0]; }
    bool isName() { return m_typeArr[1]; }
    bool isFret() { return m_typeArr[2]; }
    bool isSound() { return m_typeArr[3]; }

    Etype next();
    Etype randNext();

        /** Adds 'qaType' key to given XML stream with types as bool attributes. For instance:
         * <qaType id="-1" score="true" name="true" guitar="true" sound="false"/>
         * @p id is to intricate qaType keys in file. */
    void toXml(int id, QXmlStreamWriter& xml);

    int fromXml(QXmlStreamReader& xml);

private:
    bool m_typeArr[4];
    char m_index;

};

NOOTKACORE_EXPORT QDataStream &operator << (QDataStream &out, TQAtype &qatype);
NOOTKACORE_EXPORT QDataStream &operator >> (QDataStream &in, TQAtype &qatype);


#endif // TQATYPE_H
