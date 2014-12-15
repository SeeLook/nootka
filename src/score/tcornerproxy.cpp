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
#include <QMenu>
#include <QBoxLayout>
#include <QDesktopWidget>
#include <QTimer>
#include <QDebug>

#if defined (Q_OS_ANDROID)
	#define PROXYTIME (1500)
#else
	#define PROXYTIME (300)
#endif


TcornerProxy::TcornerProxy(TscoreScene* scene, QWidget* widget, Qt::Corner cornerPos) :
	TscoreItem(scene),
	m_proxy(0),
	m_corner(cornerPos),
	m_widget(widget),
	m_widgetHasMouse(false)
{
	enableTouchToMouse(false);
	m_view = scene->views()[0];
	setFlags(ItemHasNoContents | ItemIgnoresTransformations);
	setZValue(100);
#if !defined (Q_OS_ANDROID)
// proxy widget
	m_proxy = scene->addWidget(widget);
	m_proxy->setZValue(105);
	m_proxy->setFlag(ItemIgnoresTransformations);
	if (widget)
			widget->installEventFilter(this);
#endif
// corner spot
#if defined (Q_OS_ANDROID) // bigger corner spot for Android
	m_side = qMin(qApp->desktop()->availableGeometry().width(), qApp->desktop()->availableGeometry().height()) / 8.0;
	m_spot = scene->addEllipse(0.0, 0.0, side() * 1.5, side() * 1.5, Qt::NoPen, Qt::NoBrush);
#else
	m_side = qMin(qApp->desktop()->availableGeometry().width(), qApp->desktop()->availableGeometry().height()) / 15.0;
	m_spot = scene->addEllipse(0.0, 0.0, side() * 0.75, side() * 0.75, Qt::NoPen, Qt::NoBrush);
#endif
	m_spot->setParentItem(this);
	QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
	blur->setBlurRadius(side() / 5.0);
	m_spot->setGraphicsEffect(blur);
	setSpotColor(QColor(0, 0, 255, 20));
	
	connect(scene, SIGNAL(sceneRectChanged(QRectF)), this, SLOT(sceneRectChangedSlot()));
	connect(m_view->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(sceneScrolled()));
	connect(this, SIGNAL(statusTip(QString)), scene, SLOT(statusTipChanged(QString))); // for forwarding tips
	
	QTimer::singleShot(50, this, SLOT(sceneRectChangedSlot()));
	QTimer::singleShot(2000, this, SLOT(hideSpot()));
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
#if !defined (Q_OS_ANDROID)
	if (hasCursor() || m_widgetHasMouse) {
			QTimer::singleShot(PROXYTIME, this, SLOT(hideWithDelay()));
	} else {
			hideSpot();
	}
#endif
}


void TcornerProxy::setSpotColor(const QColor& c) {
	m_colorOfSpot = c;
	m_spot->setBrush(QBrush(m_colorOfSpot));
	QGraphicsDropShadowEffect *dse = new QGraphicsDropShadowEffect;
	dse->setOffset(0.0, 0.0);
	QColor col(m_colorOfSpot);
	col.setAlpha(180);
	dse->setColor(col);
	dse->setBlurRadius(side() / 4.0);
	if (proxy())
			proxy()->setGraphicsEffect(dse);
}


void TcornerProxy::hideSpot() {
	m_spot->hide();
	proxy()->hide();
}


//####################################################################################################
//########################################## PROTECTED ###############################################
//####################################################################################################

void TcornerProxy::sceneRectChangedSlot() {
	QTimer::singleShot(10, this, SLOT(sceneScrolled()));
	// This way spots are always in the place
}


