/***************************************************************************
 *   Copyright (C) 2017-2021 by Tomasz Bojczuk                             *
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
  setAntialiasing(true);
  setAcceptedMouseButtons(Qt::LeftButton);

  // Reset instrument zoom when goes disabled by setting scale to 1 - only mobile so far
  connect(this, &QQuickItem::enabledChanged, this, [=]{
    if (!isEnabled())
      setScale(1.0);
  });
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


void TcommonInstrument::showNoteName(Tnote::EnameStyle st, const Tnote &n, quint32 techn, const QColor &textColor) {
  Q_UNUSED(textColor)
  Q_UNUSED(techn)
  if (n.isValid()) {
      auto defStyle = Tnote::defaultStyle;
      Tnote::defaultStyle = st;
      p_extraName = QString("<font color=\"%1\">%2</font>").arg(textColor.name()).arg(n.styledName());
      Tnote::defaultStyle = defStyle;
  } else
      p_extraName.clear();
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

