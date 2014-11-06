/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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

#include <nootkacoreglobal.h>
#include <QGraphicsItem>


/** 
 * This is base class for tips. 
 * It centering html text, paints background rounded rectangle in given color
 * and drops shadow. If bgColor is -1 the shadow is on a text.
 * Also it implements moving a tip with mouse. This is disable by default
 * 
*/
class NOOTKACORE_EXPORT TgraphicsTextTip : public QGraphicsTextItem
{

	Q_OBJECT
	
public:
  TgraphicsTextTip(QString text, QColor bgColor = -1);
  TgraphicsTextTip();
	virtual ~TgraphicsTextTip();
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
  
      /** Overwrites QGraphicsTextItem method and make given text centered. */
  void setHtml(QString htmlText);
  
  QColor bgColor() { return m_bgColor; } // Background color of tip
  void setBgColor(QColor col); // Background color of tip
  
      /** This method align center content of item.
       * Put text before call it. */
  static void alignCenter(QGraphicsTextItem *tip);
	
      /** Adds drop shadow with defaults color/blur to an item. */
  static void setDropShadow(QGraphicsTextItem *tip, QColor shadowColor = -1);
	
	void setTipMovable(bool mov) { m_movable = mov; setAcceptHoverEvents(true); }
	bool isMovable() { return m_movable; }
	
			/** It overrides this method to handle mouse cursor over the link */
	void setTextInteractionFlags(Qt::TextInteractionFlags flags);
	
protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);
	
	void linkHoveredSlot(const QString & link);
  
  
private:
  QColor 							m_bgColor;
	bool 								m_movable;
	QPointF 						m_lastPos;
	Qt::CursorShape			m_lastLinkCursor;
};

#endif // TGRAPHICSTEXTTIP_H
