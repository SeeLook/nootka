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

#include "tmainscoreobject.h"
#include <score/tscoreobject.h>
#include <taction.h>
#include <tglobals.h>
#include <qtr.h>
#include <tnootkaqml.h>
#include <tsound.h>
#include <tcolor.h>
#include <music/tkeysignature.h>
#include <music/ttechnical.h>
#include <music/tmelody.h>
#include <score/tnoteitem.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQuick/qquickitem.h>
#include <QtCore/qtimer.h>

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

  m_showNamesAct = new Taction(tr("Show note names"), QString(), this);
  m_showNamesAct->setCheckable(true);
  m_showNamesAct->setChecked(GLOB->namesOnScore());
  m_showNamesAct->setTip(tr("Shows names of all notes on the staff."));

  m_extraAccidsAct = new Taction(tr("Additional accidentals"), QString(), this);
  m_extraAccidsAct->setCheckable(true);
  m_extraAccidsAct->setTip(tr("Shows accidentals from the key signature also next to a note. <b>WARING! It never occurs in real scores - use it only for theoretical purposes.</b>"));
//   m_extraAccidsAct->setChecked(GLOB->????);

  m_deleteLastAct = new Taction(tr("Delete note"), QStringLiteral("delete"), this);
  m_clearScoreAct = new Taction(tr("Delete all notes"), QStringLiteral("clear-score"), this);

  m_zoomOutAct = new Taction(tr("Zoom score out"), QStringLiteral("zoom-out"), this);
  m_zoomInAct = new Taction(tr("Zoom score in"), QStringLiteral("zoom-in"), this);

  m_playAct = new Taction(qTR("QShortcut", "Play"), QStringLiteral("playMelody"), this);
  m_recModeAct = new Taction(QString(), QString(), this);
  m_recModeAct->setTip(tr("Notes are written on the score one by one. Either playing, selecting fret or note name adds a new note to the staff automatically."));
  m_openXmlAct = new Taction(qTR("QShortcut", "Open"), QStringLiteral("open"), this);
  connect(m_openXmlAct, &Taction::triggered, this, &TmainScoreObject::openXmlActSlot);

  m_saveXmlAct = new Taction(qTR("QShortcut", "Save"), QStringLiteral("save"), this);
  connect(m_saveXmlAct, &Taction::triggered, this, &TmainScoreObject::saveXmlActSlot);

  m_randMelodyAct = new Taction(QGuiApplication::translate("TmelMan", "Generate melody"), QStringLiteral("melody"), this);
  connect(m_randMelodyAct, &Taction::triggered, this, &TmainScoreObject::randMelodySlot);
  m_randMelodyAct->setTip(tr("Generate a melody with random notes."));

  m_melodyActions << m_playAct << m_recModeAct << m_openXmlAct << m_saveXmlAct << m_randMelodyAct;

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
  connect(m_scoreObj, &TscoreObject::readOnlyNoteClicked, this, &TmainScoreObject::readOnlyNoteClicked);
  connect(m_showNamesAct, &Taction::triggered, [=]{ m_scoreObj->setShowNoteNames(m_showNamesAct->checked()); });
//   connect(m_extraAccidsAct);
  connect(m_deleteLastAct, &Taction::triggered, [=]{ m_scoreObj->deleteLastNote(); });
  connect(m_clearScoreAct, &Taction::triggered, [=]{ m_scoreObj->clearScore(); });
  connect(m_playAct, &Taction::triggered, SOUND, &Tsound::playScore);
  connect(m_recModeAct, &Taction::triggered, [=]{ m_scoreObj->setRecordMode(!m_scoreObj->recordMode()); });
  connect(m_zoomOutAct, &Taction::triggered, [=]{ m_scoreObj->setScaleFactor(qMax(0.4, m_scoreObj->scaleFactor() - 0.2)); });
  connect(m_zoomInAct, &Taction::triggered, [=]{ m_scoreObj->setScaleFactor(qMin(m_scoreObj->scaleFactor() + 0.2, 1.4)); });
  connect(GLOB, &Tglobals::isExamChanged, this, &TmainScoreObject::isExamChangedSlot);
  connect(m_scoreObj, &TscoreObject::singleNoteChanged, this, &TmainScoreObject::singleModeSlot);
  connect(GLOB, &Tglobals::showEnharmNotesChanged, this, &TmainScoreObject::checkSingleNoteVisibility);
  connect(GLOB, &Tglobals::enableDoubleAccidsChanged, this, &TmainScoreObject::checkSingleNoteVisibility);
  connect(m_scoreObj, &TscoreObject::keySignatureChanged, [=]{
    if (GLOB->keySignatureEnabled() && GLOB->showKeyName() && !GLOB->isExam())
      emit keyNameTextChanged();
  });
}


