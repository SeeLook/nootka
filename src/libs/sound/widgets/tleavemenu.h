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

#ifndef TLEAVEMENU_H
#define TLEAVEMENU_H

#include <QMenu>
#include <QApplication>
#include <QDesktopWidget>

/**
 * QMenu that closes when mouse leaves it.
 * It has no parent.
 * It fixes its position when menu wants lay over available screen
 */
class TleaveMenu : public QMenu
{
public:
  TleaveMenu() : QMenu() {}

protected:
  virtual void leaveEvent(QEvent*) {
    close();
  }

  virtual void showEvent(QShowEvent* e) {
    move(qMin<int>(x(), qApp->desktop()->availableGeometry().width() - width() - 5),
         qMin<int>(y(), qApp->desktop()->availableGeometry().height() - height() - 5));
    QMenu::showEvent(e);
  }
};

#endif // TLEAVEMENU_H
