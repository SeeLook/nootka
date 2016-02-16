/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef TTOUCHAREA_H
#define TTOUCHAREA_H

#include "nootkacoreglobal.h"
#include <QtWidgets/qscrollarea.h>


/**
 * @class TtouchArea is @class QScrollArea with touch support.
 * It support vertical and horizontal scrolling by finger.
 * It has already built in @class QWidget, so adding layout to it
 * goes through @p setLayout() method of TtouchArea
 */
class NOOTKACORE_EXPORT TtouchArea : public QScrollArea
{

public:
  TtouchArea(QWidget* parent = 0);

  static bool touchEnabled(); /** @p TRUE when touch is enabled */

  void setLayout(QLayout* l) { widget()->setLayout(l); }

};

#if defined (Q_OS_ANDROID)
class QLabel;

    /** Generates @class QLabel with text of given status tip.
     * Font is smaller, height has static physical size.
     * NOTICE: @p statusTip() of given widget is emptied.
     * Returns pointer to generated @class QLabel.
     * When @p indent is @p TRUE, left context margin of the label is set.
     * @p wordWrap parameter sets label to word-wrapping the text. */
QLabel* getLabelFromStatus(QWidget* w, bool indent = true, bool wordWrap = false);
#endif

#endif // TTOUCHAREA_H
