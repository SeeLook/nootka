/***************************************************************************
 *   Copyright (C) 2012-2019 by Tomasz Bojczuk                             *
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


class Tlevel;
class Tnote;
class Texam;
class TQAunit;


/**
 * This is abstract class for bar and linear charts.
 * It performs some common methods.
 * Deriving class has to implement pure virtual @p init() method
 */
class TmainChart : public Tchart
{

  Q_OBJECT

public:
  explicit TmainChart(QObject* parent = nullptr);

  void setExam(Texam* e);
  void setChartSettings(const Tsettings& s);

  Texam* exam() { return p_currExam; }

  virtual void init() = 0;

  int qCount() const { return p_sortedLists.count(); }
  int goodCount() const { return p_goodAnsw.size(); }
  int badCount() const { return p_badAnsw.size(); }
  TgroupedQAunit* group(int grNr) { return grNr > -1 && grNr < p_sortedLists.size() ? &p_sortedLists[grNr] : nullptr; }

  QString ticText(TQAunit* unit, int questNr = 0);

      /**
       * Returns @p TQAunit pointer of @p unitNr question in @p p_sortedLists
       */
  TQAunit* getSorted(int unitNr);
  TqaPtr* getSortedPtr(int ptrNr);

protected:
      /**
       * Sorts exam data by params given in chartSett. 
       * Initializes sortedLists, hasListUnrelated and kindOfAccids.
       */
  void sort();

      /**
       * Performs common elements for all kinds of charts.
       */
  void prepareChart(int maxX);

protected:
  Tsettings                   p_chartSett;
  Texam                      *p_currExam = nullptr;
  bool                        p_hasListUnrelated; /**< Returns true if list contains unrelated list of questions. */
  TgroupedQAunit              p_goodAnsw, p_badAnsw;
  QList<TgroupedQAunit>       p_sortedLists;
  int                         p_goodSize; /**< number of lists with good answers */
  QList<char>                 p_kindOfAccids;

};

#endif // TMAINCHART_H
