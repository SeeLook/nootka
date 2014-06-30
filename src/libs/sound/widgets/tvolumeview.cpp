/***************************************************************************
 *   Copyright (C) 2013-2014 by Tomasz Bojczuk                             *
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
#include <QMouseEvent>
#include <qtooltip.h>
// #include <QDebug>

#define TICK_WIDTH (2)
#define TICK_GAP (3)
#if defined (Q_OS_WIN)
  #define Y_OFF (-6)
#else
  #define Y_OFF (-1)
#endif


TvolumeView::TvolumeView(QWidget* parent) :
  TabstractSoundView(parent),
  m_volume(0.0f), m_prevVol(0.0f),
  m_pitchColor(Qt::red),
  m_alpha(0),
  m_drawKnob(false)
{
  setMinimumSize(200, 17);
  resizeEvent(0);
}

TvolumeView::~TvolumeView() {}


void TvolumeView::setVolume(float vol, int alpha) {
  m_volume = vol;
  if (m_volume == 0.0) // skip first 0.0 volume to avoid flickering
    m_volume = m_prevVol;
  m_alpha = alpha;
// 	if (m_prevVol != m_volume || m_alpha != alpha || m_drawKnob)
  repaint();
  m_prevVol = vol;
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
			QColor bg = palette().window().color();
			bg.setAlpha(200);
      painter.setBrush(bg);
      noteColor = tc;
  }
  painter.drawRoundedRect(painter.viewport(), 4, 4);
  painter.setPen(noteColor);
  painter.setFont(nootFont);
  painter.drawText(0, Y_OFF, width(), height(), Qt::AlignRight, "n");
  qreal tickWidth = TICK_WIDTH - 1.0;
  for (int i = 1; i < m_ticksCount - 2; i++) {
    if (i >= m_ticksCount * m_minVolume)
      tickWidth = TICK_WIDTH;
    if (m_volume * m_ticksCount >= i)
      painter.setPen(QPen(m_tickColors[i], tickWidth, Qt::SolidLine, Qt::RoundCap));
    else 
      painter.setPen(QPen(tc, tickWidth, Qt::SolidLine, Qt::RoundCap));
		float ticH = (((float)i * m_hiTickStep + 1.0));
    painter.drawLine(QLineF((i + 1) * (TICK_GAP + TICK_WIDTH) - TICK_WIDTH, (height() - ticH) / 2,
                            (i + 1) * (TICK_GAP + TICK_WIDTH) - TICK_WIDTH, height() - (height() - ticH) / 2));
  }
  if (m_drawKnob) {
		painter.setPen(Qt::NoPen);
		QColor knobBrush = palette().highlight().color();
// 		knobBrush.setAlpha(150);
		painter.setBrush(knobBrush);
		float xPos = (float)(width() - m_noteWidth) * m_minVolume; 
		painter.drawEllipse(QRectF(xPos - height() / 2 + 2, 2, height(), height() - 2)); // shade
		painter.setBrush(knobBrush.lighter());
		painter.drawEllipse(QRectF(xPos - height() / 2, 0, height(), height() - 2));
// 		painter.setBrush(palette().base());
// 		painter.drawRoundedRect(QRectF(xPos - height() / 4 + 2, height() / 3 + 4, height() / 4, height() - height() / 4), 
// 														height() / 6, height() / 6);
		painter.setBrush(palette().text());
		painter.drawEllipse(QRectF(xPos - height() / 4, height() / 5, height() * 0.6, height() * 0.6));
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
    if (i <= m_ticksCount * 0.2)
      m_tickColors << startColor;
      else if (i <= m_ticksCount * 0.5)
        m_tickColors << gradColorAtPoint((width() - m_noteWidth) * 0.2, (width() - m_noteWidth) * 0.55, startColor, middleColor, 
                                         (i + 1) * ((width() - m_noteWidth) / m_ticksCount));
      else if (i <= m_ticksCount * 0.6)
          m_tickColors << middleColor;
      else if ( i <= m_ticksCount * 0.8)
          m_tickColors << gradColorAtPoint((width() - m_noteWidth) * 0.6, (width() - m_noteWidth) * 0.82,
                                       middleColor, endColor, (i + 1) * ((width() - m_noteWidth) / m_ticksCount));
      else 
          m_tickColors << gradColorAtPoint((width() - m_noteWidth) * 0.8, (width() - m_noteWidth),
                                       endColor, totalColor, (i + 1) * ((width() - m_noteWidth) / m_ticksCount));
  }
}


void TvolumeView::mouseMoveEvent(QMouseEvent* event) {
	float minV = (float)event->pos().x() / (float)(width() - m_noteWidth);
	if (minV >= 0.1 && minV < 0.81) {
		m_minVolume = minV;
		setToolTip(QString("%1 %").arg((int)(m_minVolume * 100)));
		QToolTip::showText(mapToGlobal(QPoint( event->pos().x(), height())), toolTip());
    emit minimalVolume(m_minVolume);
	}
}


void TvolumeView::leaveEvent(QEvent* ) {
		m_drawKnob = false;
		update();
}


void TvolumeView::enterEvent(QEvent* ) {
		m_drawKnob = true;
}


