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
#include <QGraphicsView>
#include <QDebug>
#include <QTimer>

TmainLine::TmainLine(QList< TQAunit >* answers, Tchart* chart) :
  m_answers(answers),
  m_chart(chart),
  m_tip(0)
{
  
  m_delTimer = new QTimer();
  connect(m_delTimer, SIGNAL(timeout()), this, SLOT(delayedDelete()));
//  TstaffLineChart *lines[m_exam->count() - 1];
   QGraphicsLineItem *ll[m_answers->size() - 1];
  
  for(int i = 0; i < m_answers->size(); i++) {
    double xPos = m_chart->xAxis->mapValue(i+1) + m_chart->xAxis->pos().x();
    m_points <<  new TquestionPoint(this, &m_answers->operator[](i));
    m_chart->scene->addItem(m_points[i]);
    m_points[i]->setZValue(50);
    m_points[i]->setPos(xPos, m_chart->yAxis->mapValue((double)m_answers->operator[](i).time / 10.0));
    if (i) {
//      lines[i-1] = new TstaffLineChart();
       ll[i-1] = new QGraphicsLineItem();
       ll[i-1]->setPen(QPen(QBrush(Qt::blue), 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
//       m_chart->scene->addItem(lines[i-1]);
       m_chart->scene->addItem(ll[i-1]);
       ll[i-1]->setLine(QLineF(m_points[i-1]->pos(), m_points[i]->pos()));
//      lines[i-1]->setLine(m_points[i-1]->pos(), m_points[i]->pos());
//      lines[i-1]->setZValue(45);
       ll[i-1]->setZValue(45);
    } 
  }
  
//   qDebug() << m_exam->answList()->operator[](1).time / 10.0;
  

}

TmainLine::~TmainLine() {
    delete m_delTimer;
}



void TmainLine::showTip(TquestionPoint *point) {
    if (m_tip)
        return;
    m_tip = new TtipChart(point);
    m_chart->scene->addItem(m_tip);
    QPointF p = point->pos();
    // determine where to display tip when point is near a view boundaries
    if (m_chart->mapFromScene(point->pos()).x() > (m_chart->width() / 2) )
        p.setX(p.x() - m_tip->boundingRect().width()/m_chart->transform().m11());
    if (m_chart->mapFromScene(point->pos()).y() > (m_chart->height() / 2) )
        p.setY(p.y() - m_tip->boundingRect().height()/m_chart->transform().m22());
    m_tip->setPos(p);
    m_curPoint = point;
}

void TmainLine::deleteTip()
{
    if (!m_delTimer->isActive()) {
        m_delTimer->start(30);
    }
}

void TmainLine::delayedDelete() {
    if (m_curPoint->isUnderMouse())
        return;
    m_delTimer->stop();
    if (m_tip) {
        m_chart->scene->removeItem(m_tip);
        delete m_tip;
        m_tip = 0;
    }
}



