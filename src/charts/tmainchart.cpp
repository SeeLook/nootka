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
#include "texamview.h"
#include "tmainline.h"
#include "txaxis.h"
#include "tyaxis.h"
#include "tstafflinechart.h"
#include "tgraphicsline.h"
#include <texamlevel.h>
#include <tnotename.h>
#include <QDebug>

TmainChart::TmainChart(Texam* exam, Tsettings &settings, QWidget* parent):
  Tchart(parent),
  m_exam(exam),
  m_settings(settings)
{
  setMouseTracking(true);

// Determine maximal rection time to prepare Y axis
  quint16 maxTime = 0;
  for(int i = 0; i < m_exam->count(); i++)
      maxTime = qMax(maxTime, m_exam->qusetion(i).time);
  yAxis->setMaxValue((double)maxTime / 10.0);
  
  if (m_settings.order == e_byNote) {
      TanswerListPtr goodAnsw, badAnsw;
      QList<TanswerListPtr> sortedLists;
      int goodSize; // number of lists with good answers
      if (m_settings.separateWrong) {
          divideGoodAndBad(m_exam->answList(), goodAnsw, badAnsw);
          sortedLists = sortByNote(goodAnsw);
          goodSize = sortedLists.size(); // number without wrong answers
          sortedLists.append(sortByNote(badAnsw));
      }
      else {
          TanswerListPtr convList = convertToPointers(m_exam->answList());
          sortedLists = sortByNote(convList);
          goodSize = sortedLists.size();
      }

      xAxis->setAnswersLists(sortedLists, m_exam->level());
      int ln = 0;
      for (int i = 0; i < sortedLists.size(); i++)
        ln += sortedLists[i].size();
      prepareChart(ln);
      m_mainLine = new TmainLine(sortedLists, this);
      int cnt = 1;
      // paint lines with average time of all the same notes
      for (int i = 0; i < goodSize; i++) { // skip wrong answers if separeted
        double aTime = calcAverTime(sortedLists[i], m_settings.inclWrongAnsw) / 10.0;
        TgraphicsLine *aNoteLine = new TgraphicsLine("<p>" + TexamView::averAnsverTimeTxt() + 
          QString("<br>%1<br>%2 s</p>").arg(tr("for a note:  ", "average reaction time for...") + "<span style=\"font-size: 20px;\"><b>" + TnoteName::noteToRichText(sortedLists[i].operator[](0)->qa.note) + "</b>").arg(aTime));
        scene->addItem(aNoteLine);
        aNoteLine->setZValue(46);
        aNoteLine->setPen(QPen(Qt::yellow, 3));
        aNoteLine->setLine(xAxis->mapValue(cnt - 0.4) + xAxis->pos().x(), yAxis->mapValue(aTime),
          xAxis->mapValue(cnt + sortedLists[i].size() -0.6) + xAxis->pos().x(), yAxis->mapValue(aTime));
        cnt += sortedLists[i].size();
      }
      cnt = 1;
      // paint highlights under grouped notes
      for (int i = 0; i < sortedLists.size(); i++) {
          QGraphicsRectItem *noteBg = new QGraphicsRectItem();
          scene->addItem(noteBg);
          QColor hiCol = palette().highlight().color();
          hiCol.setAlpha(30);
          if (i%2) {
            noteBg->setBrush(QBrush(hiCol));
            noteBg->setPen(Qt::NoPen);
            noteBg->setRect(xAxis->mapValue(cnt), 0, sortedLists[i].size() * xAxis->questWidth(), yAxis->boundingRect().height());
            noteBg->setZValue(-1);
          }
          cnt += sortedLists[i].size();
      }      
  }   

//   qDebug() << m_exam->userName() << "max time" << (double)maxTime / 10.0;
  
  if (m_settings.order == e_byNumber) {
      xAxis->setAnswersList(exam->answList(), exam->level());
      prepareChart(m_exam->count());
      m_mainLine = new TmainLine(m_exam->answList(), this);
      TgraphicsLine *averLine = new TgraphicsLine("<p>" +
          TexamView::averAnsverTimeTxt() + QString("<br><span style=\"font-size: 20px;\">%1 s</span></p>").arg(m_exam->averageReactonTime() / 10.0));
      scene->addItem(averLine);
//   averLine->setLine(QPointF(xAxis->mapValue(1), yAxis->mapValue(m_exam->averageReactonTime()/10.0)),
//     QPointF(xAxis->mapValue(m_exam->count()), yAxis->mapValue(m_exam->averageReactonTime()/10.0))
//   );
      averLine->setZValue(20);
      averLine->setPen(QPen(Qt::yellow, 3));
      averLine->setLine(xAxis->mapValue(1) + xAxis->pos().x(), yAxis->mapValue(m_exam->averageReactonTime()/10.0),
          xAxis->mapValue(m_exam->count()) + xAxis->pos().x(), yAxis->mapValue(m_exam->averageReactonTime()/10.0));
  }
  
}


