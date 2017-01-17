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
#include "destaff.h"
#include "denote.h"
#include "dekeysignature.h"

#include <tscorestaff.h>
#include <tscoreclef.h>
#include <tscoremeter.h>
#include <tscorekeysignature.h>
#include <tscorenote.h>
#include <music/tmeter.h>

#include <QtGui/qpainter.h>
#include <QtWidgets/qapplication.h>
#include <QtGui/qpalette.h>

#include "checktime.h"



/**
 * @p parent() is Flickable and @p parentItem() is Flickable.contentItem
 */
DeScore::DeScore(QQuickItem* parent) :
  QQuickItem(parent),
  m_flickContent(nullptr)
{
  setAcceptHoverEvents(true);
  m_staves << new DeStaff(this, 0, this);
}


DeScore::~DeScore()
{
  int stavesCount = m_staves.count();
  for (int s = 0; s < stavesCount; ++s)
    delete m_staves.takeLast();
}


Tclef::EclefType DeScore::clef() {  return staff(0)->clef(); }


void DeScore::setClef(Tclef::EclefType c) {
  for (int s = 0; s < m_staves.count(); ++s) {
    QSignalBlocker(staff(s));
    staff(s)->onClefChanged(Tclef(c));
    m_staves[s]->update(); //TODO call update only when visible
  }
  emit clefChanged();
}


Tmeter::Emeter DeScore::meter() { return Tmeter::NoMeter; } // TODO !!!!


void DeScore::setMeter(Tmeter::Emeter m) {
  if (staff(0)->count()) {
    qDebug() << "[DeScore] Changing a meter when notes are on the score is not supported yet!";
    return;
  }
  staff(0)->setMeter(m);
  // TODO No need for update staff here AS LONG AS changing meter is not supported
}



qint8 DeScore::keySignature() {
  return staff(0)->scoreKey() ? staff(0)->keySignature() : 0;
}


void DeScore::setKeySignature(qint8 k) {
  if (!staff(0)->scoreKey()) {
    for (int s = 0; s < m_staves.count(); ++s) {
      staff(s)->setEnableKeySign(true);
    }
  }
  for (int s = 0; s < m_staves.count(); ++s) {
    if (k != staff(0)->keySignature()) {
      staff(s)->scoreKey()->setKeySignature(k);
      staff(0)->update(); //TODO call update only when visible
    }
  }
}


void DeScore::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) {
  if (boundingRect().width() > 0 && newGeometry != oldGeometry) {
    QTextStream o(stdout);
    o << "\033[01;31m[DeScore]\033[01;00m " << newGeometry.width() << "x" << newGeometry.height() << "\n";
    if (parentItem() && boundingRect().height() > 0) {
      auto p = qobject_cast<QQuickItem*>(parent());
      parentItem()->setWidth(boundingRect().width());
      parentItem()->setHeight(p->height() * m_staves.count());
      for (int s = 0; s < m_staves.count(); ++s) {
        m_staves[s]->setWidth(boundingRect().width());
        m_staves[s]->setHeight(p->height());
        m_staves[s]->setY(s * p->height() * 0.7);
      }
    }
  }
  QQuickItem::geometryChanged(newGeometry, oldGeometry);
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void DeScore::componentComplete() {
  QQuickItem::componentComplete();
  if (!m_flickContent) {
    m_flickContent = qobject_cast<QQuickItem*>(parent());
    connect(m_flickContent, &QQuickItem::widthChanged, [=]{ setWidth(m_flickContent->width()); });
    connect(m_flickContent, &QQuickItem::heightChanged, [=]{ setHeight(m_flickContent->height() * m_staves.count()); });
    setWidth(m_flickContent->width());
    setHeight(m_flickContent->height() * m_staves.count());
  }
}


void DeScore::deleteKeySlot() {
  for (int s = 0; s < m_staves.count(); ++s) {
    staff(s)->setEnableKeySign(false);
    staff(s)->update();
  }
}


void DeScore::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData& value) {
  if (change == QQuickItem::ItemChildAddedChange) {
    auto scoreNote = qobject_cast<DeNote*>(value.item);
    if (scoreNote) { // changed item is a note
        auto ns = staff(0)->insertNote(staff(0)->count(), *scoreNote->note(), true);
        scoreNote->setNoteSegment(ns);
        staff(0)->update();
        connect(scoreNote, &DeNote::updateNote, [=]{
            m_staves[scoreNote->noteSegment()->staff()->number()]->update();
        });
    } else {
        auto scoreKey = qobject_cast<DeKeySignature*>(value.item);
        if (scoreKey) {
            if (staff(0)->scoreKey()) {
                qDebug() << "[DeScore] Only single key signature is supported!";
                scoreKey->deleteLater();
            } else {
                scoreKey->setScore(this);
                setKeySignature(scoreKey->keySignature());
                staff(0)->scoreKey()->showKeyName(true);
                connect(scoreKey, &DeKeySignature::destroyed, this, &DeScore::deleteKeySlot);
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







