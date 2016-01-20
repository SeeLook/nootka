/***************************************************************************
 *   Copyright (C) 2015-2016 by Tomasz Bojczuk                             *
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

#include "ttouchmenu.h"
#include <tmtr.h>
#include <QtCore/qtimer.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qdesktopwidget.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qcheckbox.h>
#include <QtGui/qevent.h>

#include <QtCore/qdebug.h>


TtouchMenu::TtouchMenu(QWidget *parent) :
  QMenu(parent),
  m_animDuration(200),
  m_animTimer(new QTimer(this)),
  m_vertical(false),
  m_horizontal(false),
  m_swiped(false)
{
  connect(m_animTimer, SIGNAL(timeout()), this, SLOT(animTimeOut()));
}


TtouchMenu::~TtouchMenu()
{
  m_animTimer->stop();
}


void TtouchMenu::addAction(QAction* a) {
  if (actions().size()) // Add additional separators when more actions
    addSeparator();
  QMenu::addAction(a);
}


QAction* TtouchMenu::exec(const QPoint& endPos, const QPoint& startPos) {
  if (startPos.x() == -1 && startPos.y() == -1)
    return QMenu::exec(endPos);
  else {
    m_endPos = endPos;
    m_startPos = startPos;
    m_horizontal = m_endPos.x() != m_startPos.x();
    m_vertical = m_endPos.y() != m_startPos.y();
    m_count = m_animDuration / 40; // 40 ms for frame
    m_step = 0;
    m_offset = QPoint((m_endPos.x() - m_startPos.x()) / m_count, (m_endPos.y() - m_startPos.y()) / m_count);
    m_animTimer->start(40);
    return QMenu::exec(startPos);
  }
}


void TtouchMenu::showEvent(QShowEvent* e) {
  move(m_startPos);
  QWidget::showEvent(e);
}


bool TtouchMenu::event(QEvent *e) {
/*  if (e->type() == QEvent::MouseButtonPress || e->type() == QEvent::MouseMove || e->type() == QEvent::MouseButtonRelease) {
    auto me = static_cast<QMouseEvent*>(e);
    if (geometry().contains(me->pos())) {
      if (me->type() == QEvent::MouseButtonPress) {
          m_touchStartPos = me->pos();
          qDebug() << "Menu pressed" << e->isAccepted();
      } else if (me->type() == QEvent::MouseMove) {
          QPoint newPos(pos());
          if (m_horizontal) {
            if (!m_swiped && qAbs<int>(m_touchStartPos.x() - me->pos().x()) > Tmtr::fingerPixels() / 3) { // if finger moved enough
              m_swiped = true; // swipe of menu started
              m_touchStartPos = me->pos(); // reset initial position
            }
            if (m_swiped) // move menu horizontally
              if (m_touchStartPos.x() - me->pos().x() > 5)
                newPos.setX(qBound<int>(m_startPos.x(), m_endPos.x() - (m_touchStartPos.x() - me->pos().x()), m_endPos.x()));
          }
          if (m_vertical) {
            if (!m_swiped && qAbs<int>(m_touchStartPos.y() - me->pos().y()) > Tmtr::fingerPixels() / 3) { // if finger moved enough
              m_swiped = true; // swipe of menu started
              m_touchStartPos = me->pos(); // reset initial position
            }
            if (m_swiped) // move menu vertically
              newPos.setY(qBound<int>(m_startPos.y(), m_endPos.y() - (m_touchStartPos.y() - me->pos().y()), m_endPos.y()));
          }
          if (m_swiped)
            move(newPos);
      } else { // release
          qDebug() << "Menu released" << e->isAccepted();
          if (m_swiped) {
            bool hideMenu = false;
            QLine distance(pos(), m_endPos);
            // check, hide menu or back to position
            if (m_horizontal)
                hideMenu = distance.dx() > Tmtr::fingerPixels() * 2;
            if (m_vertical)
                hideMenu = distance.dy() > Tmtr::fingerPixels() * 2;
            m_swiped = false;
            if (!hideMenu) {
              qDebug() << "back to pos" << m_step;
              move(m_endPos);
              return true;
            } else
                qDebug() << "Menu moved much - hide";
          } else {
            if (QLineF(QLine(m_touchStartPos, me->pos())).length() >= Tmtr::fingerPixels() / 3) // ignore touch when finger moved much
              return false;
          }
      }
    }
  }*/
  return QMenu::event(e);
}


void TtouchMenu::animTimeOut() {
  m_step++;
  if (m_step < m_count) {
    move(m_startPos.x() + (m_offset.x() ? m_offset.x() * m_step : 0), m_startPos.y() + (m_offset.y() ? m_offset.y() * m_step : 0));
    update();
  } else if (m_step == m_count) {
    move(m_endPos);
    update();
  } else
    m_animTimer->stop();
}











