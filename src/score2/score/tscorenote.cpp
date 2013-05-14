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


#include "tscorenote.h"
#include "tscorescene.h"
#include "tscorestaff.h"
#include "tdropshadoweffect.h"
#include <QGraphicsEffect>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QGraphicsView>
#include <QDebug>


/*static*/
QString TscoreNote::getAccid(int accNr) {
    const int accCharTable[6] = { 0xe123, 0xe11a, 0x20, 0xe10e, 0xe125, 0xe116 };
    QString str = QString(QChar(accCharTable[accNr+2]));
    return str;
}


QFont TscoreNote::getAccidFont() {
  qreal fontFactor = 4.5;
  QFont font(QFont("nootka"));
  font.setPointSizeF(fontFactor);
  QFontMetrics fMetr(font);
  qreal fact = font.pointSizeF() / fMetr.boundingRect(QChar(0xe11a)).height();
  font.setPointSizeF(font.pointSizeF() * fact);
  return font;
}


TscoreNote::TscoreNote(TscoreScene* scene, TscoreStaff* staff, int index) :
  TscoreItem(scene),
  m_workPosY(0.0), m_mainPosY(0.0),
  m_curentAccid(0), m_accidental(0),
  m_index(index)
{
  setStaff(staff);
  m_height = staff->height();
  m_workColor = scene->views()[0]->palette().highlight().color();
  m_workColor.setAlpha(200);
  m_mainColor = scene->views()[0]->palette().windowText().color();
  
  for (int i = 0; i < 7; i++) {
      m_upLines[i] = new QGraphicsLineItem();
      m_upLines[i]->hide();
      registryItem(m_upLines[i]);

      m_mainUpLines[i] = new QGraphicsLineItem();
      m_mainUpLines[i]->hide();
      registryItem(m_mainUpLines[i]);
      
      m_upLines[i]->setLine(2.5, 2 * (i + 1), boundingRect().width(), 2 * (i + 1));
      m_mainUpLines[i]->setLine(m_upLines[i]->line());
      if (i < 5) {
          m_downLines[i] = new QGraphicsLineItem();
          m_downLines[i]->hide();
          registryItem(m_downLines[i]);

          m_mainDownLines[i] = new QGraphicsLineItem();
          m_mainDownLines[i]->hide();
          registryItem(m_mainDownLines[i]);
          
          m_downLines[i]->setLine(2.5, 2 * (i + 13), boundingRect().width(), 2 * (i + 13));
          m_mainDownLines[i]->setLine(m_downLines[i]->line());
      }
  }
  
  m_workNote = createNoteHead();
  QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
  blur->setBlurRadius(0.7);
  m_workNote->setGraphicsEffect(blur);
  
  m_mainNote = createNoteHead();
  
  m_mainAccid = new QGraphicsSimpleTextItem();
  registryItem(m_mainAccid);
  
  QGraphicsBlurEffect *accidBlur = new QGraphicsBlurEffect;
  accidBlur->setBlurRadius(blur->blurRadius());
  m_workAccid = new QGraphicsSimpleTextItem();
  m_workAccid->setBrush(QBrush(m_workColor));
  m_workAccid->setGraphicsEffect(accidBlur);
  registryItem(m_workAccid);
  m_workAccid->hide();
  
  QFont font(getAccidFont());
  m_workAccid->setFont(font);
  m_mainAccid->setFont(font);
  
  setColor(m_mainColor);
  setPointedColor(m_workColor);
  m_workNote->setZValue(35);
  m_workAccid->setZValue(m_workNote->zValue());
  m_mainNote->setZValue(34); // under
  m_mainAccid->setZValue(m_mainNote->zValue());
  setAmbitus(34, 5);
  
  
  setStatusTip(tr("Click to sellect a note, use mouse wheel to change accidentals."));
}


TscoreNote::~TscoreNote() {}

//##########################################################################################################
//#################### PUBLIC METHODS    ###################################################################
//##########################################################################################################

void TscoreNote::setColor(QColor color) {
    m_mainColor = color;
    m_mainNote->setPen(QPen(m_mainColor));
    m_mainNote->setBrush(QBrush(m_mainColor, Qt::SolidPattern));
    m_mainAccid->setBrush(QBrush(m_mainColor));
    for (int i = 0; i < 7; i++) {
        m_mainUpLines[i]->setPen(QPen(color));
        if (i < 5) m_mainDownLines[i]->setPen(QPen(color));
    }
//     if (m_strNr)
//         m_strNr->setBrush(QBrush(m_mainColor));
}

void TscoreNote::setPointedColor(QColor color) {
    m_workColor = color;
    m_workNote->setPen(QPen(m_workColor));
    m_workNote->setBrush(QBrush(m_workColor, Qt::SolidPattern));
    m_workAccid->setBrush(QBrush(m_workColor));
    for (int i = 0; i < 7; i++) {
        m_upLines[i]->setPen(QPen(color));
        if (i < 5) m_downLines[i]->setPen(QPen(color));
    }
}


