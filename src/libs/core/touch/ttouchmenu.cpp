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
  setStyleSheet(QString("QMenu::item { height: %1px; width: %2px; margin: 10px; padding: 2px %1px 2px %1px}").
        arg(qApp->desktop()->availableGeometry().height() / 12).
        arg(qApp->desktop()->availableGeometry().width() / 5));
//  m_widget = new QWidget(this);
//   m_layout = new QVBoxLayout;
//   m_layout->setContentsMargins(0, 0, 0, 0);
//   m_widget->setLayout(m_layout);
//   QVBoxLayout *lay = new QVBoxLayout;
//   lay->addWidget(m_widget);
//   lay->setContentsMargins(0, 0, 0, 0);
//   setLayout(lay);
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
//   if (a->isCheckable()) {
//     QCheckBox *checkBox = new QCheckBox(a->text(), this);
//     checkBox->setChecked(a->isChecked());
//     checkBox->setFixedHeight(qApp->desktop()->availableGeometry().height() / 12);
//     m_layout->addWidget(checkBox, 0, Qt::AlignCenter);
//     connect(checkBox, SIGNAL(clicked()), a, SLOT(trigger()));
//   } else {
//     QPushButton *button = new QPushButton(a->icon(), a->text(), this);
//     button->setFixedHeight(qApp->desktop()->availableGeometry().height() / 12);
//     button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
//     m_layout->addWidget(button);
//     connect(button, SIGNAL(clicked()), a, SLOT(trigger()));
//   }
//   resize(m_widget->sizeHint());
}


void TtouchMenu::exec(const QPoint& endPos, const QPoint& startPos) {
  if (startPos.x() == -1 && startPos.y() == -1)
    QMenu::exec(endPos);
  else {
    m_endPos = endPos;
    m_startPos = startPos;
    m_count = m_animDuration / 40; // 40 ms for frame
    m_step = 0;
    m_offset = QPoint((m_endPos.x() - m_startPos.x()) / m_count, (m_endPos.y() - m_startPos.y()) / m_count);
    m_animTimer->start(40);
    QMenu::exec(startPos);
  }
}


void TtouchMenu::showEvent(QShowEvent* e) {
  move(m_startPos);
  QWidget::showEvent(e);
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











