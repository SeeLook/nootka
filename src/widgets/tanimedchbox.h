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

#ifndef TANIMEDCHBOX_H
#define TANIMEDCHBOX_H

#include <QCheckBox>

class QTimer;

/** 
 * This is ordinary QCheckBox but with simple, blinking animation encouraging to click it....
 * Animation is ivoked by anim() method with number of blinks as a parameter.
 * Default blink time is 150 ms but can be changed by seccond anim() parameter
 */
class TanimedChBox : public QCheckBox
{
  Q_OBJECT
  
public:
  explicit TanimedChBox(QWidget* parent = 0);
  
  void startAnimation(int blinkNr, int timeGap = 200);
  
protected slots:
  void animSlot();
  
private:
  QTimer      *m_timer;
  int          m_maxBlinks, m_currBlink;

};

#endif // TANIMEDCHBOX_H
