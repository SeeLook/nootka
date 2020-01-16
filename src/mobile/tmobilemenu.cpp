/***************************************************************************
 *   Copyright (C) 2017-2020 by Tomasz Bojczuk                             *
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
#include <tmtr.h>
#include <taction.h>
#include <tglobals.h>
#include <tsound.h>
#include <main/tmainscoreobject.h>

#include <QtGui/qguiapplication.h>
#include <QtCore/qmath.h>
#include <QtCore/qdebug.h>


TmobileMenu* TmobileMenu::m_instance = nullptr;


TmobileMenu::TmobileMenu(QQuickItem* parent) :
  QQuickItem(parent)
{
  if (m_instance)
    qDebug() << "[TmobileMenu] FIXME! Instance already exists";

  m_instance = this;
  setAntialiasing(true);
  setAcceptedMouseButtons(Qt::LeftButton);

  m_pitchDetectAct = new Taction(QGuiApplication::translate("MainMenuMobile", "Pitch recognition",
                                                            "Android menu entry, could be 'Note recognition' or 'detection' as well"),
                                 QStringLiteral("mic"), this);
  m_pitchDetectAct->setBgColor(Qt::blue);

  m_tempoAct = new Taction(tr("Tempo"), QStringLiteral("metronome"), this);
  m_tempoAct->setBgColor(QColor(0, 102, 0)); // dark green

  connect(SOUND, &Tsound::initialized, this, &TmobileMenu::init);
}


TmobileMenu::~TmobileMenu() {
  m_instance = nullptr;
}


int TmobileMenu::fingerPixels() const {
  return Tmtr::fingerPixels();
}


#define FLY_ANGLE (0.375246) //(qDegreesToRadians(86.0) / 4.0)
#define FLY_OFF (1.53589) //(qDegreesToRadians(88.0))

qreal TmobileMenu::flyX(int itemNr) {
  return qSin(FLY_OFF - itemNr * FLY_ANGLE) * static_cast<qreal>(Tmtr::fingerPixels() * 5);
}

qreal TmobileMenu::flyY(int itemNr) {
  return qCos(FLY_OFF - itemNr * FLY_ANGLE) * static_cast<qreal>(Tmtr::fingerPixels() * 5);
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
      m_showFlys = true;
      emit showFlysChanged();
      auto fly = childAt(event->pos().x(), event->pos().y());
      if (fly != m_currentFlyItem) {
        m_currentFlyItem = fly;
        currentFlyChanged();
      }
  } else {
      m_showFlys = false;
      emit showFlysChanged();
      if (m_currentFlyItem) {
        m_currentFlyItem = nullptr;
        emit currentFlyChanged();
      }
  }
}


void TmobileMenu::mouseReleaseEvent(QMouseEvent* event) {
  m_pressed = false;
  emit pressedChanged();
  m_showFlys = false;
  emit showFlysChanged();
  if (m_currentFlyItem) {
    emit flyClicked();
    m_currentFlyItem = nullptr;
    emit currentFlyChanged();
  }
  if (event->pos().x() < Tmtr::fingerPixels() && event->pos().y() < Tmtr::fingerPixels())
    emit clicked();
}


void TmobileMenu::init() {
  singleNoteModeSlot();
  connect(GLOB, &Tglobals::singleNoteModeChanged, this, &TmobileMenu::singleNoteModeSlot);
  connect(m_pitchDetectAct, &Taction::triggered, this, [=] {
    SOUND->setStoppedByUser(!SOUND->stoppedByUser());
    if (SOUND->listening())
      SOUND->stopListen();
    else
      SOUND->startListen();
  });
}


void TmobileMenu::singleNoteModeSlot() {
  if (GLOB->isSingleNote())
    setFlyActions(nullptr, nullptr, nullptr, nullptr, m_pitchDetectAct);
  else
    setFlyActions(MAIN_SCORE->playAct(), MAIN_SCORE->recModeAct(), m_tempoAct,
                  MAIN_SCORE->scoreMenuAct(), m_pitchDetectAct);
}


void TmobileMenu::setFlyActions(Taction* a1, Taction* a2, Taction* a3, Taction* a4, Taction* a5) {
  if (m_1flyAct != a1) {
    m_1flyAct = a1;
    emit fly1actChanged();
  }
  if (m_2flyAct != a2) {
    m_2flyAct = a2;
    emit fly2actChanged();
  }
  if (m_3flyAct != a3) {
    m_3flyAct = a3;
    emit fly3actChanged();
  }
  if (m_4flyAct != a4) {
    m_4flyAct = a4;
    emit fly4actChanged();
  }
  if (m_5flyAct != a5) {
    m_5flyAct = a5;
    emit fly5actChanged();
  }
}
