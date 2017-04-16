/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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

#ifndef TINSTRUMENT_H
#define TINSTRUMENT_H


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>


/**
 * Describes instrument
 */
class NOOTKACORE_EXPORT Tinstrument {

  Q_GADGET

  Q_PROPERTY(Etype type READ type WRITE setType)
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(int typeINT READ typeINT)

public:

  enum Etype {
    NoInstrument = 0, // 0, however level and exam save it as 255 for backward comparability
    ClassicalGuitar = 1,
    ElectricGuitar = 2,
    BassGuitar = 3,
    Piano = 4
  };
  Q_ENUM(Etype)

  Tinstrument(Etype type = NoInstrument);

  Etype type() const { return m_type; }
  Q_INVOKABLE void setType(Etype t) { m_type = t; }
  int typeINT() const { return static_cast<int>(m_type); }

    /**
     * Translated name of an instrument.
     */
  QString name();

    /**
     * letter of instrument symbol (singer for NoInstrument).
     */
  QString glyph();

  QString static staticName(Etype t);

private:
  Etype             m_type;

};


#endif //TINSTRUMENT_H

