/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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

#include "tbandoneonbg.h"
#include "music/ttechnical.h"
#include "tglobals.h"

#include <QtQml/qqmlengine.h>
#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>
#include "checktime.h"


struct BandoButt {
  qreal x;
  qreal y;
  qint8 open;
  qint8 close;
};


static BandoButt buttArray[71] = {
// left hand buttons - 33 pieces
//     X   ,    Y   ,Open,Close
  { 21.2244, 17.8522, -7, -9 }, // E-1 D-1
  { 8.77049, 63.5745, -9, -7 }, // D-1 E-1
  { 19.7463, 45.3289, 5, -2 }, // E0 A-1                          E0 (5) x 3
  { 28.0139, 58.4429, 0, 5 }, // H-1 E0
  { 52.674, 51.8859, 20, 19 }, // G1 F#1
  { 43.9788, 38.9145, 9, 5 }, // G#0 E0
  { 35.5687, 25.3728, 3, -4 }, // D0 G-1
  { 45.5468, 13.2566, -2, 3 }, // A-1 D0
  { 57.2354, 2.56579, -3, -3 }, // G#-1 G#-1
  { 60.799, 20.5263, 10, 8 }, // A0 G0
  { 69.4941, 35.3509, 12, 10 }, // H0 A0            # 10
  { 77.9042, 49.1776, 22, 21 }, // A1 G#1
  { 73.0577, 8.69517, 8, 11 }, // G0 A#0
  { 86.4568, 19.5285, 13, 12 }, // C1 H0
  { 86.4568, 0, -1, -1 }, // A#-1 A#-1
  { 95.7222, 33.7829, 15, 14 }, // D1 C#1
  { 103.99, 49.1776, 16, 24 }, // D#1 H1
  { 99.5709, 9.26534, 4, 13 }, // D#0 C1
  { 113.968, 1.14036, 2, 4 }, // C#0 D#0
  { 111.83, 20.6688, 17, 15 }, // E1 D1
  { 119.67, 36.4912, 19, 17 }, // F#1 E1            # 20
  { 126.084, 11.6886, 18, 2 }, // F1 C#0
  { 128.65, 51.0307, 7, 6 }, // F#0 F0
  { 136.917, 24.0899, 1, 18 }, // C0 F1
  { 142.477, 5.13158, 6, 16 }, // F0 D#1
  { 144.33, 38.9145, 14, 9 }, // C#1 G#0
  { 153.452, 55.0219, -8, -10 }, // D#-1 C#-1
  { 152.74, 17.5329, 11, 1 }, // A#0 C0
  { 161.862, 31.2171, -4, 7 }, // G-1 F#0
  { 170.558, 46.7543, -5, 0 }, // F#-1 H-1
  { 179.11, 61.7215, -11, -6 }, // C-1 F-1          # 30
  { 178.825, 25.5153, -6, -5 }, // F-1 F#-1
  { 169.275, 11.9737, 21, 20 }, // G#1 G1
// right hand buttons - 38 pieces
  { 26.0417, 33.4609, 12, 12 }, // H0 H0
  { 18.9575, 70.2402, 11, 11 }, // A#0 A#0
  { 12.5596, 53.4871, 10, 10 }, // A0 A0
  { 39.8729, 63.0944, 16, 16 }, // D#1 D#1
  { 31.8882, 48.2275, 18, 18 }, // F1 F1
  { 34.7614, 19.897, 13, 15 }, // C1 D1
  { 46.5546, 29.8194, 17, 19 }, // E1 F#1
  { 44.2828, 8.27076, 14, 13 }, // C#1 C1         # 40
  { 55.1407, -3.22183, 48, 46 }, // H3 A3                     A3 (46) x 3
  { 55.9759, 16.8902, 15, 14 }, // D1 C#1
  { 53.9714, 45.1873, 23, 17 }, // A#1 E1
  { 59.7177, 59.6533, 30, 30 }, // F2 F2
  { 66.8003, 27.4474, 26, 31 }, // C#2 F#2
  { 67.6355, 5.96555, 46, 45 }, // A3 G#3
  { 78.4934, 15.7543, 20, 21 }, // G1 G#1
  { 79.5958, -4.65841, 46, 45 }, // A3 G#3
  { 73.5155, 43.2831, 21, 22 }, // G#1 A1
  { 80.6649, 57.4818, 28, 29 }, // D#2 E2         # 50        E2 (29) x 3
  { 88.3489, 27.748, 19, 20 }, // F#1 G1
  { 94.4627, 43.116, 24, 26 }, // H1 C#2
  { 101.311, 58.0497, 31, 33 }, // F#2 G#2
  { 109.029, 29.1178, 22, 24 }, // A1 H1
  { 100.61, 16.8902, 35, 23 }, // A#2 A#1
  { 90.2198, 4.89646, 43, 35 }, // F#3 A#2
  { 103.75, -3.9234, 44, 43 }, // G3 F#3
  { 113.272, 6.60032, 41, 37 }, // E3 C3
  { 121.557, 20.1642, 37, 25 }, // C3 C2
  { 115.41, 44.2519, 27, 29 }, // D2 E2           # 60
  { 121.958, 59.9874, 34, 36 }, // A2 H2
  { 136.391, 48.1273, 33, 34 }, // G#2 A2
  { 128.84, 33.8284, 25, 27 }, // C2 D2
  { 142.237, 24.2735, 39, 39 }, // D3 D3
  { 134.82, 11.7787, 40, 40 }, // D#3 D#3
  { 126.702, -0.51574, 42, 42 }, // F3 F3
  { 149.487, 38.9733, 29, 32 }, // E2 G2
  { 156.169, 53.5395, 36, 38 }, // H2 C#3
  { 143.206, 62.5933, 38, 41 }, // C#3 E3
  { 161.681, 70.578, 32, 28 } // G2 D#2          # 70
};


