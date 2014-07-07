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


#ifndef TCORNERPROXY_H
#define TCORNERPROXY_H

#include <QGraphicsProxyWidget>
#include <score/tscoreitem.h>

class QWidget;
class QGraphicsView;
class QWidget;
class TscoreScene;


/**
 * This class represents spot in a corner of a score 
 * which calls some widget when mouse cursor is over it.
 * It grabs status tips of the widget.
 * Color of a spot can be managed through @p setSpotColor()
 */
class TcornerProxy : public TscoreItem
{

	Q_OBJECT
	
public:
	TcornerProxy(TscoreScene* scene, QWidget* widget, Qt::Corner cornerPos);
	
	QGraphicsProxyWidget* proxy() { return m_proxy; }
	qreal side() { return m_side; } /** It is square shaped so this is either width and height */
	
	void setSpotColor(const QColor& c); /** It is blue by default */
	QColor spotColor() { return m_colorOfSpot; }
	
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	
signals:
	void cornerReady();
	
public slots:
	void hideWithDelay();
	
	
protected:
#if defined (Q_OS_ANDROID)
	virtual void longTap(const QPointF& cPos);
	virtual void touched(const QPointF& cPos);
	virtual void untouched(const QPointF& cPos);
#else
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);	
	virtual bool eventFilter(QObject* ob, QEvent* event);
	virtual void timerEvent(QTimerEvent* te);
#endif
	
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {
		Q_UNUSED(painter)
		Q_UNUSED(option)
		Q_UNUSED(widget)
	}
	
	
protected slots:
	void sceneRectChangedSlot(); /** Refresh positions when scene is scaled (rect is changed) */
	void sceneScrolled();
	
private:
	QGraphicsProxyWidget				*m_proxy;
	QGraphicsEllipseItem				*m_spot;
	Qt::Corner									 m_corner;
	QWidget											*m_widget;
	QGraphicsView								*m_view;
	qreal												 m_side;
	QColor											 m_colorOfSpot;
	int													 m_signalTimer;
	bool												 m_widgetHasMouse;
	
};

#endif // TCORNERPROXY_H
