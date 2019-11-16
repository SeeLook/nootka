/***************************************************************************
 *   Copyright (C) 2018-2019 by Tomasz Bojczuk                             *
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

#ifndef TTECHNICAL_H
#define TTECHNICAL_H


#include "nootkacoreglobal.h"
#include "tfingerpos.h"
#include <QtCore/qobject.h>


#define ON_UPPER (1) // first bit
#define BOW_DIRECTION (6) // 2nd and 3rd bits
#define FINGERING (56) // 4th to 6th bits

#define NO_TECHNICALS (255) // 255 means that there is no any technical information


/**
 * @p Ttechnical extends information about note in the score.
 * It has guitar position in @p TfingerPos
 * - bow direction (down/up) - for bandoneon it is bellow state (opening/closing)
 * - note position on the grand staff (on upper staff of or on the lower one)
 * - finger number [0 - 5], -1 - is undefined
 *
 * All parameters can be represented by single @p quint32 value
 * available through @p data() and set through @p setData()
 *
 * In general it corresponds with <technical> tag of Music XML format
 */
class NOOTKACORE_EXPORT Ttechnical
{

  Q_GADGET

public:
  Ttechnical() {}
  Ttechnical(quint32 fromData);

      /**
       * Returns @p TRUE when all extra note parameters are unset
       */
  bool isEmpty() const { return !m_fingerPos.isValid() && m_otherData == 0; }

      /**
       * Resets all extra note parameters to null
       */
  void reset() { m_fingerPos.setData(NO_TECHNICALS); m_otherData = 0; }

  TfingerPos& fingerPos() { return m_fingerPos; }
  void setFingerPos(TfingerPos fp) { m_fingerPos.setData(fp.data()); }

  bool onUpperStaff() const { return !(m_otherData & ON_UPPER); }
  void setOnUpperStaff(bool onUpper);

  enum EbowDirection {
    BowUndefined = 0,
    BowDown = 2, /**< For bandoneon it is bellow opening */
    BowUp = 4 /**< For bandoneon it is bellow closing */
  };
  Q_ENUM(EbowDirection)

  EbowDirection bowing() const { return static_cast<EbowDirection>(m_otherData & BOW_DIRECTION); }
  void setBowing(EbowDirection b);

      /**
       * Finger number [0 - 5].
       * -1 is returned when undefined
       */
  int finger() const { return ((m_otherData & FINGERING) >> 3) - 1; }
  void setFinger(int fingerNr);

  quint32 data() const;
  void setData(quint32 d);

  void toXml (QXmlStreamWriter& xml, const QString& tag = QLatin1String("technical")) const;
  void fromXml(QXmlStreamReader& xml);

private:
  TfingerPos              m_fingerPos;
  quint16                 m_otherData = 0;
};

#endif // TTECHNICAL_H
