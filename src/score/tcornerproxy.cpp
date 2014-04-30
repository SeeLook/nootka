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
#include <QGraphicsLayout>
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
	m_proxy->setFlag(ItemIgnoresTransformations);
// corner spot
	m_spot = scene->addEllipse(0.0, 0.0, 3.0, 3.0, Qt::NoPen, Qt::NoBrush);
	m_spot->setParentItem(this);
	m_spot->hide();
	m_spot->setGraphicsEffect(new QGraphicsBlurEffect());
	setSpotColor(QColor(0, 0, 255, 20));
	sceneRectChangedSlot();
	
	connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChangedSlot()));
	connect(m_view->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sceneScrolled()));
	connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString))); // for forwarding tips
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
	}
}


void TcornerProxy::setSpotColor(const QColor& c) {
	m_colorOfSpot = c;
	m_spot->setBrush(QBrush(m_colorOfSpot));
	QGraphicsDropShadowEffect *dse = new QGraphicsDropShadowEffect;
	dse->setOffset(0.0, 0.0);
	QColor col(m_colorOfSpot);
	col.setAlpha(150);
	dse->setColor(col);
	proxy()->setGraphicsEffect(dse);
}


//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TcornerProxy::sceneRectChangedSlot() {
	proxy()->adjustSize(); // adjust proxy size to widget size
	m_side = m_view->rect().height() / 7.0;
	m_spot->setRect(0.0, 0.0, side() * 0.75, side() * 0.75);
	m_spot->setPos(side() * -0.375, side() * -0.375);
	static_cast<QGraphicsDropShadowEffect*>(proxy()->graphicsEffect())->setBlurRadius(side() / 2.0);
	sceneScrolled();
}


void TcornerProxy::sceneScrolled() {
// determine TcornerProxy position depends on the corner - in scaled scene coordinates
	QRectF vv = m_view->mapToScene(m_view->rect().adjusted(0, 0,
									m_view->verticalScrollBar()->isVisible() ? -m_view->verticalScrollBar()->width() : 0, 0)).boundingRect();
	qreal xx = vv.x(), yy = vv.y();
	if (m_corner == Qt::BottomLeftCorner || m_corner == Qt::BottomRightCorner)
		yy = vv.y() + vv.height();
	if (m_corner == Qt::TopRightCorner || m_corner == Qt::BottomRightCorner)
		xx = vv.x() + vv.width();
	setPos(xx, yy);
// determine proxy widget position
  qreal gap = (side() / 4.0) / m_view->transform().m11();
	if (m_corner == Qt::BottomLeftCorner || m_corner == Qt::BottomRightCorner)
		yy = yy - proxy()->boundingRect().height() / m_view->transform().m11() - gap;
  else
    yy += gap;
	if (m_corner == Qt::TopRightCorner || m_corner == Qt::BottomRightCorner)
		xx = xx - proxy()->boundingRect().width() / m_view->transform().m11() - gap;
  else
    xx += gap;
	proxy()->setPos(xx, yy);
// 	qDebug() << "main pos" << pos() << "spot" << m_spot->pos() << "proxy" << proxy()->pos() << vv;
}


void TcornerProxy::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
	qreal xx = qAbs<qreal>(event->pos().x()), yy = qAbs<qreal>(event->pos().y());
	qreal reachPoint = qMin(xx, yy);
	qreal alphaFactor = (side() / 2.0 - reachPoint) / (side() / 2.0);
	QColor spotColor = m_colorOfSpot;
	spotColor.setAlpha(qMin(20 + (int)(alphaFactor * 250.0), 255));
	m_spot->setBrush(QBrush(spotColor));
// 	qreal moveGap = (side() /2.0 - reachPoint) / 2.0;
// 	m_spot->setPos(moveGap, moveGap);
// 	qDebug() << reachPoint << alphaFactor << event->pos();
	if (reachPoint < side() / 8.0) {
		proxy()->show();
	}
}


void TcornerProxy::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	m_spot->show();
// 	sceneRectChangedSlot();
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
	} else if (event->type() == QEvent::Leave) {
      m_spot->hide();
      proxy()->hide();
	}
	return QObject::eventFilter(ob, event);
}






