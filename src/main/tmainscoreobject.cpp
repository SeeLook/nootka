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

#include "tmainscoreobject.h"
#include <score/tscoreobject.h>
#include <taction.h>
#include <tglobals.h>
#include <qtr.h>
#include <tnootkaqml.h>
#include <tsound.h>
#include <tcolor.h>
#include <music/tkeysignature.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQuick/qquickitem.h>
#include <QtCore/qdebug.h>


TmainScoreObject* TmainScoreObject::m_instance = nullptr;


TmainScoreObject::TmainScoreObject(QObject* parent) :
  QObject(parent)
{
  if (m_instance) {
    qDebug() << "TmainScoreObject instance already exists!";
    return;
  }
  m_instance = this;

  m_playAct = new Taction(qTR("QShortcut", "Play"), QStringLiteral("playMelody"), this);
  m_recModeAct = new Taction(QString(), QString(), this);
  m_showNamesAct = new Taction(tr("Show note names"), QString(), this);
  m_showNamesAct->setCheckable(true);
  m_showNamesAct->setChecked(GLOB->namesOnScore());

  m_extraAccidsAct = new Taction(tr("Additional accidentals"), QString(), this);
  m_extraAccidsAct->setCheckable(true);
//   m_showNamesAct->setChecked(GLOB->????);

  m_deleteLastAct = new Taction(tr("Delete note"), QStringLiteral("delete"), this);
  m_clearScoreAct = new Taction(tr("Delete all notes"), QStringLiteral("clear-score"), this);

  m_zoomOutAct = new Taction(tr("Zoom score out"), QStringLiteral("zoom-out"), this);
  m_zoomInAct = new Taction(tr("Zoom score in"), QStringLiteral("zoom-in"), this);

  m_openXmlAct = new Taction(qTR("QShortcut", "Open"), QStringLiteral("open"), this);
  connect(m_openXmlAct, &Taction::triggered, this, &TmainScoreObject::openXmlActSlot);

  m_saveXmlAct = new Taction(qTR("QShortcut", "Save"), QStringLiteral("save"), this);
  connect(m_saveXmlAct, &Taction::triggered, this, &TmainScoreObject::saveXmlActSlot);

  connect(qApp, &QGuiApplication::paletteChanged, this, &TmainScoreObject::paletteSlot);

  isExamChangedSlot();
}


TmainScoreObject::~TmainScoreObject()
{
  m_instance = nullptr;
}


void TmainScoreObject::setScoreObject(TscoreObject* scoreObj) {
  if (m_scoreObj) {
    qDebug() << "[TmainScoreObject] score object was already set. FIX IT!";
    return;
  }
  m_scoreObj = scoreObj;
  connect(m_scoreObj, &TscoreObject::clicked, this, &TmainScoreObject::clicked);
  connect(m_showNamesAct, &Taction::triggered, [=]{ m_scoreObj->setShowNoteNames(m_showNamesAct->checked()); });
//   connect(m_extraAccidsAct);
  connect(m_deleteLastAct, &Taction::triggered, [=]{ m_scoreObj->deleteLastNote(); });
  connect(m_clearScoreAct, &Taction::triggered, [=]{ m_scoreObj->clearScore(); });
  connect(m_playAct, &Taction::triggered, SOUND, &Tsound::playScore);
  connect(m_recModeAct, &Taction::triggered, [=]{ m_scoreObj->setRecordMode(!m_scoreObj->recordMode()); });
  connect(m_zoomOutAct, &Taction::triggered, [=]{ m_scoreObj->setScaleFactor(qMax(0.4, m_scoreObj->scaleFactor() - 0.2)); });
  connect(m_zoomInAct, &Taction::triggered, [=]{ m_scoreObj->setScaleFactor(qMin(m_scoreObj->scaleFactor() + 0.2, 1.4)); });
  connect(GLOB, &Tglobals::isExamChanged, this, &TmainScoreObject::isExamChangedSlot);
}


void TmainScoreObject::setReadOnly(bool ro) { m_scoreObj->setReadOnly(ro); }

void TmainScoreObject::clearScore() { m_scoreObj->clearScore(); }


void TmainScoreObject::askQuestion(Tmelody* mel) {
  m_scoreObj->setBgColor(Tcolor::merge(Tcolor::alpha(GLOB->EquestionColor, 20), qApp->palette().base().color()));
  m_scoreObj->setMelody(mel);
  m_scoreObj->setReadOnly(true);
  m_questionMark->setVisible(true);
}


void TmainScoreObject::askQuestion(const Tnote& note, char realStr) {
  m_scoreObj->setBgColor(Tcolor::merge(Tcolor::alpha(GLOB->EquestionColor, 20), qApp->palette().base().color()));
  m_scoreObj->setNote(m_scoreObj->note(0), note);
  m_questionMark->setVisible(true);
}


void TmainScoreObject::askQuestion(const Tnote& note, const TkeySignature& key, char realStr) {
  m_scoreObj->setKeySignature(static_cast<int>(key.value()));
  askQuestion(note, realStr);
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmainScoreObject::openXmlActSlot() {
  SOUND->stopListen();
  m_scoreObj->openMusicXml(NOO->getXmlToOpen());
  SOUND->startListen();
}


void TmainScoreObject::saveXmlActSlot() {
  SOUND->stopListen();
  m_scoreObj->saveMusicXml(NOO->getXmlToSave());
  SOUND->startListen();
}


void TmainScoreObject::isExamChangedSlot() {
  m_scoreActions.clear();
  if (GLOB->isExam()) {
      m_scoreActions << m_zoomOutAct << m_zoomInAct << m_deleteLastAct << m_clearScoreAct;
      if (!m_questionMark) {
        m_scoreObj->component()->setData("import QtQuick 2.9; Text { anchors.centerIn: parent ? parent : undefined; scale: parent ? parent.height / height : 1; text: \"?\"; font { family: \"Nootka\"; pixelSize: 20 }}",
                     QUrl());
        m_questionMark = qobject_cast<QQuickItem*>(m_scoreObj->component()->create());
        if (m_questionMark) {
          m_questionMark->setParentItem(qvariant_cast<QQuickItem*>(qobject_cast<QQuickItem*>(m_scoreObj->parent())->property("bgRect")));
          m_questionMark->setVisible(false);
          paletteSlot();
        }
      }
  } else {
      m_scoreActions << m_playAct << m_recModeAct << m_openXmlAct << m_saveXmlAct << m_showNamesAct << m_extraAccidsAct
                    << m_zoomOutAct << m_zoomInAct << m_deleteLastAct << m_clearScoreAct;
      if (m_questionMark) {
        delete m_questionMark;
        m_questionMark = nullptr;
      }
  }
  emit scoreActionsChanged();
}


void TmainScoreObject::paletteSlot() {
  if (m_questionMark)
    m_questionMark->setProperty("color", Tcolor::merge(NOO->alpha(GLOB->wrongColor(), 40), qApp->palette().base().color()));
}
