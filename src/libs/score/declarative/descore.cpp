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


#include "descore.h"
#include "denote.h"
#include "dekeysignature.h"

#include <tscorescene.h>
#include <tscorestaff.h>
#include <tscoreclef.h>
#include <tscorekeysignature.h>
#include <music/tmeter.h>

#include <QtGui/qpainter.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>

#include <QtCore/QDebug>
#include <QtCore/QElapsedTimer>

#define CONCAT_(x,y) x##y
#define CONCAT(x,y) CONCAT_(x,y)
#define CHECKTIME(x)  \
    QElapsedTimer CONCAT(sb_, __LINE__); \
    CONCAT(sb_, __LINE__).start(); \
    x \
    qDebug() << __FUNCTION__ << ":" << __LINE__ << " Elapsed time: " <<  CONCAT(sb_, __LINE__).nsecsElapsed() / 1000 << " μs.";



static int staffNum = 0;


DeScore::DeScore(QQuickItem* parent) :
  QQuickPaintedItem(parent)
{
  setRenderTarget(QQuickPaintedItem::FramebufferObject);
//   setPerformanceHint(QQuickPaintedItem::FastFBOResizing);
  setAntialiasing(true);

  m_scene = new TscoreScene;
  m_staff = new TscoreStaff(m_scene, 0);
  m_staff->setStafNumber(0);
  m_staff->setMeter(Tmeter(Tmeter::e_2_4));
  m_staff->setDisabled(true);

  setAcceptHoverEvents(true);

  staffNum++;
  m_staffNr = staffNum;
}


DeScore::~DeScore()
{
  delete m_scene;
}


Tclef::EclefType DeScore::clef() {  return m_staff->scoreClef()->clef().type(); }


void DeScore::setClef(Tclef::EclefType c) {
  qDebug() << "[DeScore" << m_staffNr << "] set clef" << c;
  m_staff->onClefChanged(Tclef(c));
  update();
  emit clefChanged();
}


qint8 DeScore::keySignature() {
  return m_staff->scoreKey() ? m_staff->scoreKey()->keySignature() : 0;
}


void DeScore::setKeySignature(qint8 k) {
  if (!m_staff->scoreKey())
    m_staff->setEnableKeySign(true);
  if (k != m_staff->scoreKey()->keySignature()) {
    m_staff->scoreKey()->setKeySignature(k);
    update();
  }
}


void DeScore::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (boundingRect().width() > 0 && newGeometry != oldGeometry) {
    qDebug() << "[DeScore" << m_staffNr << "] resize" << oldGeometry << newGeometry << boundingRect();
    m_scene->setSceneRect(0.0, 0.0, boundingRect().width(), boundingRect().height());
    m_staff->setScale(newGeometry.height() / m_staff->height());
    m_staff->setViewWidth((newGeometry.width() - 25) / m_staff->scale());
    m_staff->setPos((m_staff->isPianoStaff() ? 2.1 : 1.0) * m_staff->scale(), 0.0);
    update();
  }
  QQuickItem::geometryChanged(newGeometry, oldGeometry);
}


void DeScore::paint(QPainter* painter) {
CHECKTIME(
//   qDebug() << "[DeScore" << m_staffNr << "] render" << boundingRect();

  if (!painter->paintEngine())
    return;

  m_scene->render(painter, boundingRect(), boundingRect());
)
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################


void DeScore::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value) {
  if (change == QQuickItem::ItemChildAddedChange) {
    auto scoreNote = qobject_cast<DeNote*>(value.item);
    if (scoreNote) { // changed item is a note
        auto ns = m_staff->insertNote(m_staff->count(), *scoreNote->note(), true);
        scoreNote->setNoteSegment(ns);
        update();
    } else {
        auto scoreKey = qobject_cast<DeKeySignature*>(value.item);
        if (scoreKey) {
            if (m_staff->scoreKey()) {
                qDebug() << "[DeScore] Only single key signature is supported!";
                scoreKey->deleteLater();
            } else {
                qDebug() << "[DeScore] adding key signature";
                scoreKey->setScore(this);
                setKeySignature(scoreKey->keySignature());
                m_staff->scoreKey()->showKeyName(true);
                connect(scoreKey, &DeKeySignature::destroyed, [=]{
                    m_staff->setEnableKeySign(false);
                    update();
                });
            }
        }
    }
  }
  QQuickItem::itemChange(change, value);
}


void DeScore::hoverMoveEvent(QHoverEvent* event) {
//   auto it = m_scene->itemAt(event->posF(), QTransform());
//   qDebug() << "[DeScore" << m_staffNr << "] item at" << event->posF() << it;
}







