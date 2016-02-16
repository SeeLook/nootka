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

#include "tscoreitem.h"
#include "tscorescene.h"
#include <touch/ttouchproxy.h>
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>
#include <QMouseEvent>
// #include <QDebug>


/* static */
bool TscoreItem::touchEnabled() {
  return TtouchProxy::touchEnabled();
}




TscoreItem::TscoreItem(TscoreScene* scene) :
  m_scene(scene),
  m_statusTip(QString()),
  m_staff(0),
  m_hasCursor(false)
{
  m_scene->addItem(this);
  if (touchEnabled())
    setAcceptTouchEvents(true);
  else
    setAcceptHoverEvents(true);
}

void TscoreItem::setStatusTip(QString status) {
  m_statusTip = status;
  if (m_statusTip.isEmpty())
      disconnect(this, SIGNAL(statusTip(QString)), m_scene, SLOT(statusTipChanged(QString)));
  else
      connect(this, SIGNAL(statusTip(QString)), m_scene, SLOT(statusTipChanged(QString)), Qt::UniqueConnection);
}

//####################################################################################
//###############################  TOUCH EVENTS ######################################
//####################################################################################

void TscoreItem::touched(const QPointF& scenePos) {
	m_hasCursor = true;
}


void TscoreItem::untouched(const QPointF& scenePos) {
	Q_UNUSED(scenePos)
	m_hasCursor = false;
}

//####################################################################################
//###############################  PROTECTED #########################################
//####################################################################################

void TscoreItem::paintBackground(QPainter* painter, QColor bgColor) {
	QLinearGradient gr(boundingRect().topLeft(), boundingRect().topRight());
	QColor c1 = bgColor;
	c1.setAlpha(80);
	QColor c2 = bgColor;
	c2.setAlpha(40);
	gr.setColorAt(0.0, c1);
	gr.setColorAt(0.1, c2);
	gr.setColorAt(0.9, c2);
	gr.setColorAt(1.0, c1);
	painter->setBrush(gr);
	painter->setPen(Qt::NoPen);
	painter->drawRect(boundingRect());
}


void TscoreItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  Q_UNUSED(event);
	m_hasCursor = true;
  if (!m_statusTip.isEmpty())
    emit statusTip(m_statusTip);
}

void TscoreItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  Q_UNUSED(event);
	m_hasCursor = false;
  emit statusTip(QString());
}


void TscoreItem::registryItem(QGraphicsItem* item) {
  scene()->addItem(item);
  item->setParentItem(this);
}









