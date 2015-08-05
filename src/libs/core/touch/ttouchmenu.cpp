/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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
#include <QTimer>
#include <QApplication>
#include <QDesktopWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QDebug>


TtouchMenu::TtouchMenu(QWidget *parent) :
  QMenu(parent),
  m_animDuration(320),
  m_animTimer(new QTimer(this))
{
//  setStyleSheet(QString("QMenu::item { height: %1px; width: %2px; margin: 10px; padding: 2px %1px 2px %1px}").
//        arg(Tmtr::fingerPixels() * 0.7).arg(Tmtr::lessScreenSide() / 5));

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
  if (e->type() == QEvent::TouchBegin || e->type() == QEvent::TouchUpdate || e->type() == QEvent::TouchEnd) {
    qDebug() << "Menu got touch";
  }
  return QMenu::event(e);
}


void TtouchMenu::animTimeOut() {
  m_step++;
  if (m_step < m_count)
    move(m_startPos.x() + (m_offset.x() ? m_offset.x() * m_step : 0), m_startPos.y() + (m_offset.y() ? m_offset.y() * m_step : 0));
  else if (m_step == m_count) {
    move(m_endPos);
    update();
  } else
    m_animTimer->stop();
}











