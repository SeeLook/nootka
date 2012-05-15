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


#include "tchart.h"
#include <QGraphicsEllipseItem>
#include <QMouseEvent>
#include <cmath>
#include "txaxis.h"
#include "tyaxis.h"
#include "tabstractaxis.h"
#include <QDebug>

Tchart::Tchart(QWidget* parent) :
	QGraphicsView(parent)
{
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setDragMode(ScrollHandDrag);
	scene = new QGraphicsScene();
	setScene(scene);
	
    
  yAxis = new TYaxis();
  scene->addItem(yAxis);
  yAxis->setLength(300);
  yAxis->setMaxValue(3);
  yAxis->setPos(20, 0);
	
  xAxis = new TXaxis();
  scene->addItem(xAxis);
  xAxis->setLength(600);
  xAxis->setPos(27, yAxis->boundingRect().height()-5);

  // stupid trick to make room for further tips of ticks of x axis
  QGraphicsEllipseItem *el = new QGraphicsEllipseItem();
  el->setPen(Qt::NoPen);
  scene->addItem(el);
  el->setRect(0, 0, 15, xAxis->rectBoundText("X").height()*4);
  el->setPos(27, yAxis->boundingRect().height() + 15);
  
}

Tchart::~Tchart()
{
  delete xAxis;
  delete yAxis;
}

void Tchart::zoom(bool in) {
    double coef = 1.125;
    if (!in)
        coef = 0.888889;
    scale(coef, coef);
}


void Tchart::wheelEvent(QWheelEvent* event) {
  if (event->modifiers() == Qt::ControlModifier) {
//    double deg = -event->delta() / 8.0;
//    double step = deg / 15.0;
//    double coef = std::pow(1.125, step);
//    qDebug() << event->delta() << coef;
//      double coef = 1.125;
//      if (event->delta() > 0)
//          coef = 0.888889;
//      scale(coef, coef);
      if  (event->delta() > 0)
          zoom(true);
      else
          zoom(false);
  }
}

void Tchart::resizeEvent(QResizeEvent* event ) {
//   double coef = event->oldSize().height() / event->size().height();
//   scale(coef, coef);
}
