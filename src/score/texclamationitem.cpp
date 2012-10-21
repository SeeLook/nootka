/***************************************************************************
 *   Copyright (C) 2012 by Tomasz Bojczuk                                  *
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


#include "texclamationitem.h"
#include <QGraphicsScene>


TexclamationItem::TexclamationItem(QGraphicsScene* scene, QColor color) :
  QGraphicsSimpleTextItem()
{
    setAcceptHoverEvents(true);
    scene->addItem(this);
    color.setAlpha(220);
    setBrush(color);
// #if defined (Q_OS_MACX)
//     setFont("nootka", 20);
// #else
    setFont(QFont("nootka", scene->height() / 5));
// #endif
    setText("!");
    setPos((scene->width() - boundingRect().width()) / 2, (scene->height() - boundingRect().height()) / 2);
    setZValue(150);
}

TexclamationItem::~TexclamationItem() {}

void TexclamationItem::hoverEnterEvent(QGraphicsSceneHoverEvent*) {
    hide();
}
