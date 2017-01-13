/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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

#include "tscorekeysignature.h"
#include <music/tkeysignature.h>
#include "tscorescene.h"
#include "tscorenote.h"
#include "tscorestaff.h"
#include <tnoofont.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qapplication.h>


/*static*/
const qreal TscoreKeySignature::relatedLine = 3.0;

void TscoreKeySignature::setKeyNameScale(QGraphicsTextItem* keyNameItem) {
    qreal factor = (KEY_WIDTH + 5.0) / (keyNameItem->boundingRect().width());
    if (keyNameItem->boundingRect().height() * factor > 8.0) // 8.0 is a measure of height - about 4 staff lines.
        factor = (8.0 / keyNameItem->boundingRect().height());
    keyNameItem->setScale(factor);
}


qint8 TscoreKeySignature::m_posOfAccid[7] = {
    0, // Fes & Fis (F#)
    3, // Ces (C#)
    -1, // Ges (G#)
    2, // Des (D#)
    5, // As  (A#)
    1, // ES  (E#)
    4 // B   (H#)  (Bb - B#) in west
};

qint8 TscoreKeySignature::m_posOfAccidFlats[7] = { 4, 1, 5, 2, 6, 3, 7 };


int nOff(Tclef::EclefType c) {
  if (c == Tclef::Treble_G || c == Tclef::Treble_G_8down)
    return 3;
  if (c == Tclef::Bass_F || c == Tclef::Bass_F_8down)
    return 5;
  if (c == Tclef::Alto_C)
    return 2;
  if (c == Tclef::Tenor_C)
    return 4;
  return 3;
}
/*end static*/


TscoreKeySignature::TscoreKeySignature(TscoreScene* scene, TscoreStaff* staff, qint8 keySign) :
  TscoreItem(scene),
  m_keySignature(keySign),
  m_clef(Tclef()),
  m_readOnly(false),
  m_bgColor(-1),
  m_clefOffset(3),
  m_maxKey(7), m_minKey(-7)
{
  setStaff(staff);
  setParentItem(staff);

  TnooFont font(5);
  for (int i = 0; i < 7; i++) {
      m_accidentals[i] = new QGraphicsSimpleTextItem();
      registryItem(m_accidentals[i]);
      m_accidentals[i]->setBrush(qApp->palette().text().color());
      m_accidentals[i]->setFont(font);
      m_accidentals[i]->setScale(scoreScene()->accidScale());
      m_accidentals[i]->hide();
  }
    
  setStatusTip(tr("Key signature - to change it, click above or below the staff or use mouse wheel."));
}


void TscoreKeySignature::setKeySignature(qint8 keySign) {
  for (int i = 1; i < 8; i++) {
        int base = 0;
        qint8 sign = 1;
        bool isFlat = false;
        int startVal = 48;
        if (keySign < 0) {
            base = 8;
            sign = -1;
            isFlat = true;
            startVal = 38;
        }
        if (i <= qAbs(keySign)) {// show accid
            m_accidentals[i - 1]->setText(TscoreNote::getAccid(sign));
            m_accidentals[i - 1]->setPos( (i - 1) * 1.3, getPosOfAccid(i - 1, isFlat) - scoreScene()->accidYoffset() - 1);
            staff()->accidInKeyArray[(startVal + sign * (i * 4)) % 7] = sign;
            m_accidentals[i - 1]->show();
        }
        else { // hide
            m_accidentals[i - 1]->hide();
            staff()->accidInKeyArray[(startVal + sign * (i * 4)) % 7] = 0;
        }
    }
//       qDebug() << (int)staff()->accidInKeyArray[0] << (int)staff()->accidInKeyArray[1] << 
//       (int)staff()->accidInKeyArray[2] << (int)staff()->accidInKeyArray[3] << 
//       (int)staff()->accidInKeyArray[4] << (int)staff()->accidInKeyArray[5] << (int)staff()->accidInKeyArray[6];
    m_keySignature = keySign;
    updateKeyName();
    if (m_lowKey && m_keySignature != m_lowKey->keySignature())
        m_lowKey->setKeySignature(m_keySignature);
    emit keySignatureChanged();
}


