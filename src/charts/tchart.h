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
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef TCHART_H
#define TCHART_H

#include <QGraphicsView>
#include "tmainline.h"

class TYaxis;
class TXaxis;
// class TabstractAxis;



/** This is base class for charts in Nootka.
 * It has got QGraphicsScene *scene and
 * two axises xAxis and yAxis witch are created by default.
 */
class Tchart : public QGraphicsView
{
	Q_OBJECT
	
	friend class TmainLine;
	
public:
    Tchart(QWidget* parent = 0);
    virtual ~Tchart();
    
protected:
    void wheelEvent(QWheelEvent *event);
      /** Keeps a chart scaled when user changes its size. */
    void resizeEvent(QResizeEvent *event);
    
    QGraphicsScene *scene;
    TXaxis *xAxis;
    TYaxis *yAxis;
    
    
private:
    
};

#endif // TCHART_H
