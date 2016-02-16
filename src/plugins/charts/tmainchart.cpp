/***************************************************************************
 *   Copyright (C) 2012-2016 by Tomasz Bojczuk                             *
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
#include "ttipchart.h"
#include "sorting.h"
#include <exam/tlevel.h>
#include <widgets/tquestionaswdg.h>
#include <exam/texam.h>
#include <QtWidgets/qscrollbar.h>
#include <QtCore/qtimer.h>




TmainChart::TmainChart(Texam* exam, Tchart::Tsettings& settings, QWidget* parent):
  Tchart(parent),
  currExam(exam),
  chartSett(settings),
  hasListUnrelated(false),
  goodSize(0)
{
  setMouseTracking(true);
  connect(horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sceneMoved()));
}


TmainChart::~TmainChart()
{}

//####################################################################################
//##################### protected methods ############################################
//####################################################################################

void TmainChart::sort() {
    TgroupedQAunit::setSkipWrong(!chartSett.inclWrongAnsw);
    if (chartSett.separateWrong) {
        divideGoodAndBad(currExam->answList(), goodAnsw, badAnsw);
        if (chartSett.order == e_byNote)
          sortedLists = sortByNote(goodAnsw, currExam->level(), hasListUnrelated);
        else
          if (chartSett.order == e_byFret)
            sortedLists = sortByFret(goodAnsw, currExam->level(), hasListUnrelated);
          else
            if (chartSett.order == e_byKey)
              sortedLists = sortByKeySignature(goodAnsw, currExam->level(), hasListUnrelated);
            else
              if (chartSett.order == e_byAccid)
              sortedLists = sortByAccidental(goodAnsw, currExam->level(), hasListUnrelated, kindOfAccids);
        // e_byQuestAndAnsw & e_byMistake never separate wrong answers
        goodSize = sortedLists.size(); // number without wrong answers
        if (chartSett.order == e_byNote)
          sortedLists.append(sortByNote(badAnsw, currExam->level(), hasListUnrelated));
        else
          if (chartSett.order == e_byFret)
            sortedLists.append(sortByFret(badAnsw, currExam->level(), hasListUnrelated));
          else
            if (chartSett.order == e_byKey)
              sortedLists.append(sortByKeySignature(badAnsw, currExam->level(), hasListUnrelated));
            else
              if (chartSett.order == e_byAccid)
              sortedLists.append(sortByAccidental(badAnsw, currExam->level(), hasListUnrelated, kindOfAccids));
    } else {
        TgroupedQAunit convList;
        convertToGroupedQAunit(currExam->answList(), convList);
        switch (chartSett.order) {
          case e_byNote:
            sortedLists = sortByNote(convList, currExam->level(), hasListUnrelated);
            break;
          case e_byFret:
            sortedLists = sortByFret(convList, currExam->level(), hasListUnrelated);
            break;
          case e_byKey:
            sortedLists = sortByKeySignature(convList, currExam->level(), hasListUnrelated);
            break;
          case e_byAccid:
            sortedLists = sortByAccidental(convList, currExam->level(), hasListUnrelated, kindOfAccids);
            break;
          case e_byQuestAndAnsw: // in this case wrong answers aren't separate
            sortedLists = sortByQAtype(convList, currExam->level(), hasListUnrelated);
            break;
          case e_byMistake:
            sortedLists = sortByMisakes(convList, currExam->level(), hasListUnrelated);
            break;
					default: break;
        }
        goodSize = sortedLists.size();
    }
}


void TmainChart::prepareChart(int maxX) {
// Grid lines
  QColor lineColor = palette().foreground().color();
  if (chartSett.type != e_bar) { // vertical lines only for linear chart
      for(int i = 5; i < maxX; i++) {
        if (i%5 == 0)
          scene->addLine(xAxis->mapValue(i) + xAxis->pos().x(), 0, xAxis->mapValue(i) + xAxis->pos().x(), yAxis->length(), 
                        QPen(QBrush(lineColor), 1, Qt::DashLine));
      }
  }
  QList<double> listY;
  yAxis->getYforGrid(listY);
  if (listY.size()) {
      for(int i = 0; i < listY.size(); i++)
        scene->addLine(xAxis->pos().x(), listY[i], xAxis->pos().x() + xAxis->length(), listY[i], QPen(QBrush(lineColor), 1, Qt::DashLine));
  }  
}


//####################################################################################
//##################### protected slots ##############################################
//####################################################################################

void TmainChart::sceneMoved() {
	qreal xOff = mapToScene(0, 0).x();
	xOff = qMax(0.0, xOff);
	yAxis->setX(qMax<qreal>(45.0, xOff));
#if defined(Q_OS_MAC)
	// Unfortunately some QT styles and
	// Mac has problem with update and this solves it.
	// It cases blinking but it is acceptable
		QTimer::singleShot(1, this, SLOT(updateSceneAfterMove()));
#else
	scene->update();
#endif
}


void TmainChart::updateSceneAfterMove() {
	scene->update();
}






