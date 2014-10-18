/***************************************************************************
 *   Copyright (C) 2012-2014 by Tomasz Bojczuk                             *
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


#include "tchart.h"
#include "txaxis.h"
#include "tyaxis.h"
#include "tabstractaxis.h"
#include "ttiphandler.h"
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <QTimer>


Tchart::Tchart(QWidget* parent) :
	QGraphicsView(parent)
{
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setDragMode(ScrollHandDrag);
	scene = new QGraphicsScene(this);
	setScene(scene);
	
    
  yAxis = new TYaxis();
  scene->addItem(yAxis);
  yAxis->setLength(400);
  yAxis->setMaxValue(3);
  yAxis->setPos(45, 0);
  yAxis->setZValue(55);
	
  xAxis = new TXaxis();
  scene->addItem(xAxis);
  xAxis->setLength(550);
  xAxis->setPos(52, yAxis->boundingRect().height() - 7);
	
	QTimer::singleShot(20, this, SLOT(ajustChartHeight()));

}


void Tchart::zoom(bool in) {
    double coef = 1.125;
    if (!in)
        coef = 0.888889;
    scale(coef, coef);
}


void Tchart::ajustChartHeight() {
	qreal factor = (viewport()->rect().height() / scene->sceneRect().height());
	if (viewport()->rect().width() > scene->sceneRect().width()) {
		factor = (viewport()->rect().height() / scene->sceneRect().height()) * 0.95;
		setSceneRect(0, 0, viewport()->rect().width() / factor, scene->sceneRect().height());
	}
	scale(factor, factor);
}



//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

bool Tchart::event(QEvent* event) {
  if (event->type() == QEvent::Wheel) {
    QWheelEvent *we = static_cast<QWheelEvent *>(event);
    if (we->modifiers() == Qt::ControlModifier) {
      if  (we->delta() > 0)
          zoom(true);
      else
          zoom(false);
      return true;
    }
  }
  if (event->type() == QEvent::Leave) // To give a last possibility to remove un-deleted tip
      if (TtipHandler::deleteTip())
        scene->update(); 
  return QGraphicsView::event(event);
}



