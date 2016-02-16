/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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

#ifndef TGRAPHICSLINE_H
#define TGRAPHICSLINE_H

#include <QGraphicsLineItem>
#include <QPen>
#include "ttiphandler.h"

class TgroupedQAunit;


/** This class represents a line on QGraphicsScene 
 * but it captures hover events and shows description text 
 * in TgraphicsTextTip. 
 * When text is empty, events are ignored. */
class TgraphicsLine : public TtipHandler
{

public:
  TgraphicsLine(TgroupedQAunit* qaGroup, QString text = "");
  TgraphicsLine(QString text = "");
  virtual ~TgraphicsLine();
    /** Sets a text of a tip appearing on hover event. */
  void setText(QString text) { m_text = text; }
  void setPen(QPen pen) { m_line->setPen(pen); }
  void setLine(qreal x1, qreal y1, qreal x2, qreal y2) { m_line->setLine(x1, y1, x2, y2); }
  
protected:
  virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
  
  
private:
  QString m_text;
  TgroupedQAunit *m_qaGroup;
  QGraphicsLineItem *m_line;
};

#endif // TGRAPHICSLINE_H