#define BIG_SCALE (1.2)
#define SMALL_SCALE (0.8)

/**
 * There are 3 cases when the same note occurs triple times on the manual (1 on the left, 2 on the right)
 * @p NOTE_ID is number of button in @p m_notesArray
 * @p BUTT_ID is number of button in @p m_buttArray
 */
#define E0_BUTT_ID (4)
#define E0_NOTE_ID (16)
#define E2_BUTT_ID (51)
#define E2_NOTE_ID (40)
#define A3_BUTT_ID (47)
#define A3_NOTE_ID (57)


TbandoneonBg::TbandoneonBg(QQuickItem* parent) :
  TcommonInstrument(parent),
  m_currentIndex(-1)
{
  for (int i = 0; i < 71; ++i) {
    if (i < 33) {
        m_notesArray[buttArray[i].open + 11].leftOpen = i + 1;
        m_notesArray[buttArray[i].close + 11].leftClose = i + 1;
    } else {
        m_notesArray[buttArray[i].open + 11].rightOpen = i + 1;
        m_notesArray[buttArray[i].close + 11].rightClose = i + 1;
    }
  }

  QQmlEngine engine;
  QQmlComponent comp(&engine, this);

  comp.setData("import QtQuick 2.9; Rectangle { color: \"blue\"; scale: 1.2 }", QUrl());
  m_circleLeftOpen.item = createCircle(&comp);
  m_circleRightOpen.item = createCircle(&comp);
  comp.setData("import QtQuick 2.9; Rectangle { color: \"#FFA500\"; scale: 1.2 }", QUrl());
  m_circleLeftClose.item = createCircle(&comp);
  m_circleRightClose.item = createCircle(&comp);
  m_circleCloseExtra.item = createCircle(&comp);

//   m_circleLeftClose.item->setObjectName("left_close");
//   m_circleLeftOpen.item->setObjectName("left_open");
//   m_circleRightClose.item->setObjectName("right_close");
//   m_circleRightOpen.item->setObjectName("right_open");

  // FIXME: dirty workaround to set bandoneon in the middle.
  // It happens due to instrument loader sets (forces) bandoneon item width to window width during loading
  // but correct width is obtained from factor
  QTimer::singleShot(50, [=]{
    if (this->parentItem()) {
      auto p = this->parentItem();
      setFactor(qMin(p->height() / 100.0, p->width() / 430.0) * 0.9);
      setFactor(qMin(p->height() / 100.0, p->width() / 430.0));
    }
  });
}


