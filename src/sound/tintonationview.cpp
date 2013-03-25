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


#define THWIDTH (3)
#define THGAP (4)

TintonationView::TintonationView(int accuracy, QWidget* parent) :
  QWidget(parent),
  m_pitchDiff(0.0f)
{
  setAccuracy(accuracy);
  setMinimumHeight(15);
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
  int lastColorThick = (qAbs(m_pitchDiff) / 0.5) * m_thicksCount;
  QPainter painter(this);
  painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
  painter.setPen(Qt::NoPen);
  painter.setBrush(QBrush(palette().window().color()));
  painter.drawRoundedRect(painter.viewport(), 4, 4);
  if (m_pitchDiff == 0.0)
    painter.setPen(QPen(Qt::darkGray));
  else
    painter.setPen(QPen(m_thickColors[lastColorThick]));
  painter.setFont(m_nooFont);
  painter.drawText(0, 0, width(), height(), Qt::AlignCenter, "n");
  for (int i = 0; i < m_thicksCount - 1; i++) {
    QColor thickColor, leftThickColor, rightThickColor;
    if (i < lastColorThick)
      thickColor = m_thickColors[i];
    else
      thickColor = Qt::darkGray;
    if (m_pitchDiff < 0) {
      leftThickColor = thickColor; rightThickColor = Qt::darkGray;
    } else {
      leftThickColor = Qt::darkGray; rightThickColor = thickColor;
    }
    int xx = m_noteX - ((i + 1) * (THGAP + THWIDTH));
    int yy = qRound((float)(m_thicksCount - i) * m_hiThickStep);
    painter.setPen(QPen(leftThickColor, THWIDTH, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(xx, yy, xx, height());
    painter.setPen(QPen(rightThickColor, THWIDTH, Qt::SolidLine, Qt::RoundCap));
    xx = (width() - m_noteX) + ((i + 1) * (THGAP + THWIDTH)) - THWIDTH;
    painter.drawLine(xx, yy, xx, height());
  }
}


void TintonationView::resizeEvent(QResizeEvent* ) {
  m_nooFont = QFont("nootka");
  m_nooFont.setPointSizeF(height() * 0.7);
  QFontMetrics fm(m_nooFont);
  QRect noteBound = fm.boundingRect("n");
  float factor = (float)height() / (float)noteBound.height();
  m_nooFont.setPointSizeF(m_nooFont.pointSizeF() * factor);
  noteBound = fm.boundingRect("n");
  m_noteX = (width() - noteBound.width() * 2) / 2;
  m_thicksCount = m_noteX / (THWIDTH + THGAP);
  m_hiThickStep = ((float)height() * 0.66) / m_thicksCount;
  m_thickColors.clear();
  for (int i = 0; i < m_thicksCount; i++) {
    if (i <= m_thicksCount/5) {
      m_thickColors << Qt::green;
//       m_thickColors << gradColorAtPoint(0, m_noteX, Qt::green, Qt::yellow, (i + 1) * m_noteX / m_thicksCount);
    }
    else if (i <= m_thicksCount*0.4)
      m_thickColors << gradColorAtPoint(0, m_noteX, Qt::green, Qt::yellow, (i + 1) * m_noteX / m_thicksCount);
//       m_thickColors << Qt::yellow;
    else
      m_thickColors << gradColorAtPoint(0, m_noteX, Qt::yellow, Qt::red, (i + 1) * m_noteX / m_thicksCount);
//       m_thickColors << Qt::red;
  }
}

/*
 http://www.qtcentre.org/threads/14307-How-to-get-the-specified-position-s-QColor-in-QLinearGradient
 You can do a linear interpolation and build the QColor. Let's assume you want the color at the point P, which is on the line between startP and endP.
             double segmentLength = sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1));
            double pdist = sqrt((xp-x1)*(xp-x1) + (yp-y1)*(yp-y1));
            double ratio = pdist/segmentLength;
            int red = (int)(ratio*startRedVal + (1-ratio)*endRedValue); //in your case, the values are 12 and 122
            int green = (int)(ratio*startGreenVal + (1-ratio)*endGreenValue); //in your case, the values are 23 and 233
            int blue = (int)(ratio*startBlueVal + (1-ratio)*endBlueValue); //in your case, the values are 24 and 244
 */
QColor TintonationView::gradColorAtPoint(float lineX1, float lineX2, QColor startC, QColor endC, float posC) {
  float segmentLength = sqrt((lineX2 - lineX1) * (lineX2 - lineX1));
  double pdist = sqrt((posC - lineX1) * (posC - lineX1));
  double ratio = pdist / segmentLength;
  int red = (int)(ratio * startC.red() + ( 1 - ratio) * endC.red()); //in your case, the values are 12 and 122
  int green = (int)(ratio * startC.green() + (1 - ratio) * endC.green()); //in your case, the values are 23 and 233
  int blue = (int)(ratio * startC.blue() + (1 - ratio) * endC.blue()); //in your case, the values are 24 and 244
  return QColor(red, green, blue);
}



