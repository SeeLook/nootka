/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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

#include "tabstractanim.h"
#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>


TabstractAnim::TabstractAnim(QGraphicsItem* it, QObject* parent) :
  QObject(parent),
  m_timer(0),
  m_item(it),
  m_duration(150)
{
  m_easingCurve = new QEasingCurve();
}


TabstractAnim::~TabstractAnim()
{
  if (timer())
    timer()->stop();
  delete m_easingCurve;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TabstractAnim::installTimer() {
  if (!m_timer) {
      m_timer = new QTimer(this);
      connect(m_timer, SIGNAL(timeout()), this, SLOT(animationRoutine()));
  }
}


void TabstractAnim::initAnim(int currStep, int stepNr, int timerStep, bool install) {
  if (install)
      installTimer();
  if (stepNr < 0)
      m_stepCount = duration() / CLIP_TIME;
  else
      m_stepCount = stepNr;
  m_currentStep = currStep;
  timer()->start(timerStep);
  animationRoutine();
}


void TabstractAnim::stopAnim() {
  timer()->stop();
  emit finished();
}


void TabstractAnim::setTimer(QTimer* tim) {
  if (m_timer)
    delete m_timer;
  m_timer = tim;
  if (m_timer)
    connect(m_timer, SIGNAL(timeout()), this, SLOT(animationRoutine()));
}






