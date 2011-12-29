/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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

#include "tvolumemeter.h"
#include <QPainter>

TvolumeMeter::TvolumeMeter(QWidget* parent):
  QWidget(parent),
  m_volume(0),
  m_pitchColor(Qt::red),
  m_noiseLevel(0.02)
{
  setBackgroundRole(QPalette::Background);
  setAutoFillBackground(true);
  setMinimumSize(200, 20);
}

TvolumeMeter::~TvolumeMeter()
{}


void TvolumeMeter::setVolume(qreal vol, int alpha) {
  m_volume = vol;
  m_alpha = alpha;
  repaint();
}


void TvolumeMeter::paintEvent(QPaintEvent* )
{
  if (m_volume == 0.0)
    return;
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing);
  painter.setPen(Qt::NoPen);
  if (m_alpha) { // m_alpha==0 means transparent - no sense to paint background
      m_pitchColor.setAlpha(m_alpha);
      painter.setBrush(QBrush(m_pitchColor));
//       QColor cc = palette().highlightedText().color();
//       cc.setAlpha(m_alpha);
//       painter.setBrush(QBrush(cc));
      painter.drawRoundedRect(painter.viewport(), 4, 4);
      painter.setPen(palette().highlightedText().color());
      painter.setFont(QFont("nootka", painter.viewport().height()-6));
      painter.drawText(painter.viewport(), Qt::AlignRight, "n");
//       painter.drawText(painter.viewport(), Qt::AlignCenter, "nnnnnnnnnn");
  }
  QLinearGradient grad = QLinearGradient(0, 5, painter.viewport().right()-20, 5);
  grad.setColorAt(0.0, Qt::darkGreen);
  grad.setColorAt(m_noiseLevel, Qt::green);
  grad.setColorAt(0.7, Qt::yellow);
  grad.setColorAt(1.0, Qt::red);
  painter.setBrush(grad);
  int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*m_volume;
  painter.drawRoundedRect(QRect(painter.viewport().left()+10,
          painter.viewport().top()+5,
          painter.viewport().left()+20+pos,
          painter.viewport().bottom()-10),
          2, 2 );
}

