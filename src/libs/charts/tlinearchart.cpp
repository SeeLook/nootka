/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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
#include "txaxis.h"
#include "sorting.h"
#include "tyaxis.h"
#include "tstatisticstip.h"
#include <exam/texam.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <exam/tattempt.h>
#include <widgets/tquestionaswdg.h>
#include <graphics/tgraphicstexttip.h>
#include <tcolor.h>
#include <tnoofont.h>
#include <QApplication>
#include <QDebug>
#include <QTimer>


QColor averColor = QColor(0, 192, 192);

TlinearChart::TlinearChart(Texam* exam, Tchart::Tsettings& settings, QWidget* parent) :
    TmainChart(exam, settings, parent)
{
// Determine y value type and its maximal value to prepare Y axis
  switch (settings.yValue) {
    case TmainLine::e_prepareTime: {
      quint32 prepTime = 0;
      for(int i = 0; i < exam->count(); i++)
        prepTime = qMax(prepTime, exam->question(i)->attempt(0)->prepareTime());
      yAxis->setMaxValue((qreal)prepTime / 10.0);
      yAxis->setUnit(TYaxis::e_prepareTime);
      break;
    }
    case TmainLine::e_attemptsCount: {
      int attemptsNr = 0;
      for(int i = 0; i < exam->count(); i++)
        attemptsNr = qMax(attemptsNr, exam->question(i)->attemptsCount());
      yAxis->setMaxValue(attemptsNr, false);
      yAxis->setUnit(TYaxis::e_attemptsCount);
      break;
    }
    case TmainLine::e_playedCount: {
      int playedNr = 0;
      for(int i = 0; i < exam->count(); i++)
        playedNr = qMax(playedNr, exam->question(i)->totalPlayBacks());
      yAxis->setMaxValue(playedNr, false);
      yAxis->setUnit(TYaxis::e_playedCount);
      break;
    }
    case TmainLine::e_effectiveness: {
      yAxis->setMaxValue(100.0); // 100% looks good for this kind
      yAxis->setUnit(TYaxis::e_effectiveness);
      break;
    }
    default: {
      quint16 maxTime = 0;
      for(int i = 0; i < exam->count(); i++)
        maxTime = qMax(maxTime, exam->question(i)->time);
      yAxis->setMaxValue((qreal)maxTime / 10.0);
      break;
    }
  }
  
	if (settings.order == e_byNumber) {
		xAxis->setAnswersList(currExam->answList(), currExam->level());
		prepareChart(currExam->count());
		m_mainLine = new TmainLine(currExam->answList(), this, settings.yValue);
		if (settings.yValue == TmainLine::e_questionTime) {
			qreal aTime = 0 , prev = 0;
			int firstCorrect = 0, okCount = 0;
			for(int i = 0; i < exam->count(); i++) { // looking for first correct answer
				if (!exam->question(i)->isWrong() || (!exam->question(i)->isWrong() && settings.inclWrongAnsw)) {
					firstCorrect = i;
					prev = exam->question(i)->time;
					okCount++;
					aTime = prev;
					break;
				}
			}
      int prevX = firstCorrect + 1;
      for(int i = firstCorrect + 1; i < exam->count(); i++) {
        if (exam->question(i)->isWrong())
          continue; // skip wrong answers in aver time
        else 
          aTime = (aTime * okCount + (exam->question(i)->time)) / (okCount + 1);
        
        okCount++;
        TgraphicsLine *averProgress = new TgraphicsLine();
        scene->addItem(averProgress);
        averProgress->setPen(QPen(averColor.darker(120), 3, Qt::DotLine));
        averProgress->setLine(xAxis->mapValue(prevX) + xAxis->pos().x(), yAxis->mapValue(prev / 10.0),
                              xAxis->mapValue(i + 1) + xAxis->pos().x(), yAxis->mapValue(aTime / 10.0));
        prevX = i + 1;
        averProgress->setZValue(20);
        prev = aTime;
      }
      
      if (exam->averageReactonTime() > 0) {
          TgraphicsLine *averLine = new TgraphicsLine(0, "<p>" +
              TexTrans::averAnsverTimeTxt() + 
              QString("<br><big><b>%1</b></big></p>").arg(Texam::formatReactTime(exam->averageReactonTime(), true)) );
          scene->addItem(averLine);
          averLine->setZValue(15);
          averLine->setPen(QPen(averColor, 2));
          averLine->setLine(xAxis->mapValue(1) + xAxis->pos().x(), yAxis->mapValue(exam->averageReactonTime() / 10.0),
              xAxis->mapValue(exam->count()) + xAxis->pos().x(), yAxis->mapValue(exam->averageReactonTime() / 10.0));
      }
  // rest of cases are available only for melodies
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
      m_mainLine = new TmainLine(sortedLists, this, settings.yValue);
      
      int goodOffset = 0; // 0 when not unrelated question list inside
      if (hasListUnrelated)
        goodOffset = -1; // do not perform a last loop 
      int cnt = 1;
      if (settings.yValue != TmainLine::e_questionTime) {
        QTimer::singleShot(10, this, SLOT(ajustChartHeight()));
        return;
      }
  // paint lines with average time of all the same notes/frets
     for (int i = 0; i < goodSize + goodOffset; i++) { // skip wrong answers if separated
        if (sortedLists[i].size() > 1) {
        TgraphicsLine *averTimeLine = new TgraphicsLine(&sortedLists[i]);

        averTimeLine->setText(sortedLists[i].fullDescription());
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
          hiCol.setAlpha(60);
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
        QGraphicsTextItem *fretText = getTextItem(30);
        QString hintText = "<b>";
        if (goodOffset && (i == goodSize -1))
          hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type");
        else
          hintText += QString("%1").arg(TfingerPos::romanFret(sortedLists[i].first()->qa.pos.fret()));
        hintText += "</b>";
        fretText->setHtml(hintText);
				TgraphicsTextTip::alignCenter(fretText);
				qreal sc = 1.0;
        if (sortedLists[i].size() * xAxis->questWidth() < fretText->boundingRect().width()) {
            sc = (sortedLists[i].size() * xAxis->questWidth()) / fretText->boundingRect().width();
            fretText->setScale(sc);
        }
        fretText->setPos(xAxis->mapValue(cnt) + 
        (sortedLists[i].size() * xAxis->questWidth() - fretText->boundingRect().width() * sc) / 2,
                         yAxis->mapValue(yAxis->maxValue()));        
        
        cnt += sortedLists[i].size();
      }      
    }
  // key signature names over the chart
    if (settings.order == e_byKey) {
      cnt = 1;
      QColor tc = palette().text().color();
      for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *keyText = getTextItem(16);
        QString hintText = "<b>";
        if (goodOffset && (i == goodSize -1))
          hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type");
        else {
            hintText += QString("%1").arg(sortedLists[i].first()->key.getName());
            hintText += "<br>" + getWasInAnswOrQuest(TQAtype::e_onScore, sortedLists[i].operator[](0).qaPtr);
        }
        hintText += "</b>";
        keyText->setHtml(hintText);
				TgraphicsTextTip::alignCenter(keyText);
        qreal sc = 1.0;
        if (sortedLists[i].size() * xAxis->questWidth() < keyText->boundingRect().width()) {
            sc = (sortedLists[i].size() * xAxis->questWidth()) / keyText->boundingRect().width();
            keyText->setScale(sc);
        }
        keyText->setPos(xAxis->mapValue(cnt) + 
        (sortedLists[i].size() * xAxis->questWidth() - keyText->boundingRect().width() * sc) / 2, 
                         yAxis->mapValue(yAxis->maxValue()));        
        cnt += sortedLists[i].size();
      }
    }
// accidentals over the chart
    if (settings.order == e_byAccid) {
      cnt = 1;
      for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *accidentalText = getTextItem(30);
        QString hintText;
        if (goodOffset && (i == goodSize -1))
          hintText += QApplication::translate("TlinearChart", "questions unrelated<br>with chart type") + "</span>";
        else 
          if (kindOfAccids[i])
            hintText += QString("%1").arg(accidToNotka(kindOfAccids[i], 40));
          else
            hintText += QApplication::translate("TlinearChart", "without accidentals");
        accidentalText->setHtml(hintText);
				TgraphicsTextTip::alignCenter(accidentalText);
        accidentalText->setPos(xAxis->mapValue(cnt) + 
        (sortedLists[i].size() * xAxis->questWidth() - accidentalText->boundingRect().width()) / 2, 
                         yAxis->mapValue(yAxis->maxValue()));        
        cnt += sortedLists[i].size();
      }      
    }
  // question/answer type over the chart
		if (settings.order == e_byQuestAndAnsw) {
			cnt = 1;
			for (int i = 0; i < goodSize; i++) { 
        QGraphicsTextItem *qaText = getTextItem(30);
				int nootFontSize = fontMetrics().boundingRect("A").height() * 2;
        QString hintText;
				if (exam->melodies()) {
					if (sortedLists[i].first()->questionAs == TQAtype::e_onScore)
						hintText = TexTrans::playMelodyTxt();
					else
						hintText = TexTrans::writeMelodyTxt();
				} else
					hintText = TnooFont::span(TquestionAsWdg::qaTypeSymbol(sortedLists[i].first()->questionAs) + "?", nootFontSize) + 
							"<br>" + TnooFont::span(TquestionAsWdg::qaTypeSymbol(sortedLists[i].first()->answerAs) + "!", nootFontSize);
        qaText->setHtml(hintText);
				TgraphicsTextTip::alignCenter(qaText);
				qreal sc = 1.0;
        if (sortedLists[i].size() * xAxis->questWidth() < qaText->boundingRect().width()) {
            sc = (sortedLists[i].size() * xAxis->questWidth()) / qaText->boundingRect().width();
            qaText->setScale(sc);
        }
        qaText->setPos(xAxis->mapValue(cnt) + 
        (sortedLists[i].size() * xAxis->questWidth() - qaText->boundingRect().width() * sc) / 2, 
                         yAxis->mapValue(0) - yAxis->length());        
        cnt += sortedLists[i].size();
      }
		}  
  }
  QTimer::singleShot(10, this, SLOT(ajustChartHeight()));
}


QGraphicsTextItem* TlinearChart::getTextItem(int fontSize) {
	QGraphicsTextItem *item = new QGraphicsTextItem();
	QFont f;
	f.setPixelSize(fontSize);
	item->setFont(f);
	QColor C(palette().text().color());
	C.setAlpha(75);
	Tcolor::merge(C, palette().base().color());
	item->setDefaultTextColor(C);
	scene->addItem(item);
	item->setZValue(15);
	return item;
}






