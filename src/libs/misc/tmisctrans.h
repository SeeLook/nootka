/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#ifndef TMISCTRANS_H
#define TMISCTRANS_H

#include "nootkamiscglobal.h"
#include <QApplication>

/** 
 * Extra translations.
 */
class NOOTKAMISC_EXPORT TmiscTrans {
  
public:
  
  static const QString note7txt() { return QApplication::translate("Tpage_3", "b", 
    "Give here a name of 7-th note preferred in your country. But only 'b' or 'h' not 'si' or something worst...");
  } // returns b or h depends on translation
  
        /** default style grab from translation 'letters' or 'solfege' */
    static const QString keyNameStyle() { return QApplication::translate("Tpage_3", "letters",
      "DO NOT TRANSLATE IT DIRECTLY. Put here 'letters' or 'solfege' This is country preferred style of naming key signatures. 'letters' means C-major/a-minor names ('major' & 'minor' also are translated by you), 'solfege' means Do-major/La-minor names"); 
    }
};

#endif // TMISCTRANS_H
