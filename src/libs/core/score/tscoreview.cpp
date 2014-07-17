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
#include "tscorescene.h"
#include <QTouchEvent>

#include <QDebug>
#include <QGraphicsObject>

#define TAP_TIME (200) //  ms
#define LONG_TAP_TIME (500) // ms


TscoreView::TscoreView(QWidget* parent) :
	QGraphicsView(parent),
	m_currentIt(0),
	m_scoreScene(0)
{
	viewport()->setAttribute(Qt::WA_AcceptTouchEvents);
}


void TscoreView::setScoreScene(TscoreScene* sc) {
	m_scoreScene = sc;
}


void TscoreView::wheelEvent(QWheelEvent* event) {
	if (event->modifiers() == Qt::ControlModifier) {
		if (m_scoreScene && m_scoreScene->isCursorVisible()) {
			if (event->delta() < 0)
				m_scoreScene->setCurrentAccid(m_scoreScene->currentAccid() - 1);
			else
				m_scoreScene->setCurrentAccid(m_scoreScene->currentAccid() + 1);
		}
	} else
		QAbstractScrollArea::wheelEvent(event);
}


bool TscoreView::viewportEvent(QEvent* event) {
	if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchUpdate || event->type() == QEvent::TouchEnd) {
		QTouchEvent *te = static_cast<QTouchEvent*>(event);
		
		if (te->touchPoints().count() == 1) {
			switch(te->touchPoints().first().state()) {
				case Qt::TouchPointPressed: {
					event->accept();
					QPointF touchScenePos = mapToScene(te->touchPoints().first().pos().toPoint());
					m_initPos = touchScenePos;
					TscoreItem *it = castItem(scene()->itemAt(touchScenePos, transform()));
					checkItem(it, touchScenePos);
					m_tapTime.start();
					m_timerIdMain = startTimer(LONG_TAP_TIME);
					break;
				}
				case Qt::TouchPointMoved: {
// 					killTimer(m_timerIdMain);
					QPointF touchScenePos = mapToScene(te->touchPoints().first().pos().toPoint());
					TscoreItem *it = castItem(scene()->itemAt(touchScenePos, transform()));
					checkItem(it, touchScenePos);
					if (it) {						
						QPointF touchPos = it->mapFromScene(touchScenePos);
						it->touchMove(touchPos);
					}
					break;
				}
				case Qt::TouchPointStationary:
// 					killTimer(m_timerIdMain);
// 					m_timerIdMain = startTimer(LONG_TAP_TIME);
					break;
				case Qt::TouchPointReleased:
					killTimer(m_timerIdMain);
					if (m_currentIt) {
						QPointF touchScenePosMap = m_currentIt->mapFromScene(mapToScene(te->touchPoints().first().pos().toPoint()));
						m_currentIt->untouched(touchScenePosMap);
						if (m_tapTime.elapsed() < TAP_TIME) {
							m_currentIt->shortTap(touchScenePosMap);
						}
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
					    QPointF touch1ScenePos = mapToScene(te->touchPoints().first().pos().toPoint());
							QPointF touch2ScenePos = mapToScene(te->touchPoints()[1].pos().toPoint());
					    m_currentIt->secondTouch(m_currentIt->mapFromScene(touch1ScenePos), m_currentIt->mapFromScene(touch2ScenePos));
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


void TscoreView::timerEvent(QTimerEvent* timeEvent) {
	if (timeEvent->timerId() == m_timerIdMain) {
		killTimer(m_timerIdMain);
		if (m_currentIt) {
				m_currentIt->longTap(m_currentIt->mapFromScene(m_initPos));
		}
	}
}


TscoreItem* TscoreView::castItem(QGraphicsItem* it) {
	if (it) {
		int cnt = 0;
		while (cnt < 3) {
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
			m_currentIt->untouched(m_currentIt->mapFromScene(touchScenePos));
		m_currentIt = it;
		if (m_currentIt)
			m_currentIt->touched(m_currentIt->mapFromScene(touchScenePos));
	}
}









