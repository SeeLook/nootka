/***************************************************************************
 *   Copyright (C) 2013-2017 by Tomasz Bojczuk                             *
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

#include "tscorestaff.h"
#include "tscorescene.h"
#include "tscoreclef.h"
#include "tscorenote.h"
#include "tscorekeysignature.h"
#include "tscorescordature.h"
#include "tnotecontrol.h"
#include "tscore5lines.h"
#include "tscoremeter.h"
#include "tscoremeasure.h"
#include "tscoretie.h"
#include <music/tnote.h>
#include <music/trhythm.h>
#include <music/tmeter.h>
#include <tnoofont.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtGui/qpalette.h>

#include <QtCore/qdebug.h>


void content(TscoreStaff* st) {
  QString c;
  for (int i = 0; i < st->count(); ++i) {
    c += QStringLiteral("<");
    if (st->noteSegment(i)->note()->isValid() && !st->noteSegment(i)->note()->isRest())
      c += st->noteSegment(i)->note()->toText();
    else
      c += QLatin1String("r");
    c += QLatin1String(">");
    c += QString("%1%2").arg(st->noteSegment(i)->note()->weight()).arg(st->noteSegment(i)->note()->hasDot() ? QStringLiteral(".") : QString());
    c += QString("(%1) ").arg(st->noteSegment(i)->rhythmGroup());
  }
  qDebug() << st->debug() << c;
}


TscoreStaff::TscoreStaff(TscoreScene* scene, int notesNr) :
  TscoreItem(scene),
  m_staffNr(-1), m_brace(0),
  m_keySignature(nullptr),
  m_upperLinePos(16.0), m_lowerStaffPos(0.0),
  m_height(38.0),
  m_viewWidth(0.0),
  m_offset(TnoteOffset(3, 2)),
  m_isPianoStaff(false),
  m_scordature(0), m_enableScord(false), m_tidyKey(false),
  m_selectableNotes(false), m_extraAccids(false),
  m_maxNotesCount(0),
  m_loNotePos(28.0), m_hiNotePos(12.0),
  m_allNotesWidth(0.0),
  m_gapFactor(1.0),
  m_shortestR(12),
  m_lockRangeCheck(false), m_autoAddedNoteId(-1),
  m_scoreMeter(nullptr)
{
  setFlag(QGraphicsItem::ItemHasNoContents);
  setZValue(10);
  setAcceptHoverEvents(true);
// Clef
  Tclef cl = Tclef();
  m_clef = new TscoreClef(scene, this, cl);
  connect(m_clef, SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
  m_clef->setZValue(29); // lower than key signature (if any)
  m_measures << new TscoreMeasure(this, 0);
// Notes
  for (int i = 0; i < notesNr; i++) {
      m_scoreNotes << new TscoreNote(scene, this, i);
      m_scoreNotes[i]->setPos(7.0 + i * m_scoreNotes[i]->boundingRect().width(), 0.0);
      m_scoreNotes[i]->setZValue(50);
      measures().last()->insertNote(i, m_scoreNotes[i]);
  }

// Staff lines, it also sets m_width of staff
  m_5lines = new Tscore5lines(scoreScene());
  m_5lines->setParentItem(this);
  prepareStaffLines();

  for (int i = 0; i < 7; i++) // reset accidentals array
    accidInKeyArray[i] = 0;
}


TscoreStaff::~TscoreStaff() {
  m_goingDelete = true;
  int measuresCnt = m_measures.count();
  for (int m = 0; m < measuresCnt; ++m)
    delete m_measures.takeLast();
  int notesCnt = count();
  for (int n = 0; n < notesCnt; ++n)
    delete m_scoreNotes.takeLast();
  if (m_scoreMeter)
    setMeter(Tmeter());
}

//####################################################################################################
//########################################## PUBLIC ##################################################
//####################################################################################################

Tclef::EclefType TscoreStaff::clef() {
  return scoreClef() ? scoreClef()->clef().type() : Tclef::NoClef;
}


qint8 TscoreStaff::keySignature() {
  return scoreKey() ? scoreKey()->keySignature() : 127;
}



int TscoreStaff::noteToPos(const Tnote& note)  {
  int nPos = m_offset.octave * 7 + m_offset.note + upperLinePos() - 1 - (note.octave * 7 + (note.note - 1));
  return nPos + ((isPianoStaff() && nPos > lowerLinePos() - 5) ? 2 : 0);
}

/**
 * Calculation of note position works As folow:
 * 1) expr: m_offset.octave * 7 + m_offset.note + upperLinePos() - 1 returns y position of note C in offset octave
 * 2) (note.octave * 7 + (note.note - 1)) is number of note to be set.
 * 3) Subtraction of them gives position of the note on staff with current clef and it is displayed
 * when this value is in staff scale.
 */
