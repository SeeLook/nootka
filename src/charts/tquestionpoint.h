/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk      	                	   	   *
 *   tomaszbojczuk@gmail.com   						                       *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TQUESTIONPOINT_H
#define TQUESTIONPOINT_H

#include <QGraphicsItem>

class TQAunit;
class TmainLine;
class QGraphicsSceneHoverEvent;

class TquestionPoint : public QGraphicsItem
{

public:
  TquestionPoint(TmainLine *parent,TQAunit *question);
  virtual ~TquestionPoint() {}
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
  TQAunit* question() { return m_question; } 
  
  static void setColors(QColor goodColor = Qt::green, QColor wrongColor = Qt::red, QColor notBadColor = Qt::darkMagenta);
  
protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  void hoverLeaveEvent(QGraphicsSceneHoverEvent * event);
  
  
private:
  TQAunit *m_question;
  TmainLine *m_parent;
  QColor m_color;
  
  static QColor m_goodColor, m_wrongColor, m_notBadColor;
  
};

#endif // TQUESTIONPOINT_H
