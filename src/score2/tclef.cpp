/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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


#include "tclef.h"
#include <QApplication>

Tclef::Tclef(Tclef::Etype type)
{
    m_type = type;
}


QString Tclef::name() {
  switch(m_type) {
    case e_treble_G:
      return QApplication::translate("Tclef", "clef G (treble)");
    case e_treble_G_8down:
      return QApplication::translate("Tclef", "clef G (treble) dropped octave down");
    case e_bass_F:
      return QApplication::translate("Tclef", "clef F (bass)");
    case e_bass_F_8down:
      return QApplication::translate("Tclef", "clef F (bass) dropped octave down");
    case e_alto_C:
      return QApplication::translate("Tclef", "clef C (alto)");
    case e_tenor_C:
      return QApplication::translate("Tclef", "clef C (tenor)");
  }
  return "";
}
