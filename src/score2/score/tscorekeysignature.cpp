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

#include "tscorekeysignature.h"
#include "tscorescene.h"
#include "tscorenote.h"
#include <QGraphicsView>
#include <QGraphicsSceneHoverEvent>

char TscoreKeySignature::m_posOfAccid[7] = {
    16, // Fes & Fis (F#)
    19, // Ces (C#)
    15, // Ges (G#)
    18, // Des (D#)
    21, // As  (A#)
    17, // ES  (E#)
    20 // B   (H#)  (Bb - B#) in west
};


TscoreKeySignature::TscoreKeySignature(TscoreScene* scene, char keySign) :
  TscoreItem(scene),
  m_keySignature(keySign),
  m_accInKeyPtr(0)
{
  
  qreal fontFactor = 3.8;
  QFont font(QFont("nootka"));
  font.setPointSizeF(fontFactor);
  QFontMetrics fMetr(font);
  qreal fact = font.pointSizeF() / fMetr.boundingRect(QChar(0xe11a)).height();
  font.setPointSizeF(font.pointSizeF() * fact);
  for (int i = 0; i < 7; i++) {
        m_accidentals[i] = new QGraphicsSimpleTextItem();
        registryItem(m_accidentals[i]);
        m_accidentals[i]->setBrush(scene->views()[0]->palette().windowText().color());
        m_accidentals[i]->setFont(font);
        m_accidentals[i]->hide();
    }
}


TscoreKeySignature::~TscoreKeySignature()
{}


void TscoreKeySignature::setKeySignature(char keySign) {
  for (int i = 1; i < 8; i++) {
        int base = 0;
        char sign = 1;
        if (keySign < 0) {
            base = 8;
            sign = -1;
        }
        if (i <= qAbs(keySign)) {// show accid
            m_accidentals[i - 1]->setText(TscoreNote::getAccid(sign));
            m_accidentals[i - 1]->setPos( i - 1, m_posOfAccid[qAbs(base - i)-1] - 3.5 + 15);
            if (m_accInKeyPtr)
              *(m_accInKeyPtr + (base - i) - 1) = sign;
            m_accidentals[i-1]->show();
        }
        else { // hide
            m_accidentals[i-1]->hide();
            if (m_accInKeyPtr)
                *(m_accInKeyPtr + (base - i) - 1) = 0;
        }
    }
    m_keySignature = keySign;
//     showKeyName();
    emit keySignatureChanged();
}




void TscoreKeySignature::setClef(Tclef clef) {
  if (clef.type() == Tclef::e_treble_G || clef.type() == Tclef::e_treble_G_8down) {
      m_posOfAccid[0] = 0;
      m_posOfAccid[1] = 3;
      m_posOfAccid[2] = -1;
      m_posOfAccid[3] = 2;
      m_posOfAccid[4] = 5;
      m_posOfAccid[5] = 1;
      m_posOfAccid[6] = 4;
  } else if (clef.type() == Tclef::e_bass_F || clef.type() == Tclef::e_bass_F_8down) {
      m_posOfAccid[0] = 2;
      m_posOfAccid[1] = 5;
      m_posOfAccid[2] = 1;
      m_posOfAccid[3] = 4;
      m_posOfAccid[4] = 7;
      m_posOfAccid[5] = 3;
      m_posOfAccid[6] = 6;
  } else if (clef.type() == Tclef::e_alto_C) {
      m_posOfAccid[0] = 3;
      m_posOfAccid[1] = 6;
      m_posOfAccid[2] = 2;
      m_posOfAccid[3] = 5;
      m_posOfAccid[4] = 8;
      m_posOfAccid[5] = 4;
      m_posOfAccid[6] = 5;
  } else if (clef.type() == Tclef::e_tenor_C) {
      m_posOfAccid[0] = 8;
      m_posOfAccid[1] = 2;
      m_posOfAccid[2] = 7;
      m_posOfAccid[3] = 3;
      m_posOfAccid[4] = 6;
      m_posOfAccid[5] = 0;
      m_posOfAccid[6] = 4;
  }
  setKeySignature(keySignature());
//   for (int i = 0; i < 7; i++)
//     m_accidentals[i]->setPos();
}

QRectF TscoreKeySignature::boundingRect() const{
  return QRectF(0, 0, 8, 40);
}


//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreKeySignature::mousePressEvent(QGraphicsSceneMouseEvent* event) {
}


void TscoreKeySignature::wheelEvent(QGraphicsSceneWheelEvent* event) {
  char prevKey = m_keySignature;
    if (event->delta() > 0) {
        if (m_keySignature < 7) m_keySignature++;
    }
    else {
        if (m_keySignature > -7) m_keySignature--;
    }
    if (m_keySignature != prevKey) {
        setKeySignature(m_keySignature);
    }
}



