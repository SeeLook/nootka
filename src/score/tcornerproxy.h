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

class QGraphicsView;
class QWidget;
class TscoreScene;


/**
 * This class represents spot in a corner of a score 
 * which calls some widget when mouse cursor is over it
 */
class TcornerProxy : public TscoreItem
{

	Q_OBJECT
	
public:
	TcornerProxy(TscoreScene* scene, QWidget* widget, Qt::Corner cornerPos);
	
	QGraphicsProxyWidget* proxy() { return m_proxy; }
	qreal side() { return m_side; } /** It is square shaped so this is either width and height */
	
	virtual QRectF boundingRect() const;
	virtual QPainterPath shape() const;
	
public slots:
	void hideWithDelay();
	
	
protected:
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent* event);
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent* event);
	
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) {}
	virtual bool eventFilter(QObject* ob, QEvent* event);
	
protected slots:
	void sceneRectChangedSlot(); /** Refresh positions when scene is scaled (rect is changed) */
	void sceneScrolled();
	
private:
	QGraphicsProxyWidget				*m_proxy;
	QGraphicsEllipseItem				*m_spot;
	Qt::Corner									 m_corner;
	QGraphicsView								*m_view;
	qreal												 m_side;
	
};

#endif // TCORNERPROXY_H