void TscoreNote::moveNote(int pos) {
    m_mainPosY = pos;
    m_mainNote->setPos(3.0, pos);
    m_mainAccid->setPos(0.0, pos - 4.35);

    if (staff()->accidInKeyArray[(39 - pos) % 7]) {
      if ( m_accidental == 0 ) 
        m_mainAccid->setText(getAccid(3)); // TODO animation of neutral
      else

        if (staff()->accidInKeyArray[(39 - pos) % 7] == m_accidental)
          m_mainAccid->setText(getAccid(0)); // TODO animation
        else
          m_mainAccid->setText(getAccid(m_accidental));
    } else
        m_mainAccid->setText(getAccid(m_accidental));
    
    if (!m_mainNote->isVisible()) {
        m_mainNote->show();
        m_mainAccid->show();
    }
//     setStringPos();
    for (int i = 0; i < 7; i++) {
        if (pos < ( 2 * (i + 1)))
            m_mainUpLines[i]->show();
        else m_mainUpLines[i]->hide();
    }
    for (int i = 0; i < 5; i++) {
        if (pos > (2 * (i + 12)))
            m_mainDownLines[i]->show();
      else m_mainDownLines[i]->hide();
    }
// #if defined(Q_OS_MAC) // others Os-es has no problem with this. Mac lives trash.
//     scoreScene()->update();
// #endif
}



void TscoreNote::hideNote() {
    m_mainNote->hide();
    m_mainAccid->hide();
    for (int i=0; i < 7; i++) m_mainUpLines[i]->hide();
    for (int i=0; i < 5; i++) m_mainDownLines[i]->hide();
    m_mainPosY = 0.0;
    m_accidental = 0;
}


void TscoreNote::hideWorkNote() {
    m_workNote->hide();
    m_workAccid->hide();
    for (int i=0; i < 7; i++) m_upLines[i]->hide();
    for (int i=0; i < 5; i++) m_downLines[i]->hide();
    m_workPosY = 0.0;
}


void TscoreNote::markNote(QColor blurColor) {
    if (blurColor == -1) {
      m_mainNote->setPen(Qt::NoPen);
      m_mainNote->setGraphicsEffect(0);
    } else {
      m_mainNote->setPen(QPen(blurColor, 2));
      m_mainNote->setGraphicsEffect(new TdropShadowEffect(blurColor));
    }
}


QRectF TscoreNote::boundingRect() const{
//   if (staff()->kindOfStaff() == TscoreStaff::e_normal)
    return QRectF(0, 0, 7, m_height);
//   else
//     return QRectF(0, 0, 7, 20);
}


void TscoreNote::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
  painter->setBrush(QColor(m_index, m_index, m_index, 50));
  painter->setPen(Qt::NoPen);
  painter->drawRect(boundingRect());
}


//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreNote::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
  if ((event->pos().y() > m_ambitMax) && (event->pos().y() < m_ambitMin)) {
      m_workNote->show();
      m_workAccid->show();
  }
  TscoreItem::hoverEnterEvent(event);
}


void TscoreNote::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
  hideWorkNote();
  TscoreItem::hoverLeaveEvent(event);
}

void TscoreNote::hoverMoveEvent(QGraphicsSceneHoverEvent* event) {
  if ((event->pos().y() > m_ambitMax) && (event->pos().y() < m_ambitMin)) {
    if (event->pos().y() != m_workPosY) {
      m_workPosY = event->pos().y();
      m_workNote->setPos(3.0, m_workPosY);
      m_workAccid->setPos(0.0, m_workPosY - 4.35);
      if (!m_workNote->isVisible()) {
        m_workNote->show();
        m_workAccid->show();
      }
      
      for (int i=0; i < 7; i++) {
        if (m_workPosY < (2 * (i + 1))) m_upLines[i]->show();
        else m_upLines[i]->hide();
      }
      for (int i=0; i < 5; i++) {
        if (m_workPosY > (2 * (i + 12))) m_downLines[i]->show();
        else m_downLines[i]->hide();
      }
    }
  }
}



void TscoreNote::mousePressEvent(QGraphicsSceneMouseEvent* event) {
  if (event->button() == Qt::LeftButton && m_workPosY) {
        m_mainPosY = m_workPosY;
        m_accidental = m_curentAccid;
        moveNote(m_mainPosY);
        emit noteWasClicked(m_index);
    }
}


void TscoreNote::wheelEvent(QGraphicsSceneWheelEvent* event) {
    int prevAcc = m_curentAccid;
    if (event->delta() < 0) {
        m_curentAccid--;
        if (m_curentAccid < (-scoreScene()->doubleAccidsFuse()))
          m_curentAccid = -(scoreScene()->doubleAccidsFuse());
    } else {
        m_curentAccid++;
        if (m_curentAccid > scoreScene()->doubleAccidsFuse())
          m_curentAccid = scoreScene()->doubleAccidsFuse();
    }
    if (prevAcc != m_curentAccid) {
        m_workAccid->setText(getAccid(m_curentAccid));
//         emit accidWasChanged(m_curentAccid);
    }
}

//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################

QGraphicsEllipseItem* TscoreNote::createNoteHead() {
  QGraphicsEllipseItem *noteHead = new QGraphicsEllipseItem();
  registryItem(noteHead);
  noteHead->setRect(0, 0, 3.5, 2);
  noteHead->hide();
  return noteHead;
}


