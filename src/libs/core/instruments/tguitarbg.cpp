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

#include "tguitarbg.h"
#include "tglobals.h"
#include "music/ttune.h"
#include "tpath.h"

#include <QtGui/qguiapplication.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>


TguitarBg::TguitarBg(QQuickItem* parent) :
  QQuickPaintedItem(parent)
{
  setRenderTarget(QQuickPaintedItem::FramebufferObject);
  //   setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
  setAntialiasing(true);
}


#include <QtCore/qelapsedtimer.h>
QElapsedTimer elti;
void TguitarBg::paint(QPainter* painter) {
  elti.restart();

//   painter->setRenderHint(QPainter::Antialiasing, true);
//   painter->setRenderHint(QPainter::TextAntialiasing, true);
//   painter->fillRect(painter->viewport(), qApp->palette().window().color());

// FINGERBOARD
  painter->setPen(QPen(Qt::black, 0, Qt::NoPen));
  if (GLOB->instrument == e_classicalGuitar)
      painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
  else {
      QColor fbEdgeColor = QColor("#AFA072");
      fbEdgeColor.setAlpha(220);
      painter->setBrush(QBrush(fbEdgeColor, Qt::SolidPattern));
  }
  QPolygon a;
  int fbThick = ((m_strGap * GLOB->Gtune()->stringNr()) / 6) / 4; // thickness of fretboard
  a.setPoints(6, m_fbRect.x(), m_fbRect.y(),
              m_fbRect.x() + qRound(m_fbRect.width() * 0.005), m_fbRect.y() - fbThick,
              m_fbRect.x() + m_fbRect.width() + fbThick, m_fbRect.y() - fbThick,
              m_fbRect.x() + m_fbRect.width() + fbThick, height() - fbThick,
              m_fbRect.x() + m_fbRect.width(), height(),
              m_fbRect.x(), height());
  painter->drawPolygon(a);
  if (GLOB->instrument == e_classicalGuitar)
    painter->setBrush(QBrush(QPixmap(Tpath::img("fingbg"))));
  else
    painter->setBrush(QBrush(QPixmap(Tpath::img("fingbg-el"))));
  painter->drawRect(m_fbRect);
// FRETS
  // zero fret (upper bridge or HUESO)
  int huesoW = qRound(m_fbRect.width() * 0.01);
  if (GLOB->instrument == e_classicalGuitar) {
      painter->setPen(Qt::NoPen);
      painter->setBrush(QBrush(QColor("#FFFBF0"),Qt::SolidPattern)); // #FFFBF0 cream color for hueso
      painter->drawRect(m_fbRect.x() - 8, m_fbRect.y() + 4, huesoW, m_fbRect.height());
      a.setPoints(4, m_fbRect.x() - 8,                            m_fbRect.y() + 4,
                  m_fbRect.x() - 8 + huesoW,                      m_fbRect.y() + 4,
                  m_fbRect.x() + m_strGap / 3 - 8 + huesoW,       m_fbRect.y() - m_strGap / 3,
                  m_fbRect.x() + m_strGap / 3 - 8,                m_fbRect.y() - m_strGap / 3);
      painter->setBrush(QBrush(QColor("#FFEEBC"),Qt::SolidPattern)); // a bit darker for its rant
      painter->drawPolygon(a);
  } else {
      QLinearGradient fretGrad(m_fbRect.x() - 8, 10.0, m_fbRect.x() - 8 + huesoW, 10.0);
      fretGrad.setColorAt(0.0, QColor("#DAE4E4"));
      fretGrad.setColorAt(0.4, QColor("#7F806E"));
      fretGrad.setColorAt(0.7, QColor("#3B382B"));
      fretGrad.setColorAt(0.9, QColor("#000000"));
      painter->setBrush(fretGrad);
      painter->drawRoundedRect(m_fbRect.x() - 8, m_fbRect.y() + 2, huesoW, m_fbRect.height() - 4, 2, 2);
  }
  // others frets
  qint8 fretMarks[GLOB->GfretsNumber]; // array keeps whether fret is marked with dots (1) or two (2)
  for (int i = 0; i < GLOB->GfretsNumber; ++i)
    fretMarks[i] = 0;
  for (int fr = 0; fr < GLOB->GmarkedFrets.size(); ++fr) {
    QString exMark, frTxt = GLOB->GmarkedFrets[fr].toString();
    if (frTxt.contains(QLatin1String("!"))) {
      exMark = QStringLiteral("!");
      frTxt.replace(QLatin1String("!"), QString());
    }
    bool ok;
    int frNr = frTxt.toInt(&ok);
    if (ok && frNr > 0 && frNr <= GLOB->GfretsNumber) {
      if (exMark.isEmpty())
        fretMarks[frNr - 1] = 1;
      else
        fretMarks[frNr - 1] = 2;
    }
  }
  for (int i = 0; i < GLOB->GfretsNumber; i++) {
    QLinearGradient fretGrad(0.0, 0.0, 1.0, 0.0);
    fretGrad.setCoordinateMode(QGradient::ObjectBoundingMode);
    fretGrad.setColorAt(0.0, QColor("#DAE4E4"));
    fretGrad.setColorAt(0.4, QColor("#7F806E"));
    fretGrad.setColorAt(0.7, QColor("#3B382B"));
    fretGrad.setColorAt(0.9, QColor("#000000"));
    painter->setBrush(fretGrad);
    painter->drawRoundedRect(m_fretsPos[i], m_fbRect.y() + 2, m_fbRect.width() * 0.0085, m_fbRect.height() - 4, 2, 2);

    if (fretMarks[i]) {
      int markW = m_strGap / 3;
      painter->setBrush(QBrush(Qt::white, Qt::SolidPattern)); //white color for circles marking 5, 7, 9... frets
      if (fretMarks[i] == 1)
        painter->drawEllipse(m_fretsPos[i] - markW / 2 - (m_fretsPos[i] - m_fretsPos[i - 1]) / 2,
                            m_fbRect.y() + m_strGap * int((GLOB->Gtune()->stringNr() / 2)) - markW / 2, markW, markW);
        else {
          painter->drawEllipse(m_fretsPos[i - 1] + 2 + ((m_fretsPos[i] - m_fretsPos[i - 1]) - 2 * markW) / 3,
                              m_fbRect.y() + m_strGap * int((GLOB->Gtune()->stringNr() / 2)) - markW / 2, markW, markW);
          painter->drawEllipse(m_fretsPos[i - 1] + 2 + (((m_fretsPos[i] - m_fretsPos[i - 1]) - 2 * markW) / 3) * 2 + markW,
                              m_fbRect.y() + m_strGap * int((GLOB->Gtune()->stringNr() / 2)) - markW / 2, markW, markW);
        }
    }
  }

  qDebug() << "[TguitarBg] painting time" << elti.nsecsElapsed() / 1000 << "[μs]";
}

void TguitarBg::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (oldGeometry.width() != newGeometry.width() || oldGeometry.height() != newGeometry.height()) {
    QSize newSize = newGeometry.size().toSize();
    m_fbRect = QRect(10, newSize.height() / 18, (6 * newSize.width()) / 7, newSize.height() - newSize.height() / 18);
    m_fretWidth = ((m_fbRect.width() + ((GLOB->GfretsNumber / 2) * (GLOB->GfretsNumber / 2 + 1))
    + GLOB->GfretsNumber / 4) / (GLOB->GfretsNumber+1)) + 1;
    m_strGap = m_fbRect.height() / GLOB->Gtune()->stringNr();
    m_fretsPos[0] = m_fbRect.x() + m_fretWidth;
    for (int i = 2; i < GLOB->GfretsNumber + 1; i++)
      m_fretsPos[i - 1] = m_fretsPos[i - 2] + (m_fretWidth - (i / 2));
    m_lastFret = m_fretsPos[GLOB->GfretsNumber - 1];
    if (m_lastFret > (m_fbRect.width() + 10))
      m_fbRect.setWidth(m_lastFret - 8);

    update();
  }
}
