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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TMAINCHART_H
#define TMAINCHART_H


#include "tchart.h"
#include "tgroupedqaunit.h"
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>


class Tlevel;
class Tnote;
class Texam;
class TquestionPoint;
class QTimer;


/**
 * This is global class for bar and linear charts.
 * It performs some common methods
 */
class TmainChart : public Tchart
{

  Q_OBJECT

public:
  explicit TmainChart(QQuickItem* parent = nullptr);
  virtual ~TmainChart();

  void setExam(Texam* e);
  void setChartSettings(const Tsettings& s);

  Texam* exam() { return p_currExam; }

  virtual void init() = 0;

protected:
  void hoverMoveEvent(QHoverEvent* event) override;

      /**
       * Sorts exam data by params given in chartSett. 
       * Initializes sortedLists, hasListUnrelated and kindOfAccids.
       */
  void sort();

      /**
       * Performs common elements for all kinds of charts.
       */
  void prepareChart(int maxX);

  void enterTimeOut();
  void leaveTimeOut();

protected:
  Tsettings                   p_chartSett;
  Texam                      *p_currExam = nullptr;
  TmainLine                  *p_mainLine = nullptr;
  bool                        p_hasListUnrelated; /**< Returns true if list contains unrelated list of questions. */
  TgroupedQAunit              goodAnsw, badAnsw;
  QList<TgroupedQAunit>       sortedLists;
  int                         goodSize; /**< number of lists with good answers */
  QList<char>                 kindOfAccids;

private:
  TtipInfo                   *m_hoveredItem = nullptr;
  QTimer                     *m_enterTimer, *m_leaveTimer;
};

#endif // TMAINCHART_H
