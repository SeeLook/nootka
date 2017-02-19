/***************************************************************************
 *   Copyright (C) 2014-2017 by Tomasz Bojczuk                             *
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

#ifndef TMETER_H
#define TMETER_H


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qxmlstream.h>


/**
 * Describes musical time signature
 */
class NOOTKACORE_EXPORT Tmeter
{

  Q_GADGET

public:

  enum Emeter {
    NoMeter = 0,
    Meter_2_4 = 1, Meter_3_4 = 2, Meter_4_4 = 4, Meter_5_4 = 8, Meter_6_4 = 16, Meter_7_4 = 32,
    Meter_3_8 = 64, Meter_5_8 = 128, Meter_6_8 = 256, Meter_7_8 = 512, Meter_9_8 = 1024, Meter_12_8 = 2048
  };

  Q_ENUM(Emeter)

  Tmeter(Emeter meter = NoMeter) : m_meter(meter) {}

  Q_INVOKABLE Emeter meter() const { return m_meter; }
  Q_INVOKABLE void setMeter(Emeter m) { m_meter = m; }

      /**
      * Returns upper digit of time signature
      */
  Q_INVOKABLE int upper() const;

      /**
      * Returns lower digit of time signature
      */
  Q_INVOKABLE int lower() const;

      /**
       * Returns text symbol of this time signature,
       * containing Scorek (or Bravura) font characters
       */
  Q_INVOKABLE QString symbol() const;

      /**
      * Returns numeric value representing duration of single measure,
      * which is based on Trhythm calculation (RVALUE)
      * 3/4 is 72, 4/4 is 96 (RVALUE), etc. 
      */
  Q_INVOKABLE quint16 duration() const;

  void toXml(QXmlStreamWriter& xml) const;
  bool fromXml(QXmlStreamReader& xml);

      /**
      * Prints current meter to std out with given text
      */
  Q_INVOKABLE void debug(const QString& text = QString());

  bool operator==(const Tmeter& m) const { return meter() == m.meter(); }
  bool operator!=(const Tmeter& m) const { return meter() != m.meter(); }

private:
  Emeter        m_meter;
};

#endif // TMETER_H
