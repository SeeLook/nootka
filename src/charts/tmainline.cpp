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

#include "tmainline.h"
#include "texam.h"
#include "txaxis.h"
#include "tyaxis.h"
#include "tchart.h"
#include <QGraphicsScene>
#include <QDebug>

TmainLine::TmainLine(Texam* exam, Tchart* chart) :
  m_exam(exam),
  m_chart(chart)
{
  
  QGraphicsLineItem *lines[m_exam->count() - 1];
//   QPointF off(m_chart->xAxis->pos().x() , 0);
  QPointF off(0 , 0);
  
  for(int i = 0; i < m_exam->count(); i++) {
    double xPos = m_chart->xAxis->mapValue(i+1) + m_chart->xAxis->pos().x();
    m_points <<  new TquestionPoint(this, &m_exam->qusetion(i));
    m_chart->scene->addItem(m_points[i]);
    m_points[i]->setZValue(50);
//     m_points[i]->setPos(m_chart->xAxis->mapValue(i+1) + m_chart->xAxis->pos().x() - m_points[i]->boundingRect().width()/2, 
//                        m_chart->yAxis->mapValue((double)m_exam->qusetion(i).time / 10.0));
    m_points[i]->setPos(xPos - m_points[i]->boundingRect().width()/2, 
                       m_chart->yAxis->mapValue((double)m_exam->qusetion(i).time / 10.0));
    if (i) {
      lines[i-1] = new QGraphicsLineItem();
//       lines[i-1]->setPen(QPen(QBrush(Qt::darkBlue), 2));
      m_chart->scene->addItem(lines[i-1]);
      lines[i-1]->setLine(QLineF(m_points[i-1]->pos() + off, m_points[i]->pos() + off));
    } else
         off.setX(m_points[i]->boundingRect().width()/2);
  }
  

}

void TmainLine::showTip(TQAunit* question)
{
//   qDebug("show tip");
}

void TmainLine::deleteTip()
{

}


