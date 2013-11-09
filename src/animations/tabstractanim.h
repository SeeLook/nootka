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

#ifndef TABSTRACTANIM_H
#define TABSTRACTANIM_H

#include <QObject>
#include <QEasingCurve>
#include <QTimer>
#include <QGraphicsItem>

#define CLIP_TIME (30) // every move per 30 ms


/** 
 * This is base-template class to implement different types of animations on QGraphicsItem
 * It has duration, and item - object on which animation is performing.
 * QTimer has to be initialized by installTimer() or set by setTimer(QTimer*)
 * than animationRoutine() is called every 30ms which has to re-implemented in subclass.
 * There is finished() signal defined but is is subclass responsibility 
 * to call it after animation finish.
 */
class TabstractAnim : public QObject
{
    Q_OBJECT
    
public:
	explicit TabstractAnim(QGraphicsItem *item = 0, QObject* parent = 0);
	virtual ~TabstractAnim();
	
	
	void setEasingCurveType(QEasingCurve::Type ecType) { m_easingCurve->setType(ecType); }
	QEasingCurve* easingCurve() { return m_easingCurve; }
			/** Shortcut to get value calculated by easingCurve. */
	inline qreal easyValue(qreal progress) { return m_easingCurve->valueForProgress(progress); }
	
	int duration() { return m_duration; }
	void setDuration(int dur) { m_duration = dur; }
	
			/** Sets another timer as main timer and removes inner one. */
	void setTimer(QTimer *tim);
	
signals:
	void finished();

protected:
	
			/** Creates QTimer instance. Without that timer() is 0 */
	void installTimer();
	QTimer* timer() { return m_timer; }
	
	QGraphicsItem* item() { return m_item; }
		
protected slots:
			/** This is virtual method (slot) which is invoked by timeOut() signal of timer.
			* By default every CLIP_TIME (30ms).
			* It has to be implemented in subclass.  */
	virtual void animationRoutine() {};

private:
    QEasingCurve 				*m_easingCurve;
    QTimer 							*m_timer;
		QGraphicsItem				*m_item;
		int 								 m_duration;
};

#endif // TABSTRACTANIM_H
