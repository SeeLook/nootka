/***************************************************************************
 *   Copyright (C) 2012-2018 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TSTAFFLINECHART_H
#define TSTAFFLINECHART_H


#include <QtWidgets/qgraphicsitem.h>



/**
 * This graphics item paints staff (five lines) from
 * QPointF @param from to QPointF @param to points.
 * Its 0,0 orign is at the begin of middle line.
 */
class TstaffLineChart : public QGraphicsItem
{

public:

  TstaffLineChart();
  virtual ~TstaffLineChart() {}
  void setLine(QPointF from, QPointF to);

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr);
  virtual QRectF boundingRect() const;
  virtual QPainterPath shape() const;

private:
  QPoint m_vector;
};

#endif // TSTAFFLINECHART_H
