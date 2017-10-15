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


#include "tmobilemenu.h"
#include "tmtr.h"
#include "taction.h"

#include <QtCore/qmath.h>
#include <QtCore/qdebug.h>


TmobileMenu::TmobileMenu(QQuickItem* parent) :
  QQuickItem(parent)
{
  setAntialiasing(true);
  setAcceptedMouseButtons(Qt::LeftButton);
}


TmobileMenu::~TmobileMenu() {}


void TmobileMenu::addAction(Taction* a) {
  m_flyActions.append(QVariant::fromValue(a));
  emit flyActionsChanged();
}


int TmobileMenu::fingerPixels() const {
  return Tmtr::fingerPixels();
}


qreal TmobileMenu::flyX(int itemNr) {
  if (m_flyActions.size() > 1) {
      qreal angle = qDegreesToRadians(86.0) / (m_flyActions.size() - 1);
      qreal off = qDegreesToRadians(88.0);
      qreal r = Tmtr::fingerPixels() * 5;
      return qSin(off - itemNr * angle) * r;
  } else
      return  Tmtr::fingerPixels() / 10.0;
}

qreal TmobileMenu::flyY(int itemNr) {
  if (m_flyActions.size() > 1) {
      qreal angle = qDegreesToRadians(86.0) / (m_flyActions.size() - 1);
      qreal off = qDegreesToRadians(88.0);
      qreal r = Tmtr::fingerPixels() * 5;
      return qCos(off - itemNr * angle) * r;
  } else
      return Tmtr::fingerPixels() * 5;
}



//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TmobileMenu::mousePressEvent(QMouseEvent*) {
  m_pressed = true;
  emit pressedChanged();
}


void TmobileMenu::mouseMoveEvent(QMouseEvent* event) {
  if (event->pos().x() > Tmtr::fingerPixels() * 1.5 || event->pos().y() > Tmtr::fingerPixels() * 1.5) {
      m_extra = true;
      emit extraChanged();
      auto fly = childAt(event->pos().x(), event->pos().y());
      if (fly != m_currentFlyItem) {
        m_currentFlyItem = fly;
        currentFlyChanged();
      }
  } else {
      m_extra = false;
      emit extraChanged();
      if (m_currentFlyItem) {
        m_currentFlyItem = nullptr;
        emit currentFlyChanged();
      }
  }
}


void TmobileMenu::mouseReleaseEvent(QMouseEvent* event) {
  m_pressed = false;
  emit pressedChanged();
  m_extra = false;
  emit extraChanged();
  if (m_currentFlyItem) {
    emit flyClicked();
    m_currentFlyItem = nullptr;
    emit currentFlyChanged();
  }
  if (event->pos().x() < Tmtr::fingerPixels() && event->pos().y() < Tmtr::fingerPixels())
    emit clicked();
}
