/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#include "ttuneobject.h"

#include <QtCore/qdebug.h>


TtuneObject::TtuneObject(QObject* parent) :
  QObject(parent),
  m_tuning(nullptr)
{
}


TtuneObject::~TtuneObject() {}


void TtuneObject::setTune(Ttune* t) {
  m_tuning = t;
  emit scordatureChanged();
}


QString TtuneObject::name() const {
  return m_tuning->name;
}


QString TtuneObject::stringName(int realStrNr) const {
  if (realStrNr > 0 && realStrNr <= m_tuning->stringNr()) {
      auto strName = m_tuning->str(realStrNr).styledName(false);
      return strName.replace(0, 1, strName[0].toUpper());
  } else
      return QString();
}


bool TtuneObject::otherThanStd(int realStrNr) const {
  return realStrNr <= m_tuning->stringNr() && !m_tuning->str(realStrNr).compareNotes(Ttune::stdTune.str(realStrNr));
}


bool TtuneObject::scordature() const {
  if (!m_tuning)
    return false;
  return m_tuning->type() == Ttune::Custom || (m_tuning->type() > Ttune::Standard_EADGBE && m_tuning->type() < Ttune::Bass4_EADG);
}


int TtuneObject::changedStrings() const {
  int ch = 0;
  for (int s = 1; s <= static_cast<int>(m_tuning->stringNr()); ++s) {
    if (otherThanStd(s))
      ch++;
  }
  return ch;
}
