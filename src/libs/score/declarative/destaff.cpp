/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#include "destaff.h"
#include <tscorescene.h>
#include <tscorestaff.h>
#include <QtGui/qpainter.h>

#include "checktime.h"


DeStaff::DeStaff(DeScore* score, int staffNr, QQuickItem* parent) :
  QQuickPaintedItem(parent),
  m_score(score)
{
  setRenderTarget(QQuickPaintedItem::FramebufferObject);
//   setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
  setAntialiasing(true);

  m_scene = new TscoreScene;
  m_staff = new TscoreStaff(m_scene, 0);
  m_staff->setStafNumber(staffNr);
  m_staff->setDisabled(true);

  setAcceptHoverEvents(true);
}


DeStaff::~DeStaff()
{
  delete m_scene;
}


void DeStaff::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (boundingRect().width() > 0 && newGeometry != oldGeometry) {
    qDebug() << "[DeStaff" << m_staff->number() << "] resize" << oldGeometry << newGeometry << boundingRect();
    m_scene->setSceneRect(0.0, 0.0, boundingRect().width(), boundingRect().height());
    m_staff->setScale(newGeometry.height() / m_staff->height());
    m_staff->setViewWidth((newGeometry.width() - 25) / m_staff->scale());
    m_staff->setPos((m_staff->isPianoStaff() ? 2.1 : 1.0) * m_staff->scale(), 0.0);
    update();
  }
  QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}


void DeStaff::paint(QPainter* painter) {
CHECKTIME(
  if (!painter->paintEngine())
    return;

  m_scene->render(painter, boundingRect(), boundingRect());
)
}

