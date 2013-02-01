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


#include "ttipscene.h"
#include <QDebug>
#include <QEvent>
#include <QApplication>
#include <QGraphicsSceneHoverEvent>


TtipScene::TtipScene(QObject* parent) :
  QGraphicsScene(parent)
{
}

TtipScene::~TtipScene() {}

// bool TtipScene::event(QEvent* event) {
//     event->setAccepted(false);
//     qDebug() << "TtipScene" << event->type() << event->isAccepted();
//     event->ignore();
//     if (event->type() == 156)
//         qApp->sendEvent(parent(), event);
//     if (event->type() == QEvent::MouseButtonPress) {
//       qDebug("mouse pressed");
//     }
//     return QGraphicsScene::event(event);
// }

void TtipScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    event->ignore();
//     QGraphicsScene::mouseMoveEvent(event);
//     event->ignore();
}

void TtipScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    event->ignore();
    QGraphicsScene::mousePressEvent(event);
    event->ignore();
}

void TtipScene::wheelEvent(QGraphicsSceneWheelEvent* event) {
    event->ignore();
//     QGraphicsScene::wheelEvent(event);
//     event->ignore();
}

void TtipScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    event->ignore();
    QGraphicsScene::mouseReleaseEvent(event);
    event->ignore();
}


