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


#include "tscoreview.h"
#include "tscoreitem.h"
#include <QTouchEvent>

#include <QDebug>
#include <QGraphicsObject>


TscoreView::TscoreView(QWidget* parent) :
	QGraphicsView(parent),
	m_currentIt(0)
{
	viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
}


bool TscoreView::viewportEvent(QEvent* event) {
	if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd) {
		QTouchEvent *te = static_cast<QTouchEvent*>(event);
		
		if (te->touchPoints().count() == 1) {
			switch(te->touchPoints().first().state()) {
				case Qt::TouchPointPressed: {
					event->accept();
					QPointF touchScenePos = mapToScene(te->touchPoints().first().pos().toPoint());
					TscoreItem *it = castItem(scene()->itemAt(touchScenePos, transform()));
					checkItem(it, touchScenePos);
					m_tapTime.start();
					break;
				}
				case Qt::TouchPointMoved: {
					QPointF touchScenePos = mapToScene(te->touchPoints().first().pos().toPoint());
					TscoreItem *it = castItem(scene()->itemAt(touchScenePos, transform()));
					checkItem(it, touchScenePos);
					if (it) {						
						QPointF touchPos = it->mapFromScene(touchScenePos);
						touchPos.setY(touchPos.y() - 6.0);
						it->cursorMoved(touchPos);
					}
					break;
				}
				case Qt::TouchPointReleased:
					if (m_currentIt) {
						m_currentIt->cursorLeaved();
						if (m_tapTime.elapsed() < 150)
							m_currentIt->cursorTapped(m_currentIt->mapFromScene(mapToScene(te->touchPoints().first().pos().toPoint())));
						m_currentIt = 0;
					}
					break;
				default:
					break;
			}
		} else if (te->touchPoints().count() == 2) {
			switch(te->touchPoints()[1].state()) {
				case Qt::TouchPointPressed: {
					if (m_currentIt) {
					    QPointF touchScenePos = mapToScene(te->touchPoints().first().pos().toPoint());
					    m_currentIt->cursorClicked(m_currentIt->mapFromScene(touchScenePos));
					  }
					break;
				}
				case Qt::TouchPointMoved:
					break;
				case Qt::TouchPointReleased:
					break;
				default:
					break;
			}
		}
		
		return true;
	}
	return QGraphicsView::viewportEvent(event);
}


TscoreItem* TscoreView::castItem(QGraphicsItem* it) {
	if (it) {
		int cnt = 0;
		while (cnt < 2) {
			if (it->type() == TscoreItem::ScoreItemType)
				return static_cast<TscoreItem*>(it);
			if (it->parentItem()) {
				it = it->parentItem();
				cnt++;
			} else
				break;
		}
	}
	return 0;
}


void TscoreView::checkItem(TscoreItem* it, const QPointF& touchScenePos) {
	if (it != m_currentIt) {
		if (m_currentIt)
			m_currentIt->cursorLeaved();
		m_currentIt = it;
		if (m_currentIt)
			m_currentIt->cursorEntered(it->mapFromScene(touchScenePos));
	}
}








