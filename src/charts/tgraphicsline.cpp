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

#include "tgraphicsline.h"
#include "tgraphicstexttip.h"
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QTimer>

TgraphicsLine::TgraphicsLine(QString text) :
  QGraphicsLineItem(),
  m_tip(0),
  m_text(text)
{
  setAcceptHoverEvents(true);
  m_delTimer = new QTimer();
  connect(m_delTimer, SIGNAL(timeout()), this, SLOT(delayedDelete()));
}

TgraphicsLine::~TgraphicsLine()
{
  if (m_tip)
    delete m_tip;
  delete m_delTimer;
}

void TgraphicsLine::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  if (m_tip || m_text == "")
        return;
  m_tip = new TgraphicsTextTip(m_text, pen().color());
  scene()->addItem(m_tip);
  m_tip->setFlag(QGraphicsItem::ItemIgnoresTransformations);
  m_tip->setZValue(77);
  m_tip->setPos(event->pos().x() - m_tip->boundingRect().width() / 2, event->pos().y());  
}

void TgraphicsLine::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  if (!m_delTimer->isActive()) {
        m_delTimer->start(30);
    }
}

void TgraphicsLine::delayedDelete() {
  if (isUnderMouse())
        return;
  m_delTimer->stop();
  if (m_tip) {
      scene()->removeItem(m_tip);
      delete m_tip;
      m_tip = 0;
  }
}