TbandoneonBg::~TbandoneonBg()
{
}


qreal TbandoneonBg::xAt(int b) { return buttArray[b].x; }

qreal TbandoneonBg::yAt(int b) { return buttArray[b].y; }

int TbandoneonBg::openAt(int b) { return buttArray[b].open; }

int TbandoneonBg::closeAt(int b) { return buttArray[b].close; }


void TbandoneonBg::setCurrentIndex(int i) {
CHECKTIME (
  m_currentIndex = i;
  if (m_currentIndex > -1) {
    Tnote n(m_closing ? buttArray[m_currentIndex].close : buttArray[m_currentIndex].open);
    n.setOnUpperStaff(m_currentIndex > 32);
    setNote(n, technical());
    emit noteChanged();
  }
)
}


void TbandoneonBg::setOpening(bool o) {
  if (o != m_opening) {
    m_opening = o;
    emit openingChanged();
    if (p_note.isValid()) {
      if (m_circleLeftClose.buttonId)
        m_circleLeftClose.item->setVisible(!m_opening);
      if (m_circleRightClose.buttonId)
        m_circleRightClose.item->setVisible(!m_opening);

      if (m_circleCloseExtra.buttonId == 4 || m_circleCloseExtra.buttonId == 51)
        m_circleRightClose.item->setVisible(!m_opening);
    }
  }
}


void TbandoneonBg::setClosing(bool c) {
  if (c != m_closing) {
    m_closing = c;
    emit closingChanged();
    if (m_circleLeftOpen.buttonId)
      m_circleLeftOpen.item->setVisible(!m_closing);
    if (m_circleRightOpen.buttonId)
      m_circleRightOpen.item->setVisible(!m_closing);
    // fix closing button scale when it remains alone
    if (m_circleLeftOpen.buttonId == m_circleLeftClose.buttonId)
      m_circleLeftClose.item->setProperty("scale", m_closing ? BIG_SCALE : SMALL_SCALE);
    if (m_circleRightOpen.buttonId == m_circleRightClose.buttonId)
      m_circleRightClose.item->setProperty("scale", m_closing ? BIG_SCALE : SMALL_SCALE);

    if (m_circleCloseExtra.buttonId == 47)
      m_circleCloseExtra.item->setVisible(!m_closing);
  }
}


/**
 * @p Tnote::onUpperStaff() determines on which pane button has to be selected
 * onUpperStaff() == TRUE - right pane, FALSE - left pane
 */
