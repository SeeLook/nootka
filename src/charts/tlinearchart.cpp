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


#include "tlinearchart.h"
#include "tgraphicsline.h"
#include "txaxis.h"
#include "sorting.h"
#include "tgraphicstexttip.h"
#include "tyaxis.h"
#include "tstatisticstip.h"
#include "texamview.h"
#include "texam.h"
#include "tnotename.h"
#include "texamlevel.h"
#include <QApplication>
#include <QDebug>


QColor averColor = QColor(0, 192, 192);

TlinearChart::TlinearChart(Texam* exam, Tchart::Tsettings& settings, QWidget* parent) :
    TmainChart(exam, settings, parent)
{
// Determine maximal rection time to prepare Y axis
  quint16 maxTime = 0;
  for(int i = 0; i < exam->count(); i++)
      maxTime = qMax(maxTime, exam->question(i).time);
  yAxis->setMaxValue((double)maxTime / 10.0);
  
    if (settings.order == e_byNumber) {
      xAxis->setAnswersList(currExam->answList(), currExam->level());
      prepareChart(currExam->count());
      m_mainLine = new TmainLine(currExam->answList(), this);
      //      QPolygonF polygon;
      double aTime = 0 , prev = 0;
      int firstCorrect = 0, okCount = 0;
      for(int i = 0; i < exam->count(); i++) { // looking for first correct answer
        if (!exam->question(i).isWrong() || (!exam->question(i).isWrong() && settings.inclWrongAnsw)) {
          firstCorrect = i;
          prev = exam->question(i).time;
          okCount++;
          aTime = prev;
          break;
        }
      }
      int prevX = firstCorrect + 1;
      for(int i = firstCorrect + 1; i < exam->count(); i++) {
        if (exam->question(i).isWrong())
          continue; // skip wrong answers in aver time
        else 
          aTime = (aTime * okCount + (exam->question(i).time)) / (okCount + 1);
        
        okCount++;        
//        polygon << QPointF(xAxis->mapValue(i + 1), yAxis->mapValue(aTime));
        TgraphicsLine *averProgress = new TgraphicsLine("<b>" + QApplication::translate("TlinearChart", "PROGRESS LINE") + "</b><br>" +
          QString("<span style=\"color: %1; font-size: 20px;\"><b> \\ </b></span>").arg(averColor.name()) +
          QApplication::translate("TlinearChart", "descending - you go better") + "<br>" + 
          QString("<span style=\"color: %1; font-size: 20px;\"><b> / </b></span>").arg(averColor.name()) +
          QApplication::translate("TlinearChart", "acending - you thinking much") + "<hr>" + TexamView::averAnsverTimeTxt() + "<br>" +
//           QApplication::translate("TlinearChart", "for answer nr") + QString(" <b>%1</b>: <b>%2 s</b>").arg(i + 1).arg(aTime / 10.0) +"<br>" + 
          QApplication::translate("TlinearChart", "for whole exam:") +
          QString(" <span style=\"font-size: 20px;\">%1</span></p>").arg(TexamView::formatReactTime(exam->averageReactonTime(), true))
        );
        scene->addItem(averProgress);
        averProgress->setPen(QPen(averColor, 3));
        averProgress->setLine(xAxis->mapValue(prevX) + xAxis->pos().x(), yAxis->mapValue(prev / 10.0),
                              xAxis->mapValue(i + 1) + xAxis->pos().x(), yAxis->mapValue(aTime / 10.0));
        prevX = i + 1;
        averProgress->setZValue(10);
        prev = aTime;
      }
//       qDebug() << aTime << exam->averageReactonTime() / 10.0;
      
      
      if (exam->averageReactonTime() > 0) {
          TgraphicsLine *averLine = new TgraphicsLine(0, "<p>" +
              TexamView::averAnsverTimeTxt() + 
              QString("<br><span style=\"font-size: 20px;\">%1</span></p>").arg(TexamView::formatReactTime(exam->averageReactonTime(), true)) );
          scene->addItem(averLine);
          averLine->setZValue(20);
          averLine->setPen(QPen(averColor.darker(120), 1));
          averLine->setLine(xAxis->mapValue(1) + xAxis->pos().x(), yAxis->mapValue(exam->averageReactonTime() / 10.0),
              xAxis->mapValue(exam->count()) + xAxis->pos().x(), yAxis->mapValue(exam->averageReactonTime() / 10.0));
      }
  }
  
  if (settings.order == e_byNote || settings.order == e_byFret ||
          settings.order == e_byKey || settings.order == e_byAccid || 
          settings.order == e_byQuestAndAnsw || settings.order == e_byMistake) {
      sort();
      xAxis->setAnswersLists(sortedLists, exam->level());
      int ln = 0;
      for (int i = 0; i < sortedLists.size(); i++)
        ln += sortedLists[i].size();
      prepareChart(ln);
      m_mainLine = new TmainLine(sortedLists, this);
      
      int goodOffset = 0; // 0 when not unrelated question list inside
      if (hasListUnrelated)
        goodOffset = -1; // do not perform a last loop 
      int cnt = 1;
  // paint lines with average time of all the same notes/frets
     for (int i = 0; i < goodSize + goodOffset; i++) { // skip wrong answers if separeted
//         double aTime = calcAverTime(sortedLists[i], !settings.inclWrongAnsw); OBSOLETE
        if (sortedLists[i].size() > 1) {
        TgraphicsLine *averTimeLine = new TgraphicsLine(&sortedLists[i]);
        QString lineText = "";
        switch (settings.order) {
          case e_byNote :
            lineText = TgroupedQAunit::for_a_note() + "<span style=\"font-size: 20px;\">  <b>" + 
                TnoteName::noteToRichText(sortedLists[i].first()->qa.note) + "</b></span>";
            break;
          case e_byFret:
            lineText = TgroupedQAunit::for_a_fret() + "<span style=\"font-size: 20px;\"><b>  " + 
                QString::number(sortedLists[i].first()->qa.pos.fret()) + "</b></span>";
            break;
          case e_byKey: {
            QString wereKeys = "";
            if (exam->level()->manualKey && sortedLists[i].first()->answerAs == TQAtype::e_asNote)
                wereKeys = "<br>(" + QApplication::translate("TlinearChart", "Key signatures gave by user") + ")";
            lineText = TgroupedQAunit::for_a_key() + "<span style=\"font-size: 20px;\">  <b>" + 
                sortedLists[i].first()->key.getName() + "</b></span>" + wereKeys;
            break;
          }
          case e_byAccid:
            if (kindOfAccids[i])
              lineText = TgroupedQAunit::for_an_accid() + "<span style=\"font-size: 20px;\">  " + accidToNotka(kindOfAccids[i]) + "</span>";
            else
              lineText = QApplication::translate("TlinearChart", "for notes without accidentals"); 
            break;
          case e_byQuestAndAnsw:
            lineText = sortedLists[i].fullDescription()/* + "<span style=\"font-size: 20px;\">  <b>" + 
                TnoteName::noteToRichText(sortedLists[i].first()->qa.note) + "</b>"*/;
            break;
          case e_byMistake:
            lineText = sortedLists[i].fullDescription();
            break;
        }        
        averTimeLine->setText(lineText);
        scene->addItem(averTimeLine);
        averTimeLine->setZValue(46);
        averTimeLine->setPen(QPen(averColor, 3)); // sea blue
        averTimeLine->setLine(xAxis->mapValue(cnt - 0.4) + xAxis->pos().x(), yAxis->mapValue(sortedLists[i].averTime() / 10.0),
          xAxis->mapValue(cnt + sortedLists[i].size() -0.6) + xAxis->pos().x(), yAxis->mapValue(sortedLists[i].averTime() / 10.0));
        }
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
    if (settings.order == e_byFret) {
      cnt = 1;
      for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *fretText = new QGraphicsTextItem();
        QFont f;
        f.setPixelSize(30);
        fretText->setFont(f);
        QString hintText = "<b style=\"color: rgba(200, 200, 200, 200); \">";
        if (goodOffset && (i == goodSize -1))
          hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type");
        else
          hintText += QString("%1</b>").arg(TfingerPos::romanFret(sortedLists[i].first()->qa.pos.fret()));
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
    if (settings.order == e_byKey) {
      cnt = 1;
      for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *keyText = new QGraphicsTextItem();
        QFont f;
        f.setPixelSize(16);
        keyText->setFont(f);
        QString hintText = "<b style=\"color: rgba(200, 200, 200, 200); \">";
        if (goodOffset && (i == goodSize -1))
          hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type") + "</b>";
        else {
            hintText += QString("%1").arg(sortedLists[i].first()->key.getName());
            hintText += "<br>" + getWasInAnswOrQuest(TQAtype::e_asNote, sortedLists[i].operator[](0).qaPtr);
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
    if (settings.order == e_byAccid) {
      cnt = 1;
      for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *accidentalText = new QGraphicsTextItem();
        QFont f;
        f.setPixelSize(30);
        accidentalText->setFont(f);
        QString hintText = "<span style=\"color: rgba(200, 200, 200, 200); \">";
        if (goodOffset && (i == goodSize -1))
          hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type") + "</span>";
        else 
          if (kindOfAccids[i])
            hintText += QString("%1").arg(accidToNotka(kindOfAccids[i], 40));
          else
            hintText += QApplication::translate("TlinearChart", "without accidentals");
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

TlinearChart::~TlinearChart() {}