void TscoreStaff::setNote(int index, const Tnote& note) {
  if (index >= 0 && index < m_scoreNotes.size()) {
    auto sn = noteSegment(index);
    int prevPos = sn->notePos();
    sn->setNote(note);
    fit();
    if (prevPos != sn->notePos()) // check it only when note was really changed
      checkNoteRange();
  }
}


Tnote* TscoreStaff::getNote(int index) {
  return m_scoreNotes[index]->note();
}


TscoreNote* TscoreStaff::insertNote(int index, const Tnote& note, bool disabled) {
  if (m_autoAddedNoteId > -1) // naughty user can insert or add new note just after clicking the last one what invokes auto adding
    addNoteTimeOut();

  qDebug() << "\n";

  //TODO: check is inserting note fit into a measure:
  // - split it if it is longer, then shift the next measure

  int measureNr = m_measures.count() - 1; // add to the last measure by default
  if (index < count())
    measureNr = measureOfNoteId(index);
  if (measureNr < 0) {
    qDebug() << debug() << "Not such a measure for index," << index << " inserting to the last measure";
    measureNr = m_measures.count() - 1;
  }
  auto inserted = insertNote(note, index, disabled);

  qDebug() << debug() << "--> inserting note" << (inserted->note()->isValid() ? inserted->note()->toText() : "rest")
           << inserted->note()->rtm.xmlType() << (inserted->note()->hasDot() ? "." : QString())
           << inserted->rhythm()->xmlType()
           << "to measure" << measureNr;
  m_measures[measureNr]->insertNote(index - m_measures[measureNr]->firstNoteId(), inserted);

  if (number() > -1) {
    emit noteIsAdding(number(), inserted->index());
  }

  fit();
  updateNotesPos(/*index*/);
  if (number() == -1) {
      updateLines();
      updateSceneRect(); // Update only for single staff view
  }
  return inserted;
}


void TscoreStaff::insertNote(int index, bool disabled) {
  insertNote(index, Tnote(), disabled);
}


void TscoreStaff::addNote(const Tnote& note, bool disabled) {
  insertNote(m_scoreNotes.size(), note, disabled);
}


void TscoreStaff::removeNote(int index) {
  if (index >= 0 && index < count()) {
    emit noteIsRemoving(number(), index);
    if (m_autoAddedNoteId > -1) {
      if (index == m_autoAddedNoteId) // just automatically added note deleted by user
        m_autoAddedNoteId = -1;
      else
        m_autoAddedNoteId--;
    }
    auto removed = m_scoreNotes[index];
    qDebug() << debug() << "Preparing to remove note nr" << index << removed->note()->toText() << removed->note()->rtm.string();
    content(this);
    int measureNr = measureOfNoteId(index);
    if (measureNr > -1) {
        m_measures[measureNr]->removeNote(index - m_measures[measureNr]->firstNoteId());
    } else
        qDebug() << debug() << "not such a measure for note with index" << index;
//     m_allNotesWidth -= removed->width();
    qDebug() << debug() << "After measure routines index is" << index << "removing id" << removed->index() 
              << (index != removed->index() ? "BOOOOOOOM!!!!" : "");

    bool tieAfter = !goingDelete() && removed->note()->rtm.tie() == Trhythm::e_tieCont;
    delete m_scoreNotes.takeAt(index);

    if (m_measures.last()->isEmpty())
      delete m_measures.takeLast(); // it is empty, so delete it then

//     if (maxNoteCount() > count())
//     if (spaceForNotes() - m_allNotesWidth - m_gapsSum <= 7.0)
//         emit freeSpace(number(), 1);
    updateIndexes();
    fit();
    if (index < count())
      updateNotesPos(index);
    if (tieAfter) // restore tie between note before removed with note after removed (it should never crash if is TRUE)
      m_scoreNotes[index - 1]->tieWithNext();
    for (int i = index; i < count(); ++i) // refresh neutrals in all next notes TODO: IT HAS TO BE DONE BY MEASURE
      m_scoreNotes[i]->moveNote(m_scoreNotes[i]->notePos());
    if (number() == -1) // single note mode only
        updateSceneRect();
  }
}


void TscoreStaff::addNotes(int index, QList<TscoreNote*>& nList) {
//   if (index >= 0 && index <= count() && nList.size() <= maxNoteCount() - index) {
      for (int i = index; i < nList.size() + index; i++) {
        TscoreNote *sn = nList[i - index];
        m_scoreNotes.insert(i, sn);
        sn->setStaff(this);
        sn->setParentItem(this);
//         sn->setStaff(this);
      }
//   }
  updateIndexes();
//   updateNotesPos(index); // TODO: update position after resolving measure
//   updateIndexes();
  checkNoteRange(false);
}


void TscoreStaff::addNote(int index, TscoreNote* freeNote) {
  m_scoreNotes.insert(index, freeNote);
  freeNote->setStaff(this);
  freeNote->setParentItem(this);
//   freeNote->setStaff(this);
  updateNotesPos(index);
  updateIndexes();
}


