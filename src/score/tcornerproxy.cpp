/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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



#include "tcornerproxy.h"
#include <score/tscorescene.h>
#include <QApplication>
#include <QGraphicsView>
#include <QGraphicsEffect>
#include <QScrollBar>
#include <QGraphicsSceneHoverEvent>
#include <QTimer>
#include <QDebug>


TcornerProxy::TcornerProxy(TscoreScene* scene, QWidget* widget, Qt::Corner cornerPos) :
	TscoreItem(scene),
	m_corner(cornerPos)
{
	m_view = scene->views()[0];
	setFlags(ItemHasNoContents | ItemIgnoresTransformations);
	setZValue(100);
// proxy widget
	m_proxy = scene->addWidget(widget);
	m_proxy->setZValue(105);
	m_proxy->hide();
	proxy()->setVisible(false);
	m_proxy->setFlag(ItemIgnoresTransformations);
// corner spot
	QColor spotColor(0, 0, 255, 20);
	m_spot = scene->addEllipse(0.0, 0.0, 3.0, 3.0, /*QPen(spotColor, 1.0)*/Qt::NoPen, QBrush(spotColor));
	m_spot->setParentItem(this);
	m_spot->hide();
	m_spot->setGraphicsEffect(new QGraphicsBlurEffect());
	sceneRectChangedSlot();
	
	connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChangedSlot()));
	connect(m_view->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sceneScrolled()));
	connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString)));
	widget->installEventFilter(this);
}


QRectF TcornerProxy::boundingRect() const {
	return QRectF(m_side / -2.0, m_side / -2.0, m_side, m_side);
}


QPainterPath TcornerProxy::shape() const {
	QPainterPath pp;
	pp.addEllipse(boundingRect());
	return pp;
}


void TcornerProxy::hideWithDelay() {
	if (isUnderMouse() || (proxy()->isVisible() && proxy()->isUnderMouse())) {
			QTimer::singleShot(300, this, SLOT(hideWithDelay()));
	} else {
			m_spot->hide();
			proxy()->hide();
			proxy()->setVisible(false);
	}
}

//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TcornerProxy::sceneRectChangedSlot() {
	m_side = m_view->rect().height() / 5.0;
	m_spot->setRect(side() * -0.25, side() * -0.25, side() * 0.5, side() * 0.5);
// 	m_spot->setRect(side() * -0.125, side() * -0.125, side() * 0.75, side() * 0.75);
// 	m_spot->setRect(boundingRect());
// 	m_spot->setPos(side() * 0.25, 0.0);
	m_spot->setPos(0.0, 0.0);
	sceneScrolled();
}


void TcornerProxy::sceneScrolled() {
// determine TcornerProxy position depends on corner - in scaled scene coordinates
	QRectF vv = m_view->mapToScene(m_view->rect().adjusted(0.0, 0.0,
									m_view->verticalScrollBar()->isVisible() ? -m_view->verticalScrollBar()->width() : -5,
																	m_view->verticalScrollBar()->value())).boundingRect();
	qreal xx = vv.x(), yy = vv.y();
	if (m_corner == Qt::BottomLeftCorner || m_corner == Qt::BottomRightCorner)
		yy = vv.height() - (side() / 4.0) / m_view->transform().m11();
	if (m_corner == Qt::TopRightCorner || m_corner == Qt::BottomRightCorner)
		xx = vv.width() - (side() / 8.0) / m_view->transform().m11();
	setPos(xx, yy);
// determine proxy widget position
	if (m_corner == Qt::BottomLeftCorner || m_corner == Qt::BottomRightCorner)
		yy = yy - (2 + proxy()->boundingRect().height()) / m_view->transform().m11();
	if (m_corner == Qt::TopRightCorner || m_corner == Qt::BottomRightCorner)
		xx = xx - (2 + proxy()->boundingRect().width()) / m_view->transform().m11();
	proxy()->setPos(xx, yy);
	qDebug() << "main pos" << pos() << "spot" << m_spot->pos() << "proxy" << proxy()->pos() << vv;
}


void TcornerProxy::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
	qreal xx = qAbs<qreal>(event->pos().x()), yy = qAbs<qreal>(event->pos().y());
// 	if (m_corner == Qt::BottomLeftCorner || m_corner == Qt::BottomRightCorner)
// 			yy = side() - yy;
	if (m_corner == Qt::TopRightCorner || m_corner == Qt::BottomRightCorner)
			xx = side() - xx;
	qreal reachPoint = qMin(xx, yy);
	qreal alphaFactor = (side() / 2.0 - reachPoint) / (side() / 2.0);
	QColor spotColor(0, 0, 255, qMin(20 + (int)(alphaFactor * 225.0), 255));
	m_spot->setBrush(QBrush(spotColor));
// 	qreal moveGap = (side() /2.0 - reachPoint) / 2.0;
// 	m_spot->setPos(moveGap, moveGap);
// 	qDebug() << reachPoint << alphaFactor << event->pos();
	if (reachPoint < side() / 8.0) {
		proxy()->setVisible(true);
		proxy()->show();
	}
}


void TcornerProxy::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	m_spot->show();
	sceneRectChangedSlot();
	TscoreItem::hoverEnterEvent(event);
}


void TcornerProxy::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
	QTimer::singleShot(300, this, SLOT(hideWithDelay()));
	TscoreItem::hoverLeaveEvent(event);
}


bool TcornerProxy::eventFilter(QObject* ob, QEvent* event) {
	if (event->type() == QEvent::StatusTip) {
		QStatusTipEvent *tipEvent = static_cast<QStatusTipEvent*>(event);
		emit statusTip(tipEvent->tip());
	}
	return QObject::eventFilter(ob, event);
}






