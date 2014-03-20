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

#include "tfadeinanim.h"


TfadeInAnim::TfadeInAnim(QGraphicsItem* item, QObject* parent) :
  TabstractAnim(item, parent)
{
  
}


void TfadeInAnim::startFadeIn() {
  item()->setOpacity(0.0);
  item()->show();
  initAnim();
}



void TfadeInAnim::animationRoutine() {
  nextStep();
  if (currentStep() <= stepsNumber()) {
      item()->setOpacity(0.0 + easyValue((qreal)currentStep() / (qreal)stepsNumber()));
  } else
      stopAnim();
}
