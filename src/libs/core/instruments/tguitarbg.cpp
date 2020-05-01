/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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

#include <QtQml/qqmlengine.h>
#include <QtGui/qguiapplication.h>
#include <QtGui/qpainter.h>
#include <QtGui/qpalette.h>
#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>
#include "checktime.h"


TguitarBg::TguitarBg(QQuickItem* parent) :
  TcommonInstrument(parent),
  m_curStr(7), m_curFret(99) // none
{
  setTune();

  QQmlEngine engine;
  QQmlComponent comp(&engine, this);

  comp.setData("import QtQuick 2.9; Rectangle { z: 5 }", QUrl());
  for (int s = 0; s < 6; ++s) {
    m_stringItems[s] = qobject_cast<QQuickItem*>(comp.create());
    m_stringItems[s]->setParentItem(this);
    m_stringItems[s]->setVisible(false);
    m_fingerItems[s] = qobject_cast<QQuickItem*>(comp.create());
    m_fingerItems[s]->setParentItem(this);
    m_fingerItems[s]->setVisible(false);
  }
  p_wrongItem = m_fingerItems[0];

  connect(GLOB, &Tglobals::guitarParamsChanged, this, &TguitarBg::updateGuitar);

  // Android has QML mouse area instead
#if defined (Q_OS_ANDROID)
  setAcceptHoverEvents(false);
  setAcceptedMouseButtons(Qt::NoButton);
#endif
}


QPointF TguitarBg::fretToPos(const TfingerPos& pos) const {
  qreal xPos = m_fbRect.x();
  if (pos.fret())
    xPos = m_fretsPos[pos.fret() - 1] - qRound(m_fretWidth / 1.5);
  return QPointF(xPos, m_fbRect.y() + m_strGap * (pos.str() - 1) + m_strGap / 2.0 - (pos.str() < 7 ? strWidth(pos.str() - 1) / 2.0 : 0.0));
}


void TguitarBg::setNote(const Tnote& n, quint32 noteDataValue) {
  Q_UNUSED(noteDataValue)
  if (!p_note.compareNotes(n)) {
      short noteNr = n.chromatic();
      bool foundPos = false;
      bool doShow = n.isValid();
      for (int s = 0; s < 6; ++ s) {
        auto strNr = GLOB->strOrder(s);
        int fret = noteNr - GLOB->Gtune()->strChromatic(strNr + 1);
        if (doShow && fret >= 0 && fret <= static_cast<int>(GLOB->GfretsNumber)) { // found
            if (fret == 0) { // open string
                m_fingerItems[strNr]->setVisible(false);
                m_stringItems[strNr]->setVisible(true);
            } else { // some fret
                m_fingerItems[strNr]->setVisible(true);
                auto p = fretToPos(TfingerPos(static_cast<unsigned char>(strNr + 1), static_cast<unsigned char>(fret)));
                m_fingerItems[strNr]->setX(p.x());
                m_fingerItems[strNr]->setY(m_stringItems[strNr]->y() - m_fingerItems[strNr]->height() / 2.0);
                m_stringItems[strNr]->setVisible(false);
            }
            foundPos = true;
            if (!GLOB->GshowOtherPos)
              doShow = false;
        } else { // not found on this string or no need to show - hide then
            m_fingerItems[strNr]->setVisible(false);
            m_stringItems[strNr]->setVisible(false);
        }
      }
      setOutOfScale(!foundPos && n.isValid());
      if (outOfScale())
        p_note.setNote(0); // invalidate it
      else
        p_note = n;
#if defined (Q_OS_ANDROID)
      emit noteWasSet();
#endif
  } else {
    if (p_wrongItem)
      p_wrongItem->setVisible(false);
    if (p_goodItem)
      p_goodItem->setVisible(false);
  }
  if (m_highlightedString && !n.isValid()) {
    m_highlightedString->setVisible(false);
    m_latestHighlightedData = 255;
  }
}


void TguitarBg::setFingerPos(const TfingerPos& fp) {
  QPoint p = fretToPos(fp).toPoint();
  for (int s = 0; s < 6; ++ s) {
    if (fp.fret() == 0) { // open string
        m_fingerItems[s]->setVisible(false);
        m_stringItems[s]->setVisible(fp.str() == s + 1);
    } else { // some fret
        if (fp.isValid() && fp.str() == s + 1) {
            m_fingerItems[s]->setVisible(true);
            m_fingerItems[s]->setX(p.x());
            m_fingerItems[s]->setY(m_stringItems[s]->y() - m_fingerItems[s]->height() / 2.0);
        } else
            m_fingerItems[s]->setVisible(false);
        m_stringItems[s]->setVisible(false);
    }
  }
}