void TscoreStaff::takeNotes(QList<TscoreNote*>& nList, int from, int to) {
  if (from >= 0 && from < count() && to < count() && to >= from) {
    for (int i = from; i <= to; i++) { // 'from' is always the next item after takeAt() on current one
      m_scoreNotes[from]->setParentItem(0); // to avoid deleting with staff as its parent
      nList << m_scoreNotes.takeAt(from);
    }
    if (m_scoreNotes.isEmpty())
        emit staffIsEmpty();
    else {
        updateNotesPos();
        updateIndexes();
    }
  }
}


void TscoreStaff::updateSceneRect() {
//   QRectF scRec = mapToScene(boundingRect()).boundingRect();
//   scene()->setSceneRect(0.0, 0.0, scRec.width() + (isPianoStaff() ? 2.5 : 1.5), scRec.height());
}


void TscoreStaff::setNoteDisabled(int index, bool isDisabled) {
  if (index >=0 && index < m_scoreNotes.size())
    m_scoreNotes[index]->setReadOnly(isDisabled);
}


void TscoreStaff::setEnableKeySign(bool isEnabled) {
  if (isEnabled != (bool)m_keySignature) {
    if (isEnabled) {
        m_keySignature = new TscoreKeySignature(scoreScene(), this);
        m_keySignature->setPos(6.5, upperLinePos() - TscoreKeySignature::relatedLine);
        m_keySignature->setClef(m_clef->clef());
        m_keySignature->setZValue(30);
        connect(m_keySignature, SIGNAL(keySignatureChanged()), this, SLOT(onKeyChanged()));
        m_flyAccid = new QGraphicsSimpleTextItem;
        registryItem(m_flyAccid);
        m_flyAccid->setFont(TnooFont(5));
        m_flyAccid->setScale(scoreScene()->accidScale());
        m_flyAccid->setZValue(255);
        m_flyAccid->hide();
        if (m_scoreNotes.size())
            m_flyAccid->setBrush(m_scoreNotes[0]->mainNote()->color());
        if (m_scoreMeter)
          m_scoreMeter->setPos(m_keySignature->x() + m_keySignature->boundingRect().width(), upperLinePos());
    } else {
        m_keySignature->blockSignals(true);
        m_keySignature->setKeySignature(0);
        onKeyChanged();
        delete m_keySignature;
        m_keySignature = 0;
        delete m_flyAccid;
        m_flyAccid = 0;
        if (m_scoreMeter)
          m_scoreMeter->setPos(6.5, upperLinePos());
    }
    updateLines();
    updateNotesPos();
  }
}


void TscoreStaff::setScordature(Ttune& tune) {
  if (!hasScordature()) {
    m_scordature = new TscoreScordature(scoreScene(), this);
    m_scordature->setParentItem(this);
    m_scordature->setZValue(35); // above key signature
  }
  m_scordature->setTune(tune);
  if (m_scordature->isScordatured())  {
      m_enableScord = true;
  } else { // nothing to show - standard tune
      delete m_scordature;
      m_scordature = 0;
      m_enableScord = false;
  }
  updateWidth();
  updateNotesPos();
}


void TscoreStaff::removeScordatute() {
  delete m_scordature;
  m_scordature = 0;
  m_enableScord = false;
  updateWidth();
  updateNotesPos();
}


void TscoreStaff::setMeter(const Tmeter& m) {
  bool changed = false;
  if (m.meter() != Tmeter::NoMeter && !m_scoreMeter) { // create score meter
      m_scoreMeter = new TscoreMeter(scoreScene(), this);
      m_scoreMeter->setPos(6.5 + (m_keySignature ? m_keySignature->boundingRect().width() : 0.0), upperLinePos());
      m_scoreMeter->setZValue(30);
      m_scoreMeter->setMeter(m);
      changed = true;
      connect(m_scoreMeter, &TscoreMeter::meterChanged, [=]{
            updateWidth();
            updateNotesPos();
      });
  } else if (m_scoreMeter && m.meter() == Tmeter::NoMeter) { // delete meter
      delete m_scoreMeter;
      m_scoreMeter = nullptr;
      if (m_keySignature)
        m_keySignature->setX(6.5);
      changed = true;
  } else if (m_scoreMeter)
      m_scoreMeter->setMeter(m); // score meter will call signal to update staff
  scoreScene()->setScoreMeter(m_scoreMeter);
  if (changed && !measures().isEmpty()) {
    measures().first()->changeMeter(m);
    for (int i = 0; i < m_scoreNotes.size(); i++) // set default rhythm value for all notes or hide all stems when no rhythms
      m_scoreNotes[i]->setRhythmEnabled((bool)m_scoreMeter);
    updateWidth();
    updateNotesPos();
  }
}



