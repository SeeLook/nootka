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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TTIPCHART_H
#define TTIPCHART_H

#include <QGraphicsTextItem>
#include "tqatype.h"

struct stat;

class TquestionPoint;
class Tnote;
class TkeySignature;
class TfingerPos;

/** This clas represent tip displayed when user hovers cursor 
 * over question point in the chart. */
class TtipChart : public QGraphicsTextItem
{

public:
  TtipChart(TquestionPoint *point);
  virtual ~TtipChart();
  
  void setPos(QPointF p);
  static QString qaTypeText(TQAtype::Etype &type);
    /** Returns html sting with note pixmap generated from @param point. */
  static QString wrapPixToHtml( Tnote note, bool clef, TkeySignature key, double factor = 4);
  static QString insertQMark();
  static QString wrapPosToHtml(TfingerPos pos);
    /** Returns roman number or given fret */
  static QString romanFret(quint8 fret);
  static const QString fretsList[25];
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
  


  TquestionPoint *m_point;

  
};

#endif // TTIPCHART_H
