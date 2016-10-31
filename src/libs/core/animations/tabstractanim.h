/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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

#include <nootkacoreglobal.h>
#include <QObject>
#include <QEasingCurve>
#include <QTimer>
#include <QGraphicsItem>

#define CLIP_TIME (50) // every move per 50 ms


/** 
 * This is base-template class to implement different types of animations on QGraphicsItem
 * It has duration, and item - object on which animation is performing.
 * QTimer has to be initialized by installTimer() or set by setTimer(QTimer*)
 * than animationRoutine() is called every 30ms which has to re-implemented in subclass.
 * There is finished() signal defined but is is subclass responsibility 
 * to call it after animation finish.
 */
class NOOTKACORE_EXPORT TabstractAnim : public QObject
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
	
			/** Preforms common routines for starting animation:
			 * - initializes the timer
			 * - resets the step counter
			 * - starts timer with CLIP_TIME step */
	void initAnim(int currStep = -1, int stepNr = -1, int timerStep = (30), bool install = true);
			/** Stops the timer and emits @p finished() signal */
	void stopAnim();
	
			/** Number of current animation step. */
	int currentStep() { return m_currentStep; }
	
			/** Sets current step to -1 or to given @p val */
	void resetStepCounter(int val = -1) { m_currentStep = val; }
	
			/** Increases current step counter +1 or given @p val */
	void nextStep(int val = 1) { m_currentStep += val; }
	
			/** Sets steps number to default value which is duration divided by CLIP_TIME */
	void setStepNumber() { m_stepCount = duration() / CLIP_TIME; }
	
			/** Overloaded value forcing steps number to @p val */
	void setStepNumber(int val) { m_stepCount = val; }
		
			/** Maximal number of animation steps. Usually a duration divided by CLIP_TIME */
	int stepsNumber() { return m_stepCount; }
		
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
		int  								 m_stepCount, m_currentStep;
};

#endif // TABSTRACTANIM_H
