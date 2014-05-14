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

#include "tscoreitem.h"
#include "tscorescene.h"
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>
#include <QEvent>
#include <QTouchEvent>

#include <QDebug>

TscoreItem::TscoreItem(TscoreScene* scene) :
  m_scene(scene),
  m_statusTip(""),
  m_staff(0)
{
  m_scene->addItem(this);
#if defined (Q_OS_ANDROID)
	setAcceptTouchEvents(true);	
#else
  setAcceptHoverEvents(true);
#endif

}

void TscoreItem::setStatusTip(QString status) {
  m_statusTip = status;
  if (m_statusTip == "")
      disconnect(this, SIGNAL(statusTip(QString)), m_scene, SLOT(statusTipChanged(QString)));
  else
      connect(this, SIGNAL(statusTip(QString)), m_scene, SLOT(statusTipChanged(QString)));
}

//####################################################################################
//###############################  PROTECTED #########################################
//####################################################################################

#if defined (Q_OS_ANDROID)
bool TscoreItem::sceneEvent(QEvent* ev) {
	if (ev->type() == QEvent::TouchBegin || ev->type() == QEvent::TouchUpdate || ev->type() == QEvent::TouchEnd) {
		QTouchEvent *te = static_cast<QTouchEvent*>(ev);
// 		if (te->touchPoints().count() == 1) {
			qDebug() << te->touchPoints().count() << te->touchPoints().first().state();
			QGraphicsSceneHoverEvent *hover = 0;
			if (te->touchPoints().first().state())
			switch(te->touchPoints().first().state()) {
				case Qt::TouchPointPressed:
					ev->accept();
					hoverEnterEvent(hover = touchToHover(te, QEvent::GraphicsSceneHoverEnter));
					break;
				case Qt::TouchPointMoved:
					hoverMoveEvent(hover = touchToHover(te, QEvent::GraphicsSceneHoverMove));
					break;
				case Qt::TouchPointReleased:
					hoverLeaveEvent(0);
					break;
				default:
					break;
			}
			if (hover)
				delete hover;
		}			 
// 		for (int i = 0; i < te->touchPoints().size(); i++)
// 			qDebug() << te->touchPoints()[i].pos() << te->touchPoints()[i].pressure() << te->touchPoints()[i].velocity();
// 	}
	return QGraphicsObject::event(ev);
}
#endif


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
  if (m_statusTip != "")
    emit statusTip(m_statusTip);
}

void TscoreItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  emit statusTip("");
}


void TscoreItem::registryItem(QGraphicsItem* item) {
  scene()->addItem(item);
  item->setParentItem(this);
}

//####################################################################################
//###############################  PROTECTED #########################################
//####################################################################################
#if defined (Q_OS_ANDROID)
QGraphicsSceneHoverEvent* TscoreItem::touchToHover(QTouchEvent* te, QEvent::Type evType) {
	QGraphicsSceneHoverEvent *hover = new QGraphicsSceneHoverEvent(evType);
	hover->setPos(te->touchPoints().first().pos());
	hover->setLastPos(te->touchPoints().first().lastPos());
	return hover;
}
#endif









