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
#include <score/tstaffitem.h>
#include <score/tstafflines.h>
#include <score/tmeasureobject.h>

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlcontext.h>
#include <QtQuick/qquickitem.h>
#include <QtCore/qtimer.h>
#include <QtCore/qsettings.h>
#include <string>

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

  m_goodNote = new Tnote();

  m_showNamesAct = new Taction(tr("Show note names"), QString(), this);
  m_showNamesAct->setCheckable(true);
  m_showNamesAct->setChecked(GLOB->namesOnScore());
  m_showNamesAct->setTip(tr("Shows names of all notes on the staff."));

  m_extraAccidsAct = new Taction(tr("Additional accidentals"), QString(), this);
  m_extraAccidsAct->setCheckable(true);
  m_extraAccidsAct->setTip(tr("Shows accidentals from the key signature also next to a note. <b>WARING! It never occurs in real scores - use it only for theoretical purposes.</b>"));
//   m_extraAccidsAct->setChecked(GLOB->????);

  m_zoomOutAct = new Taction(tr("Zoom score out"), QStringLiteral("zoom-out"), this);
  m_zoomInAct = new Taction(tr("Zoom score in"), QStringLiteral("zoom-in"), this);

  m_playAct = new Taction(qTR("TtoolBar", "Play"), QStringLiteral("playMelody"), this);
  m_playAct->setBgColor(QColor(0, 255, 0));
  m_recModeAct = new Taction(qApp->translate("MainScore", "Note by note"), QStringLiteral("record"), this);
  m_recModeAct->setBgColor(Qt::red);
  m_recModeAct->setCheckable(true);
  m_recModeAct->setTip(tr("Notes are written on the score one by one. Either playing, selecting fret or note name adds a new note to the staff automatically."));
  m_openXmlAct = new Taction(qTR("QShortcut", "Open"), QStringLiteral("open"), this);
  connect(m_openXmlAct, &Taction::triggered, this, &TmainScoreObject::openXmlActSlot);

  m_saveXmlAct = new Taction(qTR("QShortcut", "Save"), QStringLiteral("save"), this);
  connect(m_saveXmlAct, &Taction::triggered, this, &TmainScoreObject::getMelodyNameSlot);

  m_randMelodyAct = new Taction(tr("Generate melody"), QStringLiteral("melody"), this);
  m_randMelodyAct->setBgColor(qApp->palette().highlight().color());
  connect(m_randMelodyAct, &Taction::triggered, this, &TmainScoreObject::randMelodySlot);
  m_randMelodyAct->setTip(tr("Generate a melody with random notes."));

  m_melodyActions << m_playAct << m_recModeAct << m_openXmlAct << m_saveXmlAct << m_randMelodyAct;

  m_nextNoteAct = new Taction(tr("Next note"), QString(), this);
  m_prevNoteAct = new Taction(tr("Previous note"), QString(), this);

  m_notesMenuAct = new Taction(tr("notes", "musical notes of course") + QStringLiteral("   ⋮"), QStringLiteral("score"), this);

  QQmlComponent actionsComp(NOO->qmlEngine(), this);
  m_openXmlAct->createQmlShortcut(&actionsComp, "StandardKey.Open; enabled: !GLOB.singleNoteMode && !GLOB.isExam");
  m_saveXmlAct->createQmlShortcut(&actionsComp, "StandardKey.Save; enabled: !GLOB.singleNoteMode && !GLOB.isExam");
  m_zoomOutAct->createQmlShortcut(&actionsComp, "StandardKey.ZoomOut; enabled: !GLOB.singleNoteMode");
  m_zoomInAct->createQmlShortcut(&actionsComp, "StandardKey.ZoomIn; enabled: !GLOB.singleNoteMode");
  m_playAct->createQmlShortcut(&actionsComp, "\" \"; enabled: !GLOB.singleNoteMode && !GLOB.isExam");
  m_recModeAct->createQmlShortcut(&actionsComp, "\"Ctrl+ \"; enabled: !GLOB.singleNoteMode && !GLOB.isExam");
  m_randMelodyAct->createQmlShortcut(&actionsComp, "\"Ctrl+M\"; enabled: !GLOB.singleNoteMode && !GLOB.isExam");
  m_nextNoteAct->createQmlShortcut(&actionsComp, "StandardKey.MoveToNextChar");
  m_prevNoteAct->createQmlShortcut(&actionsComp, "StandardKey.MoveToPreviousChar");

  connect(qApp, &QGuiApplication::paletteChanged, this, &TmainScoreObject::paletteSlot);
  connect(SOUND, &Tsound::playingChanged, this, [=]{
    m_playAct->setIconTag(SOUND->playing() ? QLatin1String("stopMelody") : QLatin1String("playMelody"));
    m_playAct->setText(SOUND->playing() ? qTR("QShortcut", "Stop") : qTR("TtoolBar", "Play"));
  });

  isExamChangedSlot();
}


