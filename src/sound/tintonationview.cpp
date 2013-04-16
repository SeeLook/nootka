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


#include "tintonationview.h"
#include <QPainter>
#include <QDebug>
#include <math.h>


#define TICK_WIDTH (2)
#define TICK_GAP (3)

TintonationView::TintonationView(int accuracy, QWidget* parent) :
  TabstractSoundView(parent),
  m_pitchDiff(0.0f)
{
  setBackgroundRole(QPalette::Background);
  setAutoFillBackground(true);
  setAccuracy(accuracy);
  setMinimumSize(200, 17);
  resizeEvent(0);
}

TintonationView::~TintonationView()
{

}

void TintonationView::setAccuracy(int accuracy) {
  m_accuracy = (Eaccuracy)accuracy;
  switch(m_accuracy) {
    case e_perfect:
      m_accurValue = 0.1; break;
    case e_normal:
      m_accurValue = 0.2; break;
    case e_sufficient:
      m_accurValue = 0.3; break;
  }
  resizeEvent(0);
}


void TintonationView::pitchSlot(float pitch) {
  bool doUpdate = false;
  if (m_pitchDiff != 0.0)
      doUpdate = true;
  m_pitchDiff = pitch - (float)qRound(pitch);
  if (doUpdate)
      update();
}


//################################################################################
//############################### protected ######################################
//################################################################################


void TintonationView::paintEvent(QPaintEvent* ) {
  int lastColorThick = (qAbs(m_pitchDiff) / 0.5) * m_ticksCount;
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(palette().window().color()));
//   painter.setBrush(QBrush(Qt::white));
  painter.drawRoundedRect(painter.viewport(), 4, 4);
  if (m_pitchDiff == 0.0)
    painter.setPen(QPen(tc));
  else
    painter.setPen(QPen(m_tickColors[lastColorThick]));
  painter.setFont(nootFont);
  painter.drawText(0, 0, width(), height(), Qt::AlignCenter, "n");
  for (int i = 0; i < m_ticksCount - 1; i++) {
    QColor thickColor, leftThickColor, rightThickColor;
    if (i < lastColorThick)
      thickColor = m_tickColors[i];
    else
      thickColor = tc;
    if (m_pitchDiff < 0) {
      leftThickColor = thickColor; rightThickColor = tc;
    } else {
      leftThickColor = tc; rightThickColor = thickColor;
    }
    int xx = m_noteX - ((i + 1) * (TICK_GAP + TICK_WIDTH));
    float yy = (float)(m_ticksCount - i) * m_hiTickStep + 1;
//     int yy = 1;
    painter.setPen(QPen(leftThickColor, TICK_WIDTH, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QLineF(xx, yy, xx, height() - 2));
    painter.setPen(QPen(rightThickColor, TICK_WIDTH, Qt::SolidLine, Qt::RoundCap));
    xx = (width() - m_noteX) + ((i + 1) * (TICK_GAP + TICK_WIDTH)) - TICK_WIDTH;
    painter.drawLine(QLineF(xx, yy, xx, height() - 2));
  }
}


void TintonationView::resizeEvent(QResizeEvent* ) {
  resizeIt(height());
//   qDebug() << "TintonationView" << height() << nootFont;
  m_noteX = (width() - noteBound.width() * 2) / 2;
  m_ticksCount = m_noteX / (TICK_WIDTH + TICK_GAP);
  m_hiTickStep = ((float)height() * 0.66) / m_ticksCount;
  m_tickColors.clear();
  for (int i = 0; i < m_ticksCount; i++) {
    if (i <= m_ticksCount*m_accurValue) {
      m_tickColors << gradColorAtPoint(0, m_noteX * m_accurValue * 1.2, startColor, middleColor, (i + 1) * (m_noteX / m_ticksCount));
    }
    else if (i <= m_ticksCount*0.5) {
      m_tickColors << gradColorAtPoint(m_noteX * m_accurValue, m_noteX * 0.55, middleColor, endColor, (i + 1) * (m_noteX / m_ticksCount));
      } else {
          m_tickColors << gradColorAtPoint(m_noteX * 0.5, m_noteX, endColor, totalColor, (i + 1) * (m_noteX / m_ticksCount));
        }
  }
}



