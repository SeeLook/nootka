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
#include "tstafflinechart.h"
#include "ttipchart.h"
#include <QGraphicsScene>
#include <QDebug>
#include <QTimer>

TmainLine::TmainLine(Texam* exam, Tchart* chart) :
  m_exam(exam),
  m_chart(chart),
  m_tip(0)
{
  
  m_delTimer = new QTimer();
  connect(m_delTimer, SIGNAL(timeout()), this, SLOT(delayedDelete()));
  TstaffLineChart *lines[m_exam->count() - 1];
//   QGraphicsLineItem *ll[m_exam->count() - 1];
  
  for(int i = 0; i < m_exam->count(); i++) {
    double xPos = m_chart->xAxis->mapValue(i+1) + m_chart->xAxis->pos().x();
    m_points <<  new TquestionPoint(this, &m_exam->qusetion(i));
    m_chart->scene->addItem(m_points[i]);
    m_points[i]->setZValue(50);
    m_points[i]->setPos(xPos, m_chart->yAxis->mapValue((double)m_exam->qusetion(i).time / 10.0));
    if (i) {
      lines[i-1] = new TstaffLineChart();
//       ll[i-1] = new QGraphicsLineItem();
//       ll[i-1]->setPen(QPen(QBrush(Qt::darkBlue), 1));
      m_chart->scene->addItem(lines[i-1]);
//       m_chart->scene->addItem(ll[i-1]);
//       ll[i-1]->setLine(QLineF(m_points[i-1]->pos(), m_points[i]->pos()));
      lines[i-1]->setLine(m_points[i-1]->pos(), m_points[i]->pos());
    } 
  }
  
//   qDebug() << lines[0]->boundingRect();
  

}

TmainLine::~TmainLine() {
    delete m_delTimer;
}



void TmainLine::showTip(TQAunit* question, QPointF pos)
{
    if (m_tip)
        return;
    m_tip = new TtipChart(question);
    m_chart->scene->addItem(m_tip);
    m_tip->setPos(pos);
}

void TmainLine::deleteTip()
{
    if (!m_delTimer->isActive()) {
        m_delTimer->start(300);
    }
}

void TmainLine::delayedDelete() {
    m_delTimer->stop();
    m_chart->scene->removeItem(m_tip);
    delete m_tip;
    m_tip = 0;
    
}



