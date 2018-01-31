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

#ifndef TQUESTIONPOINT_H
#define TQUESTIONPOINT_H

#include "ttiphandler.h"
#include "tgroupedqaunit.h"

class QGraphicsSceneHoverEvent;


/** 
 * This class is a point on a chart. It has shape of note taken from nootka font. 
 * Also this class keeps colors for all Nootka Chart engine.
 * They are initialized in static setColors method
 */
class TquestionPoint : public TtipHandler
{

public:
  TquestionPoint(TqaPtr qaPtr);
  virtual ~TquestionPoint();
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
  TQAunit* question() { return m_qaPtr.qaPtr; }
  QColor color() { return m_color; }
    /** Number of a question. */
  unsigned int nr() { return m_qaPtr.nr; }
  
  static void setColors(QColor goodColor = Qt::green, QColor wrongColor = Qt::red,
                        QColor notBadColor = QColor("#FF8000"), QColor shadowColor = QColor(63, 63, 63, 180),
                        QColor bgColor = Qt::white);
  
  static QColor bgColor() { return m_bgColor; }
  static QColor shadowColor() { return m_shadowColor; } // shadow of tip and point
  static QColor wrongColor() { return m_wrongColor; }
  static QColor notBadColor() { return m_notBadColor; }
  static QColor goodColor() { return m_goodColor; }
  
protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
  
  
private:
  TqaPtr m_qaPtr;
  QColor m_color;
  
  static QColor m_goodColor, m_wrongColor, m_notBadColor, m_shadowColor, m_bgColor;
      /** Common method for constructors.
       * Determines color of a note point and sets shadow effect. */
  void setColor();
  
};

#endif // TQUESTIONPOINT_H