void TscoreStaff::setDisabled(bool disabled) {
  scoreClef()->setReadOnly(disabled);
  scoreClef()->setAcceptHoverEvents(!disabled); // stops displaying status tip
  if (scoreKey()) {
    scoreKey()->setAcceptHoverEvents(!disabled); // stops displaying status tip
    scoreKey()->setReadOnly(disabled);
  }
  for (int i = 0; i < count(); i++)
    m_scoreNotes[i]->setReadOnly(disabled);
  if (m_scoreMeter)
    m_scoreMeter->setReadOnly(!disabled);
//   setControlledNotes(!disabled);
}


QRectF TscoreStaff::boundingRect() const {
  return QRectF(0, 0, m_width, m_height);
}


int TscoreStaff::accidNrInKey(int noteNr, char key) {
  int accidNr;
  switch ((56 + notePosRelatedToClef(noteNr, m_offset)) % 7 + 1) {
    case 1: accidNr = 1; break;
    case 2: accidNr = 3; break;
    case 3: accidNr = 5; break;
    case 4: accidNr = 0; break;
    case 5: accidNr = 2; break;
    case 6: accidNr = 4; break;
    case 7: accidNr = 6; break;
  }
  if (key < 0)
    accidNr = 6 - accidNr;
  return accidNr;
}


void TscoreStaff::setPianoStaff(bool isPiano) {
  if (isPiano != m_isPianoStaff) {
    m_isPianoStaff = isPiano;
    if (isPiano) {
        m_upperLinePos = 14.0;
        m_lowerStaffPos = 28.0;
        m_height = 44.0;
        createBrace();
    } else {
        m_upperLinePos = 16.0;
        m_lowerStaffPos = 0.0;
        m_height = 38.0;
        delete m_brace;
    }
    prepareStaffLines();
    if (m_scoreMeter) {
        m_scoreMeter->setPianoStaff(m_isPianoStaff);
        m_scoreMeter->setY(upperLinePos());
    }
    if (m_keySignature)
        m_keySignature->setPos(6.5, upperLinePos() - TscoreKeySignature::relatedLine);
    for (int i = 0; i < count(); i++) {
      noteSegment(i)->adjustSize();
      noteSegment(i)->setAmbitus(isPiano ? 40 : 34, 2); // TODO It may cause problems when any other class will invoke note ambitus
    }
    emit pianoStaffSwitched();
  }
}


int TscoreStaff::fixNotePos(int pianoPos) {
  if (isPianoStaff() && pianoPos > lowerLinePos() - 4)
    return pianoPos - 2; // piano staves gap
  else
    return pianoPos;
}


void TscoreStaff::setViewWidth(qreal viewW) {
  qDebug() << debug() << "Old width" << m_viewWidth << "New width" << viewW;
  if (viewW != m_viewWidth) {
      m_viewWidth = viewW;
      if (viewW > 0.0)
        m_maxNotesCount = getMaxNotesNr(mapFromScene(viewW, 0.0).x());
      else
        m_maxNotesCount = 0;
      updateLines(); // calls updateWidth() as well
      updateNotesPos();
  }
}


void TscoreStaff::checkNoteRange(bool doEmit) {
  if (m_lockRangeCheck)
    return;
  qreal oldHi = m_hiNotePos, oldLo = m_loNotePos;
    findHighestNote();
    findLowestNote();
    if (doEmit && oldHi != m_hiNotePos)
      emit hiNoteChanged(number(), oldHi - m_hiNotePos);
    if (doEmit && oldLo != m_loNotePos)
      emit loNoteChanged(number(), m_loNotePos - oldLo);
    return;
}


int TscoreStaff::measureOfNoteId(int id) {
  if (m_measures.isEmpty()) {
    qDebug() << debug() << "No measures in this staff, can not find it for note" << id;
    return - 1;
  }
  if (m_measures.count() == 1)
    return 0;
  auto lastMeas = lastMeasure();
  if (lastMeas && !lastMeas->isEmpty()) {
    if (id >= lastMeas->lastNoteId())
      return m_measures.count() - 1; // last measure for the last note
  }
  for (int i = 0; i < m_measures.size(); ++i) {
    if (id >= m_measures[i]->firstNoteId() && id <= m_measures[i]->lastNoteId())
      return i;
  }

  // TODO: It should never occur, delete it
  qDebug() << debug() << "There is no measure for note id:" << id << "in this staff with notes number" << count();
  return -1;
}


TscoreMeasure* TscoreStaff::nextMeasure(TscoreMeasure* before) {
  if (before == lastMeasure()) {
      auto st = nextStaff();
      return st ? st->firstMeasure() : nullptr;
  } else
      return m_measures[before->id() + 1];
}


TscoreStaff* TscoreStaff::nextStaff() {
  auto st = this;
  emit getNextStaff(st);
  return st;
}


TscoreStaff * TscoreStaff::prevStaff() {
  auto st = this;
  emit getPrevStaff(st);
  return st;
}


/**
 * It is hard to imagine when other than the last measure of the staff is taken
 */
