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


void Trhythm::setRhythmValue(const std::string& nVal) {
  for (int i = 0; i < 6; ++i) {
    if (nVal == rhythmStrings[i]) {
        m_r = (Erhythm)i;
        return;
    }
  }
}


const QString Trhythm::xmlType() {
  if (m_r == e_none)
      return QString();
  else
      return QString::fromStdString(rhythmStrings[(int)std::log2<int>(weight()) + 1]);
}


int Trhythm::duration() {
  if (m_r == e_none)
    return -1;
  int d = ((2 * RVALUE) / weight()) / (isTriplet() ? 3 : 2);
  return d + (hasDot() ? d / 2 : 0);
}


void Trhythm::setRhythm(quint16 durationValue) {
  m_params = 0;
  m_r = e_none;
  quint16 tmpDur = durationValue;
  if (durationValue > RVALUE * 1.5)
    qDebug() << "Too big rhythm duration value" << durationValue;
  else {
    if (RVALUE % durationValue) { // has dot
      m_params = e_dot;
      durationValue = (durationValue / 3) * 2;
    }
    quint32 r = RVALUE / durationValue;
    if (!hasDot() && qNextPowerOfTwo(r - 1) != r) { // it is triplet
      m_params = e_triplet;
      durationValue = (durationValue * 3) / 2;
      r = RVALUE / durationValue;
    }
    if (qNextPowerOfTwo(r - 1) > 16)
      qDebug() << "Unsupported rhythm value" << qNextPowerOfTwo(r - 1);
    else {
      m_r = Erhythm(r);
      // debug message TODO comment it when tested
      if (tmpDur != duration())
        qDebug() << "Input duration" << tmpDur << "is different than obtained!!!" << duration();
    }
  }
}


void Trhythm::debug(const QString& text) {
  if (m_r == e_none)
    qDebug() << text << "no rhythm";
  else
    qDebug() << text << xmlType() << "rest" << isRest() << "dot" << hasDot() << "triplet" << isTriplet() << "duration" << duration();
}

