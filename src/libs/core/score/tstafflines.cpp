/***************************************************************************
 *   Copyright (C) 2017-2019 by Tomasz Bojczuk                             *
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

#include "tstafflines.h"

#include <QtGui/qpalette.h>
#include <QtGui/qpainter.h>
#include <QtGui/qguiapplication.h>


#define LINE_WIDTH (0.2)


/**
 * There is fixed height set to 9.0 whether actual staff height is about 8.2
 * To able to paint line with its complete width we need to set that staff height bigger
 */
TstaffLines::TstaffLines(QQuickItem* parent) :
  QQuickPaintedItem(parent)
{
  setAcceptHoverEvents(true);
  setRenderTarget(QQuickPaintedItem::FramebufferObject);
  setAntialiasing(true);
  setHeight(9.0);
  connect(qApp, &QGuiApplication::paletteChanged, this, [=]{ update(); });
}


void TstaffLines::setStaffScale(qreal stScale) {
  if (stScale != m_staffScale) {
    m_staffScale = stScale;
    setTextureSize(QSize(qRound(m_staffScale * width()), qRound(height() * m_staffScale)));
    update();
  }
}


void TstaffLines::paint(QPainter* painter) {
  painter->setPen(QPen(qApp->palette().color(isEnabled() ? QPalette::Active : QPalette::Disabled, QPalette::Text), LINE_WIDTH));
  for (int l = 0; l < 5; ++l) {
    qreal yy = l * 2.0 + LINE_WIDTH / 2.0;
    painter->drawLine(QPointF(0.0, yy), QPointF(width(), yy));
  }
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TstaffLines::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (newGeometry.width() != oldGeometry.width() || newGeometry.height() != oldGeometry.height()) {
    setTextureSize(QSize(qRound(m_staffScale * newGeometry.width()), qRound(newGeometry.height() * m_staffScale)));
    update();
  }
}


