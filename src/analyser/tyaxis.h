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

#ifndef TYAXIS_H
#define TYAXIS_H

#include <QGraphicsItem>


class TYaxis : public QGraphicsItem
{

public:
  
  TYaxis();
  virtual ~TYaxis();
  
  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);
  virtual QRectF boundingRect() const;
      /** Maximum value of a data on Y axis. Needs update(). */
  void setMaxValue(qreal val) { m_maxVal = val; }
  qreal maxValue() { return m_maxVal; }
      /** Sets a lenght of axis in pixels. Needs update() after. */
  void setLenght(int len) { m_lenght = len; }
      /**  Returns y position of given value in data units. */
  inline double mapToY(double val) { return val * m_yScale; }
  
  
private:
  qreal m_maxVal, m_multi;
  qreal m_topTick; // highest tick
  qreal m_yScale; // factor to calculate Y orign
  int m_lenght;
  int m_textPosOffset; // half of text height
  

};

#endif // TYAXIS_H
