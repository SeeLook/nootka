/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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


#ifndef TABSTRACTSOUNDVIEW_H
#define TABSTRACTSOUNDVIEW_H


#include <QtWidgets/qwidget.h>
#include "nootkasoundglobal.h"


class NOOTKASOUND_EXPORT TabstractSoundView : public QWidget {

public:

  explicit TabstractSoundView(QWidget* parent = 0);

  QColor gradColorAtPoint(float lineX1, float lineX2, QColor startC, QColor endC, float posC);

  static QColor startColor, middleColor, endColor, totalColor, disabledColor;
  QColor tc;

  virtual void setDisabled(bool isDisabled);

  virtual void setEnabled(bool isEnabled) { setDisabled(!isEnabled); }

protected:
  virtual void resizeIt(int myHeight);

      /** Width of a tick dependent on screen DPI */
  static int tickWidth() { return m_tickWidth; }

      /** Distance between ticks dependent on screen DPI */
  static int tickGap() { return m_tickGap; }

protected:
  QFont           nootFont;
  QRect           noteBound;
  static int      m_tickWidth;
  static int      m_tickGap;

};



#endif // TABSTRACTSOUNDVIEW_H
