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
 *  You should have received a copy of the GNU General Public License      *
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
#include "ttipchart.h"
#include "sorting.h"
#include "texamlevel.h"
#include "tquestionaswdg.h"
#include "tnotename.h"
#include <QDebug>


QColor averColor = QColor(0, 192, 192);

TmainChart::TmainChart(Texam* exam, Tsettings &settings, QWidget* parent):
  Tchart(parent),
  m_exam(exam),
  m_settings(settings),
  m_hasListUnrelated(false)
{
  setMouseTracking(true);

// Determine maximal rection time to prepare Y axis
  quint16 maxTime = 0;
  for(int i = 0; i < m_exam->count(); i++)
      maxTime = qMax(maxTime, m_exam->question(i).time);
  yAxis->setMaxValue((double)maxTime / 10.0);
  
  if (m_settings.order == e_byNumber) {
      xAxis->setAnswersList(exam->answList(), exam->level());
      prepareChart(m_exam->count());
      m_mainLine = new TmainLine(m_exam->answList(), this);
      /** NOTE
       * I'm not sure for now what to do with curve shows a progress
       * Let's see...
       * code below has wrong counting of average time
       * 
      //      QPolygonF polygon;
      double aTime = 0 , prev = 0;
      int firstCorrect = 0, okCount = 0;
      for(int i = 0; i < m_exam->count(); i++) { // looking for first correct answer
        if (!m_exam->question(i).isWrong()) {
          firstCorrect = i;
          prev = m_exam->question(i).time / 10.0;
          okCount++;
          aTime = prev;
          break;
        }
      }
      int prevX = firstCorrect + 1;
      for(int i = firstCorrect + 1; i < m_exam->count(); i++) {
        if (m_exam->question(i).isWrong())
          continue; // skip wrong answers in aver time
        else 
          aTime = (aTime * okCount + (m_exam->question(i).time / 10)) / (okCount + 1);
        
        okCount++;        
//        polygon << QPointF(xAxis->mapValue(i + 1), yAxis->mapValue(aTime));
        QGraphicsLineItem *averProgress = new QGraphicsLineItem;
        scene->addItem(averProgress);
        averProgress->setPen(QPen(averColor, 1));
        averProgress->setLine(xAxis->mapValue(prevX) + xAxis->pos().x(), yAxis->mapValue(prev),
                              xAxis->mapValue(i + 1) + xAxis->pos().x(), yAxis->mapValue(aTime));
        prevX = i + 1;
        averProgress->setZValue(10);
        prev = aTime;
      }
      qDebug() << aTime << m_exam->averageReactonTime() / 10.0;
      */
      TgraphicsLine *averLine = new TgraphicsLine("<p>" +
          TexamView::averAnsverTimeTxt() + 
          QString("<br><span style=\"font-size: 20px;\">%1</span></p>").arg(TexamView::formatReactTime(m_exam->averageReactonTime(), true)) );
      scene->addItem(averLine);
      averLine->setZValue(20);
      averLine->setPen(QPen(averColor, 3));
      averLine->setLine(xAxis->mapValue(1) + xAxis->pos().x(), yAxis->mapValue(m_exam->averageReactonTime() / 10.0),
          xAxis->mapValue(m_exam->count()) + xAxis->pos().x(), yAxis->mapValue(m_exam->averageReactonTime() / 10.0));
  }
  
  if (m_settings.order == e_byNote || m_settings.order == e_byFret ||
          m_settings.order == e_byKey || m_settings.order == e_byAccid) {
      QList<char> kindOfAccids;
      TanswerListPtr goodAnsw, badAnsw;
      QList<TanswerListPtr> sortedLists;
      int goodSize; // number of lists with good answers
      if (m_settings.separateWrong) {
          divideGoodAndBad(m_exam->answList(), goodAnsw, badAnsw);
          if (m_settings.order == e_byNote)
            sortedLists = sortByNote(goodAnsw, m_exam->level(), m_hasListUnrelated);
          else 
            if (m_settings.order == e_byFret)
              sortedLists = sortByFret(goodAnsw, m_exam->level(), m_hasListUnrelated);
            else
              if (m_settings.order == e_byKey)
                sortedLists = sortByKeySignature(goodAnsw, m_exam->level(), m_hasListUnrelated);
              else
                if (m_settings.order == e_byAccid)
                sortedLists = sortByAccidental(goodAnsw, m_exam->level(), m_hasListUnrelated, kindOfAccids);
          goodSize = sortedLists.size(); // number without wrong answers
          if (m_settings.order == e_byNote)
            sortedLists.append(sortByNote(badAnsw, m_exam->level(), m_hasListUnrelated));
          else
            if (m_settings.order == e_byFret)
              sortedLists.append(sortByFret(badAnsw, m_exam->level(), m_hasListUnrelated));
            else
              if (m_settings.order == e_byKey)
                sortedLists.append(sortByKeySignature(badAnsw, m_exam->level(), m_hasListUnrelated));
              else
                if (m_settings.order == e_byAccid)
                sortedLists.append(sortByAccidental(badAnsw, m_exam->level(), m_hasListUnrelated, kindOfAccids));
                
      }
      else {
          TanswerListPtr convList = convertToPointers(m_exam->answList());
          if (m_settings.order == e_byNote)
            sortedLists = sortByNote(convList, m_exam->level(), m_hasListUnrelated);
          else
            if (m_settings.order == e_byFret)
              sortedLists = sortByFret(convList, m_exam->level(), m_hasListUnrelated);
            else
              if (m_settings.order == e_byKey)
                sortedLists = sortByKeySignature(convList, m_exam->level(), m_hasListUnrelated);
              else
                if (m_settings.order == e_byAccid)
                  sortedLists = sortByAccidental(convList, m_exam->level(), m_hasListUnrelated, kindOfAccids);
          goodSize = sortedLists.size();
      }

      xAxis->setAnswersLists(sortedLists, m_exam->level());
      int ln = 0;
      for (int i = 0; i < sortedLists.size(); i++)
        ln += sortedLists[i].size();
      prepareChart(ln);
      m_mainLine = new TmainLine(sortedLists, this);
      
      int goodOffset = 0; // 0 when not unrelated question list inside
      if (m_hasListUnrelated)
        goodOffset = -1; // do not perform a last loop 
      int cnt = 1;
  // paint lines with average time of all the same notes/frets
      for (int i = 0; i < goodSize + goodOffset; i++) { // skip wrong answers if separeted
        double aTime = calcAverTime(sortedLists[i], !m_settings.inclWrongAnsw);
        QString aTimeText = TexamView::formatReactTime(qRound(aTime), true);
        TgraphicsLine *averTimeLine = new TgraphicsLine();
        QString lineText = "";
        if (m_settings.order == e_byNote)
          lineText += "<p>" + TexamView::averAnsverTimeTxt() + QString("<br>%1<br>%2</p>").arg(tr("for a note:", "average reaction time for...") + "<span style=\"font-size: 20px;\">  <b>" + TnoteName::noteToRichText(sortedLists[i].operator[](0)->qa.note) + "</b>").arg(aTimeText);
        else
          if (m_settings.order == e_byFret)
            lineText += "<p>" + TexamView::averAnsverTimeTxt() + QString("<br>%1<br>%2</p>").arg(tr("for a fret:", "average reaction time for...") + "<span style=\"font-size: 20px;\"><b>  " + 
            QString::number(sortedLists[i].operator[](0)->qa.pos.fret()) + "</b>").arg(aTimeText);
          else
            if (m_settings.order == e_byKey) {
              QString wereKeys = "";
              if (m_exam->level()->manualKey && sortedLists[i].operator[](0)->answerAs == TQAtype::e_asNote)
                wereKeys = "<br>" + tr("Key signatures gave by user");
                
              lineText += "<p>" + TexamView::averAnsverTimeTxt() + QString("<br>%1<br>%2%3</p>").arg(tr("for a key:", "average reaction time for...") + "<span style=\"font-size: 20px;\">  <b>" + 
              sortedLists[i].operator[](0)->key.getName() + "</b></span>").arg(aTimeText).arg(wereKeys);
            } else
              if (m_settings.order == e_byAccid) {
                QString accStr, accidClue;
                accStr = accidToNotka(kindOfAccids[i]);
                if (kindOfAccids[i])
                    accidClue = tr("for an accidental:", "average reaction time for...") + "<span style=\"font-size: 20px;\">  " +
                        accStr + "</span>";
                else
                    accidClue = tr("for notes without accidentals"); 
                lineText += "<p>" + TexamView::averAnsverTimeTxt() + 
                QString("<br><span style=\"font-size: 20px;\">%1<br>%2</span></p>").arg(accidClue).arg(aTimeText);
              }
        
        averTimeLine->setText(lineText);
        scene->addItem(averTimeLine);
        averTimeLine->setZValue(46);
        averTimeLine->setPen(QPen(QColor(0, 192, 192), 3)); // sea blue
        averTimeLine->setLine(xAxis->mapValue(cnt - 0.4) + xAxis->pos().x(), yAxis->mapValue(aTime / 10),
          xAxis->mapValue(cnt + sortedLists[i].size() -0.6) + xAxis->pos().x(), yAxis->mapValue(aTime / 10));
        cnt += sortedLists[i].size();
      }
      cnt = 1;
  // paint highlights under grouped items
      for (int i = 0; i < sortedLists.size(); i++) {
          QGraphicsRectItem *groupBg = new QGraphicsRectItem();
          scene->addItem(groupBg);
          QColor hiCol = palette().highlight().color();
          hiCol.setAlpha(30);
          if (i%2) {
            groupBg->setBrush(QBrush(hiCol));
            groupBg->setPen(Qt::NoPen);
            groupBg->setRect(xAxis->mapValue(cnt), 0, sortedLists[i].size() * xAxis->questWidth(), yAxis->boundingRect().height());
            groupBg->setZValue(-1);
          }
          cnt += sortedLists[i].size();
      }
  // fret number over the chart
    if (m_settings.order == e_byFret) {
      cnt = 1;
      for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *fretText = new QGraphicsTextItem();
        QFont f;
        f.setPixelSize(30);
        fretText->setFont(f);
        QString hintText = "<b style=\"color: rgba(200, 200, 200, 200); \">";
        if (goodOffset && (i == goodSize -1))
          hintText += tr("questions unrelated<br>with chart type");
        else
          hintText += QString("%1</b>").arg(TfingerPos::romanFret(sortedLists[i].operator[](0)->qa.pos.fret()));
        hintText += "</b>";          
        fretText->setHtml(hintText);
        scene->addItem(fretText);
        TgraphicsTextTip::alignCenter(fretText);
        fretText->setPos(xAxis->mapValue(cnt) + 
        (sortedLists[i].size() * xAxis->questWidth() - fretText->boundingRect().width()) / 2, 
                         yAxis->mapValue(yAxis->maxValue()));        
        fretText->setZValue(3);
        
        cnt += sortedLists[i].size();
      }      
    }
  // key signature names over the chart
    if (m_settings.order == e_byKey) {
      cnt = 1;
      for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *keyText = new QGraphicsTextItem();
        QFont f;
        f.setPixelSize(16);
        keyText->setFont(f);
        QString hintText = "<b style=\"color: rgba(200, 200, 200, 200); \">";
        if (goodOffset && (i == goodSize -1))
          hintText += tr("questions unrelated<br>with chart type") + "</b>";
        else {
            hintText += QString("%1").arg(sortedLists[i].operator[](0)->key.getName());
            hintText += "<br><span style=\"font-family: nootka; font-size: 20px\">";
            if (sortedLists[i].operator[](0)->answerAs == TQAtype::e_asNote)
              hintText += "!";
            else
              hintText += "?";
            hintText += "</span></b>";
        }
        keyText->setHtml(hintText);
        scene->addItem(keyText);
        TgraphicsTextTip::alignCenter(keyText);
        keyText->setPos(xAxis->mapValue(cnt) + 
        (sortedLists[i].size() * xAxis->questWidth() - keyText->boundingRect().width()) / 2, 
                         yAxis->mapValue(yAxis->maxValue()));        
        keyText->setZValue(3);
        cnt += sortedLists[i].size();
      }
    }
