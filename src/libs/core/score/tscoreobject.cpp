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

#include "tscoreobject.h"
#include "tstaffitem.h"
#include "tmeasureobject.h"
#include "tnoteitem.h"
#include "tbeamobject.h"
#include "tnotepair.h"
#include "music/tmeter.h"
#include "music/tmelody.h"
#include "taction.h"

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtQml/qqmlengine.h>
#include <QtQml/qqmlcontext.h>
#include <QtCore/qtimer.h>

#include <QtCore/qdebug.h>
#include "checktime.h"


#define WIDTH_CHANGE_DELAY (50) // when score width changes, give 50 ms before staves will be resized


TscoreObject::TscoreObject(QObject* parent) :
  QObject(parent),
  m_keySignEnabled(false),
  m_showExtraAccids(false),
  m_remindAccids(false),
  m_enableDoubleAccids(false),
  m_showNoteNames(false),
  m_clefOffset(TclefOffset(3, 2)),
  m_width(0.0), m_adjustInProgress(false),
  m_nameStyle(static_cast<int>(Tnote::defaultStyle)),
  m_workRhythm(new Trhythm()) // quarter by default
{
  m_meter = new Tmeter();
  setMeter(4); // Tmeter::Meter_4_4
  m_measures << getMeasure(0); // new TmeasureObject(0, this);

  m_widthTimer = new QTimer(this);
  m_widthTimer->setSingleShot(true);
  connect(m_widthTimer, &QTimer::timeout, this, &TscoreObject::adjustScoreWidthSlot);

  for (int i = 0; i < 7; i++) // reset accidentals array
    m_accidInKeyArray[i] = 0;

  m_touchHideTimer = new QTimer(this);
  connect(m_touchHideTimer, &QTimer::timeout, [=]{
      changeActiveNote(nullptr);
      setPressedNote(nullptr);
      m_touchHideTimer->stop();
  });
  m_enterTimer = new QTimer(this);
  m_enterTimer->setSingleShot(true);
  connect(m_enterTimer, &QTimer::timeout, this, &TscoreObject::enterTimeElapsed);
  m_leaveTimer = new QTimer(this);
  m_leaveTimer->setSingleShot(true);
  connect(m_leaveTimer, &QTimer::timeout, this, &TscoreObject::leaveTimeElapsed);
  m_bgColor = qApp->palette().base().color();
}


TscoreObject::~TscoreObject()
{
  delete m_meter;
  delete m_qmlComponent;
  delete m_workRhythm;
  qDeleteAll(m_segments);
  qDeleteAll(m_spareSegments);
}

//#################################################################################################
//###################          Musical parameters      ############################################
//#################################################################################################

void TscoreObject::setClefType(Tclef::EclefType ct) {
  if (m_clefType != ct) {
    auto oldClef = m_clefType;
    m_clefType = ct;
    updateClefOffset();
    emit clefTypeChanged();

    if (notesCount() > 0) {
        bool pianoChanged = (oldClef == Tclef::PianoStaffClefs && m_clefType != Tclef::PianoStaffClefs)
                            || (oldClef != Tclef::PianoStaffClefs && m_clefType == Tclef::PianoStaffClefs);
        for (int n = 0; n < notesCount(); ++n) {
          auto noteSeg = m_segments[n];
          if (pianoChanged)
            noteSeg->item()->setHeight(m_clefType == Tclef::PianoStaffClefs ? 49.0 : 38.0);
          if (m_clefType == Tclef::NoClef) {
              Tnote newNote(Tnote(), noteSeg->note()->rtm);
              newNote.rtm.setStemDown(false);
              noteSeg->item()->setStemHeight(STEM_HEIGHT);
              noteSeg->setNote(newNote);
          } else {
              Tnote newNote(*noteSeg->note());
              if (oldClef == Tclef::NoClef) {
                  int globalNr = m_clefOffset.octave * 7 - (7 - m_clefOffset.note);
                  newNote.setNote(static_cast<char>(56 + globalNr) % 7 + 1);
                  newNote.setOctave(static_cast<char>(56 + globalNr) / 7 - 8);
              } else
                  fitToRange(newNote);
              noteSeg->setNote(newNote);
          }
        }
        for (int m = 0; m < m_measures.count(); ++m)
          m_measures[m]->refresh();
        if (!pianoChanged) // otherwise adjustScoreWidth() will be called due to score scale changes
          adjustScoreWidth();
    }
  }
}


/**
 * When meter is changed and there are notes on the score, all segments are flushed and stored
 * then measures and staves are deleted,
 * then all notes are added from scratch, but stored segments are reused to improve single segment creation time
 */
void TscoreObject::setMeter(int m) {
  Tmeter::Emeter newMeter = static_cast<Tmeter::Emeter>(m);
  Tmeter::Emeter prevMeter = m_meter->meter();
  if (m_meter->meter() != newMeter) {
    // set notes grouping
    m_meter->setMeter(newMeter);
    m_meter->fillMeterGroups(m_meterGroups);
    if (measuresCount())
      firstMeasure()->meterChanged();
    emit meterChanged();
    setWorkRhythm(Trhythm(newMeter == Tmeter::NoMeter ? Trhythm::NoRhythm : (newMeter <= Tmeter::Meter_7_4 ? Trhythm::Quarter : Trhythm::Eighth)));

    if (!m_singleNote && measuresCount() && firstMeasure()->noteCount() > 0) {
      clearScorePrivate();
      QList<Tnote> oldList = m_notes;
      m_notes.clear();
      for (int n = 0; n < oldList.size(); ++n) {
        // TODO fold (merge) all ties
        if (m_meter->meter() == Tmeter::NoMeter) // remove rhythm then
          oldList[n].setRhythm(Trhythm(Trhythm::NoRhythm));
        if (prevMeter == Tmeter::NoMeter) // initialize empty rhythm
          oldList[n].setRhythm(Trhythm()); // quarter by default
        addNote(oldList[n]);
      }
      m_activeBarNr = 0;
      adjustScoreWidth();
    }
    emitLastNote();
  }
}


int TscoreObject::meterToInt() const { return static_cast<int>(m_meter->meter()); }


void TscoreObject::setKeySignature(int k) {
  qint8 key = static_cast<qint8>(k);
  if (m_keySignEnabled && key != m_keySignature) {
    if (key != m_keySignature) {
      m_keySignature = key;
      for (int i = 1; i < 8; i++) {
        qint8 sign = k < 0 ? -1 : 1;
        int startVal = k < 0 ? 38 : 48;
        if (i <= qAbs(k))
          m_accidInKeyArray[(startVal + sign * (i * 4)) % 7] = sign;
        else
          m_accidInKeyArray[(startVal + sign * (i * 4)) % 7] = 0;
      }
      m_keyChanged = true;
      for (TmeasureObject* m : qAsConst(m_measures))
        m->keySignatureChanged();
      if (notesCount() > 0)
        adjustScoreWidth();
      emit keySignatureChanged();
    }
  }
}


/** @p static
 * This method creates @p outList of notes that have pitch of @p n note
 * but splits @p dur duration into possible rhythmic values.
 */
