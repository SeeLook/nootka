/***************************************************************************
 *   Copyright (C) 2017-2021 by Tomasz Bojczuk                             *
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
#include <QtCore/qtimer.h>
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

  m_flyTimer = new QTimer(this);
  m_flyTimer->setInterval(2000);
  connect(m_flyTimer, &QTimer::timeout, this, [=]{
    m_flyTimer->stop();
    if (!m_showText) {
      m_showText = true;
      emit showTextChanged();
    }
  });

  m_examActions << nullptr << nullptr << nullptr << nullptr;

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


void TmobileMenu::setForceText(bool forceT) {
  if (m_forceText != forceT) {
    m_forceText = forceT;
    emit forceTextChanged();
  }
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TmobileMenu::mousePressEvent(QMouseEvent*) {
  setPressed(true);
}


void TmobileMenu::mouseMoveEvent(QMouseEvent* event) {
  if (event->pos().x() > Tmtr::fingerPixels() * 1.5 || event->pos().y() > Tmtr::fingerPixels() * 1.5) {
      m_showFlys = true;
      emit showFlysChanged();
      setFlyItem(childAt(event->pos().x(), event->pos().y()));
  } else {
      m_showFlys = false;
      emit showFlysChanged();
      setFlyItem(nullptr);
  }
}


void TmobileMenu::mouseReleaseEvent(QMouseEvent* event) {
  setPressed(false);
  m_showFlys = false;
  emit showFlysChanged();
  if (m_currentFlyItem) {
    emit flyClicked();
    setFlyItem(nullptr);
  }
  if (event->pos().x() < width() && event->pos().y() < height())
    emit clicked();
}


void TmobileMenu::init() {
  singleNoteModeSlot();
  connect(GLOB, &Tglobals::singleNoteModeChanged, this, &TmobileMenu::singleNoteModeSlot);
  connect(GLOB, &Tglobals::isExamChanged, this, &TmobileMenu::singleNoteModeSlot);
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


void TmobileMenu::setFlyItem(QQuickItem* fi) {
  if (m_currentFlyItem != fi) {
    m_currentFlyItem = fi;
    emit currentFlyChanged();
  }
}


void TmobileMenu::setPressed(bool pr) {
  if (pr != m_pressed) {
    m_pressed = pr;
    emit pressedChanged();
    if (m_pressed) {
      m_flyTimer->start();
    } else {
      m_flyTimer->stop();
      if (m_showText) {
        m_showText = false;
        emit showTextChanged();
      }
    }
  }
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


void TmobileMenu::setExamMenuEntries(Taction* a1, Taction* a2, Taction* a3, Taction* a4) {
  m_examActions[0] = a1;
  m_examActions[1] = a2;
  m_examActions[2] = a3;
  m_examActions[3] = a4;
  // Emit with delay to allow menu drawer hide.
  // Otherwise user may see the next entries or even tap any by chance
  QTimer::singleShot(500, this, &TmobileMenu::examActionsChanged);
}
