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

#ifndef TANIMEDITEM_H
#define TANIMEDITEM_H

#include <QObject>
#include <QGraphicsItem>
#include <QEasingCurve>

#define CLIP_TIME (30) // every move per 30 ms

class QTimer;


/** This class implements moving animation of QGraphicsItem. 
 * Default duration is 150 ms and item is moving every 30 ms. */
class TanimedItem : public QObject
{
    Q_OBJECT

public:
	explicit TanimedItem(QGraphicsItem* item, QObject* parent = 0);
	
	void setDuration(int duration) { m_duration = duration; m_step = m_duration / CLIP_TIME; }
	
	void startMoving(const QPointF& start, const QPointF& stop);
	bool isMoving() { return !(bool)m_currStep; }
	
	void setEasingCurveType(QEasingCurve::Type type) { m_easingCurve.setType(type); }
	
signals:
	void finished();
	
protected slots:
	void movingSlot();
	
private:
	QTimer										*m_timer;
	QPointF										 m_startPos, m_endPos;
	QGraphicsItem							*m_item;
	int												 m_step, m_currStep, m_duration;
	QGraphicsLineItem 				*m_line;
	QEasingCurve							 m_easingCurve;
  

};

#endif // TANIMEDITEM_H