// accidentals over the chart
    if (m_settings.order == e_byAccid) {
      cnt = 1;
      for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *accidentalText = new QGraphicsTextItem();
        QFont f;
        f.setPixelSize(30);
        accidentalText->setFont(f);
        QString hintText = "<span style=\"color: rgba(200, 200, 200, 200); \">";
        if (goodOffset && (i == goodSize -1))
          hintText += tr("questions unrelated<br>with chart type") + "</span>";
        else 
          if (kindOfAccids[i])
            hintText += QString("%1").arg(accidToNotka(kindOfAccids[i], 40));
          else
            hintText += tr("without accidentals");
        hintText += "</span>";
        accidentalText->setHtml(hintText);
        scene->addItem(accidentalText);
        TgraphicsTextTip::alignCenter(accidentalText);
        accidentalText->setPos(xAxis->mapValue(cnt) + 
        (sortedLists[i].size() * xAxis->questWidth() - accidentalText->boundingRect().width()) / 2, 
                         yAxis->mapValue(yAxis->maxValue()));        
        accidentalText->setZValue(3);
        cnt += sortedLists[i].size();
      }      
    }
  
    
    
  }

}


TmainChart::~TmainChart()
{}

//####################################################################################
//##################### public method ################################################
//####################################################################################







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
  axisUnit->setBrush(QColor(palette().text().color()));
  scene->addItem(axisUnit);
  axisUnit->setText("[s]");
  axisUnit->setPos(xAxis->pos().x() + 7, -5);

}

