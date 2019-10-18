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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tmainline.h"
#include "tgroupedqaunit.h"
#include "txaxis.h"
#include "tyaxis.h"
#include "tchart.h"
#include "tstafflinechart.h"
#include "tquestionpoint.h"
#include <exam/tattempt.h>
#include <exam/texam.h>


inline qreal TmainLine::yValue(TQAunit* question, TmainLine::EyValue valType) {
  switch (valType) {
    case e_playedCount:
      return m_chart->yAxis->mapValue(question->totalPlayBacks());
    case e_prepareTime:
      return m_chart->yAxis->mapValue((double)question->attempt(0)->prepareTime() / 10.0);
    case e_attemptsCount:
      return m_chart->yAxis->mapValue(question->attemptsCount());
    case e_effectiveness:
      return m_chart->yAxis->mapValue(question->effectiveness());
    default:
      return m_chart->yAxis->mapValue(question->getTime()); // default - answer time
  }
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

TmainLine::TmainLine(QList<TQAunit*>* answers, Tchart* chart, TmainLine::EyValue yVal) :
  m_answers(answers),
  m_chart(chart)
{
  for(int i = 0; i < m_answers->size(); i++) {
    double xPos = m_chart->xAxis->mapValue(i + 1) + m_chart->xAxis->pos().x();
    TqaPtr tmpQA;
    tmpQA.qaPtr = m_answers->operator[](i);
    tmpQA.nr = i + 1;
    auto p = new TquestionPoint(tmpQA);
//     m_chart->setParentForItem(p);
    p->setZValue(50);
    p->setPos(xPos, yValue(m_answers->operator[](i), yVal));
    m_points << p;
    if (i) {
      auto line = new TstaffLineChart();
//       m_chart->setParentForItem(line);
      line->setLine(m_points[i - 1]->pos(), p->pos());
      line->setZValue(45);
      m_lines << line;
    }
  }
}


TmainLine::TmainLine(QList<TgroupedQAunit >& listOfLists, Tchart* chart, TmainLine::EyValue yVal) :
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
//       m_chart->setParentForItem(m_points[cnt]);
      m_points[cnt]->setZValue(50);
      m_points[cnt]->setPos(xPos, yValue(listOfLists[i].operator[](j).qaPtr, yVal));
      if (cnt) {
        auto line = new TstaffLineChart();
//         m_chart->setParentForItem(line);
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




