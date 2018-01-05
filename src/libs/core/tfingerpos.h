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


#ifndef TFINGERPOS_H
#define TFINGERPOS_H


#include <nootkacoreglobal.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qxmlstream.h>


/**
 * A class describing a finger position on a guitar fingerboard.
 * It supports up to six strings and up to 39 frets.
 * Default constructor @p TfingerPos() creates invalid position.
 * It can be detected with @p isValid() method
*/
class NOOTKACORE_EXPORT TfingerPos
{
public:
  TfingerPos() { m_pos = 255; }
  TfingerPos(unsigned char realStr, unsigned char fret) {
      setPos(realStr, fret);
  }

      /**
       * Constructor making @p TfingerPos instance from given integer value
       */
  TfingerPos(quint8 positionData) { setData(positionData); }

      /**
       * Returns integer value represents fret string number as @p TfingerPos stores that
       */
  quint8 data() const { return m_pos; }
  void setData(quint8 d) { m_pos = d; }

      /**
       * Returns string number (real [1-6])
       */
  quint8 str() const { return (m_pos / 40) + 1; }

      /**
       * Returns fret number [0-40]
       */
  quint8 fret() const { return m_pos % 40; }

      /**
       * Returns @p True when position is valid
       */
  bool isValid() const { return m_pos < 241; }

  void setPos(unsigned char realStr, unsigned char fret) {
      m_pos = (realStr - 1) * 40 + fret;
  }

      /** [STATIC]
       * List of QString with roman representation of numbers 0-24.
       */
  static const QString fretsList[25];

      /** [STATIC]
       * Returns string with roman number of given value. Range [0-24] is supported
       */
  static QString romanFret(quint8 fret);

      /**
       * Returns string with roman number. Range [0-24] is supported
       */
  QString romanFret() { return romanFret(fret()); }

      /**
       * TfingerPos in HTML format as a string fe.: 3 XVII
       */
  QString toHtml() const;

  bool operator==( TfingerPos f2) const { return m_pos == f2.m_pos; }
  bool operator!=( TfingerPos f2) const { return m_pos != f2.m_pos; }

  friend QDataStream &operator<< (QDataStream &out, const TfingerPos &fPos) {
      out << fPos.m_pos;
      return out;
  }

  friend QDataStream &operator>> (QDataStream &in, TfingerPos &fPos) {
      in >> fPos.m_pos;
      return in;
  }

      /**
       * Writes XML structure with <string> and <fret> wrapped into <tag> (if not empty) - by default it is <technical>
       */
  void toXml (QXmlStreamWriter& xml, const QString& tag = QLatin1String("technical")) const;
  void fromXml(QXmlStreamReader& xml);

  protected:
  quint8 m_pos;
};



#endif // TFINGERPOS_H
