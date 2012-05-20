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

#ifndef TQUESTIONPOINT_H
#define TQUESTIONPOINT_H

#include <QGraphicsItem>

class TtipChart;
class TQAunit;
class TmainLine;
class QGraphicsSceneHoverEvent;


/** This class is a point on a chart. It has shape of note taken from nootka font.
 * It has pointer to question pointed by it and poiter to parent @class TmainLine.
 * It grabs hoverEnterEvent & hoverLeaveEvent and calls to TmainLine::showTip()
 * 
 * Also this class keeps colors for all Nootka Chart engine.
 * They are initialised in satic setColors method
 */
class TquestionPoint : public QGraphicsItem
{

public:
  TquestionPoint(TmainLine *parent,  TQAunit *question);
  virtual ~TquestionPoint() {}
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
  TQAunit* question() { return m_question; }
  QColor color() { return m_color; }
  
  static void setColors(QColor goodColor = Qt::green, QColor wrongColor = Qt::red,
                        QColor notBadColor = Qt::darkMagenta, QColor shadowColor = QColor(63, 63, 63, 180),
                        QColor bgColor = Qt::white);
  
  static QColor bgColor() { return m_bgColor; }
  static QColor shadowColor() { return m_shadowColor; } // shadow of tip and point
  
protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent *);
  
  
private:
  TQAunit *m_question;
  TmainLine *m_parent;
  QColor m_color;
  
  static QColor m_goodColor, m_wrongColor, m_notBadColor, m_shadowColor, m_bgColor;
  
};

#endif // TQUESTIONPOINT_H
