/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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

#ifndef TGRAPHICSTEXTTIP_H
#define TGRAPHICSTEXTTIP_H

#include <nootkacoreglobal.h>
#include <QGraphicsItem>
#include <QGraphicsScene>


/**
 * This is base class for tips.
 * It centering html text, paints background rounded rectangle in given color
 * and drops shadow. If bgColor is -1 the shadow is on a text.
 * @p baseColor() and @p frameColor() determine tip look.
 * Also it implements moving a tip with mouse. This is disable by default
 * and achieved through @p setTipMovable().
 * @class TgraphicsTextTip emits @p clicked() signal.
*/
class NOOTKACORE_EXPORT TgraphicsTextTip : public QGraphicsTextItem
{

  Q_OBJECT

public:
  TgraphicsTextTip(const QString& text, QColor bgColor = -1);
  TgraphicsTextTip();
  virtual ~TgraphicsTextTip();

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;

  void setHtml(const QString& htmlText); /**< Overwrites QGraphicsTextItem method and make given text centered. */

  void setScale(qreal sc);

  qreal realW() { return boundingRect().width() * scale(); } /**< boundingRect().width() * scale() */
  qreal realH() { return boundingRect().height() * scale(); } /**< boundingRect().height() * scale() */

      /** Sets current position respecting scene bounding rectangle.  */
  void setFixPos(qreal xx, qreal yy) {
      setPos(qBound(2.0, xx, scene()->width() - realW() - 5.0), qBound(2.0, yy, scene()->height() - realH() - 5.0));
  }
  void setFixPos(const QPointF& pp) { setFixPos(pp.x(), pp.y()); }

  QColor bgColor() { return m_bgColor; } /**< Colorized background color of tip */
  void setBgColor(const QColor& col); /**< Sets background color of tip */

  QColor baseColor() { return m_baseColor; } /**< Color of base background (palette base by default) */
  void setBaseColor(const QColor& bColor) { m_baseColor = bColor; update(); }

  QColor frameColor() { return m_frameColor; } /**< Color of tip frame (by default it is background color) */
  void setFrameColor(const QColor& fColor) { m_frameColor = fColor; update(); }

      /** This method align center content of item.
       * Put text before call it. */
  static void alignCenter(QGraphicsTextItem *tip);

      /** Adds drop shadow with defaults color/blur to an item. */
  static void setDropShadow(QGraphicsTextItem *tip, QColor shadowColor = -1);

  void setTipMovable(bool mov) { m_movable = mov; setAcceptHoverEvents(true); }
  bool isMovable() { return m_movable; }

      /** It overrides this method to handle mouse cursor over the link */
  void setTextInteractionFlags(Qt::TextInteractionFlags flags);

signals:
  void clicked(); /**< When item was clicked but doesn't move */
  void moved(); /**< When item was moved by user. setPoS() doesn't invoke it! */
  void entered(); /**< Mouse entered on tip */
  void leaved(); /**< Mouse leaved tip */

protected:
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event);
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event);
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event);

  void linkHoveredSlot(const QString & link);


private:
  QColor               m_bgColor, m_baseColor, m_frameColor;
  bool                 m_movable, m_mouseClick;
  QPointF             m_lastPos;
  Qt::CursorShape      m_lastLinkCursor;
};

#endif // TGRAPHICSTEXTTIP_H