void TbandoneonBg::setNote(const Tnote& n, quint32 noteDataValue) {
  if (!n.isValid() && m_sideHighlight != HighlightNone) {
    m_sideHighlight = HighlightNone;
    emit sideHighlightChanged();
  }
  if (!n.isValid() && !p_note.isValid())
    return;
  if (!n.isValid()) {
    hideCircles();
    setOpening(false);
    setClosing(false);
    m_currentIndex = -1;
    return;
  }
  Ttechnical techn(noteDataValue);
  setOpening(techn.bowing() == Ttechnical::BowDown);
  setClosing(techn.bowing() == Ttechnical::BowUp);
  int chromaticNew = n.chromatic();
  if (chromaticNew < -11 || chromaticNew > 48) {
    setOutOfScale(true);
    p_note.setNote(0);
    hideCircles();
    emit outOfScaleChanged();
    return;
  }
  setOutOfScale(false);
  int chromaticOld = p_note.isValid() ? p_note.chromatic() : 1000;
  if (chromaticNew != chromaticOld || n.onUpperStaff() != p_note.onUpperStaff()) {
    if (chromaticNew == chromaticOld && n.onUpperStaff() != p_note.onUpperStaff())
      hideCircles(); // reset marked buttons when note is the same but moved to upper/lower staff
      // otherwise @p checkCircle condition doesn't occur and buttons remains on unappropriated side
    p_note = n;
    chromaticNew += 11;

    if (m_notesArray[chromaticNew].leftOpen != m_circleLeftOpen.buttonId)
      checkCircle(m_notesArray[chromaticNew].leftOpen, m_circleLeftOpen, !m_closing && !n.onUpperStaff());
    if (m_notesArray[chromaticNew].leftClose != m_circleLeftClose.buttonId)
      checkCircle(m_notesArray[chromaticNew].leftClose, m_circleLeftClose, !m_opening && !n.onUpperStaff());
    qreal scale = m_notesArray[chromaticNew].leftOpen && !m_opening && !m_closing
                && m_notesArray[chromaticNew].leftOpen == m_notesArray[chromaticNew].leftClose ? SMALL_SCALE : BIG_SCALE;
    m_circleLeftClose.item->setProperty("scale", scale);
    if (m_notesArray[chromaticNew].rightOpen != m_circleRightOpen.buttonId)
      checkCircle(m_notesArray[chromaticNew].rightOpen, m_circleRightOpen, !m_closing && n.onUpperStaff());
    if (m_notesArray[chromaticNew].rightClose != m_circleRightClose.buttonId)
      checkCircle(m_notesArray[chromaticNew].rightClose, m_circleRightClose, !m_opening && n.onUpperStaff());
    scale = m_notesArray[chromaticNew].rightOpen && !m_opening && !m_closing
            && m_notesArray[chromaticNew].rightOpen == m_notesArray[chromaticNew].rightClose ? SMALL_SCALE : BIG_SCALE;
    m_circleRightClose.item->setProperty("scale", scale);
    if (chromaticNew == E0_NOTE_ID || chromaticNew == E2_NOTE_ID) { // occur when closing
        if (chromaticNew == E0_NOTE_ID)
          m_circleCloseExtra.buttonId = E0_BUTT_ID;
        else
          m_circleCloseExtra.buttonId = E2_BUTT_ID;
        if (chromaticNew == E0_NOTE_ID)
          checkCircle(m_circleCloseExtra.buttonId, m_circleCloseExtra, !m_opening && !n.onUpperStaff());
        else
          checkCircle(m_circleCloseExtra.buttonId, m_circleCloseExtra, !m_opening && n.onUpperStaff());
        m_circleCloseExtra.item->setProperty("color", QColor(255, 165, 0)); // #FFA500
    } else if (chromaticNew == A3_NOTE_ID) { // occur when opening
        m_circleCloseExtra.buttonId = A3_BUTT_ID;
        checkCircle(m_circleCloseExtra.buttonId, m_circleCloseExtra, !m_closing && n.onUpperStaff());
        m_circleCloseExtra.item->setProperty("color", QColor(0, 0, 255)); // blue
    } else
        m_circleCloseExtra.item->setVisible(false);
  }
}


void TbandoneonBg::askQuestion(const Tnote& n, quint32 noteDataValue) {
  setNote(n, noteDataValue);
}


void TbandoneonBg::highlightAnswer(const Tnote& n, quint32 noteData) {
  Ttechnical techn(noteData);
  setOpening(techn.bowing() == Ttechnical::BowDown);
  setClosing(techn.bowing() == Ttechnical::BowUp);
  if (n.isValid()) {
    m_sideHighlight = n.onUpperStaff() ? HighlightRight : HighlightLeft;
    emit sideHighlightChanged();
  }
}



int TbandoneonBg::technical() {
  Ttechnical nd;
  nd.setBowing(m_opening ? Ttechnical::BowDown : (m_closing ? Ttechnical::BowUp : Ttechnical::BowUndefined));
  return nd.data();
}


