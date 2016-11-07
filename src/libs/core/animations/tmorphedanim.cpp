/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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

#include "tmorphedanim.h"

TmorphedAnim::TmorphedAnim(QGraphicsEllipseItem* ellipse, QObject* parent) :
  TabstractAnim(ellipse, parent),
  m_ellipse(ellipse)
{
}


void TmorphedAnim::startMorphing(const QLineF& line, qreal width, bool toLine) {
  m_line = line;
  m_toLine = toLine;
  m_width = width;
  m_startPos = m_ellipse->pos();
  initAnim();
}


void TmorphedAnim::animationRoutine() {
  nextStep();
  if (currentStep() <= stepsNumber()) {
    qreal ww, hh, easy;
    easy = easyValue((qreal)currentStep() / (qreal)stepsNumber());
    int mm = currentStep();
//     if (!m_toLine)
//       mm = qAbs(currentStep() - stepsNumber());
    ww = m_ellipse->rect().width() + ((m_line.length() - m_ellipse->rect().width()) / (qreal)stepsNumber()) * mm * easy;
    hh = m_ellipse->rect().height() + ((m_width - m_ellipse->rect().height()) / (qreal)stepsNumber()) * mm * easy;
    m_ellipse->setRect(0, 0, ww, hh);
  } else
      stopAnim();
}






