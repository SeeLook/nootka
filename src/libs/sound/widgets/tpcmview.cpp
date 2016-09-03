/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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

#include "tpcmview.h"
#include <QtGui/qpainter.h>


TpcmView::TpcmView(QWidget* parent) :
  QWidget(parent)
{
  setMaximumHeight(fontMetrics().height() / 4);
  setContentsMargins(0, 0, 0, 0);

  m_grad.setColorAt(0.0, QColor(192, 255, 255)); // light blue
//   m_grad.setColorAt(0.2, QColor(192, 255, 255));
  m_grad.setColorAt(0.3, QColor(0, 0, 255)); // blue
  m_grad.setColorAt(0.5, QColor(0, 0, 192)); // dark blue
  m_grad.setColorAt(0.8, QColor(192, 0, 192)); // violet
  m_grad.setColorAt(1.0, QColor(128, 0, 128));
}


void TpcmView::setPCMvolume(qreal vol) {
  if (vol != m_pcmVolume) {
    m_pcmVolume = vol;
    repaint();
  }
}


void TpcmView::resizeEvent(QResizeEvent*) {
  m_grad.setStart(0.0, 1.0);
  m_grad.setFinalStop(width(), 1.0);
}



void TpcmView::paintEvent(QPaintEvent*) {
  QPainter painter(this);
  painter.setPen(QPen(m_grad, height(), Qt::SolidLine, Qt::RoundCap));
  painter.drawLine(0, height() / 2, qRound(width() * m_pcmVolume), height() / 2);
}