qint8 TscoreKeySignature::getPosOfAccid(int noteNr, bool flatKey) {
  qint8 yPos;
  if (flatKey)
    yPos = m_posOfAccidFlats[noteNr] + relatedLine + (m_clefOffset - 3);
  else {
    yPos = m_posOfAccid[noteNr] + relatedLine + (m_clefOffset - 3);
    if (m_clef.type() == Tclef::Tenor_C && (noteNr == 0 || noteNr == 2))
        yPos += 7;
  }
  return yPos;
}


QPointF TscoreKeySignature::accidTextPos(int noteNr) {
  if (noteNr >= 0 && noteNr < 7)
      return mapToScene(m_accidentals[noteNr]->pos());
  else 
      return QPointF();
}


void TscoreKeySignature::setClef(Tclef clef) {
  if (clef.type() == Tclef::PianoStaffClefs) {
    m_clef = Tclef(Tclef::Treble_G);
    if (!m_lowKey) {
        m_lowKey = new TscoreKeySignature(scoreScene(), staff());
        m_lowKey->setParentItem(this);
//         m_lowKey->setPos(0.0, staff()->lowerLinePos() - 2.0);
        m_lowKey->setPos(0.0, 14.0);
        m_lowKey->setClef(Tclef(Tclef::Bass_F));
        m_lowKey->setZValue(30);
//         m_lowKey->setRelatedLine(2);
//         setRelatedLine(staff()->upperLinePos());
        m_lowKey->setKeySignature(keySignature());
        connect(m_lowKey, SIGNAL(keySignatureChanged()), this, SLOT(onLowKeyChanged()));
    }
  } else {
    m_clef = clef;
//     setRelatedLine(staff()->upperLinePos());
    if (m_lowKey) {
      delete m_lowKey;
    }
  }
  m_clefOffset = nOff(m_clef.type());
  setKeySignature(keySignature());
}


void TscoreKeySignature::showKeyName(bool showIt) {
  if (showIt) {
    if (!m_keyNameText) {
      m_keyNameText = new QGraphicsTextItem();
      registryItem(m_keyNameText);
      m_keyNameText->setZValue(7);
      setKeySignature(keySignature());
    }
  }  else {
      delete m_keyNameText;
      m_keyNameText = 0;
  }
}


QRectF TscoreKeySignature::boundingRect() const{
    return QRectF(0, 0, KEY_WIDTH + 1.0, KEY_HEIGHT);
}


void TscoreKeySignature::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  Q_UNUSED(option)
  Q_UNUSED(widget)
  if (m_bgColor != -1) {
      paintBackground(painter, m_bgColor);
  }
}


void TscoreKeySignature::setMaxKey(int mk) {
  m_maxKey = static_cast<qint8>(qBound(-7, mk, 7));
  if (m_maxKey < m_minKey)
    m_minKey = m_maxKey;
  if (m_keySignature > m_maxKey)
    setKeySignature(m_maxKey);
}


void TscoreKeySignature::setMinKey(int mk) {
  m_minKey = static_cast<qint8>(qBound(-7, mk, 7));
  if (m_minKey > m_maxKey)
    m_maxKey = m_minKey ;
  if (m_keySignature < m_minKey)
    setKeySignature(m_minKey);
}


//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreKeySignature::onLowKeyChanged() {
  setKeySignature(m_lowKey->keySignature());
}



void TscoreKeySignature::increaseKey(int step) {
  qint8 prevKey = m_keySignature;
  if (step == 1) {
      if (m_keySignature < m_maxKey)
        m_keySignature++;
  } else {
      if (m_keySignature > m_minKey)
        m_keySignature--;
  }
  if (m_keySignature != prevKey)
    setKeySignature(m_keySignature);
}


void TscoreKeySignature::updateKeyName() {
  if (m_keyNameText) {
      m_keyNameText->setHtml(TkeySignature::getMajorName(m_keySignature) + QLatin1String("<br>") +
                              TkeySignature::getMinorName(m_keySignature));
      setKeyNameScale(m_keyNameText);
      m_keyNameText->setPos((boundingRect().width() - m_keyNameText->boundingRect().width() * m_keyNameText->scale()) / 2 - 2.5,
            -2.0 - m_keyNameText->boundingRect().height() * m_keyNameText->scale());
      }
}




