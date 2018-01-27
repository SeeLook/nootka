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

#include "taddnoteitem.h"
#include "tscoreobject.h"
#include "tnoteitem.h"

#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>


TaddNoteItem::TaddNoteItem(QQuickItem* parent) :
  QQuickItem(parent),
  m_yPos(0.0)
{
  m_hideTimer = new QTimer(this);
  m_hideTimer->setSingleShot(true);
  connect(m_hideTimer, &QTimer::timeout, [=]{
      if (m_active) {
        m_active = false;
        emit activeChanged();
      }
      m_yPos = 0.0;
      emit yPosChanged();
  });

  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
}


TaddNoteItem::~TaddNoteItem() {}


void TaddNoteItem::setScoreObject(TscoreObject* sc) {
  if (sc != m_scoreObject) {
    m_scoreObject = sc;
  }
}


void TaddNoteItem::hoverEnterEvent(QHoverEvent* event) {
  m_hideTimer->stop();
  m_active = true;
  m_hovered = true;
  if (m_scoreObject->isPianoStaff() && event->pos().y() >= m_scoreObject->upperLine() + 10.6 && event->pos().y() <= m_scoreObject->upperLine() + 11.6)
      m_yPos = 0.0;
  else {
      int yy = qFloor(event->pos().y());
      if (yy != static_cast<int>(m_yPos))
        m_yPos = m_scoreObject->clefType() == Tclef::NoClef ? m_scoreObject->upperLine() + 7.0 : qFloor(event->pos().y());
  }
  emit activeChanged();
  emit yPosChanged();
}


void TaddNoteItem::hoverLeaveEvent(QHoverEvent*) {
  m_hideTimer->start(500);
  m_hovered = false;
}


void TaddNoteItem::hoverMoveEvent(QHoverEvent* event) {
  if (m_scoreObject->isPianoStaff() && event->pos().y() >= m_scoreObject->upperLine() + 10.6 && event->pos().y() <= m_scoreObject->upperLine() + 11.6)
    return;

  int yy = qFloor(event->pos().y());
  if (yy != static_cast<int>(m_yPos)) {
    m_yPos = m_scoreObject->clefType() == Tclef::NoClef ? m_scoreObject->upperLine() + 7.0 : qFloor(event->pos().y());
    emit yPosChanged();
  }
}


void TaddNoteItem::mousePressEvent(QMouseEvent*) {
  m_hideTimer->stop();
  setKeepMouseGrab(true);
  if (!m_hovered) {
    m_active = true;
    emit activeChanged();
    m_scoreObject->setTouched(true);
    m_touchElapsed.restart();
  }
}


void TaddNoteItem::mouseReleaseEvent(QMouseEvent*) {
  if (keepMouseGrab())
    setKeepMouseGrab(false);
  if (m_active) {
    if (m_hovered) { // mouse
        addNote();
    } else { // touch
        if (m_touchElapsed.elapsed() < 190) {
          if (m_yPos > 0.0)
            addNote();
        }
        m_hideTimer->stop();
        m_hideTimer->start(2500);
        m_scoreObject->setTouched(false);
    }
  }
}


/**
 * Bound Y coordinate [0 - 46] FIXME: It seems to be Qt bug
 */
void TaddNoteItem::mouseMoveEvent(QMouseEvent* event) {
  int yy = qFloor(event->pos().y());
  if (m_touchElapsed.elapsed() > 200 && yy > 0.0 && yy < 46.0 && static_cast<int>(m_yPos) != yy) {
    m_yPos = yy;
    emit yPosChanged();
  }
}


void TaddNoteItem::addNote() {
  m_scoreObject->addNote(m_scoreObject->posToNote(m_yPos), true);
  if (m_scoreObject->recordMode())
    m_scoreObject->setSelectedItem(nullptr);
}