QString TmainScoreObject::keyNameText() const {
  return m_scoreObj ? NOO->majAndMinKeyName(m_scoreObj->keySignature()) : QString();
}


void TmainScoreObject::setReadOnly(bool ro) {
  m_scoreObj->setReadOnly(ro);
  m_scoreObj->setAllowAdding(!ro);
}


void TmainScoreObject::clearScore() {
  m_scoreObj->clearScore();
  if (m_questionKey) {
    delete m_questionKey;
    m_questionKey = nullptr;
  }
  m_questionMark->setVisible(false);
  m_scoreObj->setBgColor(qApp->palette().base().color());
  if (m_scoreObj->singleNote()) {
    m_scoreObj->note(1)->setTechnical(255);
    m_scoreObj->note(0)->markNoteHead(Qt::transparent);
    m_scoreObj->note(1)->markNoteHead(Qt::transparent);
  }
  showNoteNames(false);
}


void TmainScoreObject::setKeySignature(const TkeySignature& key) { m_scoreObj->setKeySignature(static_cast<int>(key.value())); }

char TmainScoreObject::keySignatureValue() {
  return static_cast<char>(m_scoreObj->keySignature());
}


Tnote TmainScoreObject::getNote(int id) {
  return m_scoreObj->noteAt(id);
}


void TmainScoreObject::setSelectedItem(int id) {
  m_scoreObj->setSelectedItem(m_scoreObj->note(id));
}


void TmainScoreObject::setTechnical(int noteId, quint32 tech) {
  m_scoreObj->setTechnical(noteId, tech);
}


bool TmainScoreObject::selectInReadOnly() const { return m_scoreObj->selectInReadOnly(); }
void TmainScoreObject::setSelectInReadOnly(bool sel) { m_scoreObj->setSelectInReadOnly(sel); }


quint32 TmainScoreObject::technical(int noteId) {
  if (noteId >= 0 && noteId < m_scoreObj->notesCount())
    return m_scoreObj->note(noteId)->technical();
}


void TmainScoreObject::getMelody(Tmelody* melody) {
  m_scoreObj->getMelody(melody);
}


void TmainScoreObject::askQuestion(Tmelody* mel, bool ignoreTechnical, const TkeySignature& melodyKey) {
  m_scoreObj->setBgColor(scoreBackgroundColor(GLOB->EquestionColor, 20));
  int transposition = 0;
  auto tempKey = mel->key();
  if (mel->key() != melodyKey) {
    transposition = mel->key().difference(melodyKey);
    mel->setKey(melodyKey);
  }
  m_scoreObj->setMelody(mel, ignoreTechnical, 0, transposition);
  mel->setKey(tempKey);
  m_scoreObj->setReadOnly(true);
  m_questionMark->setVisible(true);
}


/**
 * We are sure that this kind of questions occurs only in single note mode
 */
void TmainScoreObject::askQuestion(const Tnote& note, quint32 technicalData) {
  m_scoreObj->setBgColor(scoreBackgroundColor(GLOB->EquestionColor, 20));
  m_scoreObj->setNote(m_scoreObj->note(1), note);
  m_questionMark->setVisible(true);
  m_scoreObj->note(1)->setTechnical(technicalData);
}


void TmainScoreObject::askQuestion(const Tnote& note, const TkeySignature& key, quint32 technicalData) {
  setKeySignature(key);
  askQuestion(note, technicalData);
}


void TmainScoreObject::prepareKeyToAnswer(const TkeySignature& fakeKey, const QString& expectKeyName) {
  setKeySignature(fakeKey);
  if (!m_questionKey) {
    auto p = qobject_cast<QQuickItem*>(parent()); // parent: MainScore.qml
    auto nameItem = qvariant_cast<QQuickItem*>(p->property("keyName"));
    m_scoreObj->component()->setData("import QtQuick 2.9; Text { horizontalAlignment: Text.AlignHCenter; font { family: \"Sans\"; pixelSize: 2 }}", QUrl());
    m_questionKey = qobject_cast<QQuickItem*>(m_scoreObj->component()->create());
    if (m_questionKey && nameItem) {
      m_questionKey->setParentItem(nameItem->parentItem());
      m_questionKey->setProperty("text", expectKeyName + QLatin1String("<br>?"));
      m_questionKey->setProperty("color", GLOB->EquestionColor);
      m_questionKey->setX(nameItem->x());
      m_questionKey->setY(nameItem->y());
    }
  }
}


