/***************************************************************************
 *   Copyright (C) 2018 by Tomasz Bojczuk                                  *
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

#include "trtmgroup.h"


/** Shortcuts of C++ statements to be more readable in code */
#define whole           (QStringLiteral("\ue1d2"))
#define half            (QStringLiteral("\ue1d3"))
#define quarter         (QStringLiteral("\ue1f1"))
#define eight           (QStringLiteral("\ue1d7"))
#define note8           (QStringLiteral("\ue1f3"))
#define note16          (QStringLiteral("\ue1f5"))
#define beam8           (QStringLiteral("\ue1f8"))
#define beam16          (QStringLiteral("\ue1fa"))
#define dot             (QStringLiteral("\ue1fc"))
#define space           (QStringLiteral(" "))

#define R4              Trhythm(Trhythm::Quarter)
#define R8              Trhythm(Trhythm::Eighth)
#define R8dot           Trhythm(Trhythm::Eighth, false, true)
#define R16             Trhythm(Trhythm::Sixteenth)


QString TrtmGroup::text() const {
  QString t;
  switch (m_gr) {
    case Gr_1: t = whole; break;
    case Gr_2: t = half; break;
    case Gr_4: t = quarter; break;
    case Gr_8_8: t = quarter + beam8 + note8; break;
    case Gr_8_16_16: t = quarter + beam8 + note8 + beam16 + note16; break;
    case Gr_16_16_16_16: t = quarter + beam16 + note16 + beam16 + note16 + beam16 + note16; break;
    case Gr_16_16_8: t = quarter + beam16 + note16 + beam8 + note8; break;
    case Gr_16_8_16: t = quarter + beam16 + note8 + beam8 + note16 ; break;

    case Gr_8_8_8: t = quarter + beam8 + note8 + beam8 + note8; break;
    case Gr_4_8: t = quarter + space + space + eight; break;
    case Gr_8_4: t = eight + space + quarter; break;
    case Gr_8_8_16_16: t = quarter + beam8 + note8 + beam8 + note8 + beam16 + note16; break;
    case Gr_8_16_16_8: t = quarter + beam8 + note8 + beam16 + note16 + beam8 + note8; break;
    case Gr_16_16_8_8: t = quarter + beam16 + note16 + beam8 + note8 + beam8 + note8; break;
    case Gr_8_16_16_16_16: t = t = quarter + beam8 + note8 + beam16 + note16 + beam16 + note16 + beam16 + note16; break;
    case Gr_16_16_16_16_8: t = quarter + beam16 + note16 + beam16 + note16 + beam16 + note16 + beam8 + note8; break;
    case Gr_16_16_8_16_16: t = quarter + beam16 + note16 + beam8 + note8 + beam8 + note8 + beam16 + note16; break;
    case Gr_16_16_16_16_16_16:  t = quarter + beam16 + note16 + beam16 + note16 + beam16 + note16 + beam16 + note16 + beam16 + note16; break;

    case Gr_16_8_8_16: t = quarter + beam16 + note8 + beam8 + note8 + beam8 + note16; break;
    case Gr_16_8_16_8: t = quarter + beam16 + note8 + beam8 + beam8 + note16 + beam8 + note8; break;
    case Gr_16_16_16_8_16: t = quarter + beam16 + note16 + beam16 + note16 + beam8 + note8 + beam8 + note16; break;
    case Gr_16_8_16_16_16: t = quarter + beam16 + note8 + beam8 + note8 + beam16 + note16 + beam16 + note16; break;

    case Gr_1dot: t = whole + space + dot; break;
    case Gr_2dot: t = half + space + dot; break;
    case Gr_4dot: t = quarter + dot; break;
    case Gr_8dot_16: t = quarter + beam8 + dot + beam8 + note16; break;
    case Gr_16_8dot: t = quarter + beam16 + note8 + dot; break;

    case Gr_8dot_8dot: t = quarter + beam8 + dot + beam8 + note8 + dot; break;
    case Gr_8dot_16_8: t = quarter + beam8 + dot + beam8 + note16 + beam8 + note8; break;
    case Gr_8dot_8_16: t = quarter + beam8 + dot + beam8 + note8 + beam8 + note16; break;
    case Gr_16_8dot_8: t = quarter + beam16 + note8 + beam8 + dot + beam8 + note8; break;
    case Gr_8_8dot_16: t = quarter + beam8 + note8 + beam8 + dot + beam8 + note16; break;
    case Gr_8dot_16_16_16: t = quarter + beam8 + dot + beam8 + note8 + beam16 + note16 + beam16 + note16; break;
    case Gr_16_8dot_16_16: t = quarter + beam16 + note8 + beam8 + dot + beam8 + note8 + beam16 + note16; break;
    case Gr_16_16_8dot_16: t = quarter + beam16 + note16 + beam8 + note8 + beam8 + dot + beam8 + note16; break;
    case Gr_16_16_16_8dot: t = quarter + beam16 + note16 + beam16 + note16 + beam8 + note8 + dot; break;
    default: break;
  }
  return t;
}


