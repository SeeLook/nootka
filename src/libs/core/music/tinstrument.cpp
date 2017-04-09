/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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

#include "tinstrument.h"
#include <QtGui/qguiapplication.h>

#include <QtCore/qdebug.h>


Tinstrument::Tinstrument(Tinstrument::Etype type) :
  m_type(type)
{
}


QString Tinstrument::name() {
  if (m_type == NoInstrument)
    return QGuiApplication::translate("Einstrument", "other instrument");
  if (m_type == ClassicalGuitar)
    return QGuiApplication::translate("Einstrument", "Classical Guitar");
  if (m_type == ElectricGuitar)
    return QGuiApplication::translate("Einstrument", "Electric Guitar");
  if (m_type == BassGuitar)
    return QGuiApplication::translate("Einstrument", "Bass Guitar");
  
  return QString();
}



QString Tinstrument::glyph() {
  QString glyph;
  switch (m_type) {
    case NoInstrument: glyph = QLatin1String("v"); break;
    case ClassicalGuitar: glyph = QLatin1String("h"); break;
    case ElectricGuitar: glyph = QLatin1String("i"); break;
    case BassGuitar: glyph = QLatin1String("j"); break;
    default: glyph = QString(); break;
  }
  return glyph;
}