qreal TguitarBg::xiiFret() const {
  return static_cast<qreal>(m_fretsPos[11]);
}


#include <QtCore/qelapsedtimer.h>
QElapsedTimer paintTimer;
void TguitarBg::paint(QPainter* painter) {
//   painter->setRenderHint(QPainter::Antialiasing, true);
//   painter->setRenderHint(QPainter::TextAntialiasing, true);
//   painter->fillRect(painter->viewport(), qApp->palette().window().color());

  paintTimer.restart();
// FINGERBOARD
  painter->setPen(Qt::NoPen);
  if (GLOB->instrument().type() == Tinstrument::ClassicalGuitar)
      painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
  else {
      QColor fbEdgeColor(175, 160, 114, 220);
      painter->setBrush(QBrush(fbEdgeColor, Qt::SolidPattern));
  }
  QPolygon a;
  int fbThick = ((m_strGap * GLOB->Gtune()->stringNr()) / 6) / 3; // thickness of fretboard
  painter->drawRect(m_fbRect.x() + m_strGap / 3 - 8, m_fbRect.y() - fbThick, m_fbRect.width() + fbThick, m_fbRect.height() + fbThick);
  if (GLOB->instrument().type() == Tinstrument::ClassicalGuitar)
    painter->setBrush(QBrush(QPixmap(Tpath::img("fingbg"))));
  else
    painter->setBrush(QBrush(QPixmap(Tpath::img("fingbg-el"))));
  painter->drawRect(m_fbRect);
// FRETS
  // zero fret (upper bridge or HUESO)
  int huesoW = qRound(m_fbRect.width() * 0.01);
  if (GLOB->instrument().type() == Tinstrument::ClassicalGuitar) {
      painter->setPen(Qt::NoPen);
      painter->setBrush(QBrush(QColor(255, 251, 240), Qt::SolidPattern)); // #FFFBF0 cream color for hueso
      painter->drawRect(m_fbRect.x() - 8, m_fbRect.y() + 4, huesoW, m_fbRect.height());
      a.setPoints(4, m_fbRect.x() - 8,                            m_fbRect.y() + 4,
                  m_fbRect.x() - 8 + huesoW,                      m_fbRect.y() + 4,
                  m_fbRect.x() + m_strGap / 3 - 8 + huesoW,       m_fbRect.y() - m_strGap / 3,
                  m_fbRect.x() + m_strGap / 3 - 8,                m_fbRect.y() - m_strGap / 3);
      painter->setBrush(QBrush(QColor(255, 238, 188), Qt::SolidPattern)); // a bit darker for its rant
      painter->drawPolygon(a);
  } else {
      QLinearGradient fretGrad(m_fbRect.x() - 8, 10.0, m_fbRect.x() - 8 + huesoW, 10.0);
      fretGrad.setColorAt(0.0, QColor(218, 228, 228));
      fretGrad.setColorAt(0.4, QColor(127, 128, 110));
      fretGrad.setColorAt(0.7, QColor(59, 56, 43));
      fretGrad.setColorAt(0.9, QColor(0, 0, 0)); // black
      painter->setBrush(fretGrad);
      painter->drawRoundedRect(m_fbRect.x() - 8, m_fbRect.y() + 2, huesoW, m_fbRect.height() - 4, 2, 2);
  }
  // others frets
  qint8 fretMarks[GLOB->GfretsNumber]; // array keeps whether fret is marked with dots (1) or two (2)
  for (unsigned int i = 0; i < GLOB->GfretsNumber; ++i)
    fretMarks[i] = 0;
  for (int fr = 0; fr < GLOB->GmarkedFrets.size(); ++fr) {
    QString exMark, frTxt = GLOB->GmarkedFrets[fr].toString();
    if (frTxt.contains(QLatin1String("!"))) {
      exMark = QStringLiteral("!");
      frTxt.replace(QLatin1String("!"), QString());
    }
    bool ok;
    int frNr = frTxt.toInt(&ok);
    if (ok && frNr > 0 && frNr <= static_cast<int>(GLOB->GfretsNumber)) {
      if (exMark.isEmpty())
        fretMarks[frNr - 1] = 1;
      else
        fretMarks[frNr - 1] = 2;
    }
  }
  for (unsigned int i = 0; i < GLOB->GfretsNumber; i++) {
    QLinearGradient fretGrad(0.0, 0.0, 1.0, 0.0);
    fretGrad.setCoordinateMode(QGradient::ObjectBoundingMode);
    fretGrad.setColorAt(0.0, QColor(218, 228, 228));
    fretGrad.setColorAt(0.4, QColor(127, 128, 110));
    fretGrad.setColorAt(0.7, QColor(59, 56, 43));
    fretGrad.setColorAt(0.9, QColor(0, 0, 0));
    painter->setBrush(fretGrad);
    painter->drawRoundedRect(m_fretsPos[i], m_fbRect.y() + 2, m_fbRect.width() * 0.0085, m_fbRect.height() - 4, 2, 2);

    if (fretMarks[i]) {
      int markW = m_strGap / 3;
      painter->setBrush(QBrush(Qt::white, Qt::SolidPattern)); //white color for circles marking 5, 7, 9... frets
      if (fretMarks[i] == 1) {
          painter->drawEllipse(m_fretsPos[i] - markW / 2 - (m_fretsPos[i] - m_fretsPos[i - 1]) / 2,
                            m_fbRect.y() + m_strGap * int((GLOB->Gtune()->stringNr() / 2)) - markW / 2, markW, markW);
      } else {
          painter->drawEllipse(m_fretsPos[i - 1] + 2 + ((m_fretsPos[i] - m_fretsPos[i - 1]) - 2 * markW) / 3,
                            m_fbRect.y() + m_strGap * int((GLOB->Gtune()->stringNr() / 2)) - markW / 2, markW, markW);
          painter->drawEllipse(m_fretsPos[i - 1] + 2 + (((m_fretsPos[i] - m_fretsPos[i - 1]) - 2 * markW) / 3) * 2 + markW,
                            m_fbRect.y() + m_strGap * int((GLOB->Gtune()->stringNr() / 2)) - markW / 2, markW, markW);
      }
    }
  }
// STRINGS
  QFont strFont = qApp->font();
  strFont.setPixelSize((int)qRound(0.75 * m_strGap));//setting font for digits
  painter->setFont(strFont);
  painter->setBrush(QBrush(Qt::NoBrush));
  for (int i = 0; i < 6; i++) {
//  drawing main strings
    qreal lineYpos = m_fbRect.y() + m_strGap / 2 + i * m_strGap;
    QLinearGradient strGrad(1.0, lineYpos - m_strWidth[i] / 2, 1.0, lineYpos + m_strWidth[i] / 2);
    strGrad.setColorAt(0.0, m_strColors[i]);
    strGrad.setColorAt(0.3, m_strColors[i]);
    strGrad.setColorAt(0.7, m_strColors[i].darker());
    painter->setPen(QPen(QBrush(strGrad), m_strWidth[i], Qt::SolidLine));
    painter->drawLine(1, lineYpos, width() - 1 - m_strGap, lineYpos);
    if (i < GLOB->Gtune()->stringNr()) {
// drawing digits of strings in circles
        painter->setPen(QPen(m_strColors[i], 1, Qt::SolidLine));
        painter->setBrush(QBrush(QColor(0, 0, 0, 180)));
        int circleX;
        if (!GLOB->GisRightHanded) {
            painter->scale (-1, 1);
            painter->translate(-width(), 0);
            circleX = 1;
        } else
            circleX = width() - 1 - m_strGap;
        QRect elRect(circleX, m_fbRect.y() + i * m_strGap, m_strGap - 1, m_strGap - 1);
        painter->drawEllipse(elRect);
        painter->setPen(QPen(Qt::green,1,Qt::SolidLine));// in green color
        painter->drawText(elRect, Qt::AlignCenter, QString::number(i + 1));
        if (!GLOB->GisRightHanded) {
            painter->translate(width(), 0);
            painter->scale (-1, 1);
        }
  // shadow of the strings
        painter->setPen(QPen(QColor(0, 0, 0, 150), m_strWidth[i], Qt::SolidLine)); // on the fingerboard
        int yy = lineYpos + m_strWidth[i] * 1.5;// m_strGap * 0.2;
        painter->drawLine(m_fbRect.x() - 6 + huesoW, yy, m_fbRect.x() + m_fbRect.width() - 1, yy);
        painter->setPen(QPen(Qt::black, 1, Qt::SolidLine)); //on upper bridge
        painter->drawLine(m_fbRect.x() - 8, lineYpos - 2, m_fbRect.x() - 8 + huesoW , lineYpos - 2);
        painter->drawLine(m_fbRect.x() - 8, lineYpos + m_strWidth[i] - 1, m_fbRect.x() - 8 + huesoW, lineYpos + m_strWidth[i] - 1);
        if (GLOB->instrument().type() != Tinstrument::ClassicalGuitar) { // shadow on the pickup if exist (bass or electric guitar)
          int pickX = width() * 0.88;
//           if (!GLOB->GisRightHanded)
//               pickX = width() - (BG_PIX->pickUpRect().x() + BG_PIX->pickUpRect().width());
          painter->setPen(QPen(QColor(28, 28, 28, 30), m_strWidth[i], Qt::SolidLine));
          yy += m_strGap * 0.1;
          qreal pickupWidth = height() * 0.6217948717948718 * 1.3;
          int subW = qRound(pickupWidth * 0.15);
          QPoint ps[5] = {
                QPoint(m_fbRect.x() + m_fbRect.width() + fbThick, yy + m_strGap * 0.12),
                QPoint(pickX, yy + m_strGap * 0.12),
                QPoint(pickX + subW, yy),
                QPoint(pickX + pickupWidth - subW, yy),
                QPoint(pickX + pickupWidth - subW * 0.3, yy + m_strGap * 0.15)
          };
          painter->drawPolyline(ps, 5);
        }

    }
  }
}


