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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tyaxis.h"
#include <exam/tqaunit.h>
#include <exam/textrans.h>
#include <QPainter>
#include <QWidget>
#include <QApplication>
#include <QGraphicsScene>



    /** When @p halfAllowed is true given time values are below 60 s for sure.
    * Minute part (@p mm) can be skipped then. */
QString timeFormated(qreal realTime, bool halfAllowed = false) {
  int t = (int)realTime;
  QString hh = "", mm = halfAllowed ? "" : "0", ss = "", ms = "";
  int dig = 0;
  if (t / 3600) {
    hh = QString("%1").arg(t / 3600);
    dig = 2;
  }
  int dig2 = 0;
  if ((t % 3600) / 60) {
    mm = QString("%1").arg((t % 3600) / 60, dig, 'i', 0, '0');
    dig2 = 2;
  }
  ss = QString("%1").arg((t % 3600) % 60, 2, 'i', 0, '0');
  if (realTime - (qreal)t)
    ms = "." + QString("%1").arg((int)((realTime - (qreal)t) * 10));
  return (hh.isEmpty() ? "" : hh + ":") + (mm.isEmpty() ? "" : mm + ":") + ss + ms;
}



TYaxis::TYaxis() :
  m_maxVal(11),
  m_multi(1),
  m_halfTick(false)
{
	m_textPosOffset = (rectBoundText("X").height() / 4);
	setUnit(e_timeInSec);
}


void TYaxis::setMaxValue(qreal val, bool allowHalf) {
	m_maxVal = val;
	qreal maxT = m_maxVal;
	while (maxT > 99) {
		m_multi = m_multi * 10;
		maxT = maxT / 10;
	}
	m_top = int(maxT) + 1 ;
	m_loop = m_top;
	m_multi2 = 1;
	if (m_top > 9) {
			m_loop = m_top / 10;
			m_multi2 = 10;
	}
	axisScale = ((length() - (2 * arrowSize)) / (m_top * m_multi));
	if (allowHalf) { // check is enough place for half ticks
		if ( ((mapValue((m_loop - 1) * m_multi * m_multi2) - mapValue(m_loop * m_multi * m_multi2))) > m_textPosOffset * 4)
			m_halfTick = true;
	}
}


void TYaxis::setUnit(TYaxis::Eunit unit) {
	switch (unit) {
		case e_timeInSec:
			m_unitDesc = QObject::tr("time [s]", "unit of Y axis");
			break;
		case e_questionNr:
			m_unitDesc = questionsNumberTxt() + " [ ]";
			break;
		case e_prepareTime:
			m_unitDesc = prepareTimeTxt() + " [s]";
			break;
		case e_attemptsCount:
			m_unitDesc = attemptsNumberTxt()  + " [ ]";
			break;
    case e_effectiveness:
      m_unitDesc = TexTrans::effectTxt().toLower()  + " [%]";
      break;
    case e_playedCount:
      m_unitDesc = playedNumberTxt() + " [ ]";
	}
	m_unit = unit;
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
  bg.setAlpha(210);
  painter->setPen(Qt::NoPen);
  painter->setBrush(bg);
  painter->drawRect(-3 * m_textPosOffset, 0, axisWidth * 4, length() - 1);
  painter->setPen(QPen(widget->palette().text().color(), 2));
  painter->drawLine(half, 0, half, length());
  drawArrow(painter, QPointF(half, 0), false);
  
  double shift = 1.0;
  if (m_halfTick && (m_unit == e_timeInSec || m_unit == e_prepareTime || m_multi2 >= 10))
      shift = 0.5;
  for (double i = shift; i <= m_loop; i += shift) {
      double v = i * m_multi * m_multi2;
      painter->drawLine(half, mapValue(v), half - tickSize, mapValue(v));
      QString ticText = QString::number(i * m_multi * m_multi2);
      if (m_unit == e_timeInSec)
        ticText = timeFormated(i * m_multi * m_multi2, m_halfTick);
      painter->drawText(half + 3, mapValue(v) + m_textPosOffset, ticText);
  }
  // paint top tick only if there is free room and Y value is not % (more than 100 makes no sense there)
  if (m_unit != e_effectiveness && ((mapValue(m_loop * m_multi * m_multi2) - mapValue(m_top * m_multi)) ) > m_textPosOffset * 4) {
      painter->drawLine(half, mapValue(m_top * m_multi), half - tickSize, mapValue(m_top * m_multi));
      QString ticText = QString::number(m_top * m_multi);
      if (m_unit == e_timeInSec)
        ticText = timeFormated(m_top * m_multi, m_halfTick);
      painter->drawText(half + 3, mapValue(m_top * m_multi) + m_textPosOffset, QString("%1").arg(ticText));
  }
  QFont f = painter->font();
  f.setBold(true);
  painter->setFont(f);
  painter->rotate(270);
  painter->drawText(QRectF(-length(), -3 * m_textPosOffset, length(), m_textPosOffset * 3), Qt::AlignCenter, m_unitDesc);
}


QRectF TYaxis::boundingRect() const{
  QRectF rect(4 * m_textPosOffset , 0, axisWidth + rectBoundText(QString::number(m_maxVal)).width() + 6 * m_textPosOffset, length());
  return rect;
}


void TYaxis::getYforGrid(QList<double>& yList) {
  yList.clear();
  double step = 1.0;
  if (m_halfTick && m_unit != e_attemptsCount && qAbs(mapValue(2 * m_multi * m_multi2) - mapValue(m_multi * m_multi2)) > 30)
    step = 0.5;
  for (double i = step; i <= m_loop; i += step)
    yList << mapValue(i * m_multi * m_multi2);
}
