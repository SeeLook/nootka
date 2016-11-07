/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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

#include "tblinkingitem.h"


TblinkingItem::TblinkingItem(QGraphicsItem* item, QObject* parent ):
  TabstractAnim(item, parent)
{
}


void TblinkingItem::startBlinking(int count) {
  initAnim(0, count *2, 150);
}


void TblinkingItem::animationRoutine() {
    nextStep();
    if (currentStep() <= stepsNumber()) {
      if (currentStep() % 2) { // phase 1, 3, ...
          item()->hide();
      } else { // phase 2, 4, ...
          item()->show();
      }
    } else
        stopAnim();
}


