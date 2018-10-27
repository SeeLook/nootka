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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TMAINLINE_H
#define TMAINLINE_H


#include <QList>


class TquestionPoint;
class TgroupedQAunit;
class TstaffLineChart;
class Tchart;
class TQAunit;


/**
 * This is main line of a chart. 
 * It paints questions points @p TquestionPoint over the scene.
 * It also performs Tips - information about question 
 */
class TmainLine
{

public:

    /**
     * Kind of data represented by Y value of a point
     * @p e_questionTime - default and only one available for single note exams.
     * @p e_effectiveness - Y axis displays effectiveness of single answer 
     * @p e_prepareTime - Y axis displays time needed to start playing (only for played answers)
     * @p e_attemptsCount - Y axis displays number of attempts.
     * @p e_playedCount - Y axis displays how many times melody was played
     * @p e_mistakesNumber - Y axis displays number of mistakes committed in single answer
     */
  enum EyValue {
    e_questionTime, e_effectiveness, e_prepareTime, e_attemptsCount, e_playedCount, e_mistakesNumber
  };

  TmainLine(QList<TQAunit*> *answers, Tchart *chart, EyValue yVal = e_questionTime);
  TmainLine(QList<TgroupedQAunit> &listOfLists, Tchart *chart, EyValue yVal = e_questionTime);
  virtual ~TmainLine();

protected:
      /**
       * Returns answer result (Y value) depend on chart type, mapped to Y axis
       */
  inline qreal yValue(TQAunit* question, EyValue valType);

private:
  QList<TQAunit*>             *m_answers;
  Tchart                      *m_chart; /**< Pointer to chart contained this plot */
  QList<TquestionPoint*>       m_points; /**< List of points */
  QList<TstaffLineChart*>      m_lines; /**< list of lines betwen points */

};
#endif // TMAINLINE_H
