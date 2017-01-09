/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#include "tscore5lines.h"
#include <QApplication>
#include <QPalette>
#include <QPen>
#include <QGraphicsEffect>



Tscore5lines::Tscore5lines(TscoreScene* scene) :
  TscoreItem(scene),
  m_isPianoStaff(false),
  m_height(9.0), m_width(1.0)
{
  setFlag(ItemHasNoContents);
  createLines(m_lines);
}


void Tscore5lines::setPianoStaff(bool isPiano) {
  if (isPiano != m_isPianoStaff) {
    m_isPianoStaff = isPiano;
    if (m_isPianoStaff) {
      createLines(m_lowLines);
      m_height = 22.0;
      m_width += 1.0; // change width, otherwise next line will be rejected
      setWidth(m_width - 1.0); // updates grand staff lines width
    } else {
      for (int i = 0; i < 5; ++i)
        delete m_lowLines[i];
      m_lowLines.clear();
      m_height = 9.0;
    }
  }
}


void Tscore5lines::setWidth(qreal w) {
  if (m_width != w) {
    m_width = w;
    for (int l = 0; l < 5; l++) {
      m_lines[l]->setLine(0.5, 0.0 + l * 2, width(), 0.0 + l * 2);
      if (isPianoStaff())
        m_lowLines[l]->setLine(0.5, 14.0 + l * 2, width(), 14.0 + l * 2);
    }
  }
}


void Tscore5lines::setDisabled(bool dis) {
  QColor lc = qApp->palette().text().color();
  qreal lh = 0.18;
  if (dis) {
    lc.setAlpha(50);
    lh = 0.24;
    QGraphicsDropShadowEffect *blured = new QGraphicsDropShadowEffect();
        blured->setBlurRadius(20);
        blured->setColor(lc);
        blured->setOffset(0.0, 0.2);
    setGraphicsEffect(blured);
  } else
      setGraphicsEffect(0);
  for (int l = 0; l < 5; l++) {
    m_lines[l]->setPen(QPen(lc, lh));
    if (isPianoStaff())
      m_lowLines[l]->setPen(QPen(lc, lh));
  }
}


QRectF Tscore5lines::boundingRect() const {
  return QRectF(0.0, -0.1, m_width, m_height);
}


//#################################################################################################
//###################               PRIVATE            ############################################
//#################################################################################################
void Tscore5lines::createLines(QList<QGraphicsLineItem*>& lines) {
  for (int i = 0; i < 5; i++) {
    lines << new QGraphicsLineItem();
    registryItem(lines[i]);
    lines[i]->setPen(QPen(qApp->palette().text().color(), 0.18));
    lines[i]->setZValue(5);
  }
}




