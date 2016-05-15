/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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

#ifndef TRHYTHM_H
#define TRHYTHM_H

#include <QtCore/qstring.h>
#include <QtCore/qmath.h>


const std::string rhythmStrings [6] = {"", "whole", "half", "quarter", "eighth", "16th"};

    /** Base value to calculate proportional values of rhythm duration:
     * 96 - whole note
     * 6 - sixteen note
     * 4 - sixteen triplet
     */
#define RVALUE (96)


/**
 * This class describes musical note value (relative duration)
 * It supports only triplet but without dots
 */
class Trhythm
{

public:

  enum Erhythm : quint8 {
    e_none = 0, e_whole = 1, e_half = 2, e_quarter = 4, e_eighth = 8, e_sixteenth = 16
  }; /**< Describes note duration */

  enum Eprefs : quint8 {
    e_rest = 1, e_dot = 2, e_triplet = 4, e_stemDown = 8
  }; /**< Addidtional note preferences */

  enum Ebeam : quint8 {
    e_noBeam = 0, e_beamStart = 16, e_beamCont = 32, e_beamEnd = 48
  }; /**< It covers 16&32 bits of @p m_prefs value. It is mapped into @enum Ebeam then */

  enum Etie : quint8 {
    e_noTie = 0, e_tieStart = 64, e_tieCont = 128, e_tieEnd = 192
  };

      /** DEFAULT CONSTRUCTOR: by default it is quarter (without dot) */
  Trhythm(Erhythm nVal = e_quarter, bool rest = false, bool dot = false, bool triplet = false)
  {
    setRhythm(nVal, rest, dot, triplet);
  }

      /** Creates rhythm from given duration value */
  Trhythm(int rhythmDuration, bool rest = false)
  {
    setRhythm(rhythmDuration);
    setRest(rest);
  }

      /** Copy constructor */
  Trhythm(const Trhythm& r) { setRhythm(r); }

      /** Sets rhythm parameters, Resets all previous values! */
  void setRhythm(Erhythm nVal, bool rest = false, bool dot = false, bool triplet = false) {
    m_r = nVal;
    m_prefs = (rest ? 1 : 0) + (dot ? 2 : (triplet ? 4 : 0));
  }

  Erhythm rhythm() const { return m_r; }
  void setRhythmValue(Erhythm nVal) { m_r = nVal; } /**< Changes rhythmic value only, state of dot, triplet, beams remains unchanged */

      /** It converts std::string into rhythm value. Doesn't change state of triplet, dot or beam. */
  void setRhythmValue(const std::string& nVal);

      /** Makes quick copy of another Trhythm instance. */
  void setRhythm(const Trhythm& r) { m_r = r.rhythm(); m_prefs = r.parameters(); }

      /** Converts given value into rhythm */
  void setRhythm(quint16 durationValue);

  int weight() const { return (int)m_r; } /**< Rhythm value cast to int: i.e. quarter is 4, half is 2 and so on */

      /** Whole note is 96, half is 48, quarter is 24 and with dot is 36. Single eight triplet is 8.
       * Base value is defined in @p RVALUE macro */
  int duration() const;

  bool isRest() const { return m_prefs & e_rest; }
  void setRest(bool rest) {
      if (rest)
        m_prefs |= e_rest;
      else
        m_prefs &= ~e_rest;
  }

  bool hasDot() const { return m_prefs & e_dot; }
  void setDot(bool dot) {
      if (dot) {
        if (!isTriplet())
            m_prefs |= e_dot;
      } else
          m_prefs &= ~e_dot;
  } /**< Allows to set dot only if no triplet */

  bool isTriplet() const { return m_prefs & e_triplet; }
  void setTriplet(bool tri) {
      if (tri) {
        if (!hasDot())
            m_prefs |= e_triplet;
      } else
          m_prefs &= ~e_triplet;
  } /**< Allows to set triplet only if no dot */

  void setBeam(Ebeam b) {
      m_prefs &= ~e_beamEnd; // reset previous beam to none (~e_beamEnd is reverted bin of 48 to set those bits to 0)
      m_prefs |= b;
  }
  Ebeam beam() const { return Ebeam(m_prefs & e_beamEnd); }

  void setTie(Etie t) {
      m_prefs &= ~e_tieEnd;
      m_prefs |= t;
  }
  Etie tie() const { return Etie(m_prefs & e_tieEnd); }

  void setStemDown(bool stemD) {
    if (stemD)
      m_prefs |= e_stemDown;
    else
      m_prefs &= ~e_stemDown;
  }
  bool stemDown() const { return m_prefs & e_stemDown; }

  QString xmlType() const;
  void debug(const char* text = 0) const; /**< Prints current rhythm parameters to std out with given text */

  bool operator==(const Trhythm& r) const {
    return m_r == r.rhythm() && ((m_prefs % 8) == (r.parameters() % 8)); // compare only first three bits of m_prefs (rest, dot and triplet)
  }

  bool operator!=(const Trhythm& r) const {
    return m_r != r.rhythm() || ((m_prefs % 8) != (r.parameters() % 8)); // compare only first three bits of m_prefs (rest, dot and triplet)
  }

protected:
  quint8 parameters() const { return m_prefs; } /**< For copy purposes */
  void setParameters(quint8 p) { m_prefs = p; }

private:
  Erhythm                 m_r;
  quint8                  m_prefs = 0;
};

#endif // TRHYTHM_H
