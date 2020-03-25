/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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

#include "tcommoninstrument.h"


TcommonInstrument::TcommonInstrument(QQuickItem* parent) :
  QQuickPaintedItem(parent)
{
  setAcceptHoverEvents(true);
  setRenderTarget(QQuickPaintedItem::FramebufferObject);
  //   setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
  setAntialiasing(true);
  setAcceptedMouseButtons(Qt::LeftButton);
}


TcommonInstrument::~TcommonInstrument() {
  restoreAfterExam();
}


void TcommonInstrument::markBorder(QQuickItem* item, int borderWidth, const QColor& borderColor) {
  auto border = qvariant_cast<QObject*>(item->property("border"));
  if (border) {
    border->setProperty("width", borderWidth);
    if (borderWidth)
      border->setProperty("color", borderColor);
  }
}


void TcommonInstrument::restoreAfterExam() {
  p_goodItem = nullptr;
  p_wrongItem = nullptr;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TcommonInstrument::hoverEnterEvent(QHoverEvent*) {
  if (!m_active) {
    m_active = true;
    setOutOfScale(false);
    emit activeChanged();
  }
}


void TcommonInstrument::hoverLeaveEvent(QHoverEvent*) {
  if (m_active) {
    m_active = false;
    emit activeChanged();
  }
}


void TcommonInstrument::setOutOfScale(bool out) {
  if (out != m_outOfScale) {
    m_outOfScale = out;
    emit outOfScaleChanged();
  }
}