TmainScoreObject::~TmainScoreObject()
{
  delete m_goodNote;
  m_instance = nullptr;
}


void TmainScoreObject::setScoreObject(TscoreObject* scoreObj) {
  if (m_scoreObj) {
    qDebug() << "[TmainScoreObject] score object was already set. FIX IT!";
    return;
  }
  m_scoreObj = scoreObj;
  m_scoreObj->enableActions();
  connect(m_scoreObj, &TscoreObject::clicked, this, &TmainScoreObject::clicked);
  connect(m_scoreObj, &TscoreObject::readOnlyNoteClicked, this, &TmainScoreObject::readOnlyNoteClicked);
  connect(m_showNamesAct, &Taction::triggered, [=]{
    m_showNamesAct->setChecked(!m_showNamesAct->checked());
    m_scoreObj->setShowNoteNames(m_showNamesAct->checked());
  });
//   connect(m_extraAccidsAct);
  connect(m_playAct, &Taction::triggered, this, &TmainScoreObject::playScoreSlot);
  connect(m_recModeAct, &Taction::triggered, this, [=]{ m_scoreObj->setRecordMode(!m_scoreObj->recordMode()); });
  connect(m_zoomOutAct, &Taction::triggered, this, [=]{ m_scoreObj->setScaleFactor(qMax(0.4, m_scoreObj->scaleFactor() - 0.2)); });
  connect(m_zoomInAct, &Taction::triggered, this, [=]{ m_scoreObj->setScaleFactor(qMin(m_scoreObj->scaleFactor() + 0.2, 1.4)); });
  connect(GLOB, &Tglobals::isExamChanged, this, &TmainScoreObject::isExamChangedSlot);
  connect(m_scoreObj, &TscoreObject::singleNoteChanged, this, &TmainScoreObject::singleModeSlot);
  connect(GLOB, &Tglobals::showEnharmNotesChanged, this, &TmainScoreObject::checkSingleNoteVisibility);
  connect(GLOB, &Tglobals::enableDoubleAccidsChanged, this, &TmainScoreObject::checkSingleNoteVisibility);
  connect(m_scoreObj, &TscoreObject::keySignatureChanged, this, [=]{
    if (GLOB->keySignatureEnabled() && GLOB->showKeyName() && !GLOB->isExam())
      emit keyNameTextChanged();
  });
  connect(m_scoreObj->clearScoreAct(), &Taction::triggered, this, [=]{
    if (!m_scoreObj->singleNote() && !GLOB->isExam())
      SOUND->stopPlaying();
  });
  m_scoreObj->clearScoreAct()->setBgColor(QColor(255, 140, 0)); // orange
#if !defined (Q_OS_ANDROID)
  m_scoreActions.prepend(m_scoreObj->editModeAct());
  m_scoreActions << m_scoreObj->insertNoteAct() << m_scoreObj->deleteNoteAct() << m_scoreObj->clearScoreAct() << m_notesMenuAct;
#else
  m_scoreActions << m_scoreObj->clearScoreAct() << m_randMelodyAct << m_openXmlAct << m_saveXmlAct;
#endif
  m_noteActions << m_scoreObj->riseAct() << m_scoreObj->lowerAct()
                << m_scoreObj->wholeNoteAct() << m_scoreObj->halfNoteAct() << m_scoreObj->quarterNoteAct() << m_scoreObj->eighthNoteAct()
                << m_scoreObj->sixteenthNoteAct() << m_scoreObj->restNoteAct() << m_scoreObj->dotNoteAct() << m_scoreObj->tieAct();

  connect(m_nextNoteAct, &Taction::triggered, this, [=]{
    if (!GLOB->isSingleNote()) {
      auto noteItem = m_scoreObj->selectedItem() ? m_scoreObj->getNext(m_scoreObj->selectedItem()) : m_scoreObj->note(0);
      if (m_scoreObj->readOnly()) {
          if (noteItem)
            emit m_scoreObj->readOnlyNoteClicked(noteItem->index());
      } else
          m_scoreObj->setSelectedItem(noteItem);
    }
  });
  connect(m_prevNoteAct, &Taction::triggered, this, [=]{
    if (!GLOB->isSingleNote()) {
      auto noteItem = m_scoreObj->selectedItem() ? m_scoreObj->getPrev(m_scoreObj->selectedItem()) : m_scoreObj->note(m_scoreObj->notesCount() - 1);
      if (m_scoreObj->readOnly()) {
          if (noteItem)
            emit m_scoreObj->readOnlyNoteClicked(noteItem->index());
      } else
          m_scoreObj->setSelectedItem(noteItem);
    }
  });
  connect(m_scoreObj, &TscoreObject::stavesHeightChanged, this, &TmainScoreObject::checkExtraStaves);
  connect(m_scoreObj, &TscoreObject::meterChanged, this, [=]{ SOUND->setCurrentMeter(m_scoreObj->meterToInt()); });
  SOUND->setCurrentMeter(m_scoreObj->meterToInt());
}


