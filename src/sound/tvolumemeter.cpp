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
  m_pitchColor(Qt::red)
{
  setBackgroundRole(QPalette::Background);
  setAutoFillBackground(true);
  setStyleSheet("border-radius: 6px;");
  setFixedHeight(20);
  m_grad = QLinearGradient(0, 5, 180, 5);
  resize();

}

int rad = 4; // background rectiangle radius

void TvolumeMeter::resize() {
  m_grad.setFinalStop(width()-20, 5);
  m_grad.setColorAt(0.0, Qt::green);
  m_grad.setColorAt(0.7, Qt::yellow);
  m_grad.setColorAt(1.0, Qt::red);
  rad = height()/5;
}


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
  if (m_alpha) { // m_alpha==0 means transparent - no sense to painnnnt background
      m_pitchColor.setAlpha(m_alpha);
      painter.setBrush(QBrush(m_pitchColor));
      painter.drawRoundedRect(painter.viewport(), rad, rad);
  }
  painter.setBrush(m_grad);
  int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*m_volume;
  painter.drawRoundedRect(QRect(painter.viewport().left()+10,
          painter.viewport().top()+5,
          painter.viewport().left()+20+pos,
          painter.viewport().bottom()-10),
          rad, rad );
}

TvolumeMeter::~TvolumeMeter()
{}
