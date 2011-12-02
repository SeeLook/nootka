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

TvolumeMeter::TvolumeMeter(QWidget* parent): QWidget(parent)
{
  setBackgroundRole(QPalette::Background);
  setAutoFillBackground(true);
  m_volume = 0;
  setFixedSize(200, 20);
  m_grad = QLinearGradient(0, 5, 180, 5);
  m_grad.setColorAt(0.0, Qt::green);
  m_grad.setColorAt(0.7, Qt::yellow);
  m_grad.setColorAt(1.0, Qt::red);

}

void TvolumeMeter::setVolume(qreal vol) {
  m_volume = vol;
  repaint();
}

void TvolumeMeter::paintEvent(QPaintEvent* )
{
  QPainter painter(this);
  if (m_volume == 0.0)
	  return;
  painter.setPen(Qt::NoPen);
  painter.setBrush(m_grad);
  int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*m_volume;
  painter.drawRect(QRect(painter.viewport().left()+10,
						painter.viewport().top()+5,
						painter.viewport().left()+20+pos,
						painter.viewport().bottom()-10));
}

TvolumeMeter::~TvolumeMeter()
{}