void TguitarBg::askQuestion(const Tnote& n, quint32 noteDataValue) {
  p_note = n;
  TfingerPos fp(static_cast<quint8>(noteDataValue));
  setFingerPos(fp);
}


void TguitarBg::highlightAnswer(const Tnote& n, quint32 noteData) {
  Q_UNUSED(n)
  if (!m_highlightedString) {
    QQmlEngine engine;
    QQmlComponent comp(&engine, this);
    comp.setData("import QtQuick 2.9; Rectangle {}", QUrl());
    m_highlightedString = qobject_cast<QQuickItem*>(comp.create());
    m_highlightedString->setParentItem(this);
    m_highlightedString->setZ(1);
  }
  TfingerPos f(static_cast<quint8>(noteData));
  if (f.str() > 0 && f.str() < 7) {
    int strNr = f.str() - 1;
    m_highlightedString->setX(m_stringItems[strNr]->x());
    m_highlightedString->setY(m_stringItems[strNr]->y());
    m_highlightedString->setWidth(m_stringItems[strNr]->width());
    m_highlightedString->setHeight(m_stringItems[strNr]->height());
    m_highlightedString->setProperty("color", GLOB->EanswerColor);
    m_highlightedString->setVisible(true);
    m_latestHighlightedData = noteData;
  }
}


