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
  
}

Tchart::~Tchart()
{
  delete xAxis;
  delete yAxis;
}

void Tchart::wheelEvent(QWheelEvent* event) {
  double deg = -event->delta() / 8.0;
  double step = deg / 15.0;
  double coef = std::pow(1.125, step);
  scale(coef, coef);
}