void TcornerProxy::sceneScrolled() {
#if defined (Q_OS_ANDROID) // bigger corner spot for Android
	if (m_widget)
		m_widget->adjustSize();
	m_spot->setPos(side() * -0.75, side() * -0.75);
#else
	proxy()->adjustSize(); // adjust proxy size to widget size
	m_spot->setPos(side() * -0.375, side() * -0.375);
#endif
// determine TcornerProxy position depends on the corner - in scaled scene coordinates
	int scrollOffset = -1;
	if (m_view->rect().height() < scoreScene()->sceneRect().height() * m_view->transform().m11())
			scrollOffset = -m_view->verticalScrollBar()->width();
	QRectF vv = m_view->mapToScene(m_view->rect().adjusted(0, 0, scrollOffset, 0)).boundingRect();
	qreal xx = vv.x(), yy = vv.y();
	if (m_corner == Qt::BottomLeftCorner || m_corner == Qt::BottomRightCorner)
		yy = vv.y() + vv.height();
	if (m_corner == Qt::TopRightCorner || m_corner == Qt::BottomRightCorner)
		xx = vv.x() + vv.width();
	setPos(xx, yy);
#if !defined (Q_OS_ANDROID)
// determine proxy widget position
  qreal gap = (side() / 4.0) / m_view->transform().m11();
	if (m_corner == Qt::BottomLeftCorner || m_corner == Qt::BottomRightCorner)
		yy = yy - proxy()->boundingRect().height() / m_view->transform().m11() - gap;
  else
    yy += gap / 2.0;
	if (m_corner == Qt::TopRightCorner || m_corner == Qt::BottomRightCorner)
		xx = xx - proxy()->boundingRect().width() / m_view->transform().m11() - gap;
  else
    xx += gap / 2.0;
	proxy()->setPos(xx, yy);
// 	qDebug() << "proxy" << proxy()->pos();
#endif
}


#if defined (Q_OS_ANDROID)
void TcornerProxy::touched(const QPointF& cPos) {
	TscoreItem::touched(cPos);
	m_spot->show();
}


void TcornerProxy::untouched(const QPointF& cPos) {
	TscoreItem::untouched(cPos);
	m_spot->hide();
}


void TcornerProxy::longTap(const QPointF& cPos) {
	Q_UNUSED(cPos)
	if (m_widget == 0) {
		emit cornerReady();
		return;
	}
	QMenu *menu = new QMenu();
	menu->setFixedSize(m_widget->size());
	m_widget->setParent(menu);
	menu->setLayout(m_widget->layout());
	menu->setStyleSheet("background-color: palette(window)");
	QPoint wPos(5, 5);
	if (m_corner == Qt::BottomLeftCorner || m_corner == Qt::BottomRightCorner)
		wPos.setY(m_view->height() - m_widget->height() - 5);
	if (m_corner == Qt::TopRightCorner || m_corner == Qt::BottomRightCorner)
		wPos.setX(m_view->width() - m_widget->width() - 5);
	menu->exec(m_view->mapToGlobal(wPos));
	m_widget->setLayout(menu->layout());
	m_widget->setParent(0);
	delete menu;
	m_spot->hide();
}
#endif


#if !defined (Q_OS_ANDROID)
void TcornerProxy::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
	qreal xx = qAbs<qreal>(event->pos().x()), yy = qAbs<qreal>(event->pos().y());
	qreal reachPoint = qMin(xx, yy);
	qreal alphaFactor = (side() / 2.0 - reachPoint) / (side() / 2.0);
	QColor spotColor = m_colorOfSpot;
	spotColor.setAlpha(qMin(20 + (int)(alphaFactor * 250.0), 255));
	m_spot->setBrush(QBrush(spotColor));
	if (reachPoint < side() / 5.0) {
		m_signalTimer = startTimer(300);
	}
}


void TcornerProxy::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
	m_spot->show();
	TscoreItem::hoverEnterEvent(event);
}


void TcornerProxy::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
	QTimer::singleShot(PROXYTIME, this, SLOT(hideWithDelay()));
	TscoreItem::hoverLeaveEvent(event);
}


void TcornerProxy::timerEvent(QTimerEvent* te) {
	if (te->timerId() == m_signalTimer) {
		killTimer(m_signalTimer);
		if (hasCursor()) {
			if (m_widget)
				proxy()->show();
			else
				emit cornerReady();
		}
	}
}


bool TcornerProxy::eventFilter(QObject* ob, QEvent* event) {
	if (event->type() == QEvent::StatusTip) {
      QStatusTipEvent *tipEvent = static_cast<QStatusTipEvent*>(event);
      emit statusTip(tipEvent->tip());
	} else if (event->type() == QEvent::Leave) {
      m_spot->hide();
      proxy()->hide();
			m_widgetHasMouse = false;
	} else if (event->type() == QEvent::Enter) {
      m_widgetHasMouse = true;
	}
	return QObject::eventFilter(ob, event);
}
#endif





