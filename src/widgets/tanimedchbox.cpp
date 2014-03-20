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

#include "tanimedchbox.h"
#include <QTimer>
#include <QDebug>


TanimedChBox::TanimedChBox(QWidget* parent) :
  QCheckBox(parent)
{
  m_timer = new QTimer(this);
  connect(m_timer, SIGNAL(timeout()), this, SLOT(animSlot()));
}


void TanimedChBox::startAnimation(int blinkNr, int timeGap) {
  m_maxBlinks = blinkNr * 2;
  m_currBlink = 0;
  m_timer->start(timeGap);
  animSlot();
}


void TanimedChBox::animSlot() {
  m_currBlink++;
  if (m_currBlink <= m_maxBlinks) {
    if (m_currBlink % 2)
      setStyleSheet(QString("background-color: palette(highlight);"));
    else
      setStyleSheet(QString(""));
  } else 
    m_timer->stop();
}


