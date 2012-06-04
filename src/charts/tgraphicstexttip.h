/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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

#ifndef TGRAPHICSTEXTTIP_H
#define TGRAPHICSTEXTTIP_H

#include <qgraphicsitem.h>


class TgraphicsTextTip : public QGraphicsTextItem
{

public:
  TgraphicsTextTip(QString text, QColor bgColor = -1);
  
  virtual ~TgraphicsTextTip();
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
  
  QColor bgColor() { return m_bgColor; }
  
      /** This method align center content of item.
       * Put text before call it. */
  static void alignCenter(TgraphicsTextTip *tip);
  
private:
  QColor m_bgColor;
};

#endif // TGRAPHICSTEXTTIP_H