void solveList(const Tnote& n, int dur, QList<Tnote>& outList) {
  Trhythm rtm(dur); // try to obtain rhythm value
  if (!rtm.isValid()) { // if it is not possible to express the duration in single rhythm - resolve it in multiple values
      TrhythmList solvList;
      Trhythm::resolve(dur, solvList);
      for (int r = 0; r < solvList.count(); ++r) {
        outList << Tnote(n, Trhythm(solvList[r].rhythm(), n.isRest(), solvList[r].hasDot(), solvList[r].isTriplet()));
      }
  } else { // just single note in the list
      rtm.setRest(n.isRest());
      outList << Tnote(n, rtm);
  }
}

void TscoreObject::addNote(const Tnote& newNote, bool fromQML) {
// CHECKTIME (

  if (m_singleNote) {
    qDebug() << "[TscoreObject] FIXME! Trying to add note in single mode";
    return;
  }

  auto lastMeasure = m_measures.last();
  if (lastMeasure->free() == 0) { // new measure is needed
    lastMeasure = getMeasure(m_measures.count());
    m_measures << lastMeasure;
    lastStaff()->appendMeasure(lastMeasure);
  }

  Tnote n = newNote;
  fitToRange(n);
  int noteDur = n.rhythm() == Trhythm::NoRhythm || m_meter->meter() == Tmeter::NoMeter ? 1 : n.duration();
  if (noteDur > lastMeasure->free()) { // split note that is adding
      int leftDuration = noteDur - lastMeasure->free();
      int lastNoteId = m_segments.count();

      QList<Tnote> notesToCurrent;
      solveList(n, lastMeasure->free(), notesToCurrent); // solve free duration in current measure
      if (notesToCurrent.isEmpty())
          qDebug() << "[TscoreObject] can't resolve duration of" << lastMeasure->free();
      else {
          if (!n.isRest()) {
            notesToCurrent.first().rtm.setTie(newNote.rtm.tie() > Trhythm::e_tieStart ? Trhythm::e_tieCont : Trhythm::e_tieStart);
            if (notesToCurrent.count() == 2)
              notesToCurrent.last().rtm.setTie(Trhythm::e_tieCont);
          }
          appendToNoteList(notesToCurrent);
          lastMeasure->appendNewNotes(lastNoteId, notesToCurrent.count());
      }

      QList<Tnote> notesToNext;
      solveList(n, leftDuration, notesToNext); // solve remaining duration for the next measure
      lastNoteId = m_segments.count(); // update id of the last note segment
      if (notesToNext.isEmpty())
          qDebug() << "[TscoreObject] can't resolve duration" << leftDuration;
      else {
          if (!n.isRest()) {
            if (notesToNext.count() == 1)
                notesToNext.first().rtm.setTie(Trhythm::e_tieEnd);
            else {
                notesToNext.first().rtm.setTie(Trhythm::e_tieCont);
                notesToNext.last().rtm.setTie(Trhythm::e_tieEnd);
            }
          }
          appendToNoteList(notesToNext);
          auto newLastMeasure = getMeasure(m_measures.count()); // new TmeasureObject(m_measures.count(), this); // add a new measure
          m_measures << newLastMeasure;
          lastStaff()->appendMeasure(newLastMeasure);
          newLastMeasure->appendNewNotes(lastNoteId, notesToNext.count());
      }
  } else { // just add new note to the last measure
      m_notes << n;
      int lastNoteId = m_segments.count();
      m_segments << getSegment(lastNoteId, &m_notes.last());
      lastMeasure->appendNewNotes(lastNoteId, 1);
  }
  emitLastNote();
  if (fromQML) {
    if (!m_recordMode)
      setSelectedItem(lastNote());
    emit noteWasAdded();
  }
// ) // CHECKTIME
}


void TscoreObject::setNote(TnoteItem* no, const Tnote& n) {
  if (no) {
    if (m_allowAdding && m_meter->meter() != Tmeter::NoMeter && no == lastNote() && no->note()->rtm != n.rtm) {
      deleteLastNote();
      addNote(n);
      emitLastNote();
      return;
    }

    int durDiff = no->note()->duration() - n.duration();

    auto oldNote = *no->wrapper()->note();
    auto newNote = n;
    if (!durDiff) {
      newNote.rtm.setBeam(oldNote.rtm.beam());
      newNote.rtm.setTie(oldNote.rtm.tie());
    }
    fitToRange(newNote);
    bool fitStaff = false;
    // disconnect tie (if any) if note pitch changed
    QPoint notesForAlterCheck;// x is first note and y is the last note to check
    if (oldNote.rtm.tie() && newNote.chromatic() != oldNote.chromatic()) {
      // alters of notes has to be checked due to note changed
      // and all measures contained note with the tie are affected. Find them then
      notesForAlterCheck = tieRange(no);
      notesForAlterCheck.setX(m_segments[notesForAlterCheck.x()]->item()->measure()->firstNoteId());
      notesForAlterCheck.setY(m_segments[notesForAlterCheck.y()]->item()->measure()->lastNoteId());
      if (oldNote.rtm.tie() == Trhythm::e_tieStart) {
          m_segments[no->index() + 1]->disconnectTie(TnotePair::e_untieNext);
      } else { // tie continue or end
          if (oldNote.rtm.tie() == Trhythm::e_tieCont)
            m_segments[no->index() + 1]->disconnectTie(TnotePair::e_untieNext);
          m_segments[no->index() - 1]->disconnectTie(TnotePair::e_untiePrev);
      }
      newNote.rtm.setTie(Trhythm::e_noTie);
      if (no->wrapper() == no->staff()->firstNote())
        no->staff()->deleteExtraTie();
      fitStaff = true;
    }

    if (durDiff) {
        no->measure()->changeNoteDuration(no->wrapper(), newNote);
        if (lastMeasure()->isEmpty())
          removeLastMeasure();
        adjustScoreWidth();
    } else {
        no->wrapper()->setNote(newNote);
        // When note or alter are different - check accidentals in whole measure and fit staff if necessary
        if (!notesForAlterCheck.isNull() || oldNote.note() != newNote.note() || oldNote.alter() != newNote.alter()) {
          if (notesForAlterCheck.isNull())
            notesForAlterCheck = QPoint(no->measure()->firstNoteId(), no->measure()->lastNoteId());
          auto measureToRefresh = m_segments[notesForAlterCheck.x()]->item()->measure();
          for (int n = notesForAlterCheck.x(); n <= notesForAlterCheck.y(); ++n) {
            if (m_segments[n]->note()->note() == oldNote.note() || m_segments[n]->note()->note() == newNote.note()) {
              fitStaff = true;
              m_segments[n]->item()->updateAlter();
            }
            // Update measure sums (notes width)
            if (m_segments[n]->item()->measure() != measureToRefresh) {
              measureToRefresh->refresh();
              measureToRefresh = m_segments[n]->item()->measure();
            }
          }
          measureToRefresh->refresh();
        }
        // update note range on current staff
        if (oldNote.note() != newNote.note() || oldNote.octave() != newNote.octave())
          no->staff()->checkNotesRange();
        // If there is a beam - prepare it again then draw
        if (no->wrapper()->beam()) {
          no->wrapper()->beam()->prepareBeam();
          if (!fitStaff)
            no->wrapper()->beam()->drawBeam();
        }
        if (fitStaff) {
          m_segments[notesForAlterCheck.x()]->item()->staff()->fit();
          if (m_segments[notesForAlterCheck.y()]->item()->staff() != m_segments[notesForAlterCheck.x()]->item()->staff())
            m_segments[notesForAlterCheck.y()]->item()->staff()->fit();
        }
    }

    if (no == m_selectedItem)
      emit selectedNoteChanged();
    if (m_singleNote && m_enharmNotesEnabled && n.isValid()) {
      TnotesList enharmList = newNote.getTheSameNotes(m_enableDoubleAccids);
      TnotesList::iterator it = enharmList.begin();
      ++it;
      if (it != enharmList.end()) {
          note(1)->setVisible(true);
          m_segments[1]->setNote(*(it));
      } else
          note(1)->setVisible(false);
      ++it;
      if (it != enharmList.end()) {
          note(2)->setVisible(true);
          m_segments[2]->setNote(*(it));
      } else
          note(2)->setVisible(false);
    }
  }
  if (!m_recordMode)
    setSelectedItem(no);
}