QString TmainScoreObject::keyNameText() const {
  return m_scoreObj ? NOO->majAndMinKeyName(m_scoreObj->keySignature()) : QString();
}


void TmainScoreObject::setMainScoreItem(QQuickItem* msItem) {
  m_mainScoreItem = msItem;
  connect(m_mainScoreItem, &QQuickItem::heightChanged, this, &TmainScoreObject::checkExtraStaves);
}


int TmainScoreObject::notesCount() const {
  return m_scoreObj->notesCount();
}


void TmainScoreObject::setReadOnly(bool ro) {
  if (ro != m_scoreObj->readOnly()) {
    m_scoreObj->setReadOnly(ro);
    m_scoreObj->setAllowAdding(!ro);
    m_notesMenuAct->setEnabled(!ro);
  }
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
    m_scoreObj->note(1)->setTechnical(NO_TECHNICALS);
    m_scoreObj->note(0)->markNoteHead(Qt::transparent);
    m_scoreObj->note(1)->markNoteHead(Qt::transparent);
  }
  showNoteNames(false);
}


void TmainScoreObject::setKeySignatureEnabled(bool enableKey) {
  m_scoreObj->setKeySignatureEnabled(enableKey);
}


void TmainScoreObject::setKeySignature(const TkeySignature& key) {
  m_scoreObj->setKeySignature(static_cast<int>(key.value()));
}

char TmainScoreObject::keySignatureValue() {
  return static_cast<char>(m_scoreObj->keySignature());
}


int TmainScoreObject::clefType() const {
  return static_cast<int>(m_scoreObj->clefType());
}


void TmainScoreObject::setClef(int clefType) {
  m_scoreObj->setClefType(static_cast<Tclef::EclefType>(clefType));
}


int TmainScoreObject::meter() const {
  return m_scoreObj->meterToInt();
}


void TmainScoreObject::setMeter(int meterType) {
  m_scoreObj->setMeter(meterType);
}


Tnote TmainScoreObject::getNote(int id) {
  return m_scoreObj->noteAt(id);
}


int TmainScoreObject::setSelectedItem(int id) {
  auto n = m_scoreObj->note(id);
  int notesSpan = 0;
  m_scoreObj->setSelectedItem(n);
  if (n) {
    notesSpan = 1;
    if (n->note()->isRest()) {
        id++;
        while (id < m_scoreObj->notesCount() && m_scoreObj->noteList().at(id).isRest()) {
          notesSpan++;
          id++;
        }
    } else if (n->note()->rtm.tie() == Trhythm::e_tieStart) {
        id++;
        while (id < m_scoreObj->notesCount() && m_scoreObj->noteList().at(id).rtm.tie() && m_scoreObj->noteList().at(id).rtm.tie() != Trhythm::e_tieStart) {
          notesSpan++;
          id++;
        }
    }
  }
  return notesSpan;
}


void TmainScoreObject::setTechnical(int noteId, quint32 tech) {
  m_scoreObj->setTechnical(noteId, tech);
}


bool TmainScoreObject::selectInReadOnly() const { return m_scoreObj->selectInReadOnly(); }
void TmainScoreObject::setSelectInReadOnly(bool sel) { m_scoreObj->setSelectInReadOnly(sel); }


quint32 TmainScoreObject::technical(int noteId) {
  if (noteId >= 0 && noteId < m_scoreObj->notesCount())
    return m_scoreObj->note(noteId)->technical();
  return 255;
}


void TmainScoreObject::setMelody(Tmelody* mel) {
  m_scoreObj->setMelody(mel);
}


void TmainScoreObject::getMelody(Tmelody* melody) {
  m_scoreObj->getMelody(melody);
}


Taction* TmainScoreObject::clearScoreAct() {
  return m_scoreObj ? m_scoreObj->clearScoreAct() : nullptr;
}


