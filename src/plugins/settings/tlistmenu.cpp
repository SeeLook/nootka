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

#include "tlistmenu.h"
#include <QtWidgets/qscroller.h>
#include <QtCore/qtimer.h>

TlistMenu::TlistMenu(QListWidget::Flow fl, QWidget* parent) :
  QListWidget(parent)
{
  setObjectName("TlistMenu"); // revert colors of navigation list
  setStyleSheet(styleSheet() + " QListWidget#TlistMenu { background: palette(text); color: palette(base); }");
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setViewMode(QListView::ListMode);
  setFlow(fl);
  setContentsMargins(0, 0, 0, 0);
  QScroller::grabGesture(viewport(), QScroller::LeftMouseButtonGesture);
}


void TlistMenu::adjustItemsLayout(int delay) {
  if (delay) // call it again after delay
    QTimer::singleShot(delay, this, [this]{ adjustItemsLayout(0); } );
  if (flow() == LeftToRight) {
    setFixedHeight(sizeHintForRow(0) + 2 * frameWidth());
//     int itemsWidth = 0;
//     for (int i = 0; i < count(); ++i)
//       itemsWidth += fontMetrics()->width(item(i)->text());
//     itemsWidth += count() * frameWidth();
//     setSpacing();
  }
}