void TscoreObject::setNote(int noteNr, const Tnote& n) {
  if (noteNr >= 0 && noteNr < notesCount())
    setNote(note(noteNr), n);
  else
    qDebug() << "[TscoreObject FIXME] Trying to set note of item that doesn't exist!" << noteNr;
}


void TscoreObject::setTechnical(int noteId, quint32 tech) {
  if (noteId >= 0 && noteId < notesCount())
    noteSegment(noteId)->setTechnical(tech);
}


TnoteItem* TscoreObject::note(int noteId) {
  return noteId > -1 && noteId < notesCount() ? m_segments[noteId]->item() : nullptr;
}


QQuickItem* TscoreObject::noteHead(int noteId) {
  return noteId > -1 && noteId < notesCount() ? m_segments[noteId]->item()->head() : nullptr;
}


Tnote TscoreObject::noteOfItem(TnoteItem* item) const {
  return item ? *item->note() : Tnote();
}


Tnote TscoreObject::noteAt(int index) const {
  return index >= 0 && index < m_notes.count() ? m_notes[index] : Tnote();
}


void TscoreObject::noteClicked(qreal yPos) {
  if (!activeNote())
    return;

  Trhythm newRhythm = m_meter->meter() == Tmeter::NoMeter ? Trhythm(Trhythm::NoRhythm) : *m_workRhythm;

  int globalNr = globalNoteNr(yPos);
  Tnote newNote(static_cast<char>(56 + globalNr) % 7 + 1, static_cast<char>(56 + globalNr) / 7 - 8,
          static_cast<char>(m_cursorAlter), newRhythm);
  newNote.setOnUpperStaff(!(isPianoStaff() && yPos > upperLine() + 13.0));
  if (m_workRhythm->isRest() || m_clefType == Tclef::NoClef)
    newNote.setNote(0);

  bool selectLastNoteAgain = m_activeNote == lastNote() && m_activeNote->note()->rtm != newRhythm;
  setNote(m_activeNote, newNote);
  if (selectLastNoteAgain) { // clicked note is the last one and changed its rhythm, so it was deleted and added again
    m_activeNote = lastNote();
    setSelectedItem(lastNote());
    emit activeNoteChanged();
  }

  emit clicked();
}


void TscoreObject::setCursorAlter(int curAlt) {
  curAlt = qBound(m_enableDoubleAccids ? -2 : -1, curAlt, m_enableDoubleAccids ? 2 : 1);
  if (curAlt != m_cursorAlter) {
    m_cursorAlter = curAlt;
    emit cursorAlterChanged();
  }
}


QString TscoreObject::alterText() {
  static const QString accids = QStringLiteral("\ue264\ue260 \ue262\ue263");
  return accids.mid(m_cursorAlter + 2, 1);
}


void TscoreObject::openMusicXml(const QString& musicFile, Tmelody* melody, bool ignoreTechnical) {
  if (!musicFile.isEmpty()) {
    bool melodyCreated = false;
    if (!melody) {
      melody = new Tmelody();
      melodyCreated = true;
    }
    if (melody->grabFromMusicXml(musicFile))
      setMelody(melody, ignoreTechnical);
    if (melodyCreated)
      delete melody;
  }
}


void TscoreObject::saveMusicXml(const QString& musicFile, const QString& title,
                                const QString& composer, int transposition) {
  if (!musicFile.isEmpty()) {
    QString fileName = musicFile;
    if (musicFile.right(4) != QLatin1String(".xml") && musicFile.right(9) != QLatin1String(".musicxml"))
      fileName += QLatin1String(".musicxml"); // prefer musicxml extension
    auto melody = new Tmelody(title, TkeySignature(static_cast<char>(keySignature())));
    getMelody(melody);
    melody->setComposer(composer);
    melody->saveToMusicXml(fileName, transposition);
    delete melody;
  }
}


void TscoreObject::setMelody(Tmelody* melody, bool ignoreTechnical, int notesAmount, int transposition) {
CHECKTIME (
  clearScorePrivate();
  m_notes.clear();
  setMeter(static_cast<int>(melody->meter()->meter()));
  setClefType(melody->clef());
  int newKey = static_cast<int>(melody->key().value());
  if (newKey != keySignature()) {
    if (!m_keySignEnabled && qAbs(newKey) != 0)
      setKeySignatureEnabled(true);
    setKeySignature(newKey);
  }
  int notesToCopy = notesAmount == 0 ? melody->length() : qMin(notesAmount, melody->length());
  for (int n = 0; n < notesToCopy; ++n) {
    if (transposition) {
        Tnote nn = melody->note(n)->p();
        nn.transpose(transposition);
        if (m_keySignature < 0 && nn.alter() == Tnote::e_Sharp)
          nn = nn.showWithFlat();
        addNote(nn);
    } else
        addNote(melody->note(n)->p());
    if (!ignoreTechnical)
      lastSegment()->setTechnical(melody->note(n)->technical());
  }
  adjustScoreWidth();
  emitLastNote();
)
}


void TscoreObject::getMelody(Tmelody* melody) {
  melody->setClef(clefType());
  melody->setMeter(m_meter->meter());
  if (m_keySignEnabled)
    melody->setKey(TkeySignature(static_cast<char>(m_keySignature)));
  for (int n = 0; n < notesCount(); ++n) {
    Ttechnical technical(noteSegment(n)->techicalData());
    melody->addNote(Tchunk(m_notes[n], technical));
  }
}


//#################################################################################################
//###################         Score switches           ############################################
//#################################################################################################

void TscoreObject::setKeySignatureEnabled(bool enKey) {
  if (enKey != m_keySignEnabled) {
    if (!enKey)
      m_keySignature = 0;
    m_keySignEnabled = enKey;
    emit keySignatureEnabledChanged();
    if (notesCount() > 0)
      adjustScoreWidth();
  }
}


void TscoreObject::setKeyReadOnly(bool ro) {
  if (m_keySignEnabled) {
    if (ro != m_keyReadOnly) {
      m_keyReadOnly = ro;
      emit keyReadOnlyChanged();
    }
  }
}


