/***************************************************************************
 *   Copyright (C) 2012-2018 by Tomasz Bojczuk                             *
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


#include "tchart.h"
#include "txaxis.h"
#include "tyaxis.h"
#include "tabstractaxis.h"
#include "tgroupedqaunit.h"

#include <QtCore/qdebug.h>
#include "checktime.h"


Tchart::Tchart(QQuickItem* parent) :
  QQuickPaintedItem(parent)
{
//   setRenderTarget(QQuickPaintedItem::FramebufferObject);
  setAntialiasing(true);
  scene = new QGraphicsScene(this);

  p_bgRect = scene->addRect(0.0, 0.0, 100.0, 350.0, Qt::NoPen, Qt::NoBrush);

  yAxis = new TYaxis();
  yAxis->setParentItem(p_bgRect);
  yAxis->setLength(300);
  yAxis->setMaxValue(9);
  yAxis->setPos(45, 0);
//   yAxis->setZValue(55);

  xAxis = new TXaxis();
  xAxis->setParentItem(p_bgRect);
  xAxis->setLength(550);
  xAxis->setPos(52, yAxis->boundingRect().height() - 7);
}


Tchart::~Tchart() {}


void Tchart::setParentForItem(QGraphicsItem* it) {
  it->setParentItem(p_bgRect);
}


void Tchart::setParentHeight(qreal pH) {
  if (pH != m_parentHeight) {
    p_bgRect->setRect(scene->sceneRect().x() / p_bgRect->scale(), scene->sceneRect().y() / p_bgRect->scale(),
                      scene->sceneRect().width() / p_bgRect->scale(), scene->sceneRect().height() / p_bgRect->scale());
    p_bgRect->setScale(pH / (p_bgRect->boundingRect().height()));
    scene->setSceneRect(scene->sceneRect().x(), scene->sceneRect().y(),
                        p_bgRect->boundingRect().width() * p_bgRect->scale(), p_bgRect->boundingRect().height() * p_bgRect->scale());
    setSize(QSizeF(scene->sceneRect().width() - scene->sceneRect().x(), scene->sceneRect().height() - scene->sceneRect().y()));
    m_parentHeight = pH;
    update();
  }
}


void Tchart::paint(QPainter* painter) {
CHECKTIME(
  if (!painter->paintEngine() || boundingRect().width() < 1.0)
    return;

  scene->render(painter);
)
}


void Tchart::setCurQ(TtipInfo* qa) {
  if (qa != m_curQ) {
    m_curQ = qa;
    emit hoveredChanged();
  }
}
