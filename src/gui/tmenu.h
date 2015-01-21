/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef TMENU_H
#define TMENU_H

#include <QMenu>


/** 
 * This is ordinary QMenu class 
 * but during @p showEvent() it shifts menu position to parent widget Y coordinate
 * To display menu below Nootka status bar (if it is visible) 
 * and pointer to status widget was set through @p setStatusWidget()
 */
class Tmenu : public QMenu
{
public:
  explicit Tmenu(QWidget* parent = 0);
  
      /** Sets status widget pointer to determine is it visible and set proper menu position. */
  static void setStatusWidget(QWidget* stat) { m_statusWidget = stat; }
  
protected:  
  virtual void showEvent(QShowEvent* event);
  
private:
  static QWidget*           m_statusWidget;
  
};

#endif // TMENU_H
