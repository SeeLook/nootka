/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tbarchart.h"
#include "tyaxis.h"
#include "txaxis.h"
#include "tbar.h"
#include "texam.h"

TbarChart::TbarChart(Texam* exam, Tchart::Tsettings& settings, QWidget* parent) :
  TmainChart(exam, settings, parent)
{
    chartSett.separateWrong = false;
    sort();
    qreal maxTime = 0;
    if (chartSett.order != Tchart::e_byMistake) {
      for(int i = 0; i < sortedLists.size(); i++)
        maxTime = qMax(maxTime, sortedLists[i].averTime());
      yAxis->setMaxValue((double)maxTime / 10.0);
    }
    else { // For e_byMistake we are looking for highest amount of questions in a group
      for(int i = 0; i < sortedLists.size(); i++)
        maxTime = qMax(maxTime, (qreal)sortedLists[i].size());
      yAxis->setMaxValue((double)maxTime);
      yAxis->setUnit(TYaxis::e_questionNr);
    }
    
    xAxis->setAnswersForBarChart(sortedLists);
    prepareChart(sortedLists.size());
    
    for(int i = 0; i < sortedLists.size(); i++) {
      Tbar *bar;
      if (chartSett.order != Tchart::e_byMistake)
        bar = new Tbar(yAxis->axisFactor() * (sortedLists[i].averTime() / 10.0), &sortedLists[i]);
      else // Y value is number of questions, bar generates tip and has to know about type to skip some data
        bar = new Tbar(yAxis->axisFactor() * (sortedLists[i].size()), &sortedLists[i], TstatisticsTip::e_mistakes);
      scene->addItem(bar);
      bar->setPos(xAxis->mapValue(i + 1) + xAxis->pos().x(), yAxis->boundingRect().height() - 2);
    }
}

TbarChart::~TbarChart(){}

