/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#ifndef TPANEITEM_H
#define TPANEITEM_H


#include <nootkacoreglobal.h>
#include "tscoreitem.h"
#include <QtGui/qfont.h>


#if defined (Q_OS_ANDROID)
  #define WIDTH (3.5)
#else
  #define WIDTH (2.5)
#endif

class TnoteControl;


/**
 * @class TpaneItem is button like graphics object with singe symbol (letter)
 * existing on side panes of score note.
 */
class NOOTKACORE_EXPORT TpaneItem : public TscoreItem
{

friend TnoteControl;

  Q_OBJECT

public:
      /** @charNr is uni-code of desired symbol in Nootka font */
  explicit TpaneItem(int charNr, TscoreScene* sc);

  void setSymbol(int charNr) { m_symbol = QString(QChar(charNr)); update(); }

  void setColor(const QColor& c) { m_color = c; }
  QColor color() { return m_color; }

      /** By default Nootka font is set with size 3, bounding rectangle is calculated by font size as well */
  QFont font() { return m_font; }
  void setFont(const QFont& f);

  bool isSelected() { return m_selected; }
  void setSelected(bool sel) { m_selected = sel; update(); }

  void setStatusTip(const QString& stat) { m_statusText = stat; }
  QString& statusText() { return m_statusText; }

  qreal bottomY() { return y() + boundingRect().height(); } /**< Y coordinate of item bottom */

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;

signals:
  void clicked(); /**< Emitted when item is pressed - it does not change state of selection! */
  void entered(TpaneItem*);
  void leaved(TpaneItem*);


protected:
  virtual void mousePressEvent(QGraphicsSceneMouseEvent*) override;

  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
  virtual void touched(const QPointF&) override;

private:
  QString           m_symbol;
  QColor            m_color;
  QFont             m_font;
  bool              m_selected;
  QString           m_statusText;
};

#endif // TPANEITEM_H
