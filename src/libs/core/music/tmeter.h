/***************************************************************************
 *   Copyright (C) 2014-2019 by Tomasz Bojczuk                             *
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

  Q_INVOKABLE Tmeter::Emeter meter() const { return m_meter; }
  Q_INVOKABLE void setMeter(Tmeter::Emeter m) { m_meter = m; }

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
  Q_INVOKABLE int duration() const;

      /**
       * returns number of up to counts depending on meter
       */
  Q_INVOKABLE int countTo() const;

  void toXml(QXmlStreamWriter& xml) const;
  bool fromXml(QXmlStreamReader& xml);

      /**
       * Prints current meter to std out with given text
       */
  Q_INVOKABLE void debug(const QString& text = QString());

  bool operator==(const Tmeter& m) const { return meter() == m.meter(); }
  bool operator!=(const Tmeter& m) const { return meter() != m.meter(); }

      /**
       * Converts given @p up upper numerator and @p lo lower denominator
       * into @p Tmeter::Emeter enumerator
       */
  static Emeter valueToMeter(int up, int lo);

  void fillMeterGroups(QList<quint8>& durationList);

  enum EbeatUnit {
    BeatQuarter = 0,
    BeatEighth = 1,
    BeatQuarterDot = 2,
    BeatHalf = 3
  };

  Q_ENUM(EbeatUnit)

  static EbeatUnit optimalBeat(Emeter m);

      /**
       * Optimal beat unit for current meter
       */
  Q_INVOKABLE Tmeter::EbeatUnit optimalBeat() const { return optimalBeat(m_meter); }

      /**
       * Returns tempo multiplexer depending on beat unit:
       * i.e.: for eighth note it is @p 2 as such as tempo with eighths is 2 times faster than tempo with quarters
       */
  Q_INVOKABLE static qreal beatTempoFactor(Tmeter::EbeatUnit bu);

      /**
       * Tempo of quarter notes (per minute), independent on beat unit
       */
  static int quarterTempo(int beatTempo, Tmeter::EbeatUnit beatUnit) {
    return qRound(static_cast<qreal>(beatTempo) / beatTempoFactor(beatUnit));
  }

      /**
       * Tempo of quarter notes (per minute), independent on beat unit
       */
  static int quarterTempo(int beatTempo, int beatUnit) {
    return Tmeter::quarterTempo(beatTempo, static_cast<EbeatUnit>(beatUnit));
  }

private:
  Emeter        m_meter;
};

#endif // TMETER_H
