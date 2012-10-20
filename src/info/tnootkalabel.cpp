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

#include "tnootkalabel.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsColorizeEffect>

TnootkaLabel::TnootkaLabel(QString pixmapPath, QWidget* parent) :
  QGraphicsView(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(("background: transparent"));
    setRenderHint(QPainter::TextAntialiasing, true);

    m_scene = new QGraphicsScene();
    setScene(m_scene);
    
    QGraphicsPixmapItem *pixItem = new QGraphicsPixmapItem(QPixmap(pixmapPath));
    m_scene->addItem(pixItem);
    resize(pixItem->pixmap().size());
    QGraphicsColorizeEffect *effect = new QGraphicsColorizeEffect();
    QColor bg = palette().window().color();
//     bg.setAlpha(220);
    effect->setColor(bg);
    pixItem->setGraphicsEffect(effect);

}

TnootkaLabel::~TnootkaLabel()
{

}

