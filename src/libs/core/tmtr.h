/***************************************************************************
 *   Copyright (C) 2015-2018 by Tomasz Bojczuk                             *
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

#ifndef TMTR_H
#define TMTR_H


#include "nootkacoreglobal.h"
#include <QtWidgets/qapplication.h>
#include <QtGui/qscreen.h>
#include <QtGui/qfont.h>


/**
 * These are static measurement units depend on available screen size,
 * or current application font size and so on.
 * It has to be initialized through @p init() method before use.
 */
class NOOTKACORE_EXPORT Tmtr
{

public:
  static void init(QApplication* a);

      /** size of finger (1 cm) in pixels */
  static int fingerPixels() { return m_fingerPixels; }

      /** height or width of a screen (less of them) */
  static int shortScreenSide() { return m_shortScreenSide; }

      /** height or width of a screen (bigger of them) */
  static int longScreenSide() { return m_longScreenSide; }

  static int screenWidth() { return qApp->screens().first()->geometry().width(); }
  static int screenHeight() { return qApp->screens().first()->geometry().height(); }

  static QFont systemFont;

private:
  static int m_fingerPixels;
  static int m_shortScreenSide;
  static int m_longScreenSide;
};

#endif // TMTR_H