void TscoreObject::setEnableDoubleAccids(bool dblEnabled) {
  if (m_enableDoubleAccids != dblEnabled) {
    m_enableDoubleAccids = dblEnabled;
    if (!m_enableDoubleAccids) {
      // TODO: convert notes with double accidentals into single-ones
    }
  }
}


/**
 * When @p m_showNoteNames is set to @p TRUE:
 * @p TmeasureObject during adding a note item calls @p TnoteItem::setNoteNameVisible() to create note name
 * This method iterates all notes and switches its state of displaying note name
 */
void TscoreObject::setShowNoteNames(bool showNames) {
  if (m_showNoteNames != showNames) {
    m_showNoteNames = showNames;
    if (notesCount()) {
      for (int n = 0; n < notesCount(); ++n)
        m_segments[n]->item()->setNoteNameVisible(m_showNoteNames && m_clefType != Tclef::NoClef && !m_singleNote);
    }
  }
}


void TscoreObject::setNameColor(const QColor& nameC) {
  if (m_nameColor != nameC) {
    m_nameColor = nameC;
    if (m_showNoteNames) {
      if (notesCount()) {
        for (int n = 0; n < notesCount(); ++n) // with hope that all items have name item created
          m_segments[n]->item()->nameItem()->setProperty("styleColor", m_nameColor);
      }
    }
  }
}


void TscoreObject::setNameStyle(int nameS) {
  m_nameStyle = nameS;
  if (m_showNoteNames) {
    if (notesCount()) {
      for (int n = 0; n < notesCount(); ++n) // with hope that all items have name item created
        m_segments[n]->item()->nameItem()->setProperty("text", m_notes[n].styledName());
    }
  }
}


void TscoreObject::setReadOnly(bool ro) {
  if (m_readOnly != ro) {
    m_readOnly = ro;
    emit readOnlyChanged();
    if (m_deleteNoteAct && !m_singleNote) {
      m_deleteNoteAct->setEnabled(!ro);
      m_clearScoreAct->setEnabled(!ro);
      m_editModeAct->setEnabled(!ro);
      m_insertNoteAct->setEnabled(!ro);
    }
    setKeyReadOnly(ro);
    if (!m_readOnly) {
      setEditMode(true);
    }
  }
}


void TscoreObject::setEditMode(bool isEdit) {
  if (isEdit != m_editMode) {
    m_editMode = isEdit;
    if (m_editModeAct)
      m_editModeAct->setChecked(m_editMode);
    emit editModeChanged();
    if (!m_editMode && m_activeNote) {
      m_activeNote = nullptr;
      emit activeNoteChanged();
      setActiveNotePos(0.0);
      setHoveredNote(nullptr);
    }
  }
}


/**
 * MainScore.qml also handles single note mode change,
 * but this method is (AND HAS TO BE) invoked first
 */
void TscoreObject::setSingleNote(bool singleN) {
  if (singleN != m_singleNote) {
    clearScore(); // In single note mode this call is ignored
    if (singleN) {
        addNote(Tnote()); // it is avoided in single note mode
        addNote(Tnote());
        addNote(Tnote());
        setShowNoteNames(false);
        m_singleNote = true;
        setNote(0, Tnote()); // reset it (hide) because addNote was performed above in multi notes mode
        setNote(1, Tnote());
        setNote(2, Tnote());
        note(0)->shiftHead(1.5);
        note(1)->shiftHead(1.5);
        note(2)->shiftHead(1.5);
        note(1)->setEnabled(false);
        note(2)->setEnabled(false);
        m_selectedItem = note(0);
    } else {
        m_singleNote = false;
        resetNoteItem(note(0));
        resetNoteItem(note(1));
        resetNoteItem(note(2));
        clearScore(); // call it again when transitioning from single note mode
    }
    emit singleNoteChanged();
  }
}


void TscoreObject::setRecordMode(bool r) {
  if (r != m_recordMode) {
    m_recordMode = r;
    emit recordModeChanged();
  }
}


void TscoreObject::setScaleFactor(qreal factor) {
  if (factor != m_scaleFactor) {
    m_scaleFactor = factor;
    emit scaleFactorChanged();
  }
}


void TscoreObject::setEnableTechnical(bool enTech) {
  if (enTech != m_enableTechnControl) {
    m_enableTechnControl = enTech;
    emit enableTechnicalChanged();
  }
}


qreal TscoreObject::stavesHeight() {
  if (m_staves.isEmpty())
    return 0.0;
  auto last = lastStaff();
  return last->y() + last->height() * last->scale();
}


void TscoreObject::changeActiveNote(TnoteItem* aNote) {
  if (aNote != m_activeNote) {
    auto prevActive = m_activeNote;
    m_activeNote = aNote;
    if (aNote == nullptr) {
        m_leaveTimer->start(600);
    } else {
        if (prevActive == nullptr)
          m_enterTimer->start(300);
        else {
          enterTimeElapsed();
          emit activeYposChanged();
        }
    }
  }
}


void TscoreObject::setActiveNotePos(qreal yPos) {
  if (!m_enterTimer->isActive() && yPos != m_activeYpos) {
    if (m_workRhythm->isRest())
      yPos = upperLine() + 4.0; // TODO it works because there is no rhythm change
    m_activeYpos = yPos;
    emit activeYposChanged();
  }
}


/**
 * Returns X coordinate of the first note in active measure (if any).
 *  or (if score is empty yet) - returns staff indent (position after meter)
 * So far it is used for positioning accidental pane on the active measure left.
 */
qreal TscoreObject::xFirstInActivBar() {
  if (m_activeBarNr < 0)
    return (firstStaff()->notesIndent() - 2.0) * firstStaff()->scale();
  else
    return (m_measures[m_activeBarNr]->first()->item()->x() - m_measures[m_activeBarNr]->first()->item()->alterWidth() - 1.0) * firstStaff()->scale();
}


/**
 * Returns X coordinate of the last note in active measure (if any),
 * but if that note is too far on the right (near a staff end), coordinate of first note is returned,
 * subtracted with 11.2 units of score scale which is width of the rhythm pane - to keep it visible.
 * So far it is used for positioning rhythm pane on the active measure right
 */
qreal TscoreObject::xLastInActivBar() {
  if (m_activeBarNr > -1) {
    qreal xx = m_measures[m_activeBarNr]->last()->item()->rightX();
    if (xx > m_width - 12.0)
      return xFirstInActivBar() - 11.2 * firstStaff()->scale();
    else
      return (xx + 7.0) * firstStaff()->scale();
  }
  return (firstStaff()->notesIndent() + 7.0) * firstStaff()->scale();
}

Trhythm TscoreObject::workRhythm() const {
  return *m_workRhythm;
}


void TscoreObject::setWorkRhythm(const Trhythm& r) {
  if (r != *m_workRhythm) {
    m_workRhythm->setRhythm(r);
    emit workRhythmChanged();
  }
}


QString TscoreObject::workRtmText() const {
  return TnoteItem::getHeadText(workRhythm());
}


int TscoreObject::workRtmValue() const { return static_cast<int>(m_workRhythm->rhythm()); }

