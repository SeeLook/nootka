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

#ifndef TCHUNK_H
#define TCHUNK_H

#include <nootkacoreglobal.h>
#include "tnote.h"
#include <tfingerpos.h>

class QXmlStreamReader;
class QXmlStreamWriter;


/**
 * This class represent a note:
 * a pitch and rhythm described by @p p() (@p Tnote)
 * and position on the fingerboard accessible through @p g() (@p TfingerPos)
 */
class NOOTKACORE_EXPORT Tchunk
{

public:
  Tchunk(const Tnote& pitch, const TfingerPos& fretPos = TfingerPos());

      /**
       * Default constructor - creates 'empty' note and position.
       */
  Tchunk() {}
  ~Tchunk();

      /**
       * The note
       */
  Tnote& p() { return m_pitch; }

  int duration() const { return m_pitch.duration(); }

      /**
       * Position a note on a guitar (if any) - by default it is null - invalid
       */
  TfingerPos& g() { return m_fretPos; }

      /** Returns @p TRUE when position on the guitar is valid. */
  bool validPos() { if (g().str() == 7) return false; else return true; }

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
  TfingerPos          m_fretPos;
};

#endif // TCHUNK_H
