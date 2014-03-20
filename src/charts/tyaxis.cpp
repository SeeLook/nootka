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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tyaxis.h"
#include <exam/tqaunit.h>
#include <QPainter>
#include <QWidget>
#include <QApplication>
#include <QGraphicsScene>


TYaxis::TYaxis() :
  m_maxVal(11),
  m_multi(1),
  m_halfTick(false)
{
    m_textPosOffset = (rectBoundText("X").height() / 4);
    setUnit(e_timeInSec);
}


void TYaxis::setMaxValue(qreal val) {
    m_maxVal = val;
    qreal maxT = m_maxVal;
    while (maxT > 99) {
      m_multi = m_multi*10;
      maxT = maxT / 10;
    }
    m_top = int(maxT) + 1 ;
    m_loop = m_top;
    m_multi2 = 1;
    if (m_top > 9) {
        m_loop = m_top / 10;
        m_multi2 = 10;
    }
    axisScale = ((length() - (2 * arrowSize)) / (m_top*m_multi));
    // check is enough place for half ticks
    if ( ((mapValue((m_loop-1)*m_multi*m_multi2) - mapValue(m_loop*m_multi*m_multi2))) > m_textPosOffset*4)
        m_halfTick = true;

}


void TYaxis::setUnit(TYaxis::Eunit unit) {
    switch (unit) {
      case e_timeInSec:
        m_unitDesc = QObject::tr("time [s]", "unit of Y axis");
        break;
      case e_questionNr:
        m_unitDesc = QApplication::translate("TanalysDialog", "Questions number") + " [ ]";
        break;
    }
}


QPainterPath TYaxis::shape() const {
	QPainterPath path;
	path.addRect(boundingRect().adjusted(0, 0, 0, scene()->height()));
	return path;
}


void TYaxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    Q_UNUSED(option)

    qreal half = axisWidth / 2;
    QColor bg = widget->palette().base().color();
    bg.setAlpha(200);
    painter->setPen(Qt::NoPen);
    painter->setBrush(bg);
    painter->drawRect(-3 * m_textPosOffset, 0, axisWidth * 3, length() - 1);
    painter->setPen(QPen(widget->palette().text().color(), 2));
    painter->drawLine(half, 0, half, length());
    drawArrow(painter, QPointF(half, 0), false);
    
    double shift = 1.0;
    if (m_halfTick && (m_unit == e_timeInSec || m_multi2 >= 10))
        shift = 0.5;
    for (double i = shift; i <= m_loop; i=i+shift) {
        double v= i*m_multi*m_multi2;
        painter->drawLine(half, mapValue(v), half - tickSize, mapValue(v));
        painter->drawText(half + 3, mapValue(v) + m_textPosOffset, QString::number(i*m_multi*m_multi2));
    }
    // paint top tick only if there is free room
    if ( ((mapValue(m_loop*m_multi*m_multi2) - mapValue(m_top*m_multi)) ) > m_textPosOffset*4) {
        painter->drawLine(half, mapValue(m_top*m_multi), half - tickSize, mapValue(m_top*m_multi));
        painter->drawText(half + 3, mapValue(m_top*m_multi) + m_textPosOffset, QString("%1").arg(m_top*m_multi));
    }
    QFont f = painter->font();
    f.setBold(true);
    painter->setFont(f);
    painter->rotate(270);
    painter->drawText(QRectF(-length(), -3 * m_textPosOffset, length(), m_textPosOffset * 3), Qt::AlignCenter, m_unitDesc);
}


QRectF TYaxis::boundingRect() const{
    QRectF rect(4 * m_textPosOffset , 0, axisWidth + rectBoundText(QString::number(m_maxVal)).width() + 3 * m_textPosOffset, length());
    return rect;
}


void TYaxis::getYforGrid(QList< double >& yList) {
  yList.clear();
  double step = 1.0;
  if (qAbs(mapValue(2*m_multi*m_multi2) - mapValue(m_multi*m_multi2)) > 30)
    step = 0.5;
  for (double i = step; i <= m_loop; i += step) {
    yList << mapValue(i*m_multi*m_multi2);
  }
}
