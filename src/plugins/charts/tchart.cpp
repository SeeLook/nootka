/***************************************************************************
 *   Copyright (C) 2012-2015 by Tomasz Bojczuk                             *
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


#include "tchart.h"
#include "txaxis.h"
#include "tyaxis.h"
#include "tabstractaxis.h"
#include "ttiphandler.h"
#include <QGraphicsEllipseItem>
#include <QScrollBar>
#include <QMouseEvent>
#include <QTimer>
// #include <QDebug>


Tchart::Tchart(QWidget* parent) :
	QGraphicsView(parent)
{
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setDragMode(ScrollHandDrag);
	scene = new TscoreScene(this);
	setScene(scene);
	
    
  yAxis = new TYaxis();
  scene->addItem(yAxis);
  yAxis->setLength(300);
  yAxis->setMaxValue(9);
  yAxis->setPos(45, 0);
  yAxis->setZValue(55);
	
  xAxis = new TXaxis();
  scene->addItem(xAxis);
  xAxis->setLength(550);
  xAxis->setPos(52, yAxis->boundingRect().height() - 7);

}


void Tchart::zoom(bool in) {
  double coef = 1.125;
  if (!in)
      coef = 0.888889;
  scale(coef, coef);
}


void Tchart::ajustChartHeight() {
	qreal factor = (viewport()->rect().height() / scene->sceneRect().height()) * 0.95;
	if (viewport()->rect().width() > (scene->sceneRect().width() + qAbs(sceneRect().x())) * factor) {
		setSceneRect(0, 0, viewport()->rect().width() / factor, scene->sceneRect().height());
	}	else {
    setSceneRect(sceneRect().adjusted(-sceneRect().x(), 0, qAbs(sceneRect().x()), 0));
	}
	scale(factor, factor);
	horizontalScrollBar()->setValue(horizontalScrollBar()->minimum());
}



//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

bool Tchart::event(QEvent* event) {
  if (event->type() == QEvent::Leave) // To give a last possibility to remove un-deleted tip
      if (TtipHandler::deleteTip())
        scene->update(); 
  return QGraphicsView::event(event);
}


void Tchart::wheelEvent(QWheelEvent* event) {
  if (event->modifiers() == Qt::ControlModifier) {
    if  (event->angleDelta().y() > 0)
        zoom(true);
    else if  (event->angleDelta().y() < 0)
        zoom(false);
  } else // normal wheel behavior - scrolling a chart
      QGraphicsView::wheelEvent(event);
}






