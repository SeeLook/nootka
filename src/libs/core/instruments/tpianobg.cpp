/***************************************************************************
 *   Copyright (C) 2017 by Tomasz Bojczuk                                  *
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

#include "tpianobg.h"
#include "tpath.h"
#include "tglobals.h"

#include <QtGui/qpainter.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qscreen.h>
#include <QtCore/qmath.h>
#include <QtCore/qdebug.h>

#include "checktime.h"


const char* const octaveNames[8] = { QT_TR_NOOP("Subcontra"), QT_TR_NOOP("Contra"), QT_TR_NOOP("Great"), QT_TR_NOOP("Small"),
                          QT_TR_NOOP("One-line"), QT_TR_NOOP("Two-line"), QT_TR_NOOP("Three-line"), QT_TR_NOOP("Four-line") };


TpianoBg::TpianoBg(QQuickItem* parent) :
  TcommonInstrument(parent),
  m_keyWidth(32.0),
  m_firstOctave(-3)
{}


TpianoBg::~TpianoBg() {}


void TpianoBg::setKeyWidth(qreal kw) {
  if (m_keyWidth != kw) {
    m_keyWidth = kw;
    calculateMetrics(width());
    update();
  }
}


void TpianoBg::setNote(const Tnote& n) {
  
}


void TpianoBg::setFirstOctave(int firstO) {
  auto fo = static_cast<char>(firstO);
  if (fo != m_firstOctave) {
    m_firstOctave = fo;
    update();
  }
}


void TpianoBg::setReadOnly(bool ro) {
  if (ro != m_readOnly) {
    m_readOnly = ro;
    setAcceptedMouseButtons(m_readOnly ? Qt::NoButton : Qt::LeftButton);
  }
}


void TpianoBg::paint(QPainter* painter) {
CHECKTIME (
  int kw = qFloor(m_keyWidth);
  auto keyPix = QPixmap(Tpath::img("pianokey")).scaled(qRound(m_keyWidth * 0.8), height() / 2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
  painter->setPen(QPen(Qt::black, m_keyWidth / 16, Qt::SolidLine));
  painter->setBrush(Qt::white);
  for (int k = 0; k < m_keysNumber; ++k) {
    painter->drawRoundedRect(m_margin + k * kw, 1, kw, height() - 2, m_keyWidth / 5, m_keyWidth / 5);
    if (k % 7 != 0 && k % 7 != 3)
      painter->drawPixmap(m_margin + k * kw - qRound(m_keyWidth * 0.4), kw, keyPix);
  }
  painter->setBrush(Qt::black);
  painter->drawRect(0, 0, width(), kw);
  if (m_margin > 0) {
    painter->drawRect(0, kw, m_margin, height() - kw);
    painter->drawRect(width() - m_margin, kw, m_margin, height() - kw);
  }
  painter->setPen(Qt::white);
  QFont f;
  f.setPixelSize(qRound(m_keyWidth * 0.6));
  painter->setFont(f);
  int octavesNr = m_keysNumber / 7;
  for (int k = 0; k < octavesNr; ++k) {
    painter->drawText(QRect(m_margin + k * 7 * kw, 0, 7 * kw, m_keyWidth), Qt::AlignCenter, octaveNames[m_firstOctave + 3 + k]);
    if (k < octavesNr - 1 || m_keysNumber - (m_keysNumber / 7) * 7 > 0) { // do not draw a tick of last octave when it is completed
      int xx = m_margin + (k + 1) * 7 * kw;
      painter->drawLine(xx, qRound(m_keyWidth / 2.0), xx, qRound(m_keyWidth - 1.0));
    }
  }
)
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TpianoBg::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (oldGeometry.width() != newGeometry.width() || oldGeometry.height() != newGeometry.height()) {
    calculateMetrics(qFloor(newGeometry.width()));
    update();
  }
}


void TpianoBg::hoverLeaveEvent(QHoverEvent*) {
  TcommonInstrument::hoverLeaveEvent(nullptr);
  m_keyRect = QRectF();
  emit keyRectChanged();
}


void TpianoBg::hoverMoveEvent(QHoverEvent* event) {
  if (event->pos().y() > m_keyWidth) {
    int kw = qFloor(m_keyWidth);
    int keyNr = qFloor((event->pos().x() - m_margin) / kw);
    Tnote newNote(static_cast<char>(keyNr % 7 + 1), m_firstOctave + static_cast<char>(keyNr / 7), 0);
    bool isWhite = true;
    if (event->pos().y() < height() / 2) { // black keys
      if (keyNr % 7 != 0 && keyNr % 7 != 3) {
        --newNote.note;
        newNote.alter = Tnote::e_Sharp;
        isWhite = false;
      }
    }
    if (m_activeNote != newNote) {
      m_activeNote.note = newNote.note; m_activeNote.octave = newNote.octave; m_activeNote.alter = newNote.alter;
      if (isWhite)
        m_keyRect.setRect(m_margin + keyNr * kw, m_keyWidth, m_keyWidth, height() - m_keyWidth);
      else
        m_keyRect.setRect(m_margin + keyNr * kw - m_keyWidth * 0.4, m_keyWidth, m_keyWidth * 0.8, height() / 2.0);
      emit keyRectChanged();
    }
  }
}


void TpianoBg::mousePressEvent(QMouseEvent* event) {
CHECKTIME(
  if (event->buttons() & Qt::LeftButton) {
    p_note = m_activeNote;
    emit noteChanged();
  }
)
}



void TpianoBg::calculateMetrics(int newWidth) {
  m_keysNumber = newWidth / qRound(m_keyWidth);
  if (m_keysNumber > 56) {
    m_keysNumber = 56;
    m_keyWidth = newWidth / m_keysNumber;
  }
  m_margin = (newWidth - m_keysNumber * qFloor(m_keyWidth)) / 2;
}