void TguitarBg::setReadOnly(bool ro) {
  if (ro != m_readOnly) {
    m_readOnly = ro;
    setAcceptedMouseButtons(m_readOnly ? Qt::NoButton : Qt::LeftButton);
  }
}


void TguitarBg::setPressed(bool pr) {
  if (pr != m_pressed) {
    m_pressed = pr;
    emit pressedChanged();
  }
}


void TguitarBg::updateGuitar() {
  setTune();
  geometryChanged(QRectF(x(), y(), width(), height()), QRectF());
}


void TguitarBg::markSelected(const QColor& markColor) {
  int borderWidth = markColor.alpha() ? qRound(height() / 40.0) : 0;
  for (int s = 0; s < 6; ++s) {
    markBorder(m_stringItems[s], borderWidth, markColor);
    markBorder(m_fingerItems[s], borderWidth, markColor);
  }
#if defined (Q_OS_ANDROID)
  emit clearGuitar();
#endif
}


void TguitarBg::showNoteName() {
}


void TguitarBg::correct(const Tnote& n, quint32 noteData) {
  if (m_selectedPos.isValid()) {
    if (m_selectedPos.fret() > 0)
      p_wrongItem = m_fingerItems[m_selectedPos.str() - 1];
    else
      p_wrongItem = m_stringItems[m_selectedPos.str() - 1];
  } else
      p_wrongItem = nullptr;
  p_note = n;
  m_goodPos = TfingerPos(static_cast<quint8>(noteData));
  if (m_goodPos.isValid()) {
      if (m_goodPos.fret() > 0)
        p_goodItem = m_fingerItems[m_goodPos.str() - 1];
      else
        p_goodItem = m_stringItems[m_goodPos.str() - 1];
  } else
      p_goodItem = nullptr;
  emit correctInstrument();
}