TscoreMeasure* TscoreStaff::takeMeasure(int measId) {
  qDebug()  << debug() << "Taking measure" << measId;
  if (measId >= 0 && measId < m_measures.count()) {
      if (m_measures[measId]->isEmpty()) {
        qDebug()  << debug() << "Measure" << measId << " to take is empty";
        return nullptr;
      }
      int firstIndex = m_measures[measId]->firstNoteId();
      int lastIndex = m_measures[measId]->lastNoteId();
      for (int i = firstIndex; i <= lastIndex; ++i) {
          m_scoreNotes[firstIndex]->setParentItem(0); // to avoid deleting with staff as its parent
          m_scoreNotes.removeAt(firstIndex); // when note segment with firstIndex is removed the next segment is at the firstIndex position
      }
      updateIndexes();
      auto retMeasure = m_measures.takeAt(measId); // take measure from list before fitting
      fit();
      // No need to update measure numbers
      // TODO: positioning?
      return retMeasure;
  } else
      qDebug() << debug() << "Unable to take measure" << measId << "out of range";
  return nullptr;
}


/**
 * Only entire measures are inserted, eventually partial, when it is the last measure,
 * so there is no need to recalculate measure (content and beaming) just set notes position.
 */
void TscoreStaff::insertMeasure(int id, TscoreMeasure* m) {
  if (m == nullptr) {
    qDebug() << debug() << "Trying to insert measure which is NULL of id:" << id;
    return;
  }
  qDebug() << debug() << "Inserting measure" << id;
  int noteId = 0;
  if (id > 0) // find index where to insert notes of the measure
      noteId = m_measures[id - 1]->lastNoteId() + 1;
  addNotes(noteId, m->notes());
  m_measures.insert(id, m);
//   addNotes(noteId, m->notes());
//   m->setStaff(this);
  for (int i = 0; i < m_measures.size(); ++i)
    m_measures[i]->setId(i);
  m->setStaff(this);
  fit();
  updateNotesPos();
}


char TscoreStaff::debug() {
  QTextStream o(stdout);
  o << "\033[01;34m[" << number() << " STAFF]\033[01;00m";
  return 32; // fake
}

//##########################################################################################################
//########################################## PROTECTED   ###################################################
//##########################################################################################################

void TscoreStaff::prepareStaffLines() {
  m_5lines->setPianoStaff(isPianoStaff());
  m_5lines->setPos(0.0, upperLinePos());
  updateLines();
  updateNotesPos();
}


void TscoreStaff::setEnableScordtature(bool enable) {
  if (enable != m_enableScord) {
    m_enableScord = enable;
    updateWidth();
    updateNotesPos();
  }
}


qreal TscoreStaff::notesOffset() {
  qreal off = 0.0;
  if (m_keySignature) {
      if (m_tidyKey)
        off = qAbs<char>(m_keySignature->keySignature()) * 1.3;
      else
        off = KEY_WIDTH + 1;
  } else if (m_enableScord)
      off = KEY_WIDTH / 2;
  if (m_scoreMeter)
    off += m_scoreMeter->width();
  return off;
}


void TscoreStaff::noteChangedWidth(int noteId) {
  Q_UNUSED(noteId)
  updateNotesPos();
}


void TscoreStaff::prepareNoteChange(TscoreNote* sn) {
  if (sn == nullptr)
    return;

  if (sn->rhythmChanged() || sn->accidChanged())
    fit();
}


TscoreNote* TscoreStaff::insertNote(const Tnote& note, int index, bool disabled) {
  index = qBound(0, index, m_scoreNotes.size()); // 0 - adds at the begin, size() - adds at the end
  if (index) {
    auto prev = m_scoreNotes[index - 1];
    if (prev->note()->rtm.tie() == Trhythm::e_tieStart || prev->note()->rtm.tie() == Trhythm::e_tieCont)
        prev->tieRemove(); // it will also set a proper tie of next note if the previous was connected with it before inserting
  }
  auto n = insert(index);
  setNote(index, note);
  m_scoreNotes[index]->setZValue(50);
  setNoteDisabled(index, disabled);
  updateIndexes();
  return n;
}


