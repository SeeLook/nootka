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

#ifndef TGUITARVIEW_H
#define TGUITARVIEW_H

#include <QGraphicsView>


class QGraphicsProxyWidget;
class TfingerBoard;
class QTouchEvent;

/**
 * Another view of guitar fingerboard
 */
class TguitarView : public QGraphicsView
{

public:
  TguitarView(QGraphicsView* guitar, QGraphicsView* parent);

  QGraphicsProxyWidget* proxy() { return m_proxy; }

  bool mapTouchEvent(QTouchEvent* te);


private:
  TfingerBoard                *m_guitar;
  QGraphicsProxyWidget        *m_proxy;

};

#endif // TGUITARVIEW_H
