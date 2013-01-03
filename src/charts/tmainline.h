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

#ifndef TMAINLINE_H
#define TMAINLINE_H


#include <qlist.h>
#include "tquestionpoint.h"

class TgroupedQAunit;
class TstaffLineChart;
class TtipChart;
class Tchart;
class TQAunit;



/** This is main line of a chart. 
 * It paints qustions points TquestionPoint over the scene.
 * It also performs Tips - information about question 
 */
class TmainLine : public QObject
{
    Q_OBJECT
    
  friend class TquestionPoint;
  
public:
  TmainLine(QList<TQAunit> *answers, Tchart *chart);
  TmainLine(QList<TgroupedQAunit> &listOfLists, Tchart *chart);
  virtual ~TmainLine();
  
protected:
  void showTip(TquestionPoint *point); // Shows question/answer data as tip
  void deleteTip();
  
private:
  QList<TQAunit> *m_answers; 
  Tchart *m_chart; // Pointer to chart contained this plot
  QList<TquestionPoint*> m_points; // List of points
  QList<TstaffLineChart*> m_lines; // list of lines betwen points
  TquestionPoint *m_curPoint;
  QTimer *m_delTimer;
  TtipChart *m_tip;
  
private slots:
  void delayedDelete();
  
  
};

#endif // TMAINLINE_H
