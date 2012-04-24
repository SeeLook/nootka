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
#include "taxis.h"
#include "tyaxis.h"

Tchart::Tchart(QWidget* parent) :
	QGraphicsView(parent)
{
	setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
	setDragMode(ScrollHandDrag);
	m_scene = new QGraphicsScene();
	setScene(m_scene);
	
// 	QGraphicsEllipseItem *point = new QGraphicsEllipseItem();
// 	m_scene->addItem(point);
// 	point->setRect(0, 0, 30, 30);
  
    
  TYaxis *axisY = new TYaxis();
  axisY->setMaxValue(450);
  m_scene->addItem(axisY);
  axisY->setLenght(300);
  axisY->setPos(10, 0);
	
  Taxis *axisX = new Taxis();
  m_scene->addItem(axisX);
  axisX->setWidth(1800);
  axisX->setPos(10, axisY->boundingRect().height()-16);
}

Tchart::~Tchart()
{}

void Tchart::wheelEvent(QWheelEvent* event) {
  double deg = -event->delta() / 8.0;
  double step = deg / 15.0;
  double coef = std::pow(1.125, step);
  scale(coef, coef);
}

