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


#ifndef TSCOREVIEW_H
#define TSCOREVIEW_H

#include <QGraphicsView>
#include <QTime>


class QGraphicsItem;
class TscoreItem;

/**
 * This class purpose is to manage touch events.
 * Detected touch events are send to TscoreItem objects to further processing
 */
class TscoreView : public QGraphicsView
{

public:

	TscoreView(QWidget* parent);
	
	
protected:
	virtual bool viewportEvent(QEvent* event);
	virtual void timerEvent(QTimerEvent* timeEvent);
	
			/** Checks is item @it of type @p TscoreItem::ScoreItemType.
			 * If not, checks it parent item and parent of parent.
			 * Returns pointer to @p TscoreItem or 0 if not found. */
	TscoreItem* castItem(QGraphicsItem* it);
	
			/** Checks is given TscoreItem different than current one and sets it to current */
	void checkItem(TscoreItem* it, const QPointF& touchScenePos);
	
private:
	TscoreItem 									*m_currentIt;
	QTime												 m_tapTime;
	int 												 m_timerIdMain;
	bool 												 m_isLongTap;
	QPointF											 m_initPos;
	
};

#endif // TSCOREVIEW_H
