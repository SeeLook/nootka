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


#include "tscorenote.h"
#include "tscorescene.h"
#include <QGraphicsEffect>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QGraphicsView>
#include <QDebug>

TscoreNote::TscoreNote(TscoreScene* scene) :
  TscoreItem(scene),
  m_workPosY(0), m_mainPosY(0)
{

//   setAcceptHoverEvents(false);
  m_workColor = scene->views()[0]->palette().highlight().color();
  m_workColor.setAlpha(200);
  
  m_workNote = createNoteHead();
  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
  blur->setBlurRadius(0.7);
  m_workNote->setGraphicsEffect(blur);
  m_workNote->setBrush(m_workColor);
  m_workNote->setPen(m_workColor);
  
  
  setStatusTip(tr("Click to sellect a note, use mouse wheel to change accidentals."));
}


TscoreNote::~TscoreNote() {}

void TscoreNote::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setBrush(QColor(1, 1, 1, 50));
  painter->setPen(Qt::NoPen);
  painter->drawRect(boundingRect());
}


QRectF TscoreNote::boundingRect() const{
  return QRectF(0, 0, 6, 40);
}

//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreNote::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  m_workNote->show();
  TscoreItem::hoverEnterEvent(event);
}


void TscoreNote::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  m_workNote->hide();
  TscoreItem::hoverLeaveEvent(event);
}

void TscoreNote::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
    if (event->pos().y() != m_workPosY) {
      m_workPosY = event->pos().y();
      m_workNote->setPos(2.5, m_workPosY);
    }
}



void TscoreNote::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  qDebug("mousePressEvent");
}


void TscoreNote::wheelEvent(QGraphicsSceneWheelEvent* event) {
  qDebug("wheelEvent");
}

//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################

QGraphicsEllipseItem* TscoreNote::createNoteHead() {
  QGraphicsEllipseItem *noteHead = new QGraphicsEllipseItem();
  scoreScene()->addItem(noteHead);
  noteHead->setParentItem(this);
  noteHead->setRect(0, 0, 3, 2);
  noteHead->hide();
  return noteHead;
}