void TscoreObject::setWorkRtmValue(int rtmV) {
  if (static_cast<Trhythm::Erhythm>(rtmV) != m_workRhythm->rhythm()) {
    m_workRhythm->setRhythmValue(static_cast<Trhythm::Erhythm>(rtmV));
    emit workRhythmChanged();
  }
}


bool TscoreObject::workRtmRest() const { return m_workRhythm->isRest(); }

void TscoreObject::setWorkRtmRest(bool hasRest) {
  if (hasRest != m_workRhythm->isRest()) {
    m_workRhythm->setRest(hasRest);
    emit workRhythmChanged();
  }
}


bool TscoreObject::workRtmDot() const { return m_workRhythm->hasDot(); }

void TscoreObject::setWorkRtmDot(bool hasDot) {
  if (hasDot != m_workRhythm->hasDot()) {
    m_workRhythm->setDot(hasDot);
    emit workRhythmChanged();
  }
}



QString TscoreObject::activeRtmText() {
  if (m_activeNote)
    return TnoteItem::getHeadText(Trhythm(m_activeNote == lastSegment()->item() ? m_workRhythm->rhythm() : m_activeNote->note()->rhythm(), m_workRhythm->isRest()));
  return QString();
}


Tnote TscoreObject::posToNote(qreal yPos) {
  int globalNr = globalNoteNr(yPos);
  return Tnote(m_workRhythm->isRest() || m_clefType == Tclef::NoClef ? 0 : static_cast<char>(56 + globalNr) % 7 + 1,
               static_cast<char>(56 + globalNr) / 7 - 8, static_cast<char>(m_cursorAlter), workRhythm());
}


void TscoreObject::setAllowAdding(bool allow) {
  if (m_singleNote)
    allow = false;
  if (allow != m_allowAdding) {
    m_allowAdding = allow;
    adjustScoreWidth();
    emit allowAddingChanged();
  }
}


TnoteItem* TscoreObject::lastNote() {
  return m_segments.isEmpty() ? nullptr : lastSegment()->item();
}


qreal TscoreObject::midLine(TnoteItem* actNote) {
  if (stavesCount() == 0)
    return 0.0;
  if (actNote && m_activeNote)
    return activeNote()->y() + (upperLine() + 4.0) * lastStaff()->scale();
  else
    return lastStaff()->y() + (upperLine() + 4.0) * lastStaff()->scale();
}


void TscoreObject::deleteNote(TnoteItem* n) {
  if (n == lastNote()) {
    deleteLastNote();
    return;
  }
  if (n) {
    int rmId = n->index();
    if (n->note()->rtm.tie()) {
      if (n->note()->rtm.tie() == Trhythm::e_tieStart) {
          m_segments[rmId + 1]->disconnectTie(TnotePair::e_untieNext);
      } else { // tie continue or end
          if (n->note()->rtm.tie() == Trhythm::e_tieCont)
            m_segments[rmId + 1]->disconnectTie(TnotePair::e_untieNext);
          m_segments[rmId - 1]->disconnectTie(TnotePair::e_untiePrev);
      }
    }
    auto segToRemove = m_segments.takeAt(rmId);
    auto m = n->measure();
    int staffNr = m->staff()->number();
    segToRemove->flush();
    m_spareSegments << segToRemove;
    m_notes.removeAt(rmId);
    for (int s = rmId; s < m_segments.count(); ++s)
      m_segments[s]->setIndex(s);
    m->removeNote(n->wrapper());
    auto lastBar = lastMeasure();
    if (lastBar->isEmpty())
      removeLastMeasure();
    adjustScoreWidth(staffNr);
  }
}



void TscoreObject::deleteLastNote() {
  if (notesCount()) {
    if (lastNote()->note()->rtm.tie() && notesCount() > 1)
      noteSegment(notesCount() - 2)->disconnectTie(TnotePair::e_untiePrev);
    bool adjust = false;
    auto lastBar = lastMeasure();
    int tempActiveBar = m_activeBarNr;
    if (lastBar->noteCount() == 1 && measuresCount() > 1)
      adjust = removeLastMeasure();
    else
      lastBar->removeLastNote();
    auto segToRemove = m_segments.takeLast();
    segToRemove->flush();
    m_spareSegments << segToRemove;
    m_notes.removeLast();
    m_activeNote = nullptr;
    if (notesCount() == 0)
      m_activeBarNr = -1;
    if (adjust)
      adjustScoreWidth();

    emit activeNoteChanged();
    emitLastNote();
    if (tempActiveBar != m_activeBarNr)
      emitActiveBarChanged();
    setSelectedItem(nullptr);
  }
}


void TscoreObject::insertNote(TnoteItem* afterItem) {
  if (afterItem) {
    afterItem->measure()->insertNote(afterItem);
    adjustScoreWidth(afterItem->staff()->number());
  }
}


void TscoreObject::clearScore() {
  if (notesCount() == 0) {
      setKeySignature(0);
      return;
    }

  if (m_singleNote) {
      setNote(note(0), Tnote());
      setNote(note(1), Tnote());
      setNote(note(2), Tnote());
  } else {
      clearScorePrivate();
      m_notes.clear();
      m_activeBarNr = -1;
      m_activeNote = nullptr;
      adjustScoreWidth();
      emitLastNote();
      setSelectedItem(nullptr);
      emit scoreWasCleared();
  }
  setKeySignature(0);
}


qreal TscoreObject::upperLine() { return m_staves.empty() ? 16.0 : firstStaff()->upperLine(); }


void TscoreObject::setWidth(qreal w) {
  if (w != m_width) {
    m_width = w;
    if (m_widthTimer->isActive())
      m_widthTimer->stop();
    m_widthTimer->start(WIDTH_CHANGE_DELAY);
  }
}


Tnote TscoreObject::highestNote() {
  switch (m_clefType) {
    case Tclef::Treble_G: return Tnote(4, 4);
    case Tclef::Treble_G_8down: return Tnote(4, 3);
    case Tclef::Bass_F: return Tnote(6, 2);
    case Tclef::Alto_C: return Tnote(5, 3);
    case Tclef::Tenor_C: return Tnote(3, 3);
    case Tclef::PianoStaffClefs: return Tnote(1, 4);
    default: return Tnote(); // percussion clef (no clef) and unsupported dropped bass
  }
}


Tnote TscoreObject::lowestNote() {
  switch (m_clefType) {
    case Tclef::Treble_G: return Tnote(7, -1);
    case Tclef::Treble_G_8down: return Tnote(7, -2);
    case Tclef::Bass_F: return Tnote(7, -2);
    case Tclef::Alto_C: return Tnote(1, -1);
    case Tclef::Tenor_C: return Tnote(6, -2);
    case Tclef::PianoStaffClefs: return Tnote(1, -1);
    default: return Tnote(); // percussion clef (no clef) and unsupported dropped bass
  }
}


TnoteItem* TscoreObject::getNext(TnoteItem* someNote) {
  if (someNote) {
    if (someNote->index() < notesCount() - 1)
      return noteSegment(someNote->index() + 1)->item();
  }
  return nullptr;
}


TnoteItem * TscoreObject::getPrev(TnoteItem* someNote) {
  if (someNote) {
    if (someNote->index() > 0)
      return noteSegment(someNote->index() - 1)->item();
  }
  return nullptr;
}


