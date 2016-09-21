/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "trhythm.h"
#include <QtCore/qdebug.h>


/**
 * Converts @p Trhythm into value:
 * - rhythmic value number: first 3 bytes [0-7]
 * - is dot: byte 7 [64]
 * - or is triplet: byte 8 [128]
 * It is used to store rhythm info in single byte for duration array
 */
inline quint8 rHash(const Trhythm& r) {
  return r.rhythm() + (r.hasDot() ? 64 : 0) + (r.isTriplet() ? 128 : 0);
}

/*-----static-------*/
/**
 * Array that keeps hash values for every numeric duration [0-96]
 * or null if no such duration
 */
static quint8  rArray[RVALUE + 1];

void Trhythm::initialize() {
  for (int d = 0; d < RVALUE + 1; ++d)
    rArray[d] = 0;

  for (int d = 1; d < 6 ; ++d) {
    Trhythm r(static_cast<Erhythm>(d));
    rArray[r.duration()] = rHash(r);
    if (d > 1) { // skip whole with dot (144 is too big)
      r.setDot(true);
      rArray[r.duration()] = rHash(r);
      r.setDot(false);
    }
    r.setTriplet(true);
    rArray[r.duration()] = rHash(r);
  }
}
/*------------------*/


void Trhythm::setRhythmValue(const std::string& nVal) {
  for (int i = 0; i < 6; ++i) {
    if (nVal == rhythmStrings[i]) {
        m_r = static_cast<Erhythm>(i);
        return;
    }
  }
}


QString Trhythm::xmlType() const {
  return QString::fromStdString(rhythmStrings[m_r]);
}


void Trhythm::setRhythm(quint16 durationValue) {
  m_prefs = 0;
  m_r = e_none;
  if (durationValue <= 96) {
      m_r = static_cast<Erhythm>(rArray[durationValue] & 7); // bit mask 11100000 to extract rhythm value
      if (m_r != e_none) {
        if (rArray[durationValue] & 64)
          m_prefs = e_dot;
        else if (rArray[durationValue] & 128)
          m_prefs = e_triplet;
      }
  } else {
      if (durationValue == 144) { // "manually catch whole note with dot"
        m_prefs = e_dot;
        m_r = e_whole;
      }
  }
}


void Trhythm::debug(const char* text) const {
  if (m_r == e_none)
    qDebug() << text << "no rhythm";
  else {
    qDebug() << text << xmlType() << "| rest" << isRest() << "| dot" << hasDot() << "| triplet" << isTriplet() << "| duration" << duration()
             << "| beam" << beam() << "| tie" << tie() << "| stem" << (stemDown() ? "down" : "up")
             << "|" << (m_prefs % 8) << m_prefs;
  }
}