Taction* TmainScoreObject::scoreMenuAct() {
  return NOO->scoreAct();
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


int TmainScoreObject::markNoteHead(const QColor& outColor, int noteNr) {
  int markedCount = 0;
  auto noteItem = m_scoreObj->note(noteNr);
  if (noteItem) {
    noteItem->markNoteHead(outColor);
    markedCount = 1;
    if (noteItem->note()->rtm.tie()) {
        noteNr++;
        while (noteNr < m_scoreObj->notesCount() && m_scoreObj->noteList()[noteNr].rtm.tie() && m_scoreObj->noteList()[noteNr].rtm.tie() != Trhythm::e_tieStart) {
          auto nextNote = m_scoreObj->note(noteNr);
          if (nextNote) {
              nextNote->markNoteHead(outColor);
              noteNr++;
              markedCount++;
          } else
              break;
        }
    } else if (noteItem->note()->isRest()) {
        noteNr++;
        while (noteNr < m_scoreObj->notesCount() && m_scoreObj->noteList()[noteNr].isRest()) {
          auto nextNote = m_scoreObj->note(noteNr);
          if (nextNote) {
              nextNote->markNoteHead(outColor);
              noteNr++;
              markedCount++;
          } else
              break;
        }
    }
  }
  return markedCount;
}


void TmainScoreObject::correctNote(const Tnote& goodNote,  bool corrAccid) {
    if (corrAccid) {
      // FIXME probably it is not necessary - animation is universal for any kind of score mistake
    }
    m_correctNoteId = m_scoreObj->singleNote() ? 0 : (m_scoreObj->selectedItem() ? m_scoreObj->selectedItem()->index() : -1);
    auto noteItem = m_scoreObj->note(m_correctNoteId);
    if (noteItem) {
      if (!m_animationObj) {
        QQmlComponent comp(m_scoreObj->qmlEngine(), QUrl(QStringLiteral("qrc:/exam/CorrectNoteAnim.qml")));
        m_animationObj = qobject_cast<QObject*>(comp.create());
        m_animationObj->setParent(this);
        connect(m_animationObj, SIGNAL(finished()), this, SLOT(correctionFinishedSlot()));
        connect(m_animationObj, SIGNAL(applyCorrect()), this, SLOT(applyCorrectSlot()));
      }
      m_animationObj->setProperty("noteHead", QVariant::fromValue(m_scoreObj->noteHead(m_correctNoteId)));
      m_animationObj->setProperty("endY", noteItem->getHeadY(goodNote) - 15.0);
      m_animationObj->setProperty("running", true);
      *m_goodNote = goodNote;
    }
}


void TmainScoreObject::correctKeySignature(const TkeySignature& keySign) {
  if (m_questionKey) {
    m_questionKey->setProperty("text", keySign.getName()  + QLatin1String("<br>!"));
    m_questionKey->setProperty("color", GLOB->EanswerColor);
  }
  if (keySign != keySignatureValue())
    m_scoreObj->setKeySignature(keySign.value());
}


void TmainScoreObject::saveMusicXml(const QString& fileName, const QString& title, const QString& composer) {
  QString fn = fileName;
  if (fileName.isEmpty()) // ask for file name if this parameter was empty
    fn = NOO->getXmlToSave(composer + QLatin1String(" - ") + title);
  if (!fn.isEmpty()) {
    m_scoreObj->saveMusicXml(fn, title, composer, GLOB->transposition());
    if (!title.isEmpty() && title != qTR("MelodyNameDialog", "Nootka melody")) {
      auto lt = GLOB->config->value(QLatin1String("Melody/recentTitles"), QStringList()).toStringList();
      lt.prepend(title);
      lt.removeDuplicates();
      while  (lt.size() > 10)
        lt.removeLast();
      GLOB->config->setValue(QLatin1String("Melody/recentTitles"), lt);
    }
    if (!composer.isEmpty() && composer != QLatin1String("Nootka The Composer")) {
      auto lc = GLOB->config->value(QLatin1String("Melody/recentComposers"), QStringList()).toStringList();
      lc.prepend(composer);
      lc.removeDuplicates();
      while (lc.size() > 10)
        lc.removeLast();
      GLOB->config->setValue(QLatin1String("Melody/recentComposers"), lc);
    }
  }
}


QStringList TmainScoreObject::recentTitles() const {
  auto lt = GLOB->config->value(QLatin1String("Melody/recentTitles"), QStringList()).toStringList();
  lt.prepend(QGuiApplication::translate("MelodyNameDialog", "Nootka melody"));
  return lt;
}


QStringList TmainScoreObject::recentComposers() const {
  auto lc = GLOB->config->value(QLatin1String("Melody/recentComposers"), QStringList()).toStringList();
  lc.prepend(QLatin1String("Nootka The Composer"));
  return lc;
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmainScoreObject::openXmlActSlot() {
  SOUND->stopListen();
  auto m = new Tmelody();
  m_scoreObj->openMusicXml(NOO->getXmlToOpen(), m, GLOB->instrument().type() != Tinstrument::Bandoneon && !GLOB->instrument().isGuitar());
  delete m;
  SOUND->startListen();
}


void TmainScoreObject::getMelodyNameSlot() {
  emit melodyNameDialog();
}


void TmainScoreObject::randMelodySlot() {
  emit melodyGenerate();
}


void TmainScoreObject::isExamChangedSlot() {
  m_scoreActions.clear();
  if (GLOB->isExam()) {
      m_scoreActions << m_zoomOutAct << m_zoomInAct;
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
      m_scoreActions << m_showNamesAct /*<< m_extraAccidsAct*/ << m_zoomOutAct << m_zoomInAct;
      if (m_questionMark) {
        delete m_questionMark;
        m_questionMark = nullptr;
      }
  }
  if (m_scoreObj) {
#if !defined (Q_OS_ANDROID)
    m_scoreActions.prepend(m_scoreObj->editModeAct());
    m_scoreActions << m_scoreObj->insertNoteAct() << m_scoreObj->deleteNoteAct() << m_scoreObj->clearScoreAct() << m_notesMenuAct;
#else
    m_scoreActions << m_scoreObj->clearScoreAct();
    if (!GLOB->isExam())
      m_scoreActions << m_randMelodyAct << m_openXmlAct << m_saveXmlAct;
#endif
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


void TmainScoreObject::applyCorrectSlot() {
  markNoteHead(GLOB->correctColor(), m_correctNoteId);
}


void TmainScoreObject::correctionFinishedSlot() {
  m_scoreObj->setNote(m_correctNoteId, *m_goodNote);
  m_correctNoteId = -1;
  emit correctionFinished();
}


void TmainScoreObject::playScoreSlot() {
  int countDownDur = 0;
  if (SOUND->tickDuringPlay()) {
    // play from selected note but tick from current bar beginning, so calculate initial duration
    if (m_scoreObj->selectedItem() && m_scoreObj->selectedItem()->index() > 0)
      countDownDur = m_scoreObj->selectedItem()->measure()->durationBefore(m_scoreObj->selectedItem());
  }
  SOUND->playNoteList(m_scoreObj->noteList(), m_scoreObj->selectedItem() ? m_scoreObj->selectedItem()->index() : 0, countDownDur);
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


void TmainScoreObject::checkExtraStaves() {
  if (m_scoreObj == nullptr || m_mainScoreItem == nullptr)
    return;

  auto firstStaff = m_scoreObj->firstStaff();
  int emptyStavesCount = qMax(0, static_cast<int>((m_mainScoreItem->height() - m_scoreObj->stavesHeight()) / (firstStaff->scale() * 16.0)));
  if (m_emptyStaves.count() != emptyStavesCount) {
    if (m_emptyStaves.count() > emptyStavesCount) { // remove some staff lines
        int toRemove = m_emptyStaves.count() - emptyStavesCount;
        for (int s = 0; s < toRemove; ++s)
          delete m_emptyStaves.takeLast();
    } else { // add empty staff lines
        for (int s = m_emptyStaves.count(); s < emptyStavesCount; ++s) {
          auto newEmpty = new TstaffLines(m_mainScoreItem);
          m_emptyStaves << newEmpty;
          newEmpty->setTransformOrigin(QQuickItem::Left);
          newEmpty->setEnabled(false);
        }
    }
  }
  if (emptyStavesCount) {
    auto sc = firstStaff->scale();
    for (int s = 0; s < emptyStavesCount; ++s) {
      auto emptyStaff = m_emptyStaves[s];
      emptyStaff->setScale(sc);
      emptyStaff->setX((m_scoreObj->clefType() == Tclef::PianoStaffClefs ? 3 : 0.5) * sc);
      emptyStaff->setWidth(firstStaff->width() - (m_scoreObj->clefType() == Tclef::PianoStaffClefs ? 3.5 : 1.0));
      emptyStaff->setStaffScale(sc);
      emptyStaff->setY(m_mainScoreItem->height() - sc * 14.0 - (emptyStavesCount - s - 1) * (sc * 16.0));
    }
  }
}
