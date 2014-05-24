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

#include "tscorescene.h"
#include <QGraphicsView>
#include <QGraphicsEffect>
#include <QGraphicsObject>

TscoreScene::TscoreScene(QObject* parent) :
  QGraphicsScene(parent)
{
  setDoubleAccidsEnabled(true);
  setCurrentAccid(0);
}


void TscoreScene::setDoubleAccidsEnabled(bool enable) {
  if (enable)
    m_dblAccFuse = 2;
  else
    m_dblAccFuse = 1;
}


void TscoreScene::addBlur(QGraphicsItem* item, qreal radius) {
  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect();
  blur->setBlurRadius(radius / views()[0]->transform().m11());
  item->setGraphicsEffect(blur);
}
