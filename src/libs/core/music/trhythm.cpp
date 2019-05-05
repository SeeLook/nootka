/***************************************************************************
 *   Copyright (C) 2016-2019 by Tomasz Bojczuk                             *
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
const std::string Trhythm::rhythmStrings[6] = { "", "whole", "half", "quarter", "eighth", "16th" };

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
//   for (quint8 r = 0; r <= RVALUE; ++r) {
//     Trhythm rm(r);
//     if (rm.rhythm() != Trhythm::e_none)
//       qDebug() << r << rm.string();
//   }
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
  m_r = NoRhythm;
  if (durationValue <= 96) {
      m_r = static_cast<Erhythm>(rArray[durationValue] & 7); // bit mask 11100000 to extract rhythm value
      if (m_r != NoRhythm) {
        if (rArray[durationValue] & 64)
          m_prefs = e_dot;
        else if (rArray[durationValue] & 128)
          m_prefs = e_triplet;
      }
  } else {
      if (durationValue == 144) { // "manually catch whole note with dot"
        m_prefs = e_dot;
        m_r = Whole;
      }
  }
}


/**
 * In most cases it returns only single element list because subtracting can be resolved with only one rhythm value.
 */
void Trhythm::sub(const Trhythm& r, TrhythmList& remained) const {
  if (r.rhythm() == NoRhythm) {
      remained << *this;
      qDebug() << "[Trhythm] subtracting null rhythm! IS IT REALLY NECESSARY?";
  } else {
      if (r.isTriplet() != isTriplet()) { // TODO: It has to be solved by changing main note
        qDebug() << "[Trhythm] Subtracting triplets and no triplets unsupported";
        return;
      }

      int baseDur = duration();
      int subDur = r.duration();

      if (subDur > baseDur) {
        qDebug() << "[Trhythm] Subtracting rhythm" << r.duration() << "is greater than" << duration();
        return;
      }
      if (baseDur - subDur == 0) { // Return empty (null) rhythm when rhythms are the same
        remained << Trhythm(NoRhythm);
        return;
      }
      Trhythm newR(baseDur - subDur, isRest());
      if (newR.rhythm() != NoRhythm) { // In most cases subtracting returns single rhythm
        remained << newR;
        return;
      }

      if (r.isTriplet() || isTriplet()) // There is no support for subtracting triplets into multiple notes
        return;
      if (baseDur == 4) // 16th triplet - nothing to subtract from
        return;

      // For the rest cases list will contain two Trhythm elements
      if (baseDur == 36 && subDur == 6) // quarter with dot (4.) minus 16th = 4 and 16th
          remained << Trhythm(Quarter, isRest()) << Trhythm(Eighth, isRest(), true);
      else if (baseDur == 48) { // subtracting form half note
          remained << Trhythm(Quarter, isRest());
          if (subDur == 6) // 2 - 16th = 4 and 8.
              remained << Trhythm(Eighth, isRest(), true);
          else if (subDur == 18) // 2 - 8. = 4 and 16th
              remained << Trhythm(Sixteenth, isRest());
      } else if (baseDur == 72) { // subtracting from half with dot
          remained << Trhythm(Whole, isRest()); // whole is always first
          if (baseDur == 6) // 2. - 16th = 2 and 8.
              remained << Trhythm(Eighth, isRest(), true);
          else if (baseDur == 12) // 2. - 8 = 2 and 8
              remained << Trhythm(Eighth, isRest());
          else if (baseDur == 18) // 2. - 8. = 2 and 16th
              remained << Trhythm(Sixteenth, isRest());
      } else if (baseDur == 96) { // subtracting from whole note
          remained << Trhythm(Whole, isRest(), true); // whole wit dot is always first
          if (subDur == 6) // 1 - 16 = 2. and 8.
            remained << Trhythm(Eighth, isRest(), true);
          else if (baseDur == 12) // 1 - 8 = 2. and 8
              remained << Trhythm(Eighth, isRest());
          else if (baseDur == 18) // 1 - 8. = 2. and 16th
              remained << Trhythm(Sixteenth, isRest());
          else if (baseDur == 36) { // 1 - 4. = 2 and 16th
              remained[0].setDot(false); // revert a dot set above
              remained << Trhythm(Sixteenth, isRest());
          }
      } else if (baseDur == 144) { // subtracting from whole and dot
          if (subDur <= 48) {
              Trhythm half(Half, isRest());
              half.sub(r, remained);
              remained.prepend(Trhythm(Whole, isRest()));
          }
      }
  }
}