void TguitarBg::applyCorrect() {
  if (p_wrongItem)
    p_wrongItem->setVisible(false);
  if (p_goodItem) {
      if (m_goodPos.fret() > 0) {
          auto p = fretToPos(m_goodPos);
          p_goodItem->setX(p.x());
          p_goodItem->setY(m_stringItems[m_goodPos.str() - 1]->y() - m_fingerItems[m_goodPos.str() - 1]->height() / 2.0);
      }
      p_goodItem->setVisible(true);
      markSelected(GLOB->correctColor());
      if (m_highlightedString)
        m_highlightedString->setVisible(false);
  }
}


void TguitarBg::finishCorrectAnim() {
  if (p_wrongItem) {
    p_wrongItem->setScale(1.0);
    p_wrongItem->setOpacity(1.0);
  }
  p_note.setNote(0);
  m_selectedPos.setData(255);
  TcommonInstrument::finishCorrectAnim();
}


void TguitarBg::pressedAt(qreal px, qreal py) {
  paintFingerAtPoint(QPointF(px, py).toPoint());
  // skip press point set - not used in that event handler
  auto me = new QMouseEvent(QEvent::MouseButtonPress, QPointF(), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
  mousePressEvent(me);
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TguitarBg::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (oldGeometry.width() != newGeometry.width() || oldGeometry.height() != newGeometry.height()) {
    QSize newSize = newGeometry.size().toSize();
    m_fbRect = QRect(10, newSize.height() / 18, (6 * newSize.width()) / 7, newSize.height() - newSize.height() / 18);
    m_fretWidth = ((m_fbRect.width() + ((GLOB->GfretsNumber / 2) * (GLOB->GfretsNumber / 2 + 1))
    + GLOB->GfretsNumber / 4) / (GLOB->GfretsNumber+1)) + 1;
    m_strGap = m_fbRect.height() / GLOB->Gtune()->stringNr();
    m_fretsPos[0] = m_fbRect.x() + m_fretWidth;
    for (unsigned int i = 2; i < GLOB->GfretsNumber + 1; i++)
      m_fretsPos[i - 1] = m_fretsPos[i - 2] + (m_fretWidth - (i / 2));
    m_lastFret = m_fretsPos[GLOB->GfretsNumber - 1];
    if (m_lastFret > (m_fbRect.width() + 10))
      m_fbRect.setWidth(m_lastFret - 8);
    qreal wFactor = newSize.height() / 150.0;
    m_strWidth[0] = m_widthFromPitch[0] * wFactor;
    m_strWidth[1] = m_widthFromPitch[1] * wFactor;
    m_strWidth[2] = m_widthFromPitch[2] * wFactor;
    m_strWidth[3] = m_widthFromPitch[3] * wFactor;
    m_strWidth[4] = m_widthFromPitch[4] * wFactor;
    m_strWidth[5] = m_widthFromPitch[5] * wFactor;

    for (int s = 0; s < 6; ++s) {
      m_stringItems[s]->setProperty("color", GLOB->GselectedColor);
      m_stringItems[s]->setWidth(newGeometry.width() - m_strGap - 2.0);
      m_stringItems[s]->setHeight(m_strWidth[s] * 1.5);
      m_stringItems[s]->setX(1.0);
      m_stringItems[s]->setY(m_fbRect.y() + static_cast<qreal>(m_strGap * s) + m_strGap / 2.0 - strWidth(s) / 2.0);

      m_fingerItems[s]->setProperty("color", GLOB->GselectedColor);
      m_fingerItems[s]->setWidth(m_fretWidth / 1.6);
      m_fingerItems[s]->setHeight(m_fretWidth / 2.46);
      m_fingerItems[s]->setProperty("radius", m_fretWidth / 3.2);
    }
    if (m_highlightedString && m_latestHighlightedData != 255)
      highlightAnswer(Tnote(), m_latestHighlightedData);

    emit fretWidthChanged();
    emit stringsGapChanged();

    if (oldGeometry.height() != newGeometry.height())
      emit heightChanged();
    if (oldGeometry.width() != newGeometry.width())
      emit widthChanged();
    if (p_note.isValid()) { // update position(s) of fingers by resetting note and setting it again
      Tnote tmpNote = p_note;
      p_note.setNote(0);
      setNote(tmpNote);
    }

    update();
  }
}


void TguitarBg::hoverMoveEvent(QHoverEvent* event) {
  if (!active())
    hoverEnterEvent(nullptr);

  paintFingerAtPoint(event->pos());
}


void TguitarBg::mousePressEvent(QMouseEvent* event) {
  if (event->buttons() & Qt::LeftButton) {
    if (m_curStr < 7) {
      m_selectedPos.setPos(m_curStr + 1, m_curFret);
      Tnote n(GLOB->Gtune()->strChromatic(m_curStr + 1) + m_curFret);
      if (GLOB->showOtherPos())
          setNote(n); // selects all possible positions on the fingerboard
      else { // it selects only just clicked position
          p_note = n;
          setFingerPos(m_selectedPos);
      }
      emit fingerPosChanged();
      emit noteChanged();
      hoverLeaveEvent(nullptr); // hide highlight that covers selected fret/string
    }
    setPressed(true);
  }
}


void TguitarBg::mouseReleaseEvent(QMouseEvent*) {
  setPressed(false);
  hoverEnterEvent(nullptr);
}


void TguitarBg::paintFingerAtPoint(QPoint p) {
  int strNr = 7, fretNr = 99;
  if ( (p.y() >= m_fbRect.y()) && (p.y() <= (height() - m_fbRect.y() - 4)) ) {
    int tx = p.x(), ty = p.y();
//     tx = mapToScene(p.x(), p.y()).x(); TODO lefthand guitars
    strNr = (ty - m_fbRect.y()) / m_strGap;
    if (tx < m_fbRect.x() || tx > m_lastFret)
      fretNr = 0;
    else {
      for (unsigned int i = 0; i < GLOB->GfretsNumber; i++) {
        if (tx <= m_fretsPos[i]) {
          fretNr = i + 1;
          break;
        }
      }
    }
  }

  if (m_curStr != strNr || m_curFret != fretNr) {
    if (fretNr > 0 && fretNr < 99) { // show finger
        m_fingerPos = fretToPos(TfingerPos(static_cast<unsigned char>(strNr + 1), static_cast<unsigned char>(fretNr)));
        if (strNr < 6)
          m_fingerPos.setY(m_stringItems[strNr]->y() - m_fingerItems[0]->height() / 2.0);
//       if (m_curStr != 7) m_workStrings[m_curStr]->hide();
    } else { // show string line
        m_fingerPos.setX(0.0);
    }
    m_curStr = strNr;
    m_curFret = fretNr;
    emit fingerPosChanged();
    emit stringChanged();
  }
}


void TguitarBg::setTune() {
  for (int i = 0; i < GLOB->Gtune()->stringNr(); i++) {
    int stringChromatic = GLOB->Gtune()->strChromatic(i + 1) + GLOB->transposition();
    if (stringChromatic > 14) { // highest than cis1
        m_strColors[i] = QColor(255, 255, 255, 125); // are nylon
        m_widthFromPitch[i] = 2; // and thinner
    } else if (stringChromatic > 10) { // highest than gis
        m_strColors[i] = QColor(255, 255, 255, 125); // are nylon
        m_widthFromPitch[i] = 2.5; // and more thick
    } else if (stringChromatic > 4) { // highest than dis
        m_strColors[i] = QColor(255, 255, 255, 150); // are nylon
        m_widthFromPitch[i] = 3; // and more thick
    } else if (stringChromatic > 0) { // highest than b-1(contra)
        m_strColors[i] = QColor(194, 148, 50); // are gold-plated
        m_widthFromPitch[i] = 3; // and more thick
    } else if (stringChromatic > -5) { // highest than g-1(contra) 1-st string of bass
        m_strColors[i] = QColor(194, 148, 50); // #C29432" // are gold-plated
        m_widthFromPitch[i] = 3.5; // and more thick
    } else if (stringChromatic > -10) { // highest than d-1(contra)
        m_strColors[i] = QColor(194, 148, 50); // are gold-plated
        m_widthFromPitch[i] = 4; // and more thick
    } else if (stringChromatic > -15) { // highest than gis-2(subcontra)
        m_strColors[i] = QColor(194, 148, 50); // are gold-plated
        m_widthFromPitch[i] = 4.5; // and more thick
    } else if (stringChromatic > -20) { // highest than dis-1(contra)
        m_strColors[i] = QColor(194, 148, 50); // are gold-plated
        m_widthFromPitch[i] = 5; // and more thick
    } else if (stringChromatic > -25) { // highest than
        m_strColors[i] = QColor(194, 148, 50); // are gold-plated
        m_widthFromPitch[i] = 6; // and more thick
    } else if (stringChromatic > -30) { // highest than
        m_strColors[i] = QColor(194, 148, 50); // are gold-plated
        m_widthFromPitch[i] = 7; // and more thick
    }
  }
}