TmainChart::~TmainChart()
{}

//####################################################################################
//##################### public method ################################################
//####################################################################################

double TmainChart::calcAverTime(TanswerListPtr& answers, bool skipWrong) {
  if (answers.isEmpty())
    return 0.0;
  double result = 0.0;
  int cnt = 0; // number of answers in average
  for (int i = 0; i < answers.size(); i++) {
    if (skipWrong && (answers.operator[](i)->wrongNote() || answers.operator[](i)->wrongPos()) ) 
      continue; // skip wrong answer
    else {
      result += answers.operator[](i)->time;
      cnt++;
    }
  }
  return result / cnt;
}

QList<Tnote> TmainChart::getTheSame(short int noteNr, TexamLevel* level) {
  Tnote workNote(noteNr); // natural or sharp by default
//   qDebug() << "main:" << QString::fromStdString(workNote.getName());
  QList<Tnote> nList;
  nList << workNote;
  bool doFlats = true, doDblAcc = true, doSharps = true;
  if (level){
    doFlats = level->withFlats;
    doDblAcc = level->withDblAcc;
    doSharps = level->withSharps;
  }
  Tnote xNote;
  if (doSharps) {
    xNote = workNote.showWithSharp();
    if (workNote != xNote) {
      nList.prepend(xNote);
//       qDebug() << QString::fromStdString(xNote.getName());
    }
  }
  if (nList.size() == 1) { // sharp not found
    if (doDblAcc) {
      xNote = workNote.showWithDoubleSharp();
      if (workNote != xNote) {
        nList.prepend(xNote);
//         qDebug() << QString::fromStdString(xNote.getName());
      }
    }
  }
  if (doFlats) {
    xNote = workNote.showWithFlat();
    if (workNote != xNote) {
      nList.append(xNote);
//       qDebug() << QString::fromStdString(xNote.getName());
    }
  }
  if (doDblAcc && nList.last().acidental != -1) { // flat not found
    xNote = workNote.showWithDoubleFlat();
    if (workNote != xNote) {
      nList.append(xNote);
//       qDebug() << QString::fromStdString(xNote.getName());
    }
  }
  return nList;
}


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
  for (short i = m_exam->level()->loNote.getChromaticNrOfNote(); i <= m_exam->level()->hiNote.getChromaticNrOfNote(); i++) {
    QList<Tnote> theSame = getTheSame(i, m_exam->level());
    for (int j = 0; j < theSame.size(); j++) {
      TanswerListPtr noteList;
      for (int k = 0; k < answList.size(); k++) {
        if (answList.operator[](k)->qa.note == theSame[j] && 
            !(answList.operator[](k)->questionAs == TQAtype::e_asFretPos && 
              answList.operator[](k)->questionAs == TQAtype::e_asFretPos)
          )
                  noteList << answList.operator[](k);
      }
      if (!noteList.isEmpty()) {
        result << noteList;
//         qDebug() << QString::fromStdString(theSame[j].getName()) << noteList.size();
      }
    }
  }
  if (m_exam->level()->questionAs.isFret() && m_exam->level()->answersAs[2].isFret()) {
      TanswerListPtr ignoredList; // ignore answers without notes
      for (int k = 0; k < answList.size(); k++)
            if (answList.operator[](k)->questionAs == TQAtype::e_asFretPos && 
              answList.operator[](k)->questionAs == TQAtype::e_asFretPos)
                      ignoredList << answList.operator[](k);
      if (!ignoredList.isEmpty())
        result << ignoredList; // add ignoredList at the end
//           qDebug() << ignoredList.size();
  }
  return result;
}

QList< TanswerListPtr > TmainChart::sortByFret(TanswerListPtr& answList) {

}


TanswerListPtr TmainChart::mergeListOfLists(QList<TanswerListPtr>& listOfLists) {
  TanswerListPtr result;
  for (int i = 0; i < listOfLists.size(); i++)
    for (int j = 0; j < listOfLists[i].size(); j++)
      result << listOfLists[i].operator[](j);
    
    return result;
}

TanswerListPtr TmainChart::convertToPointers(QList<TQAunit> *examList) {
    TanswerListPtr result;
    for (int i = 0; i< examList->size(); i++)
        result << &examList->operator [](i);
    return result;
}


//####################################################################################
//##################### private method ###############################################
//####################################################################################

void TmainChart::prepareChart(int maxX) {
  // Grid lines
  QColor lineColor = palette().foreground().color();
  for(int i = 5; i < maxX; i++) {
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
  QGraphicsSimpleTextItem *axisUnit = new QGraphicsSimpleTextItem();
  scene->addItem(axisUnit);
  axisUnit->setText("[s]");
  axisUnit->setPos(xAxis->pos().x() + 7, -5);

}

