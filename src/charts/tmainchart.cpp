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

#include "tmainchart.h"
#include "texam.h"
#include "tmainline.h"
#include "txaxis.h"
#include "tyaxis.h"
#include <QDebug>

TmainChart::TmainChart(Texam *exam, QWidget* parent): 
  Tchart(parent),
  m_exam(exam)
{
  setMouseTracking(true);
  
  xAxis->setExam(exam);
  
  
// Determine maximal rection time to prepare Y axis
  quint16 maxTime = 0;
  for(int i = 0; i < m_exam->count(); i++)
      maxTime =   qMax(maxTime, m_exam->qusetion(i).time);
  yAxis->setMaxValue((double)maxTime / 10.0);
  qDebug() << m_exam->userName() << "max time" << (double)maxTime / 10.0;
  
// Grid lines
  QColor lineColor = palette().foreground().color();
  lineColor.setAlpha(120);
  for(int i = 5; i < m_exam->count(); i++) {
    if (i%5 == 0)
      scene->addLine(xAxis->mapValue(i) + xAxis->pos().x(), 0,
        xAxis->mapValue(i) + xAxis->pos().x(), yAxis->length(), 
                     QPen(QBrush(lineColor), 1, Qt::DashLine));
  }
  QList<double> listY = yAxis->getYforGrid();
  if (listY.size()) {
      for(int i = 0; i < listY.size(); i++)
        scene->addLine(xAxis->pos().x(), listY[i],
        xAxis->pos().x() + xAxis->length(), listY[i], 
                     QPen(QBrush(lineColor), 1, Qt::DashLine));
  }  
  
  m_mainLine = new TmainLine(m_exam, this);
  
}


TmainChart::~TmainChart()
{}
