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

#include "tscoreobject.h"
#include "tstaffobject.h"
#include "tmeasureobject.h"
#include "tnoteobject.h"
#include "tbeamobject.h"
#include "tnotepair.h"
#include "music/tmeter.h"
#include "music/tnote.h"
#include "music/tmelody.h"

#include <QtQml/qqmlengine.h>
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
  m_clefOffset(TclefOffset(3, 1)),
  m_width(0.0), m_adjustInProgress(false)
{
  m_qmlEngine = new QQmlEngine;
  m_qmlComponent = new QQmlComponent(m_qmlEngine, this);
  m_meter = new Tmeter();
  setMeter(4); // Tmeter::Meter_4_4
  m_measures << new TmeasureObject(0, this);

  m_widthTimer = new QTimer(this);
  m_widthTimer->setSingleShot(true);
  connect(m_widthTimer, &QTimer::timeout, this, &TscoreObject::adjustScoreWidth);

  for (int i = 0; i < 7; i++) // reset accidentals array
    m_accidInKeyArray[i] = 0;

  m_touchHideTimer = new QTimer(this);
  connect(m_touchHideTimer, &QTimer::timeout, [=]{
      changeActiveNote(nullptr);
      setPressedNote(nullptr);
      m_touchHideTimer->stop();
  });
}


TscoreObject::~TscoreObject()
{
  delete m_meter;
  delete m_qmlComponent;
  delete m_qmlEngine;
  qDebug() << "[TscoreObject] deleted";
}

//#################################################################################################
//###################          Musical parameters      ############################################
//#################################################################################################

void TscoreObject::setClefType(Tclef::EclefType ct) {
  if (m_clefType != ct) {
    auto oldClef = m_clefType;
    m_clefType = ct;
    updateClefOffset();
    if (notesCount() < 1)
      return;

    Tnote loNote = lowestNote();
    Tnote hiNote = highestNote();
    for (int n = 0; n < notesCount(); ++n) {
      auto noteSeg = m_segments[n];
      if (m_clefType == Tclef::NoClef) {
          auto noteSeg = m_segments[n];
          Tnote newNote(Tnote(), noteSeg->note()->rtm);
          newNote.rtm.setStemDown(false);
          noteSeg->item()->setStemHeight(STEM_HEIGHT);
          noteSeg->setNote(newNote);
      } else {
          Tnote newNote(*noteSeg->note());
          if (oldClef == Tclef::NoClef) {
              int globalNr = m_clefOffset.octave * 7 - (7 - m_clefOffset.note);
              newNote.note = static_cast<char>(56 + globalNr) % 7 + 1;
              newNote.octave = static_cast<char>(56 + globalNr) / 7 - 8;
          } else {
              if (!newNote.isRest()
                  && ((newNote.octave > hiNote.octave || (newNote.octave == hiNote.octave && newNote.note > hiNote.note))
                  || (newNote.octave < loNote.octave || (newNote.octave == loNote.octave && newNote.note < loNote.note))))
              {
                newNote.note = 0; newNote.octave = 0; // invalidate note
                newNote.setRest(true);
                newNote.rtm.setTie(Trhythm::e_noTie);
                // TODO fix beam
              }
          }
          noteSeg->setNote(newNote);
      }
    }
    for (int m = 0; m < m_measures.count(); ++m)
      m_measures[m]->refresh();
    adjustScoreWidth();
  }
}


/**
 * When meter is changed and there are notes on the score, all segments are flushed and stored
 * then measures and staves are deleted (TODO it could be improved someday),
 * then all notes are added from scratch, but stored segments are reused to improve single segment creation time
 */
void TscoreObject::setMeter(int m) {
CHECKTIME (
  Tmeter::Emeter newMeter = static_cast<Tmeter::Emeter>(m);
  Tmeter::Emeter prevMeter = m_meter->meter();
  if (m_meter->meter() != newMeter) {
    // set notes grouping
    m_meter->setMeter(newMeter);
    updateMeterGroups();
    if (measuresCount())
      firstMeasure()->meterChanged();
    emit meterChanged();

    if (measuresCount() && firstMeasure()->noteCount() > 0) {
      clearScore();
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
      adjustScoreWidth();
    }
  }
)
}


int TscoreObject::meterToInt() const { return static_cast<int>(m_meter->meter()); }


