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
#include <texamview.h>
#include "tmainline.h"
#include "txaxis.h"
#include "tyaxis.h"
#include "tstafflinechart.h"
#include "tgraphicsline.h"
#include <texamlevel.h>
#include <QDebug>

TmainChart::TmainChart(Texam* exam, Tchart::EanswersOrder order, QWidget* parent): 
  Tchart(parent),
  m_exam(exam),
  m_analysType(order)
{
  setMouseTracking(true);
  
  if (m_analysType == e_byNote) {
      TanswerListPtr goodAnsw, badAnsw;
      divideGoodAndBad(m_exam->answList(), goodAnsw, badAnsw);
      sortByNote(goodAnsw);
      
      
  } /*else  */
    xAxis->setAnswersList(exam->answList(), exam->level());
    
  prepareChart();
  
// Determine maximal rection time to prepare Y axis
  quint16 maxTime = 0;
  for(int i = 0; i < m_exam->count(); i++)
      maxTime =   qMax(maxTime, m_exam->qusetion(i).time);
  yAxis->setMaxValue((double)maxTime / 10.0);
//   qDebug() << m_exam->userName() << "max time" << (double)maxTime / 10.0;
  
  
  m_mainLine = new TmainLine(m_exam->answList(), this);
  TgraphicsLine *averLine = new TgraphicsLine(
    "<p style=\"font-size: 20px;\">" + TexamView::averAnsverTimeTxt() + QString("<br>%1</p>").arg(m_exam->averageReactonTime()));
  scene->addItem(averLine);
//   averLine->setLine(QPointF(xAxis->mapValue(1), yAxis->mapValue(m_exam->averageReactonTime()/10.0)),
//     QPointF(xAxis->mapValue(m_exam->count()), yAxis->mapValue(m_exam->averageReactonTime()/10.0))
//   );
  averLine->setZValue(20);
  averLine->setPen(QPen(Qt::yellow, 3));
  averLine->setLine(xAxis->mapValue(1) + xAxis->pos().x(), yAxis->mapValue(m_exam->averageReactonTime()/10.0),
    xAxis->mapValue(m_exam->count()) + xAxis->pos().x(), yAxis->mapValue(m_exam->averageReactonTime()/10.0));
  
  QGraphicsSimpleTextItem *axisUnit = new QGraphicsSimpleTextItem();
  scene->addItem(axisUnit);
  axisUnit->setText("[s]");
  axisUnit->setPos(xAxis->pos().x() + 7, -5);
}


TmainChart::~TmainChart()
{}

//####################################################################################
//##################### public method ################################################
//####################################################################################
/*
void TmainChart::setAnalyse(TmainChart::EanswersOrder order) {
  switch (order) {
    case e_byNumber:
      doAnalyseByNumber();
      break;
    case e_byNote:
      doAnalyseByNote();
      break;
  }
}*/

void TmainChart::divideGoodAndBad(QList< TQAunit >* list, TanswerListPtr& goodList, TanswerListPtr& badList) {
  for (int i = 0; i < list->size(); i++) {
    if (list->operator[](i).wrongNote() || list->operator[](i).wrongPos())
      badList << &list->operator[](i);
    else
      goodList << &list->operator[](i);
  }
}


QList<TanswerListPtr> TmainChart::sortByNote(TanswerListPtr& answList) {
  QList<TanswerListPtr> result;
  qDebug() << (int)m_exam->level()->loNote.getChromaticNrOfNote() << (int)m_exam->level()->hiNote.getChromaticNrOfNote();
  for (int i = m_exam->level()->loNote.getChromaticNrOfNote(); i <= m_exam->level()->hiNote.getChromaticNrOfNote(); i++) {
    qDebug() << i;
    TnotesList notesL = Tnote(i).getTheSameNotes(m_exam->level()->withDblAcc);
    TnotesList::iterator m = notesL.begin();
    TanswerListPtr sameNotesList;
    while( m != notesL.end() ){
      ++m;
      sameNotesList.clear();
      for (int i = 0; i < answList.size(); i++) {
        if (answList.operator[](i)->qa.note == *(m) )
          sameNotesList << answList.operator[](i);
      }
      if (!sameNotesList.isEmpty()) {
  //       result << al;
        qDebug() << QString::fromStdString((*m).getName()) << sameNotesList.size();
      }
    }
  }
  return result;
}


void TmainChart::doAnalyseByNumber() {

}


void TmainChart::doAnalyseByNote() {
  

}

//####################################################################################
//##################### private method ###############################################
//####################################################################################

void TmainChart::prepareChart() {
  // Grid lines
  QColor lineColor = palette().foreground().color();
  for(int i = 5; i < m_exam->count(); i++) {
    if (i%5 == 0)
      scene->addLine(xAxis->mapValue(i) + xAxis->pos().x(), 0,
        xAxis->mapValue(i) + xAxis->pos().x(), yAxis->length(), 
                     QPen(QBrush(lineColor), 1, Qt::DashLine));
  }
  QList<double> listY;
  yAxis->getYforGrid(listY);
  if (listY.size()) {
      for(int i = 0; i < listY.size(); i++)
        scene->addLine(xAxis->pos().x(), listY[i],
        xAxis->pos().x() + xAxis->length(), listY[i],
                     QPen(QBrush(lineColor), 1, Qt::DashLine));
  }  


}

