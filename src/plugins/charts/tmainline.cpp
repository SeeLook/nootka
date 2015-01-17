/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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

#include "tmainline.h"
#include <exam/texam.h>
#include <exam/tgroupedqaunit.h>
#include <exam/tattempt.h>
#include "txaxis.h"
#include "tyaxis.h"
#include "tchart.h"
#include "tstafflinechart.h"
#include "tquestionpoint.h"
#include <QGraphicsScene>
#include <QGraphicsView>
// #include <QDebug>


TmainLine::TmainLine(QList<TQAunit>* answers, Tchart* chart, TmainLine::EyValue yVal) :
  m_answers(answers),
  m_chart(chart)
{  
  for(int i = 0; i < m_answers->size(); i++) {
    double xPos = m_chart->xAxis->mapValue(i + 1) + m_chart->xAxis->pos().x();
    TqaPtr tmpQA;
    tmpQA.qaPtr = &m_answers->operator[](i);
    tmpQA.nr = i + 1;
    m_points <<  new TquestionPoint(tmpQA);
    m_chart->scene->addItem(m_points[i]);
    m_points[i]->setZValue(50);
		qreal yy;
    switch (yVal) {
      case e_playedCount:
        yy = m_chart->yAxis->mapValue(m_answers->operator[](i).totalPlayBacks());
        break;
      case e_prepareTime:
        yy = m_chart->yAxis->mapValue(m_answers->operator[](i).attempt(0)->prepareTime());
        break;
      case e_attemptsCount:
        yy = m_chart->yAxis->mapValue(m_answers->operator[](i).attemptsCount());
        break;
      case e_effectiveness:
        yy = m_chart->yAxis->mapValue(m_answers->operator[](i).effectiveness());
        break;
      default:
        yy = m_chart->yAxis->mapValue(m_answers->operator[](i).getTime()); // default - answer time 
        break;
    }
    m_points[i]->setPos(xPos, yy);
    if (i) {
      TstaffLineChart *line = new TstaffLineChart();
        m_chart->scene->addItem(line);
      line->setLine(m_points[i - 1]->pos(), m_points[i]->pos());
      line->setZValue(45);
      m_lines << line;
    } 
  }  
}


TmainLine::TmainLine(QList<TgroupedQAunit>& listOfLists, Tchart* chart) :
  m_chart(chart)
{
	int ln = 0, cnt = 0;
	for (int i = 0; i < listOfLists.size(); i++) {
		ln += listOfLists[i].size();
	}
  
  for(int i = 0; i < listOfLists.size(); i++) {
    for (int j = 0; j < listOfLists[i].size(); j++) {
        double xPos = m_chart->xAxis->mapValue(cnt+1) + m_chart->xAxis->pos().x();
        m_points << new TquestionPoint(listOfLists[i].operator[](j));
        m_chart->scene->addItem(m_points[cnt]);
        m_points[cnt]->setZValue(50);
        m_points[cnt]->setPos(xPos, m_chart->yAxis->mapValue(listOfLists[i].operator[](j).qaPtr->getTime()));
        if (cnt) {
					TstaffLineChart *line = new TstaffLineChart();
						m_chart->scene->addItem(line);
					line->setLine(m_points[cnt-1]->pos(), m_points[cnt]->pos());
					line->setZValue(45);
					m_lines << line;
        } 
        cnt++;
    }
  }
}


TmainLine::~TmainLine() {
    m_points.clear(); // clear a scene from deleted elements
    m_lines.clear();
}




