/***************************************************************************
 *   Copyright (C) 2013-2019 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tlinearchart.h"
#include "tgraphicsline.h"
// #include "txaxis.h"
#include "sorting.h"
// #include "tyaxis.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>
#include <tcolor.h>
#include <tnoofont.h>
#include <QtCore/qdebug.h>
#include <QtCore/qtimer.h>

#include <QtGui/qfontmetrics.h>


QColor averColor = QColor(0, 192, 192);


TlinearChart::TlinearChart(QQuickItem* parent) :
  TmainChart(parent)
{
}


void TlinearChart::init() {
// Determine y value type and its maximal value to prepare Y axis
  switch (p_chartSett.yValue) {
    case Tchart::e_YprepareTime: {
      quint32 prepTime = 0;
      for(int i = 0; i < p_currExam->count(); i++)
        prepTime = qMax(prepTime, p_currExam->question(i)->attempt(0)->prepareTime());
      setMaxValue(static_cast<qreal>(prepTime) / 10.0);
      setUnit(e_prepareTime);
      break;
    }
    case Tchart::e_YattemptsCount: {
      int attemptsNr = 0;
      for(int i = 0; i < p_currExam->count(); i++)
        attemptsNr = qMax(attemptsNr, p_currExam->question(i)->attemptsCount());
      setMaxValue(static_cast<qreal>(attemptsNr), false);
      setUnit(e_attemptsCount);
      break;
    }
    case Tchart::e_YplayedCount: {
      int playedNr = 0;
      for(int i = 0; i < p_currExam->count(); i++)
        playedNr = qMax(playedNr, p_currExam->question(i)->totalPlayBacks());
      setMaxValue(static_cast<qreal>(playedNr), false);
      setUnit(e_playedCount);
      break;
    }
    case Tchart::e_Yeffectiveness: {
      setMaxValue(100.0); // 100% looks good for this kind
      setUnit(e_effectiveness);
      break;
    }
    default: {
      quint16 maxTime = 0;
      for(int i = 0; i < p_currExam->count(); i++)
        maxTime = qMax(maxTime, p_currExam->question(i)->time);
      setMaxValue(static_cast<qreal>(maxTime) / 10.0);
      setUnit(e_timeInSec);
      break;
    }
  }

  if (p_chartSett.order == e_byNumber) {
//     xAxis->setAnswersList(p_currExam->answList(), p_currExam->level());
    prepareChart(p_currExam->count());
    qreal tSum = 0.0;
    int okCount = 0;
    for(int i = 0; i < p_currExam->count(); i++) {
      qreal t = static_cast<qreal>(p_currExam->question(i)->time);
      if (!p_currExam->question(i)->isWrong() || (p_currExam->question(i)->isWrong() && p_chartSett.inclWrongAnsw)) {
        okCount++;
        tSum += t;
      }
      p_averChunks << (okCount ? tSum / static_cast<qreal>(okCount) : 0.0);
    }
//     p_mainLine = new TmainLine(p_currExam->answList(), this, p_chartSett.yValue);
//     if (p_chartSett.yValue == Tchart::e_YquestionTime) {
//       p_averageTime = 0.0;
//       qreal prev = 0.0;
//       int firstCorrect = 0, okCount = 0;
//       for(int i = 0; i < p_currExam->count(); i++) { // looking for first correct answer
//         if (!p_currExam->question(i)->isWrong() || (!p_currExam->question(i)->isWrong() && p_chartSett.inclWrongAnsw)) {
//           firstCorrect = i;
//           prev = p_currExam->question(i)->time;
//           okCount++;
// //           p_averageTime = prev;
//           break;
//         }
//       }
//       int prevX = firstCorrect + 1;
//       for(int i = firstCorrect + 1; i < p_currExam->count(); i++) {
//         if (p_currExam->question(i)->isWrong())
//           continue; // skip wrong answers in aver time
//         else 
//           p_averageTime = (p_averageTime * okCount + (p_currExam->question(i)->time)) / (okCount + 1);

//         okCount++;
//         auto averProgress = new TgraphicsLine();
//         setParentForItem(averProgress);
//         averProgress->setPen(QPen(averColor.darker(120), 3, Qt::DotLine));
//         averProgress->setLine(xAxis->mapValue(prevX) + xAxis->pos().x(), yAxis->mapValue(prev / 10.0),
//                               xAxis->mapValue(i + 1) + xAxis->pos().x(), yAxis->mapValue(p_averageTime / 10.0));
//         prevX = i + 1;
//         averProgress->setZValue(20);
//         prev = p_averageTime;
//       }

//       if (p_currExam->averageReactonTime() > 0) {
//           auto averLine = new TgraphicsLine(nullptr, TexTrans::averAnsverTimeTxt()
//               + QString("<br><big><b>%1</b></big>").arg(Texam::formatReactTime(p_currExam->averageReactonTime(), true)));
//           setParentForItem(averLine);
//           averLine->setZValue(15);
//           averLine->setPen(QPen(averColor, 2));
//           averLine->setLine(xAxis->mapValue(1) + xAxis->pos().x(), yAxis->mapValue(p_currExam->averageReactonTime() / 10.0),
//               xAxis->mapValue(p_currExam->count()) + xAxis->pos().x(), yAxis->mapValue(p_currExam->averageReactonTime() / 10.0));
//       }
  // rest of cases are available only for melodies
//     }
  }

  if (p_chartSett.order == e_byNote || p_chartSett.order == e_byFret ||
          p_chartSett.order == e_byKey || p_chartSett.order == e_byAccid ||
          p_chartSett.order == e_byQuestAndAnsw || p_chartSett.order == e_byMistake) {
      sort();
//       xAxis->setAnswersLists(p_sortedLists, p_currExam->level());
      int ln = 0;
      for (int i = 0; i < p_sortedLists.size(); i++)
        ln += p_sortedLists[i].size();
      prepareChart(ln);
//       p_mainLine = new TmainLine(p_sortedLists, this, p_chartSett.yValue);

      int goodOffset = 0; // 0 when not unrelated question list inside
      if (p_hasListUnrelated)
        goodOffset = -1; // do not perform a last loop 
      int cnt = 1;
      if (p_chartSett.yValue != Tchart::e_YquestionTime) {
        return;
      }
  // paint lines with average time of all the same notes/frets
     for (int i = 0; i < p_goodSize + goodOffset; i++) { // skip wrong answers if separated
        if (p_sortedLists[i].size() > 1) {
//           auto averTimeLine = new TgraphicsLine(&p_sortedLists[i]);
// 
//           averTimeLine->setText(p_sortedLists[i].fullDescription());
//           setParentForItem(averTimeLine);
//           averTimeLine->setZValue(46);
//           averTimeLine->setPen(QPen(averColor, 3)); // sea blue
//           averTimeLine->setLine(xAxis->mapValue(cnt - 0.4) + xAxis->pos().x(), yAxis->mapValue(p_sortedLists[i].averTime() / 10.0),
//             xAxis->mapValue(cnt + p_sortedLists[i].size() -0.6) + xAxis->pos().x(), yAxis->mapValue(p_sortedLists[i].averTime() / 10.0));
        }
        cnt += p_sortedLists[i].size();
      }
      cnt = 1;
      // paint highlights under grouped items
//       for (int i = 0; i < p_sortedLists.size(); i++) {
//         auto groupBg = new QGraphicsRectItem();
//         setParentForItem(groupBg);
//         QColor hiCol = qApp->palette().highlight().color();
//         hiCol.setAlpha(60);
//         if (i%2) {
//           groupBg->setBrush(QBrush(hiCol));
//           groupBg->setPen(Qt::NoPen);
//           groupBg->setRect(xAxis->mapValue(cnt), 0.0, p_sortedLists[i].size() * xAxis->questWidth(), yAxis->boundingRect().height());
//           groupBg->setZValue(-1);
//         }
//         cnt += p_sortedLists[i].size();
//       }
  // fret number over the chart
    if (p_chartSett.order == e_byFret) {
      cnt = 1;
//       for (int i = 0; i < p_goodSize; i++) {
//         auto fretText = getTextItem(20);
//         QString hintText = QLatin1String("<b>");
//         if (goodOffset && (i == p_goodSize -1))
//           hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type");
//         else
//           hintText += QString("%1").arg(TfingerPos::romanFret(p_sortedLists[i].first()->qa.pos().fret()));
//         hintText += QLatin1String("</b>");
//         fretText->setHtml(hintText);
//         TXaxis::alignC(fretText);
//         qreal sc = 1.0;
//         if (p_sortedLists[i].size() * xAxis->questWidth() < fretText->boundingRect().width()) {
//             sc = (p_sortedLists[i].size() * xAxis->questWidth()) / fretText->boundingRect().width();
//             fretText->setScale(sc);
//         }
//         fretText->setPos(xAxis->mapValue(cnt) + 
//         (p_sortedLists[i].size() * xAxis->questWidth() - fretText->boundingRect().width() * sc) / 2,
//                          yAxis->mapValue(yAxis->maxValue()));
// 
//         cnt += p_sortedLists[i].size();
//       }
    }
  // key signature names over the chart
    if (p_chartSett.order == e_byKey) {
      cnt = 1;
//       for (int i = 0; i < p_goodSize; i++) { 
//         auto keyText = getTextItem(16);
//         QString hintText = QLatin1String("<b>");
//         if (goodOffset && (i == p_goodSize -1))
//           hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type");
//         else {
//             hintText += QString("%1").arg(p_sortedLists[i].first()->key.getName());
//             hintText += QLatin1String("<br>") + getWasInAnswOrQuest(TQAtype::e_onScore, p_sortedLists[i].operator[](0).qaPtr);
//         }
//         hintText += QLatin1String("</b>");
//         keyText->setHtml(hintText);
//         TXaxis::alignC(keyText);
//         qreal sc = 1.0;
//         if (p_sortedLists[i].size() * xAxis->questWidth() < keyText->boundingRect().width()) {
//             sc = (p_sortedLists[i].size() * xAxis->questWidth()) / keyText->boundingRect().width();
//             keyText->setScale(sc);
//         }
//         keyText->setPos(xAxis->mapValue(cnt) + 
//         (p_sortedLists[i].size() * xAxis->questWidth() - keyText->boundingRect().width() * sc) / 2, 
//                          yAxis->mapValue(yAxis->maxValue()));        
//         cnt += p_sortedLists[i].size();
//       }
    }
// accidentals over the chart
    if (p_chartSett.order == e_byAccid) {
      cnt = 1;
//       for (int i = 0; i < p_goodSize; i++) {
//         auto accidentalText = getTextItem(20);
//         QString hintText;
//         if (goodOffset && (i == p_goodSize - 1))
//           hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type") + "</span>";
//         else 
//           if (p_kindOfAccids[i])
//             hintText += QString("%1").arg(accidToNotka(p_kindOfAccids[i], 40));
//           else
//             hintText += QApplication::translate("TlinearChart", "without accidentals");
//         accidentalText->setHtml(hintText);
//         TXaxis::alignC(accidentalText);
//         accidentalText->setPos(xAxis->mapValue(cnt) +
//         (p_sortedLists[i].size() * xAxis->questWidth() - accidentalText->boundingRect().width()) / 2, 
//                          yAxis->mapValue(yAxis->maxValue()));
//         cnt += p_sortedLists[i].size();
//       }
    }
  // question/answer type over the chart
    if (p_chartSett.order == e_byQuestAndAnsw) {
      cnt = 1;
//       for (int i = 0; i < p_goodSize; i++) { 
//         auto qaText = getTextItem(20);
//         int nootFontSize = QFontMetrics(qApp->font()).boundingRect("A").height() * 2;
//         QString hintText;
//         if (p_currExam->melodies()) {
//             if (p_sortedLists[i].first()->questionAs == TQAtype::e_onScore)
//               hintText = TexTrans::playMelodyTxt();
//             else
//               hintText = TexTrans::writeMelodyTxt();
//         } else
//             hintText = TnooFont::span(qaSymbol(p_sortedLists[i].first()->questionAs) + QLatin1String("?"), nootFontSize)
//             + QLatin1String("<br>") + TnooFont::span(qaSymbol(p_sortedLists[i].first()->answerAs) + QLatin1String("!"), nootFontSize);
//         qaText->setHtml(hintText);
//         TXaxis::alignC(qaText);
//         qreal sc = 1.0;
//         if (p_sortedLists[i].size() * xAxis->questWidth() < qaText->boundingRect().width()) {
//             sc = (p_sortedLists[i].size() * xAxis->questWidth()) / qaText->boundingRect().width();
//             qaText->setScale(sc);
//         }
//         qaText->setPos(xAxis->mapValue(cnt) + 
//         (p_sortedLists[i].size() * xAxis->questWidth() - qaText->boundingRect().width() * sc) / 2, 
//                          yAxis->mapValue(0) - yAxis->length());        
//         cnt += p_sortedLists[i].size();
//       }
    }
  }
}


// QGraphicsTextItem* TlinearChart::getTextItem(int fontSize) {
//   auto item = new QGraphicsTextItem();
//   QFont f;
//   f.setPixelSize(fontSize);
//   item->setFont(f);
//   QColor C(qApp->palette().text().color());
//   C.setAlpha(75);
//   Tcolor::merge(C, qApp->palette().base().color());
//   item->setDefaultTextColor(C);
//   setParentForItem(item);
//   item->setZValue(15);
//   return item;
// }






