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

#ifndef TLISTMENU_H
#define TLISTMENU_H

#include <QtWidgets/qlistwidget.h>

/**
 * Subclass of @class QListWidget which role is a menu
 * for switching tabs (stacked layout widgets or so).
 * Used in Nootka under mobile platforms.
 * By default it has left-to-right flow
 */
class TlistMenu : public QListWidget
{

public:
  TlistMenu(QListWidget::Flow fl = LeftToRight, QWidget* parent = 0);

      /** Adjusts equally available free space between items.
       * If @p delay is set it is called with it,
       * to be invoked from any constructor
       * and processed when all sizes have been already set. */
  void adjustItemsLayout(int delay = 0);

};

#endif // TLISTMENU_H
