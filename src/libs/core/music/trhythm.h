/***************************************************************************
 *   Copyright (C) 2014-2020 by Tomasz Bojczuk                             *
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


#include "nootkacoreglobal.h"
#include <QtCore/qobject.h>
#include <QtCore/qdatastream.h>
#include <QtCore/qmath.h>


    /**
     * Almost powers of 2, used to quickly mapping @p Erhythm enumerator into weight of rhythm value
     */
static const quint8 rtm2weightArr[6] = {0, 1, 2, 4, 8, 16};


    /**
     * Base value to calculate proportional values of rhythm duration:
     * 96 - whole note
     * 6 - sixteen note
     * 4 - sixteen triplet
     */
#define RVALUE (96)

    /**
     * Array with duration values
     */
static const quint8 durArray[6][3] = {
//  |  bare note |      dot         |    triplet |
    { 0,             0,                  0           }, // none
    { RVALUE,     (RVALUE * 3) / 2,  (RVALUE * 2) / 3}, // whole note      (96, 144, 64)
    { RVALUE / 2, (RVALUE * 3) / 4,   RVALUE / 3     }, // half note       (48, 72,  32)
    { RVALUE / 4, (RVALUE * 3) / 8,   RVALUE / 6     }, // quarter note    (24, 36,  16)
    { RVALUE / 8, (RVALUE * 3) / 16,  RVALUE / 12    }, // eighth note     (12, 18,   8)
    { RVALUE / 16,(RVALUE * 3) / 32,  RVALUE / 24    }  // sixteenth note  (6,  9,    4)
};

#define DUR_COUNT   (10) // total number of single notes duration (TODO: no triplets yet)
static const quint8 durOrder[DUR_COUNT] = { 144, 96, 72, 48, 36, 24, 18, 12, 9, 6 };

class Trhythm;

typedef QList<Trhythm> TrhythmList;


/**
 * This class describes musical note value (relative duration)
 * but has limitations for Nootka purposes.
 * It supports triplets but without dots
 *
 * To have full functionality it has to be initialized by static @p initialize()
 */
class NOOTKACORE_EXPORT Trhythm
{

  Q_GADGET

public:

  static const std::string rhythmStrings[6];
      /**
       * Initialize class @p Trhythm with values of duration.
       * Without this, @p setRhythm(int) will not work.
       */
  static void initialize();

      /**
       * Describes note duration
       */
  enum Erhythm : quint8 {
    NoRhythm = 0, Whole = 1, Half = 2, Quarter = 3, Eighth = 4, Sixteenth = 5
  };
  Q_ENUM(Erhythm)

      /**
       * Additional note preferences
       */
  enum Eprefs : quint8 {
    e_rest = 1, e_dot = 2, e_triplet = 4, e_stemDown = 8
  };

      /**
       * It covers 16&32 bits of @p m_prefs value. It is mapped into @enum Ebeam then
       */
  enum Ebeam : quint8 {
    e_noBeam = 0, e_beamStart = 16, e_beamCont = 32, e_beamEnd = 48
  };
  Q_ENUM(Ebeam)

  enum Etie : quint8 {
    e_noTie = 0, e_tieStart = 64, e_tieCont = 128, e_tieEnd = 192
  };
  Q_ENUM(Etie)

      /**
       * DEFAULT CONSTRUCTOR: by default it is quarter (without dot)
       */
  Trhythm(Erhythm nVal = Quarter, bool rest = false, bool dot = false, bool triplet = false)
  {
    setRhythm(nVal, rest, dot, triplet);
  }

      /**
       * Creates rhythm from given duration value
       */
  Trhythm(int rhythmDuration, bool rest = false)
  {
    setRhythm(rhythmDuration);
    setRest(rest);
  }

      /**
       * Sets rhythm parameters, Resets all previous values!
       */
  void setRhythm(Erhythm nVal, bool rest = false, bool dot = false, bool triplet = false) {
    m_r = nVal;
    m_prefs = (rest ? 1 : 0) + (dot ? 2 : (triplet ? 4 : 0));
  }

  Erhythm rhythm() const { return m_r; }

      /**
       * Changes rhythmic value only, state of dot, triplet, beams remains unchanged
       */
  void setRhythmValue(Erhythm nVal) { m_r = nVal; }

      /**
       * It converts std::string into rhythm value. Doesn't change state of triplet, dot or beam.
       */
  void setRhythmValue(const std::string& nVal);

