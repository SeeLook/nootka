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


#include "ttiphandler.h"
#include "tgraphicstexttip.h"
#include "tstatisticstip.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>


/*static*/
TgraphicsTextTip* TtipHandler::tip = 0;
QTimer* TtipHandler::m_delTimer = 0;
QGraphicsObject* TtipHandler::m_initObject = 0;


TtipHandler::TtipHandler()
{
  setAcceptHoverEvents(true);
  if (!m_delTimer) {
    m_delTimer = new QTimer();
    connect(m_delTimer, SIGNAL(timeout()), this, SLOT(delayedDelete()));
  }
}

TtipHandler::~TtipHandler() {
  if (m_delTimer) {
    delete m_delTimer;
    m_delTimer = 0;
  }
  deleteTip();
}


bool TtipHandler::deleteTip() {
  if (tip) {
    delete tip;
    tip = 0;
    return true;
  }
  return false;
}




void TtipHandler::handleTip(QPointF scenePos) {
    if (m_delTimer && m_delTimer->isActive()) {
      m_delTimer->stop();
    }
    if (tip) {
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
    }
}


void TtipHandler::hoverLeaveEvent(QGraphicsSceneHoverEvent*) {
    if (m_delTimer /*&& !m_delTimer->isActive()*/) {
        m_delTimer->start(350);
    }
}

void TtipHandler::delayedDelete() {
    if (isUnderMouse())
        return;
    m_delTimer->stop();
    if (tip) {
        deleteTip();
        update();
        scene()->update();
    }
}