void TscoreStaff::fit() {
  if (m_scoreNotes.isEmpty()) {
    qDebug() << debug() << "Empty staff - nothing to fit";
    return;
  }

  int measureNr = 0, mCnt = 0;
  qreal factor = 2.0;
  m_gapsSum = 0.0;
  m_allNotesWidth = 0.0;
  bool needShift = false;

  for (int n = 0; n < m_scoreNotes.size(); ++n) {
      if (mCnt < m_measures.count() && !m_measures[mCnt]->isEmpty() && noteSegment(n) == m_measures[mCnt]->firstNote()) {
        measureNr = mCnt; // determine number of current measure
        mCnt++;
      }
      m_gapsSum += noteSegment(n)->space();
      m_allNotesWidth += noteSegment(n)->width();
      if (n > 1) {
        factor = (spaceForNotes() - m_allNotesWidth) / m_gapsSum;
        if (factor < 1.0) { // shift current measure and the next ones
          needShift = true;
          break; // rest of the notes goes to the next staff
        }
      }
  }
  if (needShift) {
      if (measureNr == 0) {
          qDebug() << debug() << "There is no space in the staff but measure is not full. Breaking measures is NOT IMPLEMENTED!";
      } else {
          qDebug() << debug() << "Fitting detects need of shifting from measure" << measureNr;
          if (m_measures.count() - measureNr - 1 > 1) {
            qDebug() << debug() << "THE NEXT STAFF WILL FIT ITSELF A FEW TIMES" << m_measures.count() - measureNr - 1;
            // TODO: if more measures are shifted this way, the next staff will fit itself a few times -TRY TO AVOID THAT
          }
          emit moveMeasure(this, m_measures.count() - 1); // it will perform fitting as well
      }
  } else if (factor > 2.0) { // staff has free space
      auto st = nextStaff();
      if (st && st != this && st->count()) {
        qDebug() << debug() << "staff has free space - getting fist measure of staff" << st->number();
        if (st->firstMeasure()->notesWidth() <= width() - contentWidth(1.0)) {
          insertMeasure(m_measures.count(), st->takeMeasure(0));
          st->updateNotesPos();
        }
      }
  }
  m_gapFactor = qBound(1.0, factor, 2.0); // notes in this staff are ready to positioning
  qDebug() << debug() << "fitting... Gap factor is" << m_gapFactor;
}


void TscoreStaff::shiftToMeasure(int measureNr, QList<TscoreNote*>& notesToShift) {
  qDebug() << debug() << "shift" << notesToShift.count() << "notes to measure" << measureNr;
  if (measureNr == m_measures.count()) { // no such a measure - create it first
    qDebug() << debug() << "Create new measure nr" << m_measures.count();
    m_measures << new TscoreMeasure(this, m_measures.count());
  }
  m_measures[measureNr]->prependNotes(notesToShift);
}


int TscoreStaff::shiftFromMeasure(int measureNr, int dur, QList<TscoreNote*>& notesToShift) {
  int retDur = 0;
  if (measureNr < m_measures.count()) {
      retDur = m_measures[measureNr]->takeAtStart(dur, notesToShift);
      if (m_measures[measureNr]->isEmpty()) {
        qDebug() << debug() << "Measure" << measureNr << "is empty - resetting duration" << retDur;
        retDur = 0; // next (the last) measure is not able to return required duration, no tie required
        delete m_measures.takeLast(); // it is empty, so delete it then
      }
  } else {
      auto st = nextStaff();
      if (st && st != this) {
        qDebug() << debug() << "Looking for notes in the next staff id" << st->number();
        st->shiftFromMeasure(0, dur, notesToShift);
        if (!notesToShift.isEmpty()) {
          QList<TscoreNote*> fakeList; // notesToShift has already all notes
          st->takeNotes(fakeList, notesToShift.first()->index(), notesToShift.last()->index());
          qDebug() << debug() << "Setting new staff for" << notesToShift.size() << "notes";
          addNotes(count(), notesToShift);
          content(this);
        }
      }
  }
  return retDur;
}


//##########################################################################################################
//####################################### PUBLIC SLOTS     #################################################
//##########################################################################################################

void TscoreStaff::onClefChanged(Tclef clef) {
  setPianoStaff(clef.type() == Tclef::PianoStaffClefs);
  switch(clef.type()) {
    case Tclef::Treble_G:
      m_offset = TnoteOffset(3, 2); break;
    case Tclef::Treble_G_8down:
      m_offset = TnoteOffset(3, 1); break;
    case Tclef::Bass_F:
      m_offset = TnoteOffset(5, 0); break;
    case Tclef::Bass_F_8down:
      m_offset = TnoteOffset(5, -1); break;
    case Tclef::Alto_C:
      m_offset = TnoteOffset(4, 1); break;
    case Tclef::Tenor_C:
      m_offset = TnoteOffset(2, 1); break;
    case Tclef::PianoStaffClefs:
      m_offset = TnoteOffset(3, 2); break;
    default: break;
  }
  m_lockRangeCheck = true;
  scoreClef()->setClef(clef);
  if (m_keySignature) {
      disconnect(m_keySignature, SIGNAL(keySignatureChanged()), this, SLOT(onKeyChanged()));
      m_keySignature->setClef(m_clef->clef());
      connect(m_keySignature, SIGNAL(keySignatureChanged()), this, SLOT(onKeyChanged()));
  }
  if (m_scoreNotes.size()) {
      for (int i = 0; i < m_scoreNotes.size(); i++) {
        if (m_scoreNotes[i]->notePos()) {
            setNote(i, *(m_scoreNotes[i]->note()));
        }
      }
  }
  m_lockRangeCheck = false;
  checkNoteRange();
  emit clefChanged(scoreClef()->clef());
}