void TscoreObject::setSelectedItem(TnoteItem* item) {
  if (item != m_selectedItem) {
    m_selectedItem = item;
    emit selectedItemChanged();
    emit selectedNoteChanged();
  }
}


void TscoreObject::selectNext(bool keep, bool skipTies) {
  if (m_selectedItem) {
    TnoteItem* next = nullptr;
    if (skipTies && m_selectedItem->note()->rtm.tie() < Trhythm::e_tieEnd) {
        auto tr = tieRange(m_selectedItem);
        next = note(tr.y());
    } else
        next = getNext(m_selectedItem);
    if (next)
      setSelectedItem(next);
    else if (!keep)
      setSelectedItem(nullptr);
  }
}


void TscoreObject::setBgColor(const QColor& bg) {
  if (bg != m_bgColor) {
    m_bgColor = bg;
    emit bgColorChanged();
  }
}


void TscoreObject::enableActions() {
  if (!m_deleteNoteAct) {
    QString tipbg = QStringLiteral("tipbg");
    m_deleteNoteAct = new Taction(tr("Delete note"), QStringLiteral("delete"), this);
    connect(m_deleteNoteAct, &Taction::triggered, [=]{ if (!m_readOnly && !m_singleNote && m_allowAdding) deleteNote(m_activeNote); });
    m_deleteNoteAct->createQmlShortcut(m_qmlComponent, "\"del\"; enabled: !score.singleNote && !score.readOnly");

    m_insertNoteAct = new Taction(tr("Insert note"), QStringLiteral("fingerpoint"), this);
    connect(m_insertNoteAct, &Taction::triggered, [=]{ if (!m_readOnly && !m_singleNote && m_allowAdding) insertNote(m_activeNote); });
    m_insertNoteAct->createQmlShortcut(m_qmlComponent, "\"ins\"; enabled: !score.singleNote && !score.readOnly");

    m_clearScoreAct = new Taction(tr("Delete all notes"), QStringLiteral("clear-score"), this);
    connect(m_clearScoreAct, &Taction::triggered, [=]{ if (!m_readOnly) clearScore(); });
    m_clearScoreAct->createQmlShortcut(m_qmlComponent, "\"Shift+del\"; enabled: !score.singleNote && !score.readOnly");

    m_editModeAct = new Taction(tr("Edit score"), QString(), this);
    m_editModeAct->setCheckable(true);
    connect(m_editModeAct, &Taction::triggered, [=]{ if (!m_readOnly && !m_singleNote) setEditMode(!editMode()); });
    m_editModeAct->createQmlShortcut(m_qmlComponent, "\"E\"; enabled: !score.singleNote && !score.readOnly");

    m_wholeNoteAct = new Taction(tr("whole note"), tipbg, this);
    connect(m_wholeNoteAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_wholeNoteAct->createQmlShortcut(m_qmlComponent, "\"1\"");

    m_halfNoteAct = new Taction(tr("half note"), tipbg, this);
    connect(m_halfNoteAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_halfNoteAct->createQmlShortcut(m_qmlComponent, "\"2\"");

    m_quarterNoteAct = new Taction(tr("quarter note"), tipbg, this);
    connect(m_quarterNoteAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_quarterNoteAct->createQmlShortcut(m_qmlComponent, "\"4\"");

    m_eighthNoteAct= new Taction(tr("eighth note"), tipbg, this);
    connect(m_eighthNoteAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_eighthNoteAct->createQmlShortcut(m_qmlComponent, "\"8\"");

    m_sixteenthNoteAct = new Taction(tr("sixteenth note"), tipbg, this);
    connect(m_sixteenthNoteAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_sixteenthNoteAct->createQmlShortcut(m_qmlComponent, "\"6\"");

    m_restNoteAct = new Taction(tr("rest"), tipbg, this);
    connect(m_restNoteAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_restNoteAct->createQmlShortcut(m_qmlComponent, "\"R\"");

    m_dotNoteAct = new Taction(tr("dot"), tipbg, this);
    connect(m_dotNoteAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_dotNoteAct->createQmlShortcut(m_qmlComponent, "\".\"");

    m_riseAct = new Taction(tr("rise", "as such as sharps rise note"), tipbg, this);
    connect(m_riseAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_riseAct->createQmlShortcut(m_qmlComponent, "\"#\"");
  
    m_lowerAct = new Taction(tr("lower", "as such as flats lower note"), tipbg, this);
    connect(m_lowerAct, &Taction::triggered, this, &TscoreObject::handleNoteAction);
    m_lowerAct->createQmlShortcut(m_qmlComponent, "\"@\"");

    m_tieAct = new Taction(QGuiApplication::translate("ScoreToolbox", "tie",
                                                      "To translate it properly, check please meaning of 'tie' in musical context."),
                           tipbg, this);
    connect(m_tieAct, &Taction::triggered, this, &TscoreObject::checkTieOfSelected);
    m_tieAct->createQmlShortcut(m_qmlComponent, "\"l\"");
  }
}


/**
 * TODO: if there will be TnoteObject implemented with all note/rhythm props
 * It would replace @p m_cursorAlter && @p m_workRhythm
 */
void TscoreObject::handleNoteAction() {
  if (!m_readOnly) {
    if (!m_singleNote && m_meter->meter() != Tmeter::NoMeter) { // For full score (melodies) only
      bool wasWorkRhythmChanged = false;
      if (sender() == m_dotNoteAct && m_workRhythm->rhythm() != Trhythm::Sixteenth) {
          m_workRhythm->setDot(!m_workRhythm->hasDot());
          wasWorkRhythmChanged = true;
      } else if (sender() == m_restNoteAct) {
          m_workRhythm->setRest(!m_workRhythm->isRest());
          if (m_workRhythm->rhythm() == Trhythm::Sixteenth)
            m_workRhythm->setDot(false);
          wasWorkRhythmChanged = true;
      } else if (sender() == m_sixteenthNoteAct) {
          if (m_workRhythm->rhythm() != Trhythm::Sixteenth) {
            m_workRhythm->setRhythmValue(Trhythm::Sixteenth);
            m_workRhythm->setDot(false); // 16th with dot not supported (yet)
            wasWorkRhythmChanged = true;
          }
      } else if (sender() == m_eighthNoteAct) {
          if (m_workRhythm->rhythm() != Trhythm::Eighth) {
            m_workRhythm->setRhythmValue(Trhythm::Eighth);
            wasWorkRhythmChanged = true;
          }
      } else if (sender() == m_quarterNoteAct) {
          if (m_workRhythm->rhythm() != Trhythm::Quarter) {
            m_workRhythm->setRhythmValue(Trhythm::Quarter);
            wasWorkRhythmChanged = true;
          }
      } else if (sender() == m_halfNoteAct) {
          if (m_workRhythm->rhythm() != Trhythm::Half) {
            m_workRhythm->setRhythmValue(Trhythm::Half);
            wasWorkRhythmChanged = true;
          }
      } else if (sender() == m_wholeNoteAct) {
          if (m_workRhythm->rhythm() != Trhythm::Whole) {
            m_workRhythm->setRhythmValue(Trhythm::Whole);
            wasWorkRhythmChanged = true;
          }
      }
      if (wasWorkRhythmChanged) {
        emit workRhythmChanged();
        return;
      }
    }
    if (sender() == m_riseAct) {
        if (m_cursorAlter < 1) // flats or none
          setCursorAlter(1); // set sharp
        else if (m_cursorAlter == 1 && m_enableDoubleAccids) // single sharp
          setCursorAlter(2); // set double sharp
        else
          setCursorAlter(0); // or none
      
    } else if (sender() == m_lowerAct) {
        if (m_cursorAlter > -1) // sharps of none
          setCursorAlter(-1); // set flat
        else if (m_cursorAlter == -1 && m_enableDoubleAccids) // single flat
          setCursorAlter(-2); // set double flat
        else
          setCursorAlter(0); // or none
    }
  }
}


void TscoreObject::checkTieOfSelected() {
  if (m_selectedItem && m_selectedItem->index() > 0) {
    auto prevNote = m_segments[m_selectedItem->index() - 1];
    auto n = *m_selectedItem->note();
    if (m_selectedItem->note()->rtm.tie() > Trhythm::e_tieStart) { // disconnect
        prevNote->disconnectTie(TnotePair::e_untiePrev);
        n.rtm.setTie(n.rtm.tie() == Trhythm::e_tieEnd ? Trhythm::e_noTie : Trhythm::e_tieStart);
        m_selectedItem->wrapper()->setNote(n);
        emit m_selectedItem->hasTieChanged();
        if (m_selectedItem->staff()->firstNote()->item() == m_selectedItem)
          m_selectedItem->staff()->deleteExtraTie();
    } else {
        if (!m_selectedItem->note()->isRest() && m_selectedItem->note()->chromatic() == prevNote->note()->chromatic()) {
          n.rtm.setTie(n.rtm.tie() == Trhythm::e_noTie ? Trhythm::e_tieEnd : Trhythm::e_tieCont);
          m_selectedItem->wrapper()->setNote(n);
          auto pn = *prevNote->note();
          pn.rtm.setTie(pn.rtm.tie() == Trhythm::e_noTie ? Trhythm::e_tieStart : Trhythm::e_tieCont);
          prevNote->setNote(pn);
          emit m_selectedItem->hasTieChanged();
          if (m_selectedItem->staff()->firstNote()->item() == m_selectedItem)
            m_selectedItem->staff()->createExtraTie(m_selectedItem);
        }
    }
    auto notesForAlterCheck = tieRange(prevNote->item());
    bool fitStaff = false;
    auto measureToRefresh = m_segments[notesForAlterCheck.x()]->item()->measure();
    notesForAlterCheck.setX(m_segments[notesForAlterCheck.x()]->item()->measure()->firstNoteId());
    notesForAlterCheck.setY(m_segments[notesForAlterCheck.y()]->item()->measure()->lastNoteId());
    for (int i = notesForAlterCheck.x(); i <= notesForAlterCheck.y(); ++i) {
      if (m_segments[i]->note()->note() == n.note()) {
        fitStaff = true;
        m_segments[i]->item()->updateAlter();
      }
      if (m_segments[i]->item()->measure() != measureToRefresh) {
        measureToRefresh->refresh();
        measureToRefresh = m_segments[i]->item()->measure();
      }
    }
    measureToRefresh->refresh();
    if (fitStaff) {
      m_segments[notesForAlterCheck.x()]->item()->staff()->fit();
      if (m_segments[notesForAlterCheck.y()]->item()->staff() != m_segments[notesForAlterCheck.x()]->item()->staff())
        m_segments[notesForAlterCheck.y()]->item()->staff()->fit();
    }
  }
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TscoreObject::addStaff(TstaffItem* st) {
  m_qmlEngine = QQmlEngine::contextForObject(st)->engine();
  m_qmlComponent = new QQmlComponent(m_qmlEngine, this);
  st->setNumber(stavesCount());
  m_staves.append(st);
  if (m_staves.count() == 1) { // initialize first measure of first staff
      st->appendMeasure(m_measures.first());
      connect(st, &TstaffItem::upperLineChanged, this, &TscoreObject::upperLineChanged);
  } else { // redirect destroyed signal to QML score
      connect(st, &TstaffItem::destroyed, [=]{ emit staffDestroying(st->number()); });
  }

  connect(st, &TstaffItem::hiNotePosChanged, [=](int staffNr, qreal offset){
    for (int i = staffNr; i < m_staves.size(); ++i) // move every staff about offset
      m_staves[i]->setY(m_staves[i]->y() + offset);
    emit stavesHeightChanged();
  });
  connect(st, &TstaffItem::loNotePosChanged, [=](int staffNr, qreal offset){
    if (staffNr == 0) // never change Y position of first staff - it is always 0
        staffNr = 1;
    if (m_staves.size() > 1 && staffNr < m_staves.size() - 1) { // ignore change of the last staff
      for (int i = staffNr; i < m_staves.size(); ++i) // move every staff about offset
        m_staves[i]->setY(m_staves[i]->y() + offset);
    }
    emit stavesHeightChanged();
  });
}

/**
 * More-less it means that staff @p sourceStaff has no space for @p count number of measures
 * starting from @p measureNr, but those measures belong to it still
 */
void TscoreObject::startStaffFromMeasure(TstaffItem* sourceStaff, int measureNr, int count) {
  TstaffItem* targetStaff = nullptr;
  if (sourceStaff == lastStaff()) { // create new staff to shift measure(s) there
      emit staffCreate();
      targetStaff = lastStaff();
  } else {
      targetStaff = m_staves[sourceStaff->number() + 1];
      targetStaff->deleteExtraTie();
  }

  for (int m = measureNr; m < measureNr + count; ++m)
    m_measures[m]->setStaff(targetStaff);
  targetStaff->setLastMeasureId(qMax(measureNr + count - 1, targetStaff->lastMeasureId()));
  targetStaff->setFirstMeasureId(measureNr);
}


void TscoreObject::deleteStaff(TstaffItem* st) {
  if (st->measuresCount() < 1) {
      bool fixStaffNumbers = st != lastStaff();
      m_staves.removeAt(st->number());
      st->deleteLater();
      if (fixStaffNumbers) {
        for (int s = 0; s < stavesCount(); ++s)
          m_staves[s]->setNumber(s);
      }
  }
}


void TscoreObject::adjustScoreWidth(int firstStaff) {
  m_adjustInProgress = true;
  int refreshStaffNr = firstStaff;
  while (refreshStaffNr < stavesCount()) {
    m_staves[refreshStaffNr]->refresh();
    refreshStaffNr++;
  }
  m_adjustInProgress = false;
  updateStavesPos();
}


void TscoreObject::updateStavesPos() {
  if (m_adjustInProgress)
    return;
  TstaffItem* prev = nullptr;
  for (QList<TstaffItem*>::iterator s = m_staves.begin(); s != m_staves.end(); ++s) {
    auto curr = *s;
    if (curr->number() != 0 && curr->number() < stavesCount())
      curr->setY(prev->y() + (prev->loNotePos() - curr->hiNotePos() + 4.0) * prev->scale()); // TODO scordature!
    prev = curr;
  }
  emit stavesHeightChanged();
}


void TscoreObject::onIndentChanged() {
  if (m_keyChanged) {
    m_keyChanged = false;
    adjustScoreWidth();
  }
}


/** FIXME: if tie starts/stops incorrectly (no start or end) it may return note number out of range */
QPoint TscoreObject::tieRange(TnoteItem* n) {
  QPoint tr;
  if (n->note()->rtm.tie()) {
    tr.setX(n->index());
    while (tr.x() > -1) {
      if (m_notes[tr.x()].rtm.tie() == Trhythm::e_tieStart)
        break;
      --tr.rx();
    }
    tr.setY(n->index());
    while (tr.y() < notesCount()) {
      if (m_notes[tr.y()].rtm.tie() == Trhythm::e_tieEnd)
        break;
      ++tr.ry();
    }
  }
  return tr;
}


void TscoreObject::setTouched(bool t) {
  if (t != m_touched) {
    m_touched = t;
    emit touchedChanged();
  }
}


TmeasureObject* TscoreObject::addMeasure() {
  auto lastM = m_measures.last();
  if (lastM->free())
    qDebug() << "[TscoreObject] FIXME!!! Last measure is not full but the new one is going to be added";
  lastM = getMeasure(m_measures.count());
  m_measures << lastM;
  lastStaff()->appendMeasure(lastM);
  return lastM;
}


TnotePair* TscoreObject::insertSilently(int id, const Tnote& n, TmeasureObject* m) {
  m_notes.insert(id, n);
  auto np = getSegment(id, &m_notes[id]);
  m_segments.insert(id, np);
  for (int s = id + 1; s < m_segments.count(); ++s)
    m_segments[s]->setIndex(s);
  if (m)
    m->insertSilently(id - m->firstNoteId(), np);
  return np;
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TscoreObject::appendToNoteList(QList<Tnote>& l) {
  for (Tnote& n : l) {
    m_notes << n;
    m_segments << getSegment(m_segments.count(), &m_notes.last());
  }
}


void TscoreObject::updateClefOffset() {
  switch (m_clefType) {
    case Tclef::Treble_G_8down: m_clefOffset.set(3, 1); break;
    case Tclef::Bass_F: m_clefOffset.set(5, 0); break;
    case Tclef::Alto_C: m_clefOffset.set(4, 1); break;
    case Tclef::Tenor_C: m_clefOffset.set(2, 1); break;
    default: m_clefOffset.set(3, 2); break; // Treble, piano staff and no clef (rhythm only)
  }
}


TnotePair* TscoreObject::getSegment(int noteNr, Tnote* n) {
  if (m_spareSegments.isEmpty())
      return new TnotePair(noteNr, n);
  else {
      auto np = m_spareSegments.takeLast();
      np->setNote(n);
      np->setIndex(noteNr);
      return np;
  }
}


TmeasureObject* TscoreObject::getMeasure(int barNr) {
  if (m_spareMeasures.isEmpty()) {
      return new TmeasureObject(barNr, this);
  } else {
      auto m = m_spareMeasures.takeLast();
      m->setNumber(barNr);
      m->meterChanged();
      return m;
  }
}


TbeamObject* TscoreObject::getBeam(TnotePair* np, TmeasureObject* m) {
  if (m_spareBeams.isEmpty()) {
      return new TbeamObject(np, m);
  } else {
      auto b = m_spareBeams.takeLast();
      b->setMeasure(m);
      b->addNote(np);
      return b;
  }
}


void TscoreObject::storeBeam(TbeamObject* b) {
  m_spareBeams << b;
}


int TscoreObject::globalNoteNr(qreal yPos) {
  if (isPianoStaff() && yPos > firstStaff()->upperLine() + 13.0)
    yPos -= 10.0;
  return m_clefOffset.octave * 7 - static_cast<int>(yPos - upperLine() - m_clefOffset.note);
}


void TscoreObject::clearScorePrivate() {
  if (measuresCount() && firstMeasure()->noteCount() > 0) {
    setSelectedItem(nullptr);
    m_activeBarNr = -1;
    changeActiveNote(nullptr);
    for (TnotePair* s : qAsConst(m_segments)) {
      s->flush();
      m_spareSegments << s;
    }
    for (TmeasureObject* m : qAsConst(m_measures)) {
      m->flush();
      m_spareMeasures << m;
    }
    m_measures.clear();
    m_segments.clear();
    while (m_staves.count() > 1) {
      auto ls = m_staves.takeLast();
      ls->deleteLater();
    }
    m_measures << getMeasure(0);
    lastStaff()->appendMeasure(firstMeasure());
    firstStaff()->setFirstMeasureId(0);
    firstStaff()->setLastMeasureId(0);
  }
}


void TscoreObject::enterTimeElapsed() {
  bool emitBarChange = false;
  if (m_activeNote && m_activeNote->measure()->number() != m_activeBarNr) {
    m_activeBarNr = m_activeNote->measure()->number();
    emitBarChange = true;
  }
  emit activeNoteChanged();
  if (emitBarChange)
    emit activeBarChanged();
  if (m_clefType == Tclef::NoClef)
    setActiveNotePos(upperLine() + 7.0);
}


void TscoreObject::leaveTimeElapsed() {
  emit activeNoteChanged();
}


void TscoreObject::fitToRange(Tnote& n) {
  Tnote loNote = lowestNote();
  Tnote hiNote = highestNote();
  if (!n.isRest()
    && ((n.octave() > hiNote.octave() || (n.octave() == hiNote.octave() && n.note() > hiNote.note()))
    || (n.octave() < loNote.octave() || (n.octave() == loNote.octave() && n.note() < loNote.note()))))
  {
    n.setNote(0); n.setOctave(0); // invalidate note
    n.setRest(true);
    n.rtm.setTie(Trhythm::e_noTie);
    n.rtm.setBeam(Trhythm::e_noBeam);
  }
}


/**
 * Set @p TnoteItem parameters to defaults , usually they are different in single note mode
 */
void TscoreObject::resetNoteItem(TnoteItem* noteItem) {
  noteItem->setVisible(true);
  noteItem->setEnabled(true);
  noteItem->setColor(qApp->palette().text().color());
  noteItem->setNoteNameVisible(m_showNoteNames && m_clefType != Tclef::NoClef);
  noteItem->shiftHead(0.0);
}


bool TscoreObject::removeLastMeasure() {
  bool adjust = false;
  if (measuresCount() > 1) {
    m_spareMeasures << m_measures.takeLast();
    auto lastSt = lastStaff();
    lastSt->setLastMeasureId(lastSt->lastMeasureId() - 1);
    if (lastSt->measuresCount() == 0) {
      deleteStaff(lastSt);
      adjust = true;
    }
    m_activeBarNr--;
    m_spareMeasures.last()->flush();
  }
  return adjust;
}


// Tnote dummyNote;
//       CHECKTIME (
//       for (int n = 0; n < 200; ++n) {
//         auto np = new TnotePair(-1, &dummyNote);
//         np->setNoteItem(new TnoteItem(st, np));
//         np->item()->setStaff(nullptr);
//         m_spareSegments << np;
//       }
//       )
