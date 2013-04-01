/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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


#include "tvolumeview.h"
#include <QPainter>
#include <QDebug>

#define TICK_WIDTH (2)
#define TICK_GAP (3)


TvolumeView::TvolumeView(QWidget* parent) :
  TabstractSoundView(parent),
  m_volume(0),
  m_pitchColor(Qt::red)
{
  setBackgroundRole(QPalette::Background);
  setAutoFillBackground(true);
  setMinimumSize(200, 17);
  resizeEvent(0);
}

TvolumeView::~TvolumeView() {}


void TvolumeView::setVolume(qreal volume, int alpha) {
  m_volume = volume;
  m_alpha = alpha;
  update();
}

//################################################################################
//############################### protected ######################################
//################################################################################


void TvolumeView::paintEvent(QPaintEvent* ) {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  QColor noteColor;
  if (m_alpha) {
      m_pitchColor.setAlpha(m_alpha);
      painter.setBrush(QBrush(m_pitchColor));
      noteColor = palette().highlightedText().color();
  } else {
      painter.setBrush(QBrush(palette().window().color()));
      noteColor = tc;
  }
  painter.drawRoundedRect(painter.viewport(), 4, 4);
  painter.setPen(noteColor);
  painter.setFont(nootFont);
  painter.drawText(0, 0, width(), height(), Qt::AlignRight, "n");
  for (int i = 1; i < m_ticksCount - 1; i++) {
    if (m_volume * m_ticksCount >= i)
      painter.setPen(QPen(m_tickColors[i], TICK_WIDTH, Qt::SolidLine, Qt::RoundCap));
    else 
      painter.setPen(QPen(tc, TICK_WIDTH, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QLineF((i + 1) * (TICK_GAP + TICK_WIDTH) - TICK_WIDTH, (float)(m_ticksCount - i) * m_hiTickStep + 1,
                            (i + 1) * (TICK_GAP + TICK_WIDTH) - TICK_WIDTH, height() - 2));
  }
}

void TvolumeView::resizeEvent(QResizeEvent* ) {
  resizeIt(height());
//   qDebug() << "TvolumeView" << height() << nootFont;
  m_noteWidth = noteBound.width();
  m_ticksCount = (width() - m_noteWidth) / (TICK_WIDTH + TICK_GAP);
  m_hiTickStep = ((float)height() * 0.66) / m_ticksCount;
  m_tickColors.clear();
  for (int i = 0; i < m_ticksCount; i++) {
    if (i <= m_ticksCount * 0.5)
      m_tickColors << gradColorAtPoint(0, (width() - m_noteWidth) * 0.55, startColor, middleColor, (i + 1) * ((width() - m_noteWidth) / m_ticksCount));
    else if ( i <= m_ticksCount * 0.7)
      m_tickColors << gradColorAtPoint((width() - m_noteWidth) * 0.5, (width() - m_noteWidth) * 0.75,
                                       middleColor, endColor, (i + 1) * ((width() - m_noteWidth) / m_ticksCount));
      else 
          m_tickColors << gradColorAtPoint((width() - m_noteWidth) * 0.7, (width() - m_noteWidth),
                                       endColor, totalColor, (i + 1) * ((width() - m_noteWidth) / m_ticksCount));
  }
}