      /**
       * Makes quick copy of another @p Trhythm instance.
       */
  void setRhythm(const Trhythm& r) { m_r = r.rhythm(); m_prefs = r.parameters(); }

      /**
       * Converts given value into rhythm
       */
  void setRhythm(quint16 durationValue);

      /**
       * Rhythm value: i.e. sixteen is 16, quarter is 4, half is 2 and so on
       */
  quint8 weight() const { return rtm2weightArr[m_r]; }

      /**
       * Rhythm is valid when it is different than @p Trhythm::NoRhythm
       */
  bool isValid() const { return m_r != NoRhythm; }

      /**
       * Whole note is 96, half is 48, quarter is 24 and with dot is 36. Single eight triplet is 8.
       * Base value is defined in @p RVALUE macro
       */
  int duration() const {
      return durArray[m_r][hasDot() ? 1 : (isTriplet() ? 2 : 0)];
  }

  bool isRest() const { return m_prefs & e_rest; }
  void setRest(bool rest) {
      if (rest)
        m_prefs |= e_rest;
      else
        m_prefs &= ~e_rest;
  }

  bool hasDot() const { return m_prefs & e_dot; }

      /**
       * Allows to set dot only if no triplet
       */
  void setDot(bool dot) {
      if (dot) {
          if (!isTriplet())
            m_prefs |= e_dot;
      } else
          m_prefs &= ~e_dot;
  }

  bool isTriplet() const { return m_prefs & e_triplet; }

      /**
       * Allows to set triplet only if no dot
       */
  void setTriplet(bool tri) {
      if (tri) {
        if (!hasDot())
            m_prefs |= e_triplet;
      } else
          m_prefs &= ~e_triplet;
  }

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

      /**
       * Subtract @p r rhythm from current rhythm and writes returned rhythm(s) to given list @p remained.
       * The list is necessary due to some of subtractions are impossible to be expressed in single @p Trhythm value.
       * I.e. half - 16th is quarter and eight with dot.
       * It doesn't change current rhythm.
       * @p remained list may be empty (unchanged) when subtraction is impossible to perform
       */
  void sub(const Trhythm& r, TrhythmList& remained) const;

      /**
       * Splits current rhythm into two rhythm values and writes result into @p twoRhythms list
       * When current rhythm is without dot it divides it into two equal parts:
       * - i.e. quarter will be two eighths
       * but notes with dots splits on main part and dot part:
       * - i.e. 8. will be 8 and sixteenth
       * It doesn't change current rhythm
       */
  void split(TrhythmList& twoRhythms) const;

      /**
       * Tries to solve given @p problemDur which is not possible to express in single @p Trhythm instance
       * with a few rhythms put into @p solvList
       */
  static void resolve(int problemDur, TrhythmList& solvList);

      /**
       * Static method returning @p TrhythmList with rhythmic values covering given @p problemDur.
       * If @p unsolvedDur pointer is set, rest of @p problemDur which can't be expressed with correct rhythmic value is put into it.
       */
  static TrhythmList resolve(int problemDur, int* unsolvedDur = nullptr);

      /**
       * Returns string with formatted rhythm value i.e. 4 or 8. or 16^3
       */
  QString string() const;

  QString xmlType() const;

      /**
       * Prints current rhythm parameters to std out with given text
       */
  void debug(const char* text = 0) const;

  bool operator==(const Trhythm& r) const {
    return m_r == r.rhythm() && ((m_prefs % 8) == (r.parameters() % 8)); // compare only first three bits of m_prefs (rest, dot and triplet)
  }

  bool operator!=(const Trhythm& r) const {
    return m_r != r.rhythm() || ((m_prefs % 8) != (r.parameters() % 8)); // compare only first three bits of m_prefs (rest, dot and triplet)
  }

  friend QDataStream& operator << (QDataStream &out, const Trhythm &r);
  friend QDataStream& operator >> (QDataStream &in, Trhythm &r);

protected:
      /**
       * For copy purposes
       */
  quint8 parameters() const { return m_prefs; }
  void setParameters(quint8 p) { m_prefs = p; }

private:
  Erhythm                 m_r;
  quint8                  m_prefs = 0;
};

NOOTKACORE_EXPORT QDataStream& operator << (QDataStream &out, const Trhythm &r);
NOOTKACORE_EXPORT QDataStream& operator >> (QDataStream &in, Trhythm &r);

#endif // TRHYTHM_H
