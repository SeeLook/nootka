/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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


#include "ttiphandler.h"
#include <graphics/tgraphicstexttip.h>
#include "tstatisticstip.h"
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
// #include <QDebug>


/*static*/
QPointer<TgraphicsTextTip>        TtipHandler::tip = 0;
QPointer<QTimer>                  TtipHandler::m_hideTimer = 0;
QPointer<QTimer>                  TtipHandler::m_showTimer = 0;
QGraphicsObject*                  TtipHandler::m_initObject = 0;
bool                              TtipHandler::m_mouseOverTip = false;


TtipHandler::TtipHandler() :
  m_entered(false)
{
  setAcceptHoverEvents(true);
  if (!m_hideTimer) {
    m_hideTimer = new QTimer();
    connect(m_hideTimer, SIGNAL(timeout()), this, SLOT(hideTip()));
  }
  if (!m_showTimer) {
    m_showTimer = new QTimer();
    connect(m_showTimer, SIGNAL(timeout()), this, SLOT(showTip()));
  }
}

TtipHandler::~TtipHandler() {
  deleteTip();
  if (m_hideTimer)
    delete m_hideTimer;
  if (m_showTimer)
    delete m_showTimer;
}


bool TtipHandler::deleteTip() {
  if (tip) {
    m_hideTimer->stop();
    m_showTimer->stop();
    delete tip;
    return true;
  }
  return false;
}


void TtipHandler::handleTip(QPointF scenePos) {
  if (m_hideTimer)
    m_hideTimer->stop();
  m_entered = true;
  if (tip && !m_showTimer->isActive()) {
    tip->hide();
    m_showTimer->start(300);
    m_initObject = this;
    scene()->addItem(tip);
    tip->setFlag(QGraphicsItem::ItemIgnoresTransformations);
    QSize s = scene()->views()[0]->size();
    QPointF mapedP = scene()->views()[0]->mapFromScene(scenePos);
//       determine where to display tip when point is near a view boundaries
    if (mapedP.x() > (s.width() / 2) ) // tip on the left
        scenePos.setX(scenePos.x() - tip->boundingRect().width() / scene()->views()[0]->transform().m11());
    if (mapedP.y() > (s.height() / 2) )
        scenePos.setY(scenePos.y() - tip->boundingRect().height() / scene()->views()[0]->transform().m22());
    tip->setPos(scenePos);
    tip->setZValue(70);
    update();
    connect(tip, SIGNAL(entered()), this, SLOT(tipEnteredSlot()));
    connect(tip, SIGNAL(leaved()), this, SLOT(tipLeavedSlot()));
  }
}


void TtipHandler::tipEnteredSlot() {
  m_mouseOverTip = true;
  m_hideTimer->stop();
}


void TtipHandler::tipLeavedSlot() {
  m_mouseOverTip = false;
  m_hideTimer->start(350);
}


void TtipHandler::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
  if (m_hideTimer) {
    m_hideTimer->start(350);
  }
  m_entered = false;
}


void TtipHandler::showTip() {
  if (m_showTimer)
    m_showTimer->stop();
  tip->show();
}


void TtipHandler::hideTip() {
  if (m_entered || m_mouseOverTip)
      return;
  m_hideTimer->stop();
  if (tip) {
    deleteTip();
    scene()->update();
  }
}