void TscoreObject::setKeySignature(int k) {
  if (m_keySignEnabled) {
    qint8 key = static_cast<qint8>(k);
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
      for (TmeasureObject* m : m_measures)
        m->keySignatureChanged();
      if (notesCount() > 0)
        adjustScoreWidth();
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

void TscoreObject::addNote(const Tnote& n) {
CHECKTIME (

  auto lastMeasure = m_measures.last();
  if (lastMeasure->free() == 0) { // new measure is needed
    lastMeasure = new TmeasureObject(m_measures.count(), this);
    m_measures << lastMeasure;
    lastStaff()->appendMeasure(lastMeasure);
  }
  qDebug() << "Note" << n.toText() << n.rtm.string() << lastMeasure->free();
  int noteDur = n.rhythm() == Trhythm::NoRhythm ? 1 : n.duration();
  if (noteDur > lastMeasure->free()) { // split note that is adding
      int leftDuration = noteDur - lastMeasure->free();
      int lastNoteId = m_segments.count();

      QList<Tnote> notesToCurrent;
      solveList(n, lastMeasure->free(), notesToCurrent); // solve free duration in current measure
      if (notesToCurrent.isEmpty())
          qDebug() << "[TscoreObject] can't resolve duration of" << lastMeasure->free();
      else {
          if (!n.isRest()) {
            notesToCurrent.first().rtm.setTie(Trhythm::e_tieStart);
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
          auto newLastMeasure = new TmeasureObject(m_measures.count(), this); // add a new measure
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

)
}


void TscoreObject::setNote(int staffNr, int noteNr, const Tnote& n) {
  if (staffNr < 0 || staffNr >= m_staves.count()) {
    qDebug() << "[TscoreObject] There is no staff number" << staffNr;
    return;
  }
//   m_staves[staffNr]->setNote(noteNr, n);
}


TnoteObject * TscoreObject::note(int noteId) {
  return noteId > -1 && noteId < notesCount() ? m_segments[noteId]->item() : nullptr;
}


void TscoreObject::noteClicked(qreal yPos) {
  if (!activeNote() || activeNote()->note()->isRest())
    return;

  int globalNr = m_clefOffset.octave * 7 - (yPos - static_cast<int>(upperLine()) - m_clefOffset.note);
  Tnote newNote(static_cast<char>(56 + globalNr) % 7 + 1, static_cast<char>(56 + globalNr) / 7 - 8,
          static_cast<char>(m_cursorAlter), activeNote()->note()->rtm);
  Tnote oldNote = *activeNote()->wrapper()->note();
  bool fitStaff = false;
// disconnect tie (if any) if note pitch changed
  QPoint notesForAlterCheck;// x is first note and y is the last note to check
  if (oldNote.rtm.tie() && newNote.chromatic() != oldNote.chromatic()) {
    // alters of notes has to be checked due to note changed
    // and all measures contained note with the tie are affected. Find them then
    notesForAlterCheck = tieRange(activeNote());
    notesForAlterCheck.setX(m_segments[notesForAlterCheck.x()]->item()->measure()->firstNoteId());
    notesForAlterCheck.setY(m_segments[notesForAlterCheck.y()]->item()->measure()->lastNoteId());
    if (oldNote.rtm.tie() == Trhythm::e_tieStart) {
        m_segments[activeNote()->index() + 1]->disconnectTie(TnotePair::e_untieNext);
    } else { // tie continue or end
        if (oldNote.rtm.tie() == Trhythm::e_tieCont)
          m_segments[activeNote()->index() + 1]->disconnectTie(TnotePair::e_untieNext);
        m_segments[activeNote()->index() - 1]->disconnectTie(TnotePair::e_untiePrev);
    }
    newNote.rtm.setTie(Trhythm::e_noTie);
    if (activeNote()->wrapper() == activeNote()->staff()->firstNote())
      activeNote()->staff()->deleteExtraTie();
    fitStaff = true;
  }
  activeNote()->wrapper()->setNote(newNote);
// When note or alter are different - check accidentals in whole measure and fit staff if necessary
  if (!notesForAlterCheck.isNull() || oldNote.note != newNote.note || oldNote.alter != newNote.alter) {
    if (notesForAlterCheck.isNull())
      notesForAlterCheck = QPoint(activeNote()->measure()->firstNoteId(), activeNote()->measure()->lastNoteId());
    auto measureToRefresh = m_segments[notesForAlterCheck.x()]->item()->measure();
    for (int n = notesForAlterCheck.x(); n <= notesForAlterCheck.y(); ++n) {
      if (m_segments[n]->note()->note == oldNote.note || m_segments[n]->note()->note == newNote.note) {
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
  if (oldNote.note != newNote.note || oldNote.octave != newNote.octave)
    activeNote()->staff()->checkNotesRange();
// If there is a beam - prepare it again then draw
  if (activeNote()->wrapper()->beam()) {
    activeNote()->wrapper()->beam()->prepareBeam();
    if (!fitStaff)
      activeNote()->wrapper()->beam()->drawBeam();
  }
  if (fitStaff) {
    m_segments[notesForAlterCheck.x()]->item()->staff()->fit();
    if (m_segments[notesForAlterCheck.y()]->item()->staff() != m_segments[notesForAlterCheck.x()]->item()->staff())
      m_segments[notesForAlterCheck.y()]->item()->staff()->fit();
  }
  emit clicked();
}


void TscoreObject::openMusicXml(const QString& musicFile) {
  if (!musicFile.isEmpty()) {
    auto melody = new Tmelody();
    if (melody->grabFromMusicXml(musicFile)) {
      clearScore();
      m_notes.clear();
      setMeter(melody->meter()->meter());
      if (melody->clef() != m_clefType) {
        setClefType(melody->clef());
        emit clefTypeChanged();
      }
      int newKey = static_cast<int>(melody->key().value());
      if (newKey != keySignature()) {
        if (!m_keySignEnabled && qAbs(newKey) != 0) {
          m_keySignEnabled = true;
          emit keySignatureEnabledChanged();
        }
        setKeySignature(newKey);
        emit keySignatureChanged();
      }
      for (int n = 0; n < melody->length(); ++n) {
        addNote(melody->note(n)->p());
      }
      adjustScoreWidth();
    }
    delete melody;
  }
}


//#################################################################################################
//###################         Score switches           ############################################
//#################################################################################################

void TscoreObject::setKeySignatureEnabled(bool enKey) {
  if (enKey != m_keySignEnabled) {
    m_keySignEnabled = enKey;
    if (notesCount() > 0)
      adjustScoreWidth();
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
 * @p TmeasureObject during adding a note item calls @p TnoteObject::setNoteNameVisible() to create note name
 * This method iterates all notes and switches its state of displaying note name
 */
void TscoreObject::setShowNoteNames(bool showNames) {
CHECKTIME (
  if (m_showNoteNames != showNames) {
    m_showNoteNames = showNames;
    for (int n = 0; n < notesCount(); ++n)
      m_segments[n]->item()->setNoteNameVisible(m_showNoteNames);
    qDebug() << "[TscoreObject] note name changed for" << notesCount() << "notes";
  }
)
}


void TscoreObject::setNameColor(const QColor& nameC) {
CHECKTIME(
  if (m_nameColor != nameC) {
    m_nameColor = nameC;
    if (m_showNoteNames) {
      for (int n = 0; n < notesCount(); ++n) // with hope that all items have name item created
        m_segments[n]->item()->nameItem()->setProperty("styleColor", m_nameColor);
    }
  }
)
}


qreal TscoreObject::stavesHeight() {
  if (m_staves.isEmpty())
    return 0.0;
  auto last = lastStaff();
  return last->staffItem()->y() + last->staffItem()->height() * last->scale();
}


void TscoreObject::changeActiveNote(TnoteObject* aNote) {
  if (aNote != m_activeNote) {
    m_activeNote = aNote;
    emit activeNoteChanged();
  }
}


void TscoreObject::setActiveNotePos(qreal yPos) {
  if (yPos != m_activeYpos) {
    m_activeYpos = yPos;
    emit activeYposChanged();
  }
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
    case Tclef::Tenor_C: return Tnote(3, 4);
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

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TscoreObject::addStaff(TstaffObject* st) {
  st->setNumber(stavesCount());
  m_staves.append(st);
  if (m_staves.count() == 1) { // initialize first measure of first staff
    st->appendMeasure(m_measures.first());
    connect(st, &TstaffObject::upperLineChanged, this, &TscoreObject::upperLineChanged);
  }

  // next staves position ca be set only when staffItem is set, see TstaffObject::setStaffItem() then
  connect(st, &TstaffObject::hiNotePosChanged, [=](int staffNr, qreal offset){
    for (int i = staffNr; i < m_staves.size(); ++i) // move every staff about offset
      m_staves[i]->staffItem()->setY(m_staves[i]->staffItem()->y() + offset);
    emit stavesHeightChanged();
  });
  connect(st, &TstaffObject::loNotePosChanged, [=](int staffNr, qreal offset){
    if (m_staves.size() > 1 && staffNr < m_staves.size() - 1) { // ignore change of the last staff
      for (int i = staffNr; i < m_staves.size(); ++i) // move every staff about offset
        m_staves[i]->staffItem()->setY(m_staves[i]->staffItem()->y() + offset);
    }
    emit stavesHeightChanged();
  });
}

/**
 * More-less it means that staff @p sourceStaff has no space for @p count number of measures
 * starting from @p measureNr, but those measures belong to it still
 */
void TscoreObject::startStaffFromMeasure(TstaffObject* sourceStaff, int measureNr, int count) {
  TstaffObject* targetStaff = nullptr;
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


void TscoreObject::deleteStaff(TstaffObject* st) {
  if (st->measuresCount() < 1) {
      bool fixStaffNumbers = st != lastStaff();
      m_staves.removeAt(st->number());
      st->staffItem()->deleteLater();
      if (fixStaffNumbers) {
        for (int s = 0; s < stavesCount(); ++s)
          m_staves[s]->setNumber(s);
      }
  }
}


void TscoreObject::adjustScoreWidth() {
CHECKTIME (
  m_adjustInProgress = true;
  for (QList<TstaffObject*>::iterator s = m_staves.begin(); s != m_staves.end(); ++s) {
    auto curr = *s;
    curr->refresh();
  }
  m_adjustInProgress = false;
  updateStavesPos();
)
}


void TscoreObject::updateStavesPos() {
  if (m_adjustInProgress)
    return;
  TstaffObject* prev = nullptr;
  for (QList<TstaffObject*>::iterator s = m_staves.begin(); s != m_staves.end(); ++s) {
    auto curr = *s;
    if (prev && curr->number() < stavesCount() - 1)
      curr->staffItem()->setY(prev->staffItem()->y() + (prev->loNotePos() - curr->hiNotePos() + 4.0) * prev->scale()); // TODO scordature!
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
QPoint TscoreObject::tieRange(TnoteObject* n) {
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

//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TscoreObject::appendToNoteList(QList<Tnote>& l) {
  for (Tnote n : l) {
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


/**
 * By assigning duration of @p 1 to meter group of @p Tmeter::NoMeter we are bending score reality.
 * There is no note with such duration, but this way @p TmeasureObject will keep one note per measure
 * and shifting measures among staves will work without any changes - the same way as such as for rhythmic notes.
 * Only drawbacks are: many measure objects (wasting RAM), accidentals for every single note (measure contains only one note)
 */
void TscoreObject::updateMeterGroups() {
  m_meterGroups.clear();
  if (m_meter->meter() == Tmeter::NoMeter)
    m_meterGroups << 1;
  else if (m_meter->lower() == 4) { // simple grouping: one group for each quarter
    m_meterGroups << 24 << 48; // 2/4 and above
    if (m_meter->meter() > Tmeter::Meter_2_4)
      m_meterGroups << 72;
    if (m_meter->meter() > Tmeter::Meter_3_4)
      m_meterGroups << 96;
    if (m_meter->meter() > Tmeter::Meter_4_4)
      m_meterGroups << 120;
    if (m_meter->meter() > Tmeter::Meter_5_4)
      m_meterGroups << 144;
    if (m_meter->meter() > Tmeter::Meter_6_4)
      m_meterGroups << 168;
  } else {
    if (m_meter->meter() == Tmeter::Meter_3_8)
      m_meterGroups << 36;
    else if (m_meter->meter() == Tmeter::Meter_5_8)
      m_meterGroups << 36 << 60;
    else if (m_meter->meter() == Tmeter::Meter_6_8)
      m_meterGroups << 36 << 72;
    else if (m_meter->meter() == Tmeter::Meter_7_8)
      m_meterGroups << 36 << 60 << 84;
    else if (m_meter->meter() == Tmeter::Meter_9_8)
      m_meterGroups << 36 << 72 << 108;
    else if (m_meter->meter() == Tmeter::Meter_12_8)
      m_meterGroups << 36 << 72 << 108 << 144;
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


void TscoreObject::clearScore() {
  if (measuresCount() && firstMeasure()->noteCount() > 0) {
    for (TnotePair* s : m_segments) {
      s->flush();
      m_spareSegments << s;
    }
    qDeleteAll(m_measures);
    m_measures.clear();
    m_segments.clear();
    while (m_staves.count() > 1) {
      auto ls = m_staves.takeLast();
      ls->staffItem()->deleteLater();
    }
    m_measures << new TmeasureObject(0, this);
    lastStaff()->appendMeasure(firstMeasure());
    firstStaff()->setFirstMeasureId(0);
  }
}
