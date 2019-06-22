/***************************************************************************
 *   Copyright (C) 2012-2019 by Tomasz Bojczuk                             *
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
//   setRenderTarget(QQuickPaintedItem::Image);
//   setRenderTarget(QQuickPaintedItem::FramebufferObject);
//   setPerformanceHint(QQuickPaintedItem::FastFBOResizing);

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


Tchart::~Tchart() {
  if (m_sceneImage)
    delete m_sceneImage;
}


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


void Tchart::update() {
  if (m_renderState != e_renderInProgress) {
    m_renderState = e_renderInProgress;
    CHECKTIME (
      if (m_sceneImage)
        delete m_sceneImage;
      m_sceneImage = new QImage(scene->sceneRect().size().toSize(), QImage::Format_ARGB32);
      m_sceneImage->fill(Qt::transparent);
      QPainter painter;
      painter.begin(m_sceneImage);
      painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
      scene->render(&painter);
      painter.end();
      m_renderState = e_renderFinished;
    )
    QQuickPaintedItem::update();
  }
}


void Tchart::paint(QPainter* painter) {
CHECKTIME(
  if (!painter->paintEngine() || boundingRect().width() < 1.0)
    return;

  if (m_renderState == e_renderFinished)
    painter->drawImage(0, 0, *m_sceneImage);
  else
    qDebug() << "[Tchart] chart image not ready yet!";
)
}


void Tchart::setCurQ(TtipInfo* qa) {
  if (qa != m_curQ) {
    m_curQ = qa;
    emit hoveredChanged();
  }
}