void TscoreStaff::setTidyKey(bool tidy) {
  if (tidy != m_tidyKey) {
    m_tidyKey = tidy;
    updateLines();
    updateNotesPos();
  }
}


void TscoreStaff::applyAutoAddedNote() {
  if (m_autoAddedNoteId > -1) {
    emit noteIsAdding(number(), m_autoAddedNoteId);
//     if (m_autoAddedNoteId == maxNoteCount() - 1) // new staff is wanted
//     if (spaceForNotes() - m_allNotesWidth - m_gapsSum < 7.0)
//         emit noMoreSpace(number());
    m_autoAddedNoteId = -1;
  }
}


// TODO: keep var for it instead of following calculations (it could be always actual when take care of it during clef/meter/key width changes)
qreal TscoreStaff::spaceForNotes() {
  return width() - (m_clef ? m_clef->boundingRect().width() : 0.0)
                 - (m_keySignature ? m_keySignature->boundingRect().width() : 0.0)
                 - (m_scoreMeter ? m_scoreMeter->width() : 0.0);
}


qreal TscoreStaff::contentWidth(qreal gapFact) {
  return (m_clef ? m_clef->boundingRect().width() : 0.0)
          + (m_keySignature ? m_keySignature->boundingRect().width() : 0.0)
          + (m_scoreMeter ? m_scoreMeter->width() : 0.0)
          + m_allNotesWidth + m_gapsSum * gapFact;
}


bool TscoreStaff::hasSpaceFor(qreal newWidth) {
  return m_allNotesWidth + newWidth + (m_gapsSum * m_gapFactor) <= spaceForNotes();
//   return width() - m_scoreNotes.last()->rightX() >= newWidth;
}


/**
 * m_allNotesWidth + m_scoreNotes.first()->estimateWidth(n) ===> physical width of all notes
 */
bool TscoreStaff::hasSpaceFor(const Tnote& n) {
  return m_allNotesWidth + m_scoreNotes.first()->estimateWidth(n) + (m_gapsSum * m_gapFactor) <= spaceForNotes();
}

//##########################################################################################################
//####################################### PROTECTED SLOTS  #################################################
//##########################################################################################################

void TscoreStaff::onPianoStaffChanged(Tclef clef) {
  setPianoStaff(clef.type() == Tclef::PianoStaffClefs);
  scoreClef()->setClef(clef);
}


void TscoreStaff::onKeyChanged() {
  for (int i = 0; i < m_scoreNotes.size(); i++) {
    auto noteSeg = m_scoreNotes[i];
    if (noteSeg->notePos())
        noteSeg->moveNote(noteSeg->notePos());
  }
}


void TscoreStaff::onNoteClicked(int noteIndex) {
  updatePitch(noteIndex);
//   auto noteOfId = m_scoreNotes[noteIndex];
//   int globalNr = notePosRelatedToClef(fixNotePos(noteOfId->notePos()) + noteOfId->ottava() * 7, m_offset);
//   noteOfId->note()->note = (char)(56 + globalNr) % 7 + 1;
//   noteOfId->note()->octave = (char)(56 + globalNr) / 7 - 8;
//   noteOfId->note()->alter = (char)noteOfId->accidental();
  if (m_autoAddedNoteId > -1) {
      addNoteTimeOut();
  }

  for (int i = noteIndex + 1; i < count(); ++i) // refresh neutrals in all next notes // TODO: move it to measure
      m_scoreNotes[i]->moveNote(m_scoreNotes[i]->notePos());

  emit noteChanged(noteIndex);
  checkNoteRange();
}


void TscoreStaff::onNoteSelected(int noteIndex) {
//   if (selectableNotes() || controlledNotes()) { // no need to check, note does it
    emit noteSelected(noteIndex);
}



void TscoreStaff::onAccidButtonPressed(int accid) {
  scoreScene()->setCurrentAccid(accid);
  /** It is enough to do this as long as every TscoreNote handles mouseHoverEvent
   * which checks value set above and changes accidental symbol if necessary. */
}


void TscoreStaff::noteGoingDestroy(QObject* n) {
  if (n == m_noteWithAccidAnimed)
    m_noteWithAccidAnimed = 0;
}


void TscoreStaff::addNoteTimeOut() {
  if (m_autoAddedNoteId > -1) {
    if (noteSegment(m_autoAddedNoteId)->notePos()) { // automatically added note was set - approve it
        applyAutoAddedNote(); // puts m_autoAddedNoteId back to -1
        qDebug() << debug() << "auto note approved" << m_scoreNotes.last()->note()->rtm.xmlType() << m_scoreNotes.last()->rhythm()->xmlType();
        m_measures.last()->insertNote(m_scoreNotes.last()->index() - m_measures.last()->firstNoteId(), m_scoreNotes.last());
        fit();
    } else if (m_autoAddedNoteId != count() - 1) { // some note was added after this one - ignore
        m_autoAddedNoteId = -1;
    } else { // user gave up
        delete noteSegment(m_autoAddedNoteId);
        m_scoreNotes.removeAt(m_autoAddedNoteId);
        m_autoAddedNoteId = -1;
    }
  }
}