void TbandoneonBg::setRightX(qreal rx) {
  if (m_rightX != rx) {
    m_rightX = rx;
    updateCircesPos();
    emit rightXChanged();
  }
}


void TbandoneonBg::setFactor(qreal f) {
  if (f != m_factor) {
    m_factor = f;
    updateCircleSize(m_circleLeftOpen.item);
    updateCircleSize(m_circleLeftClose.item);
    updateCircleSize(m_circleRightOpen.item);
    updateCircleSize(m_circleRightClose.item);
    updateCircleSize(m_circleCloseExtra.item);
    emit factorChanged();
  }
}

void TbandoneonBg::markSelected(const QColor& markColor) {
  int borderWidth = markColor.alpha() ? qRound(height() / 50.0) : 0;
  markBorder(m_circleLeftOpen.item, borderWidth, markColor);
  markBorder(m_circleRightOpen.item, borderWidth, markColor);
  markBorder(m_circleLeftClose.item, borderWidth, markColor);
  markBorder(m_circleRightClose.item, borderWidth, markColor);
  markBorder(m_circleCloseExtra.item, borderWidth, markColor);
}


void TbandoneonBg::showNoteName() {
}


void TbandoneonBg::correct(const Tnote& n, quint32 noteData) {
  if (m_circleLeftOpen.item->isVisible())
    p_wrongItem = m_circleLeftOpen.item;
  else if (m_circleLeftClose.item->isVisible())
    p_wrongItem = m_circleLeftClose.item;
  else if (m_circleRightOpen.item->isVisible())
    p_wrongItem = m_circleRightOpen.item;
  else if (m_circleRightClose.item->isVisible())
    p_wrongItem = m_circleRightClose.item;
  m_goodNote = n;
  m_goodTechn = noteData;

  int goodChromatic = m_goodNote.chromatic() + 11;
  Ttechnical goodTechn(m_goodTechn);
  if (m_notesArray[goodChromatic].leftOpen && goodTechn.bowing() == Ttechnical::BowDown && !n.onUpperStaff()) {
      p_goodItem = m_circleLeftOpen.item;
      m_goodButton = m_notesArray[goodChromatic].leftOpen;
  } else if (m_notesArray[goodChromatic].leftClose && goodTechn.bowing() == Ttechnical::BowUp && !n.onUpperStaff()) {
      p_goodItem = m_circleLeftClose.item;
      m_goodButton = m_notesArray[goodChromatic].leftClose;
  } else if (m_notesArray[goodChromatic].rightOpen && goodTechn.bowing() == Ttechnical::BowDown && n.onUpperStaff()) {
      p_goodItem = m_circleRightOpen.item;
      m_goodButton = m_notesArray[goodChromatic].rightOpen;
  } else if (m_notesArray[goodChromatic].rightClose && goodTechn.bowing() == Ttechnical::BowUp && n.onUpperStaff()) {
      p_goodItem = m_circleRightClose.item;
      m_goodButton = m_notesArray[goodChromatic].rightClose;
  } else {
      p_goodItem = nullptr;
  }

  emit correctInstrument();
}


void TbandoneonBg::applyCorrect() {
  if (p_goodItem && m_goodNote.isValid()) {
    TbandCircle bc;
    bc.buttonId = m_goodButton;
    bc.item = p_goodItem;
    Ttechnical goodTechn(m_goodTechn);
    p_note.setNote(0); // invalidate to stop setting button visible by following methods
    setOpening(goodTechn.bowing() == Ttechnical::BowDown);
    setClosing(goodTechn.bowing() == Ttechnical::BowUp);
    hideCircles();
    if (p_goodItem == m_circleLeftClose.item)
      checkCircle(m_goodButton, m_circleLeftClose);
    else if (p_goodItem == m_circleLeftOpen.item)
      checkCircle(m_goodButton, m_circleLeftOpen);
    else if (p_goodItem == m_circleRightClose.item)
      checkCircle(m_goodButton, m_circleRightClose);
    else if (p_goodItem == m_circleRightOpen.item)
      checkCircle(m_goodButton, m_circleRightOpen);
    p_note = m_goodNote;
    markSelected(GLOB->correctColor());
  }
  if (p_wrongItem) {
    p_wrongItem->setVisible(false);
    p_wrongItem->setScale(BIG_SCALE);
    p_wrongItem->setOpacity(1.0);
  }
}


