/***************************************************************************
 *   Copyright (C) 2014-2018 by Tomasz Bojczuk                             *
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

#ifndef TCHUNK_H
#define TCHUNK_H


#include <nootkacoreglobal.h>
#include "tnote.h"
#include "ttechnical.h"


class QXmlStreamReader;
class QXmlStreamWriter;


/**
 * This class represent a note:
 * a pitch and rhythm described by @p p() (@p Tnote)
 * and technical data of a note like:
 * @li position on the fingerboard accessible through @p g() (@p TfingerPos)
 * @li or entire data through @p d()
 */
class NOOTKACORE_EXPORT Tchunk
{

public:
  Tchunk(const Tnote& pitch, const TfingerPos& fretPos = TfingerPos());

  Tchunk(const Tnote& pitch, const Ttechnical& technical);

      /**
       * Default constructor - creates 'empty' note and position.
       */
  Tchunk() {}
  ~Tchunk() {}

      /**
       * The note
       */
  Tnote& p() { return m_pitch; }

  int duration() const { return m_pitch.duration(); }

      /**
       * Position a note on a guitar (if any) - by default it is null - invalid
       */
  TfingerPos& g() { return m_technical.fingerPos(); }

      /**
       * Extra note data like guitar position, bow direction, staff position (upper/lower) and fingering
       */
  Ttechnical& t() { return m_technical; }

  bool onUpperStaff() const { return m_technical.onUpperStaff(); }
  void setOnUpperStaff(bool onUpper) { m_technical.setOnUpperStaff(onUpper); }

  Ttechnical::EbowDirection bowing() const { return m_technical.bowing(); }
  void setBowing(Ttechnical::EbowDirection b) { m_technical.setBowing(b); }

      /**
        * Finger number [0 - 5].
        * -1 is returned when undefined
        */
  int finger() const { return m_technical.finger(); }
  void setFinger(int fi) { m_technical.setFinger(fi); }

      /**
       * Numeric value representing all extra note parameters
       */
  quint32 technical() const { return m_technical.data(); }
  void setTechnical(quint32 nd) { m_technical.setData(nd); }

      /**
       * Returns @p TRUE when position on the guitar is valid.
       */
  bool validPos() { return g().str() != 7; }

      /**
       * If @p staffNr is set appropriate <staff>staffNr</staff> is added
       */
  void toXml(QXmlStreamWriter& xml, int* staffNr = nullptr);

      /**
       * Trough @p staffNr (if set) is returned staff number the note belongs to.
       */
  bool fromXml(QXmlStreamReader& xml, int* staffNr = nullptr);

      /**
       * Converts beam type into music XML compatible string
       */
  static QString beamToString(Trhythm::Ebeam b);

      /**
       * In music XML structure ties are written in two places:
       * below 'pitch' with <tie type />
       * and inside <notations/> with <tied type />
       * but we have single method to generate them both and this enumerator is to distinguish them
       */
  enum EtieTag { e_tie, e_tied };

      /**
       * @p t has to be kind of tie but not empty
       */
  void tieToXml(QXmlStreamWriter& xml, Trhythm::Etie t, EtieTag tag);

private:
  Tnote               m_pitch;
  Ttechnical          m_technical;
};

#endif // TCHUNK_H
