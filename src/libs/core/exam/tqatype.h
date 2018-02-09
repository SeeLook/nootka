/***************************************************************************
 *   Copyright (C) 2011-2018 by Tomasz Bojczuk                             *
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
#include <QtCore/qdatastream.h>


class QXmlStreamReader;
class QXmlStreamWriter;


/**
 * Question or Answer type witch says is appropriate type enabled or not.
 *
 * Also it randomize possible type by calling @p randNext(),
 * or returns next possible type by calling @p next().
 *
 * Questions/answers state are stored into single byte value
 */
class NOOTKACORE_EXPORT TQAtype
{

public:
  TQAtype() {}
  TQAtype(bool _onScore, bool _asName, bool _asFretPos, bool _asSound);
  TQAtype(int val);

  enum Etype : qint8 { e_onScore = 0, e_asName = 1, e_onInstr = 2, e_asSound = 3 };

      // Those strange numbers are bit reverse values to unset one
  void setOnScore(bool onScore) { m_value = onScore ? m_value | 1 : m_value & 14; }
  void setAsName(bool isName) { m_value = isName ? m_value | 2 : m_value & 13; }
  void setOnInstr(bool onInstr) { m_value = onInstr ? m_value | 4 : m_value & 11; }
  void setAsSound(bool isSound) { m_value = isSound ? m_value | 8 : m_value & 7; }

  bool isOnScore() const { return m_value & 1; }
  bool isName() const { return m_value & 2; }
  bool isOnInstr() const { return m_value & 4; }
  bool isSound() const { return m_value & 8; }

      /**
       * Number that represents bits combination:
       * @li as note : 1, @li as name : 2, @li on instrument : 4, @li as sound : 8
       */
  int value() const { return static_cast<int>(m_value); }
  void setValue(int v) { m_value = static_cast<quint8>(v); }

  Etype next();
  Etype randNext();

      /**
       * Adds 'qaType' key to given XML stream with types as bool attributes. For instance:
       * <qaType id="-1" score="true" name="true" guitar="true" sound="false"/>
       * @p id is to intricate qaType keys in file.
       */
  void toXml(int id, QXmlStreamWriter& xml);

  int fromXml(QXmlStreamReader& xml);

private:
  quint8 m_value = 0;
  qint8 m_index = 0;

};

NOOTKACORE_EXPORT QDataStream &operator << (QDataStream &out, TQAtype &qatype);
NOOTKACORE_EXPORT QDataStream &operator >> (QDataStream &in, TQAtype &qatype);


#endif // TQATYPE_H