bool TbandoneonBg::canBeLeftOpen(short chromNoteNr) {
  int nrInArr = chromNoteNr + 11;
  return nrInArr >= 0 && nrInArr < 60 && m_notesArray[nrInArr].leftOpen;
}


bool TbandoneonBg::canBeLeftClose(short chromNoteNr) {
  int nrInArr = chromNoteNr + 11;
  return nrInArr >= 0 && nrInArr < 60 && m_notesArray[nrInArr].leftClose;
}


bool TbandoneonBg::canBeRightOpen(short chromNoteNr) {
  int nrInArr = chromNoteNr + 11;
  return nrInArr >= 0 && nrInArr < 60 && m_notesArray[nrInArr].rightOpen;
}


bool TbandoneonBg::canBeRightClose(short chromNoteNr) {
  int nrInArr = chromNoteNr + 11;
  return nrInArr >= 0 && nrInArr < 60 && m_notesArray[nrInArr].rightClose;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TbandoneonBg::getNote() {
  if (m_currentIndex < 0)
    return;
  p_note.setChromatic(m_closing ? buttArray[m_currentIndex].close : buttArray[m_currentIndex].open);
  p_note.setOnUpperStaff(m_currentIndex > 32);
}


QQuickItem* TbandoneonBg::createCircle(QQmlComponent* comp) {
  auto circle = qobject_cast<QQuickItem*>(comp->create());
  circle->setParentItem(this);
  circle->setVisible(false);
  circle->setZ(10);
  return circle;
}


void TbandoneonBg::updateCircleSize(QQuickItem* it) {
  it->setWidth(height() / 7.5);
  it->setHeight(height() / 7.5);
  it->setProperty("radius", height() / 15.0);
}


void TbandoneonBg::checkCircle(int butNr, TbandCircle& c, bool visible) {
  c.buttonId = butNr;
  if (c.buttonId) {
      c.item->setX(buttArray[c.buttonId - 1].x * m_factor * (butNr > 33 ? 1.2 : 1.0) + (butNr > 33 ? m_rightX : 0.0));
      c.item->setY(buttArray[c.buttonId - 1].y * m_factor + height() * 0.09375);
      c.item->setVisible(visible);
  } else
      c.item->setVisible(false);
}


void TbandoneonBg::updateCircesPos() {
  if (p_note.isValid()) {
    int chromaticNr = p_note.chromatic() + 11;
    checkCircle(m_notesArray[chromaticNr].leftOpen, m_circleLeftOpen, m_circleLeftOpen.item->isVisible());
    checkCircle(m_notesArray[chromaticNr].leftClose, m_circleLeftClose, m_circleLeftClose.item->isVisible());
    checkCircle(m_notesArray[chromaticNr].rightOpen, m_circleRightOpen, m_circleRightOpen.item->isVisible());
    checkCircle(m_notesArray[chromaticNr].rightClose, m_circleRightClose, m_circleRightClose.item->isVisible());
  }
}


void TbandoneonBg::hideCircles() {
  m_circleLeftOpen.item->setVisible(false);
  m_circleLeftOpen.buttonId = 0;
  m_circleLeftClose.item->setVisible(false);
  m_circleLeftClose.buttonId = 0;
  m_circleRightOpen.item->setVisible(false);
  m_circleRightOpen.buttonId = 0;
  m_circleRightClose.item->setVisible(false);
  m_circleRightClose.buttonId = 0;
  m_circleCloseExtra.item->setVisible(false);
  m_circleCloseExtra.buttonId = 0;
}