TrhythmList TrtmGroup::rhythm() const {
  TrhythmList rl;
  switch (m_gr) {
    case Gr_1: rl << Trhythm(Trhythm::Whole); break;
    case Gr_2: rl << Trhythm(Trhythm::Half); break;
    case Gr_4: rl << R4; break;
    case Gr_8_8: rl << R8 << R8; break;
    case Gr_8_16_16: rl << R8 << R16 << R16; break;
    case Gr_16_16_16_16: rl << R16 << R16 << R16 << R16; break;
    case Gr_16_16_8: rl << R16 << R16 << R8; break;
    case Gr_16_8_16: rl << R16 << R8 << R16; break;

    case Gr_8_8_8: rl << R8 << R8 << R8; break;
    case Gr_4_8: rl << R4 << R8; break;
    case Gr_8_4: rl << R8 << R4; break;
    case Gr_8_8_16_16: rl << R8 << R8 << R16 << R16; break;
    case Gr_8_16_16_8: rl << R8 << R16 << R16 << R8; break;
    case Gr_16_16_8_8: rl << R16 << R16 << R8 << R8; break;
    case Gr_8_16_16_16_16: rl << R8 << R16 << R16 << R16 << R16; break;
    case Gr_16_16_16_16_8: rl << R16 << R16 << R16 << R16 << R8 ; break;
    case Gr_16_16_8_16_16: rl << R16 << R16 << R8 << R16 << R16; break;
    case Gr_16_16_16_16_16_16: rl << R16 << R16 << R16 << R16 << R16 << R16; break;

    case Gr_16_8_8_16: rl << R16 << R8 << R8 << R16; break;
    case Gr_16_8_16_8: rl << R16 << R8 << R16 << R8; break;
    case Gr_16_16_16_8_16: rl <<  R16 << R16 << R16 << R8 << R16; break;
    case Gr_16_8_16_16_16: rl <<  R16 << R8 << R16 << R16 << R16; break;

    case Gr_1dot: rl << Trhythm(Trhythm::Whole, false, true); break;
    case Gr_2dot: rl << Trhythm(Trhythm::Half, false, true); break;
    case Gr_4dot: rl << Trhythm(Trhythm::Quarter, false, true); break;

    case Gr_8dot_16: rl << R8dot << R16; break;
    case Gr_16_8dot: rl << R16 << R8dot; break;
//     // Triple metre (duration: 4. -> one quarter and eight) ==== DOTS
    case Gr_8dot_8dot: rl << R8dot << R8dot; break;
    case Gr_8dot_16_8: rl << R8dot << R16 << R8; break;
    case Gr_8dot_8_16: rl << R8dot << R8 << R16; break;
    case Gr_16_8dot_8: rl << R16 << R8dot << R8; break;
    case Gr_8_8dot_16: rl << R8 << R8dot << R16; break;
    case Gr_8dot_16_16_16: rl << R8dot << R16 << R16 << R16; break;
    case Gr_16_8dot_16_16: rl << R16 << R8dot << R16 << R16; break;
    case Gr_16_16_8dot_16: rl << R16 << R16 << R8dot << R16; break;
    case Gr_16_16_16_8dot: rl << R16 << R16 << R16 << R8dot; break;
    default: break;
  }
  return rl;
}


bool TrtmGroup::isSpecial() const {
  return (m_gr != Gr_undefined && m_gr < Gr_4) || m_gr == Gr_1dot || m_gr == Gr_2dot;
}


bool TrtmGroup::isDuple() const {
  return (m_gr > Gr_2 && m_gr <= Gr_16_8_16) || m_gr == Gr_8dot_16 || m_gr == Gr_16_8dot;
}


bool TrtmGroup::isTriple() const {
  return (m_gr >= Gr_8_8_8 && m_gr <= Gr_16_8_16_16_16) || m_gr == Gr_4dot || (m_gr >= Gr_8dot_8dot && m_gr <= Gr_16_16_16_8dot);
}


int TrtmGroup::duration() const {
  int dur = 0;
  TrhythmList rl = rhythm();
  for (int r = 0; r < rl.count(); ++r)
    dur += rl[r].duration();
  return dur;
}
