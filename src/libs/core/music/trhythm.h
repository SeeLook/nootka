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


/**
 * This class describes musical note value (relative duration)
 * It supports only triplet but without dots
 */
class Trhythm
{

public:

	enum Erhythm {
		e_none = 0, e_whole = 1, e_half = 2, e_quarter = 4, e_eighth = 8, e_sixteenth = 16
	}; /**< Describes note duration */

  enum Epreferences {
    e_rest = 1, e_dot = 2, e_triplet = 4, e_stemDown = 8
  }; /**< Addidtional note preferences */

			/** DEFAULT CONSTRUCTOR: by default it is quarter (without dot) */
	Trhythm(Erhythm nVal = e_quarter, bool rest = false, bool dot = false, bool triplet = false)
	{
    setRhythm(nVal, rest, dot, triplet);
  }

      /** Copy constructor */
  Trhythm(const Trhythm& r) { setRhythm(r); }

			/** Sets rhythm parameters, Resets all previous values! */
	void setRhythm(Erhythm nVal, bool rest = false, bool dot = false, bool triplet = false) {
		m_r = nVal;
    m_params = (rest ? 1 : 0) + (dot ? 2 : (triplet ? 4 : 0));
	}

	Erhythm rhythm() const { return m_r; }
	void setRhythmValue(Erhythm nVal) { m_r = nVal; } /**< Changes rhythmic value only, state of dot and triplet remains unchanged */

			/** It converts std::string into rhythm value */
	void setRhythmValue(const std::string& nVal) {
		for (int i = 0; i < 6; ++i) {
			if (nVal == rhythmStrings[i]) {
				m_r = (Erhythm)i;
				return;
			}
		}
	}

      /**< Makes quick copy of another Trhythm instance   */
  void setRhythm(const Trhythm& r) { m_r = r.rhythm(); m_params = r.parameters(); }

  int weight() { return (int)m_r; } /**< Rhythm value cast to int: i.e. quarter is 4, half is 2 and so on */

      /** Whole note is 1.0, half is 0.5, quarter is 0.25 and with dot is 0.375. Single eight triplet is 0.16667 */
  qreal duration() {
      qreal d = (2.0 / weight()) / (isTriplet() ? 3.0 : 2.0);
      return d + (hasDot() ? d / 2.0 : 0.0);
  }

	bool isRest() const { return m_params & e_rest; }
  void setRest(bool rest) {
      if (rest)
        m_params |= e_rest;
      else if (isRest())
        m_params ^= e_rest;
  }

	bool hasDot() const { return m_params & e_dot; }
	void setDot(bool dot) {
      if (dot) {
        if (!isTriplet())
            m_params |= e_dot;
      } else if (hasDot())
          m_params ^= e_dot;
  } /**< Allows to set dot only if no triplet */

	bool isTriplet() const { return m_params & e_triplet; }
	void setTriplet(bool tri) {
      if (tri) {
        if (!hasDot())
            m_params |= e_triplet;
      } else if (isTriplet())
          m_params ^= e_triplet;
  } /**< Allows to set triplet only if no dot */

	QString const xmlType() {
    if (m_r == e_none)
      return QString();
    else
      return QString::fromStdString(rhythmStrings[(int)std::log2<int>(m_r) + 1]);
	}

protected:
  quint8 parameters() const { return m_params; } /**< For copy purposes */
  void setParameters(quint8 p) { m_params = p; }

private:
	Erhythm 						  	m_r;
  quint8                  m_params; /**< Stores additional parameters as a logic flags of @enum EnotePrefs */
};

#endif // TRHYTHM_H
