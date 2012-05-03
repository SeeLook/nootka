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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "txaxis.h"
#include "tqaunit.h"
#include "texam.h"
#include <QPainter>


TXaxis::TXaxis(Texam* exam) :
  m_qWidth(50)
{
  if (exam)
      setExam(exam);
  else {
      setLength(100);
      m_exam = 0;
  }
  axisScale = m_qWidth;
}

TXaxis::~TXaxis()
{}

void TXaxis::setExam(Texam* exam) {
  m_exam = exam;
  setLength(m_qWidth * m_exam->count());
  update(boundingRect());
}


void TXaxis::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  
    Q_UNUSED(option)
    Q_UNUSED(widget)
      
  QRectF rect = boundingRect();
  qreal half = axisWidth /  2.0;
  painter->drawLine(0, half, length(), half);
  drawArrow(painter, QPointF(length(), half), true);
  
//   if (m_qList->isEmpty()) // for empty questions list draw only axis
//       return;
  
  int b = length() / m_qWidth -1;
  for (int i=1; i <= b; i++) {
    painter->drawLine(i*m_qWidth, half, i*m_qWidth, half + tickSize);
    if (m_exam) {
        QString tt = QString("%1.\n%2").arg(i).arg(QString::fromStdString(m_exam->qusetion(i).qa.note.getName()));
        QRectF rr = rectBoundText(tt);
        painter->drawText(QRectF(i*50 - rr.width()/2, 0, rr.width(), rr.height()*3), Qt::AlignCenter, tt);
    }
  }
  
}

QRectF TXaxis::boundingRect()
{
  QRectF rect(0 ,0, length(), axisWidth*8);
  rect.translate(0, -axisWidth / 2.0);
  return rect;
}