void TmainScoreObject::showNoteNames(bool showName) {
  if (m_scoreObj->singleNote()) {
      m_scoreObj->note(0)->setNoteNameVisible(showName);
//       m_scoreObj->note(1)->setNoteNameVisible(showName);
  } else {
      m_scoreObj->setShowNoteNames(showName);
  }
}


void TmainScoreObject::showNoteName(int noteNr, bool showName) {
  auto note = m_scoreObj->note(noteNr);
  if (note)
    note->setNoteNameVisible(showName);
}


void TmainScoreObject::forceAccidental(int accid) {
  m_scoreObj->setCursorAlter(accid);
}


void TmainScoreObject::unLockScore() {
  m_scoreObj->setBgColor(scoreBackgroundColor(GLOB->EanswerColor, 20));
  setReadOnly(false);
}


void TmainScoreObject::lockKeySignature(bool lock) {
  m_scoreObj->setKeyReadOnly(lock);
}


void TmainScoreObject::markNoteHead(const QColor& outColor, int noteNr) {
  auto note = m_scoreObj->note(noteNr);
  if (note)
    note->markNoteHead(outColor);
}


void TmainScoreObject::correctNote(const Tnote& goodNote, char keySign, bool corrAccid) {
  if (m_scoreObj->singleNote()) {
    m_scoreObj->setNote(0, goodNote);
    if (keySign != keySignatureValue())
      m_scoreObj->setKeySignature(keySign);
    if (corrAccid) {
      // TODO
    }
    markNoteHead(GLOB->correctColor(), 0);
  }
  QTimer::singleShot(1500, [=]{ emit correctionFinished(); }); // Fake so far
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


void TmainScoreObject::randMelodySlot() {
  qDebug() << "[TmainScoreObject] random melody";
}


void TmainScoreObject::isExamChangedSlot() {
  m_scoreActions.clear();
  if (GLOB->isExam()) {
      m_scoreActions << m_zoomOutAct << m_zoomInAct << m_deleteLastAct << m_clearScoreAct;
      if (!m_questionMark) {
        m_scoreObj->component()->setData("import QtQuick 2.9; Text { anchors.centerIn: parent ? parent : undefined; scale: parent ? parent.height / height : 1; text: \"?\"; font { family: \"Nootka\"; pixelSize: 20 }}", QUrl());
        m_questionMark = qobject_cast<QQuickItem*>(m_scoreObj->component()->create());
        if (m_questionMark) {
          m_questionMark->setParentItem(qvariant_cast<QQuickItem*>(qobject_cast<QQuickItem*>(m_scoreObj->parent())->property("bgRect")));
          m_questionMark->setVisible(false);
          paletteSlot();
        }
      }
      singleModeSlot();
  } else {
      m_scoreActions << m_showNamesAct << m_extraAccidsAct << m_zoomOutAct << m_zoomInAct << m_deleteLastAct << m_clearScoreAct;
      if (m_questionMark) {
        delete m_questionMark;
        m_questionMark = nullptr;
      }
  }
  emit scoreActionsChanged();
}


void TmainScoreObject::singleModeSlot() {
  if (GLOB->isSingleNote()) {
    m_scoreObj->setRecordMode(false);
    if (GLOB->isExam()) {
        m_scoreObj->note(1)->setColor(qApp->palette().text().color());
        m_scoreObj->note(2)->setColor(qApp->palette().text().color());
    } else {
        m_scoreObj->note(1)->setColor(GLOB->getEnharmNoteColor());
        m_scoreObj->note(2)->setColor(GLOB->getEnharmNoteColor());
    }
    checkSingleNoteVisibility();
  }
}



void TmainScoreObject::paletteSlot() {
  if (m_questionMark)
    m_questionMark->setProperty("color", scoreBackgroundColor(GLOB->EquestionColor, 40));
}


QColor TmainScoreObject::scoreBackgroundColor(const QColor& c, int alpha) {
  return Tcolor::merge(NOO->alpha(c, alpha), qApp->palette().base().color());
}


void TmainScoreObject::checkSingleNoteVisibility() {
  if (m_scoreObj && m_scoreObj->singleNote()) {
    m_scoreObj->note(1)->setVisible(GLOB->showEnharmNotes() || GLOB->isExam());
    m_scoreObj->note(2)->setVisible(!GLOB->isExam() && GLOB->showEnharmNotes() && GLOB->enableDoubleAccids());
    m_scoreObj->setNote(0, m_scoreObj->noteAt(0)); // refresh
  }
}
