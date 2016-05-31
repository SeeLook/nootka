/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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

#include "tmenuwidget.h"
#include <QtGui/qpainter.h>
#include <QtCore/qtimer.h>

#define ANIM_DURATION (100)

TmenuWidget::TmenuWidget(QWidget* parent) :
  QWidget(parent),
  m_touched(false),
  m_marked(-1)
{
  setContentsMargins(0, 0, 0, 0);
}


void TmenuWidget::animate() {
  m_marked = 0;
  repaint();
  QTimer::singleShot(ANIM_DURATION, this, SLOT(animSlot()));
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmenuWidget::animSlot() {
  m_marked++;
  if (m_marked < 3)
      QTimer::singleShot(ANIM_DURATION, this, SLOT(animSlot()));
  else
      m_marked = -1;
  repaint();
}


void TmenuWidget::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setPen(Qt::NoPen);
  if (m_touched)
    painter.setBrush(palette().highlight());
  else
    painter.setBrush(palette().text());
  painter.drawRect(contentsRect());
  painter.setBrush(palette().base());
  int s = height() / 4;
  for (int i = 0; i < 3; ++i) {
    if (m_marked == i)
      painter.setBrush(Qt::red);
    else
      painter.setBrush(palette().base());
    painter.drawEllipse(((width() - 3 * s) / 4) * (i + 1) + i * s , (height() - s) / 2, s, s);
  }
}


void TmenuWidget::mousePressEvent(QMouseEvent*) {
  m_touched = true;
  update();
}


void TmenuWidget::mouseReleaseEvent(QMouseEvent*) {
  m_touched = false;
  update();
  QTimer::singleShot(5, [=]{ emit clicked(); });
}

