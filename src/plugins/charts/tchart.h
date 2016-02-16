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


#ifndef TCHART_H
#define TCHART_H

#include <QGraphicsView>
#include "tmainline.h"
#include <tscorescene.h>


class TYaxis;
class TXaxis;



/** 
 * This is base class for charts in Nootka.
 * It has got @p QGraphicsScene *scene() and
 * two axis-es xAxis and yAxis which are created by default.
 */
class Tchart : public QGraphicsView
{
	Q_OBJECT
	
	friend class TmainLine;
	
public:
    
  enum EanswersOrder {
    e_byNumber,
    e_byNote,
    e_byFret,
    e_byAccid,
    e_byKey,
    e_byMistake,
    e_byQuestAndAnsw, // both above
    e_byStyle,
    e_byClef,
  };
  
  enum EchartType {
    e_linear, e_bar, e_pie
  }; /** Types of charts. */

  struct Tsettings {
    bool inclWrongAnsw; // include wrong answers to average time of sorted group of answers
    bool separateWrong; // separate wrong answers and correct/almost good ones
    EanswersOrder order;
    EchartType type;
    TmainLine::EyValue yValue;
  };

  Tchart(QWidget* parent = 0);
  
  virtual void setAnalyse(EanswersOrder order) {} // prototype only

  void zoom(bool in = true); // zoom view, by default zoom in but when false zoom out
  TscoreScene *scene;
  
protected:
  virtual bool event(QEvent* event);
  virtual void wheelEvent(QWheelEvent* event);
  
  TXaxis *xAxis;
  TYaxis *yAxis;
  
protected slots:
void ajustChartHeight(); /** Invoked by timer after chart creation to adjust scene height to view height. */
    
private:
    
};

#endif // TCHART_H