void Trhythm::split(TrhythmList& twoRhythms) const {
  if (rhythm() == NoRhythm || rhythm() == Sixteenth)
    return; // nothing to split

  if (hasDot()) {
      twoRhythms << Trhythm(rhythm(), isRest(), false) // no triplet for sure
                << Trhythm(static_cast<Erhythm>(rhythm() + 1), isRest());
          // Also when there is a dot, for sure it is not lowest possible rhythm, so we may add 1 to rhythm value to obtain its half
  } else {
      Trhythm half(static_cast<Erhythm>(rhythm() + 1), isRest(), false, isTriplet());
      twoRhythms << half << half;
  }
  if (!isRest() && twoRhythms.size() == 2) {
    twoRhythms.first().setStemDown(stemDown());
    twoRhythms.last().setStemDown(stemDown());
  }
}


void Trhythm::resolve(int problemDur, TrhythmList& solvList) {
  int smallestDur = 0;
  for (int i = 1; i < 6; ++i) { // looking for smallest rhythm to divide given duration @p dur
      smallestDur = Trhythm(static_cast<Trhythm::Erhythm>(i)).duration(); // no triplets here
      if (smallestDur < problemDur) {
        if ((problemDur / smallestDur) * smallestDur == problemDur)
          break;
      }
  }
  if (smallestDur) {
      int chunksNr = problemDur / smallestDur; // number of smallest rhythm values in duration @p dur
      int step = 0;
      solvList << Trhythm() << Trhythm();
      do { // find when rhythms of two divided notes of duration are valid
          step++;
          solvList[0].setRhythm((chunksNr - step) * smallestDur);
          solvList[1].setRhythm(smallestDur * step);
      } while (step < chunksNr - 1 && (!solvList[0].isValid() || !solvList[1].isValid()));
  }
}


TrhythmList Trhythm::resolve(int problemDur, int* unsolvedDur) {
  int workDur = problemDur;
  TrhythmList rList;
  for (int d = 0; d < DUR_COUNT; ++d) {
    while (workDur > durOrder[d]) {
      rList << Trhythm(durOrder[d]);
      workDur -= durOrder[d];
    }
    if (workDur == durOrder[d]) {
        rList << Trhythm(durOrder[d]);
        workDur -= durOrder[d];
        break;
    } // else try next loop with smaller rhythmic value
  }
  if (workDur > 0) {
    qDebug() << "[Trhythm] Can not resolve duration of" << problemDur << ". Remains" << workDur;
    if (unsolvedDur)
      *unsolvedDur = workDur;
  }
  return rList;
}


QString Trhythm::string() const {
  QString ret = QString::number(weight());
  if (isRest())
    ret.prepend(QStringLiteral("R"));
  if (hasDot())
    ret.append(QStringLiteral("."));
  else if (isTriplet())
    ret.append(QStringLiteral("^3"));
  return ret;
}


void Trhythm::debug(const char* text) const {
  if (m_r == NoRhythm)
    qDebug() << text << "no rhythm";
  else {
    qDebug() << text << xmlType() << "| rest" << isRest() << "| dot" << hasDot() << "| triplet" << isTriplet() << "| duration" << duration()
             << "| beam" << beam() << "| tie" << tie() << "| stem" << (stemDown() ? "down" : "up")
             << "|" << (m_prefs % 8) << m_prefs;
  }
}


QDataStream& operator << (QDataStream& out, const Trhythm& r) {
  out << static_cast<quint8>(r.rhythm()) << static_cast<quint8>(r.parameters());
  return out;
}


QDataStream& operator >>(QDataStream& in, Trhythm& r) {
  quint8 rr, p;
  in >> rr >> p;
  r.setRhythm(static_cast<Trhythm::Erhythm>(rr));
  r.setParameters(p);
  return in;
}
