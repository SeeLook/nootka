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


/** This is baase class for tips. 
 * It centering html text, paints background rounded rect in given color
 * and drops shadow. If bgColor is -1 the shadow is on a text.
*/
class TgraphicsTextTip : public QGraphicsTextItem
{

public:
  TgraphicsTextTip(QString text, QColor bgColor = -1);
  TgraphicsTextTip();
  
  virtual ~TgraphicsTextTip();
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
  
      /** Overwrites QGraphicsTextItem method and make given text centered. */
  void setHtml(QString htmlText);
  
      /** Background color of tip */
  QColor bgColor() { return m_bgColor; }
  void setBgColor(QColor col) { m_bgColor = col; }
  
      /** This method align center content of item.
       * Put text before call it. */
  static void alignCenter(QGraphicsTextItem *tip);
      /** Adds drop shadow with defaults color/blur to an item. */
  static void setDropShadow(QGraphicsTextItem *tip);
  
private:
  QColor m_bgColor;
};

#endif // TGRAPHICSTEXTTIP_H