void TscoreStaff::updatePitch(int noteIndex) {
  auto sn = m_scoreNotes[noteIndex];
  int globalNr = notePosRelatedToClef(fixNotePos(sn->notePos()) + sn->ottava() * 7, m_offset);
  sn->note()->note = (char)(56 + globalNr) % 7 + 1;
  sn->note()->octave = (char)(56 + globalNr) / 7 - 8;
  sn->note()->alter = (char)sn->accidental();
  qDebug() << debug() << "updating pitch of" << noteIndex << sn->note()->toText();
}


//##########################################################################################################
//########################################## PRIVATE     ###################################################
//##########################################################################################################

void TscoreStaff::updateIndexes() {
  qDebug() << debug() << "updating indexes";
  for (int i = 0; i < m_scoreNotes.size(); i++)
    m_scoreNotes[i]->changeIndex(i); // Update index of next notes in the list
}


void TscoreStaff::updateNotesPos(int startId) {
  qDebug() << debug() << "updating notes positions from" << startId;
  if (m_scoreNotes.isEmpty())
    return;

  if (startId == 0)
    m_scoreNotes[0]->setX(6.5 + notesOffset());
  else
    m_scoreNotes[startId]->setX(m_scoreNotes[startId - 1]->rightX());

  int m = 0;
  for (int i = startId + 1; i < m_scoreNotes.size(); i++) { // update positions of the notes
      auto noteSeg = m_scoreNotes[i]; // cache pointer to TscoreNote for multiple reuse
      noteSeg->setX(m_scoreNotes[i - 1]->rightX());
      if (m < m_measures.count() && !m_measures[m]->isEmpty() && noteSeg == m_measures[m]->lastNote()) {
        m_measures[m]->checkBarLine();
        m++;
      }
      noteSeg->update();
  }
//     m_scoreNotes[i]->setPos(7.0 + off + i * m_scoreNotes[0]->boundingRect().width(), 0);
}


void TscoreStaff::updateLines() {
  updateWidth();
  m_5lines->setWidth(width());
}


void TscoreStaff::updateWidth() {
  qreal off = notesOffset();
  if (m_scoreNotes.size() < 1)
      m_width = 10.0 + off + 2.0;
  else
      m_width = 10.0 + off + m_scoreNotes.size() * m_scoreNotes[0]->boundingRect().width() + 2.0;
  if (m_viewWidth > 0.0)
      m_width = m_viewWidth;
}


void TscoreStaff::createBrace() {
  m_brace = new QGraphicsSimpleTextItem();
  registryItem(m_brace);
  m_brace->setFont(TnooFont(22));
  m_brace->setText(QString(QChar(0xe16c)));
  m_brace->setBrush(qApp->palette().text().color());
//   m_brace->setScale(22.18 / m_brace->boundingRect().height());
  m_brace->setScale(1.05619047619047619047);
  m_brace->setPos(-2.4 * m_brace->scale(), upperLinePos() + (22.18 - m_brace->boundingRect().height() * m_brace->scale()) / 2.0);
  m_brace->setZValue(7);
}


int TscoreStaff::getMaxNotesNr(qreal maxWidth) {
  maxWidth -= 1.0; // staff lines margins
  if (scoreClef())
    maxWidth -= CLEF_WIDTH;
  if (scoreKey())
    maxWidth -= KEY_WIDTH + 1;
  else if (hasScordature())
    maxWidth -= KEY_WIDTH / 2;
  return int(maxWidth / 7.0);
}


void TscoreStaff::findHighestNote() {
  m_hiNotePos = upperLinePos() - 4.0;
  for (int i = 0; i < m_scoreNotes.size(); i++) {
    auto noteSeg = m_scoreNotes[i];
    if (noteSeg->notePos()) // is visible
      m_hiNotePos = qMin(qreal(noteSeg->notePos() - (noteSeg->note()->rtm.stemDown() ? 2 : 4)), m_hiNotePos);
  }
}


void TscoreStaff::findLowestNote() {
  if (hasScordature()) {
    m_loNotePos = height();
    return;
  }
  m_loNotePos = (isPianoStaff() ? lowerLinePos(): upperLinePos()) + 13.0;
  for (int i = 0; i < m_scoreNotes.size(); i++)
      m_loNotePos = qMax(qreal(m_scoreNotes[i]->notePos() + (m_scoreNotes[i]->note()->rtm.stemDown() ? 4 : 2)), m_loNotePos);
}


TscoreNote* TscoreStaff::insert(int index) {
  auto newNote = new TscoreNote(scoreScene(), this, index);
  newNote->setZValue(50);
  m_scoreNotes.insert(index, newNote);
  return newNote;
}








