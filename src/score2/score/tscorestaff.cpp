/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include "tscorestaff.h"
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>

TscoreStaff::TscoreStaff(QGraphicsScene* scene)
{
//   m_palette = scene()->views()[0]->palette();
  setAcceptHoverEvents(true);
  for (int i = 0; i < 5; i++) {
    m_lines[i] = new QGraphicsLineItem();
    scene->addItem(m_lines[i]);
    m_lines[i]->setPen(QPen(scene->views()[0]->palette().text().color(), 0.1));
    m_lines[i]->setLine(1, 16 + i * 2, boundingRect().width() - 2, 16 + i * 2);
  }
}


TscoreStaff::~TscoreStaff()
{
}


void TscoreStaff::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  emit statusTip("TscoreStaff");
}

void TscoreStaff::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  emit statusTip("");
}


void TscoreStaff::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
QGraphicsItem::hoverMoveEvent(event);
}



void TscoreStaff::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {

}


QRectF TscoreStaff::boundingRect() const {
  return QRectF(0, 0, 80, 40);
}


