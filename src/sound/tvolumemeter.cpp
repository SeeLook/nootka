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
  setMinimumHeight(30);
  setMinimumWidth(200);

}

void TvolumeMeter::setVolume(qreal vol) {
  m_volume = vol;
  repaint();
}



void TvolumeMeter::paintEvent(QPaintEvent* )
{
  QPainter painter(this);

  painter.setPen(palette().text().color());
  painter.drawRect(QRect(painter.viewport().left()+10,
						  painter.viewport().top()+10,
						  painter.viewport().right()-20,
						  painter.viewport().bottom()-20));
  if (m_volume == 0.0)
	  return;

  painter.setPen(Qt::red);
  int pos = ((painter.viewport().right()-20)-(painter.viewport().left()+11))*m_volume;
  for (int i = 0; i < 10; ++i) {
	  int x1 = painter.viewport().left()+11;
	  int y1 = painter.viewport().top()+10+i;
	  int x2 = painter.viewport().left()+20+pos;
	  int y2 = painter.viewport().top()+10+i;
	  if (x2 < painter.viewport().left()+10)
		  x2 = painter.viewport().left()+10;

	  painter.drawLine(QPoint(x1, y1),QPoint(x2, y2));
  }

}

TvolumeMeter::~TvolumeMeter()
{

}
