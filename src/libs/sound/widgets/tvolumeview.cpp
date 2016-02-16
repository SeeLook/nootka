/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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


#include "tvolumeview.h"
#include <QPainter>
#include <QMouseEvent>
#include <QToolTip>
#include <QApplication>
#include <QDebug>

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
  m_drawKnob(false), m_leftButton(false),
  m_paused(true), m_activePause(false),
  m_overNote(false), m_drawPaused(false)
{
  setMinimumSize(200, 17);
#if !defined (Q_OS_ANDROID)
  setMouseTracking(true);
  setStatusTip(tr("Shows volume level of input sound and indicates when the note was pitch-detected.") + "<br>" +
        tr("Drag a knob to adjust minimum input volume."));
#endif
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


void TvolumeView::setPauseActive(bool active) {
	m_activePause = active;
}


void TvolumeView::setDisabled(bool isDisabled) {
	TabstractSoundView::setDisabled(isDisabled);
	if (m_activePause) {
		if (isDisabled)
			m_paused = isDisabled;
	}
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TvolumeView::paintEvent(QPaintEvent* ) {
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  QColor noteColor = qApp->palette().text().color();
  if (m_alpha) { // Background if note is detected
      m_pitchColor.setAlpha(m_alpha);
      painter.setBrush(QBrush(m_pitchColor));
      noteColor = qApp->palette().highlightedText().color();
			painter.drawRoundedRect(painter.viewport(), 4, 4);
  }
  painter.setFont(nootFont);
  QString nSymbol = (m_activePause && m_paused) ? "o" : "n";
	QRect nRect = painter.fontMetrics().boundingRect(nSymbol);
	if (m_drawPaused) { // Stop/start highlight
		painter.setBrush(m_overNote ? qApp->palette().highlightedText().color().darker(95) : qApp->palette().highlight().color());
		painter.drawRoundedRect(width() - nRect.width() * 1.8, 0, nRect.width() * 1.8, height(), 50, 50, Qt::RelativeSize);
  }
	if (m_drawPaused) // and note symbol
		painter.setPen(m_overNote ? qApp->palette().highlight().color() : qApp->palette().highlightedText().color());
  else if (m_alpha)
		painter.setPen(noteColor);
  else
		painter.setPen(tc);
	painter.drawText(0, 0, width() - nRect.width() * 0.3, height(), Qt::AlignRight, nSymbol);

  qreal tickWidth = TICK_WIDTH - 1.0;
  for (int i = 1; i < m_ticksCount - 2; i++) { // volume ticks
    if (i >= m_ticksCount * m_minVolume)
      tickWidth = TICK_WIDTH;
		if (m_activePause && m_paused)
			painter.setPen(QPen(disabledColor, tickWidth, Qt::SolidLine, Qt::RoundCap));
    else if (m_volume * m_ticksCount >= i)
      painter.setPen(QPen(m_tickColors[i], tickWidth, Qt::SolidLine, Qt::RoundCap));
    else 
      painter.setPen(QPen(tc, tickWidth, Qt::SolidLine, Qt::RoundCap));
		float ticH = (((float)i * m_hiTickStep + 1.0));
    painter.drawLine(QLineF((i + 1) * (TICK_GAP + TICK_WIDTH) - TICK_WIDTH, (height() - ticH) / 2,
                            (i + 1) * (TICK_GAP + TICK_WIDTH) - TICK_WIDTH, height() - (height() - ticH) / 2));
  }
  if (m_drawKnob) { // volume knob
		painter.setPen(Qt::NoPen);
		QColor knobBrush = qApp->palette().highlight().color(), shade = qApp->palette().text().color();
		if (m_leftButton)
			knobBrush = knobBrush.lighter();
		shade.setAlpha(150);
		painter.setBrush(shade);
		float xPos = (float)(width() - m_noteWidth) * m_minVolume; 
		painter.drawEllipse(QRectF(xPos - height() * 0.5, height() * 0.1, height() * 0.9, height() * 0.9)); // shade
		QLinearGradient lg(xPos, 0, xPos, height());
		lg.setColorAt(0, knobBrush);
		lg.setColorAt(0.3, knobBrush);
		lg.setColorAt(0.8, knobBrush.darker());
		painter.setBrush(QBrush(lg));
		painter.drawEllipse(QRectF(xPos - height() * 0.5, height() * 0.05, height() * 0.9, height() * 0.9));
		painter.setBrush(qApp->palette().highlightedText());
		painter.setPen(QPen(shade, 0.7));
		painter.drawEllipse(QRectF(xPos - height() * 0.27, height() * 0.27, height() * 0.45, height() * 0.45));
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
  if (!isEnabled())
    return;
  if (isPauseActive())
      m_drawPaused = true;
  if (event->x() >= width() - m_noteWidth * 1.5) {
    if (!m_overNote) {
      if (parentWidget()) {
        QStatusTipEvent sEv(tr("Switch on/off pitch detection"));
        qApp->sendEvent(parentWidget(), &sEv);
      }
    }
    m_overNote = true;
  } else {
    if (m_overNote) {
      if (parentWidget()) {
        QStatusTipEvent sEv(statusTip());
        qApp->sendEvent(parentWidget(), &sEv);
      }
    }
    m_overNote = false;
    if (!isPaused())
      m_drawKnob = true;
    if (m_leftButton) {
      float minV = (float)event->pos().x() / (float)(width() - m_noteWidth);
      if (minV >= 0.1 && minV < 0.81) {
        m_minVolume = minV;
        setToolTip(QString("%1 %").arg((int)(m_minVolume * 100)));
        QToolTip::showText(QCursor::pos(), toolTip());
        emit minimalVolume(m_minVolume);
      }
    }
  }
	update();
}


void TvolumeView::mousePressEvent(QMouseEvent* event) {
  if (m_activePause) {
    if (event->button() == Qt::LeftButton) {
      if (m_overNote) {
        m_paused = !m_paused;
        emit paused();
      } else
        m_leftButton = true;
    }
    update();
  }
}


void TvolumeView::mouseReleaseEvent(QMouseEvent*) {
	m_leftButton = false;
}



void TvolumeView::leaveEvent(QEvent* ) {
	m_drawKnob = false;
	m_overNote = false;
	m_drawPaused = false;
	update();
}


void TvolumeView::enterEvent(QEvent* ) {

}


