/***************************************************************************
 *   Copyright (C) 2012-2013 by Tomasz Bojczuk                             *
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
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include "txaxis.h"
#include "tyaxis.h"
#include "tabstractaxis.h"
#include <QDebug>



Tchart::Tchart(QWidget* parent) :
	QGraphicsView(parent)
{
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setDragMode(ScrollHandDrag);
	scene = new QGraphicsScene(this);
	setScene(scene);
	
    
  yAxis = new TYaxis();
  scene->addItem(yAxis);
  yAxis->setLength(300);
  yAxis->setMaxValue(3);
  yAxis->setPos(20, 0);
  yAxis->setZValue(55);
	
  xAxis = new TXaxis();
  scene->addItem(xAxis);
  xAxis->setLength(600);
  xAxis->setPos(27, yAxis->boundingRect().height() - 7);

  // stupid trick to make room for further tips of ticks of x axis
  QGraphicsEllipseItem *el = new QGraphicsEllipseItem();
  el->setPen(Qt::NoPen);
  scene->addItem(el);
  el->setRect(0, 0, 15, xAxis->rectBoundText("X").height()*4);
  el->setPos(27, yAxis->boundingRect().height() + 15);

}

Tchart::~Tchart() {}

void Tchart::zoom(bool in) {
    double coef = 1.125;
    if (!in)
        coef = 0.888889;
    scale(coef, coef);
}


//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

bool Tchart::event(QEvent* event)
{
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
  return QGraphicsView::event(event);
}



void Tchart::resizeEvent(QResizeEvent* event ) {
//   double coef = (double)event->oldSize().height() / (double)event->size().height();
//   scale(coef, coef);
}
