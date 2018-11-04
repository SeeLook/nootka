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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tmainchart.h"
#include "tmainline.h"
#include "txaxis.h"
#include "tyaxis.h"
#include "tstafflinechart.h"
#include "tgraphicsline.h"
#include "tquestionpoint.h"
#include "tbar.h"
#include "sorting.h"
#include <exam/tlevel.h>
#include <exam/texam.h>
#include <QtCore/qtimer.h>




TmainChart::TmainChart(QQuickItem* parent):
  Tchart(parent),
  p_hasListUnrelated(false),
  goodSize(0)
{
  setAcceptHoverEvents(true);
  m_enterTimer = new QTimer(this);
  connect(m_enterTimer, &QTimer::timeout, this, &TmainChart::enterTimeOut);
  m_leaveTimer = new QTimer(this);
  connect(m_leaveTimer, &QTimer::timeout, this, &TmainChart::leaveTimeOut);
}


TmainChart::~TmainChart()
{}


void TmainChart::setExam(Texam* e) {
  if (e != p_currExam) {
    p_currExam = e;
  }
}


void TmainChart::setChartSettings(const Tchart::Tsettings& s) {
  p_chartSett = s;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmainChart::sort() {
    TgroupedQAunit::setSkipWrong(!p_chartSett.inclWrongAnsw);
    if (p_chartSett.separateWrong) {
        divideGoodAndBad(p_currExam->answList(), goodAnsw, badAnsw);
        if (p_chartSett.order == e_byNote)
          sortedLists = sortByNote(goodAnsw, p_currExam->level(), p_hasListUnrelated);
        else
          if (p_chartSett.order == e_byFret)
            sortedLists = sortByFret(goodAnsw, p_currExam->level(), p_hasListUnrelated);
          else
            if (p_chartSett.order == e_byKey)
              sortedLists = sortByKeySignature(goodAnsw, p_currExam->level(), p_hasListUnrelated);
            else
              if (p_chartSett.order == e_byAccid)
              sortedLists = sortByAccidental(goodAnsw, p_currExam->level(), p_hasListUnrelated, kindOfAccids);
        // e_byQuestAndAnsw & e_byMistake never separate wrong answers
        goodSize = sortedLists.size(); // number without wrong answers
        if (p_chartSett.order == e_byNote)
          sortedLists.append(sortByNote(badAnsw, p_currExam->level(), p_hasListUnrelated));
        else
          if (p_chartSett.order == e_byFret)
            sortedLists.append(sortByFret(badAnsw, p_currExam->level(), p_hasListUnrelated));
          else
            if (p_chartSett.order == e_byKey)
              sortedLists.append(sortByKeySignature(badAnsw, p_currExam->level(), p_hasListUnrelated));
            else
              if (p_chartSett.order == e_byAccid)
              sortedLists.append(sortByAccidental(badAnsw, p_currExam->level(), p_hasListUnrelated, kindOfAccids));
    } else {
        TgroupedQAunit convList;
        convertToGroupedQAunit(p_currExam->answList(), convList);
        switch (p_chartSett.order) {
          case e_byNote:
            sortedLists = sortByNote(convList, p_currExam->level(), p_hasListUnrelated);
            break;
          case e_byFret:
            sortedLists = sortByFret(convList, p_currExam->level(), p_hasListUnrelated);
            break;
          case e_byKey:
            sortedLists = sortByKeySignature(convList, p_currExam->level(), p_hasListUnrelated);
            break;
          case e_byAccid:
            sortedLists = sortByAccidental(convList, p_currExam->level(), p_hasListUnrelated, kindOfAccids);
            break;
          case e_byQuestAndAnsw: // in this case wrong answers aren't separate
            sortedLists = sortByQAtype(convList, p_currExam->level(), p_hasListUnrelated);
            break;
          case e_byMistake:
            sortedLists = sortByMisakes(convList, p_currExam->level(), p_hasListUnrelated);
            break;
          default: break;
        }
        goodSize = sortedLists.size();
    }
}


void TmainChart::prepareChart(int maxX) {
// Grid lines
  QColor lineColor = qApp->palette().foreground().color();
  if (p_chartSett.type != e_bar) { // vertical lines only for linear chart
      for(int i = 5; i < maxX; i++) {
        if (i % 5 == 0) {
          auto l = new QGraphicsLineItem(xAxis->mapValue(i) + xAxis->pos().x(), 0.0, xAxis->mapValue(i) + xAxis->pos().x(), yAxis->length(), p_bgRect);
          l->setPen(QPen(QBrush(lineColor), 1, Qt::DashLine));
        }
      }
  }
  QList<double> listY;
  yAxis->getYforGrid(listY);
  if (listY.size()) {
      for(int i = 0; i < listY.size(); i++) {
        auto l = new QGraphicsLineItem(xAxis->pos().x(), listY[i], xAxis->pos().x() + xAxis->length(), listY[i], p_bgRect);
        l->setPen(QPen(QBrush(lineColor), 1, Qt::DashLine));
      }
  }
}


void TmainChart::hoverMoveEvent(QHoverEvent* event) {
  auto it = scene->itemAt(event->pos(), p_bgRect->transform());
  if (it && it->type() >= QUESTION_POINT_TYPE) {
      TtipInfo* p = nullptr;
      if (it->type() == QUESTION_POINT_TYPE) {
          auto grIt = qgraphicsitem_cast<TquestionPoint*>(it);
          p = static_cast<TtipInfo*>(grIt);
      } else if (it->type() == CHART_LINE_TYPE) {
          auto grIt = qgraphicsitem_cast<TgraphicsLine*>(it);
          p = static_cast<TtipInfo*>(grIt);
      } else {
          auto grIt = qgraphicsitem_cast<Tbar*>(it);
          p = static_cast<TtipInfo*>(grIt);
      }
      if (m_hoveredItem != p) {
        m_enterTimer->start(300);
        m_leaveTimer->stop();
        m_hoveredItem = p;
        m_hoveredItem->setCursorPos(event->pos());
      }
  } else if (m_hoveredItem) {
      m_enterTimer->stop();
      m_leaveTimer->start(500);
  }
}


void TmainChart::enterTimeOut() {
  m_enterTimer->stop();
  if (m_hoveredItem)
    setCurQ(m_hoveredItem);
  else
    qDebug() << "[TmainChart] FIXME! No hovered item!";
  emit hoveredChanged();
}


void TmainChart::leaveTimeOut() {
  m_leaveTimer->stop();
  m_hoveredItem = nullptr;
  setCurQ(nullptr);
  emit hoveredChanged();
}
