/***************************************************************************
 *   Copyright (C) 2016 by Tomasz Bojczuk                                  *
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

#include "tscoremeasure.h"
#include "tscorenote.h"
#include "tscorestaff.h"
#include "tscoremeter.h"
#include "tscorebeam.h"
#include "tscoretie.h"
#include "tscorescene.h"
#include <music/tmeter.h>
#include <music/tnote.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>
#include <QtGui/qpen.h>
#include <QtGui/qpalette.h>


/** For debug purposes: displays content of given measure */
void content (TscoreMeasure* m) {
  QString c;
  if (m->isEmpty())
      c = QStringLiteral("is empty");
  else
      for (TscoreNote* sn : m->notes()) {
        c += QStringLiteral("<");
        if (sn->note()->isValid() && !sn->note()->isRest())
          c += sn->note()->toText();
        else
          c += QLatin1String("r");
        c += QLatin1String(">");
        c += QString("%1%2").arg(sn->note()->weight()).arg(sn->note()->hasDot() ? QStringLiteral(".") : QString());
        c += QString("(%1) ").arg(sn->rhythmGroup());
      }
  qDebug() << m->debug() << c;
}


/*static*/
int TscoreMeasure::groupDuration(const QList<TscoreNote*>& notes) {
  int total = 0;
  for (TscoreNote* sn : notes)
    total += sn->note()->duration();
  return total;
}

//#################################################################################################
//###################            TscoreMeasure         ############################################
//#################################################################################################
/**
 * This class is grouping notes (TscoreNote) into measure.
 * Notes are added through:
 * - @p insertNote() - adds single note
 * - @p prependNotes() - adds QList<TscoreNote*> at the measure beginning
 * - @p noteChangedSlot() - connected to every note when it changes the rhythm
 *
 * In @p updateRhythmicGroups() each note gets rhythmic group number
 * and array m_firstInGr keeps indexes of first note in each group.
 * (groups are defined in TscoreMeter class and depend on meter)
 * @p resolveBeaming() sets beams (creates TscoreBeam if necessary)
 * @p checkBarLine() manages measure bar line
 *
 * @p releaseAtEnd() takes a given duration of notes from the measure end
 * then @p shiftReleased() moves them to the next measure
 */
TscoreMeasure::TscoreMeasure(TscoreStaff* staff, int nr) :
  QObject(staff),
  m_id(nr),
  m_staff(staff),
  m_firstInGr(new qint8[1])
{
  m_duration = m_staff->scoreScene()->scoreMeter() ? m_staff->scoreScene()->scoreMeter()->meter()->duration() : 0;
  m_free = duration();
  m_barLine = new QGraphicsLineItem;
  m_barLine->hide();
  m_barLine->setPen(QPen(qApp->palette().text().color(), 0.2));
  m_barLine->setZValue(50);
}


TscoreMeasure::~TscoreMeasure()
{
  delete[] m_firstInGr;
  delete m_barLine;
}


Tmeter* TscoreMeasure::meter() const {
  return m_staff->scoreScene()->scoreMeter()->meter();
}


TscoreMeter* TscoreMeasure::scoreMeter() const {
  return m_staff->scoreScene()->scoreMeter();
}


void TscoreMeasure::setStaff(TscoreStaff* st) {
  m_staff = st;
  setParent(st);
  for (TscoreBeam* b : m_beams)
    b->changeStaff(st);

  if (st->firstMeasure() == this) { // this measure was the last and went at the beginning of the next staff
      auto first = firstNote();
      if (first->note()->rtm.tie() > Trhythm::e_tieStart) { // tie continues or ends on first note
        auto prev = first->prevNote();
        if (prev->tie())
          prev->tie()->checkStaves();

        else // TODO: delete if not occurs
          qDebug() << debug() << "first note has tie flag set but tie doesn't exists";
      }
  } else if (st->lastMeasure() == this) { // this measure was the first and went at the end of previous staff
      auto last = lastNote();
      if (last->tie())
        last->tie()->checkStaves();
  }
}


void TscoreMeasure::changeMeter(const Tmeter& m) {
  m_duration = m.duration();
  m_free = duration();
  // TODO: recalculate measure, move notes if not fit to the meter to next measure or ask for notes from next
  if (m.meter() == Tmeter::e_none) {
      for (TscoreNote* sn : m_notes) {
        sn->setRhythm(Trhythm(Trhythm::e_none));
      }
  } else {
      for (TscoreNote* sn : m_notes) {
        sn->setRhythm(Trhythm(m.lower() == 4 ? Trhythm::e_quarter : Trhythm::e_eighth, !sn->note()->isValid()));
        m_free -= sn->note()->duration();
      }
  }
//   qDebug() << debug() << "duration" << duration() << "remained" << m_free;
  // TODO: recalculate or remove beams when no meter
}


void TscoreMeasure::prependNotes(QList<TscoreNote*>& nl) {
  QList<TscoreNote*> notesToOut;
  Tnote newNote; // new note that has to be created when rhythm duration is split
  int notesDur = groupDuration(nl);

  if (nl.first()->note()->rhythm() != Trhythm::e_none && m_free - notesDur < 0) // move notes to the next measure if not enough space
    releaseAtEnd(notesDur, notesToOut, newNote);

  for (int i = 0; i < nl.size(); ++i) {
    m_notes.insert(i, nl[i]);
    connect(nl[i], &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
  }

  updateRhythmicGroups();
  resolveBeaming(m_notes.first()->rhythmGroup());
  checkBarLine();

  for (TscoreBeam* b : m_beams)
    b->performBeaming();

  shiftReleased(notesToOut, newNote);

  content(this);
}


void TscoreMeasure::appendNotes(QList<TscoreNote*>& nl) {
  if (groupDuration(nl) > m_free) { // TODO: It should never occur - delete checking after all
    qDebug() << debug() << "MEASURE HAS NO FREE SPACE FOR SO MANY NOTES. appendNotes skipped";
    return;
  }

  int lastIndex = lastNoteId() - firstNoteId();
  for (int i = 0; i < nl.size(); ++i) {
    m_notes.append(nl[i]);
    connect(nl[i], &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
  }

  updateRhythmicGroups();
  resolveBeaming(m_notes[lastIndex]->rhythmGroup());
  checkBarLine();
}


void TscoreMeasure::insertNote(int id, TscoreNote* sn) {
//   qDebug() << debug() << sn->note()->rtm.xmlType() << "inserting id" << id << "count" << m_notes.count() << "free" << m_free;
  int noteDur = sn->note()->duration();
  int resolveAfter = 0; // un-resolvable duration of new note

  QList<TscoreNote*> notesToOut;
  Tnote newNote; // new note that has to be created when rhythm duration is split

  if (sn->note()->rhythm() != Trhythm::e_none && m_free - noteDur < 0) { // move notes to the next measure if not enough space
    int toRelease = releaseAtEnd(noteDur - m_free, notesToOut, newNote, id);
    if (toRelease > 0) { // There is still not enough space
      if (m_free > 0) { // measure has some free space for the new note but not for entire
          if (Trhythm(m_free).rhythm() == Trhythm::e_none) {
              Trhythm subRhythm(noteDur - m_free, sn->note()->isRest());
              subRhythm.setTie(sn->note()->rtm.tie());
              subRhythm.setStemDown(sn->note()->rtm.stemDown());
              if (subRhythm.rhythm() == Trhythm::e_none)
                qDebug() << debug() << "can not shrink note to duration" << noteDur - m_free;
              sn->setRhythm(subRhythm);
              splitThenInsert(m_free, id, *sn->note(), sn->isReadOnly());
              insertNote(lastNoteId() + 1, sn);
              if (!sn->note()->isRest())
                lastNote()->tieWithNext();
              return;
          }
          Trhythm oldRhythm(sn->note()->rtm);
          sn->setRhythm(Trhythm(m_free, sn->note()->isRest()));
          copyRhythmParams(sn, oldRhythm);
//           sn->note()->rtm.setTie(oldRhythm.tie());
//           sn->note()->rtm.setStemDown(oldRhythm.stemDown());
          if (newNote.rhythm() != Trhythm::e_none) // TODO: remove when tested
            qDebug() << "NEW NOTE is already created!!!";
          Trhythm newRtm(noteDur - m_free, sn->note()->isRest());
          if (newRtm.rhythm() == Trhythm::e_none) // two notes have to be added
            resolveAfter = noteDur - m_free; // do it after
          else
            newNote = Tnote(*sn->note(), newRtm);
          noteDur = m_free;
      } else { // measure has not at all space
          qDebug() << debug() << "move entire note" << sn->note()->toText();
          notesToOut << sn;
          m_staff->shiftToMeasure(m_staff->measures().indexOf(this) + 1, notesToOut);
          return;
      }
    }
  }

  m_free -= noteDur;
  m_notes.insert(id, sn);

  qDebug() << debug() << "measure got a note" << sn->note()->rtm.xmlType() << "FREE" << m_free << "out" << notesToOut.count();
  connect(sn, &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);

  updateRhythmicGroups();
  resolveBeaming(sn->rhythmGroup());
  checkBarLine();

  shiftReleased(notesToOut, newNote);
  if (resolveAfter) {
    auto lastN = lastNote();
    splitThenInsert(resolveAfter, lastN->index() + 1, *lastN->note(), lastN->isReadOnly());
  }

  content(this);
}


void TscoreMeasure::removeNote(int noteToRemove) {
  if (noteToRemove >= 0 && noteToRemove < m_notes.count()) {
      int rmDur = m_notes[noteToRemove]->note()->duration();
      disconnect(m_notes[noteToRemove], &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
      if (m_barLine->parentItem() == m_notes[noteToRemove])
          m_barLine->setParentItem(nullptr); // reset bar-line parent - otherwise it will be deleted with the note
      m_notes.removeAt(noteToRemove);

      if (rmDur == duration()) {
        qDebug() << "A special case when removing note is as long as the measure - remove whole measure then";
      }
      m_free += rmDur;
      fill();
  } else
      qDebug() << debug() << "Tried to remove note out of range";
}


qreal TscoreMeasure::notesWidth() {
  if (!isEmpty())
    return lastNote()->rightX() - firstNote()->x();
  else
    return 0.0;
}


int TscoreMeasure::freeAt(int noteId) const {
  int total = 0;
  for (int i = noteId; i < m_notes.size(); ++i)
    total += m_notes[i]->note()->duration();
  return total;
}


int TscoreMeasure::durationAt(int noteId) const {
  int total = 0;
  int end = qMin<int>(noteId, m_notes.size());
  for (int i = 0; i < end; ++i)
    total += m_notes[i]->note()->duration();
  return total;
}


int TscoreMeasure::firstNoteId() const {
  return m_notes.count() ? m_notes.first()->index() : 0;
}


int TscoreMeasure::lastNoteId() const {
  return m_notes.count() ? m_notes.last()->index() : 0;
}


char TscoreMeasure::debug() {
  QTextStream o(stdout);
  o << " \033[01;33m[" << QString("%1/%2").arg(id()).arg(m_staff->number()) << " MEASURE]\033[01;00m";
  return 32; // fake
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TscoreMeasure::noteChangedSlot(TscoreNote* sn) {
  Tnote newNote; // new note that has to be created when rhythm duration is split
  QList<TscoreNote*> notesToOut;

  if (sn->rhythmChanged()) {
    int prevDur = sn->rhythm()->duration();
    int newDur = sn->note()->duration();
    qDebug() << debug() << "rhythm changed from" << prevDur << "to" << newDur << "free" << m_free;
    if (m_free - (newDur - prevDur) < 0) {
        qDebug() << debug() << "recalculate() needs release measure about" << newDur - prevDur - m_free;
        int rel = releaseAtEnd(newDur - prevDur - m_free, notesToOut, newNote, sn->index() - firstNoteId());
        qDebug() << debug() << "RECALCULATED, remained" << rel;
        updateRhythmicGroups();
        resolveBeaming(sn->rhythmGroup());
        checkBarLine();
    } else if (newDur == prevDur) {
        if (sn->note()->isRest() != sn->rhythm()->isRest())
          qDebug() << debug() << "note" << sn->index() << "changed to/from rest";
        resolveBeaming(sn->rhythmGroup(), sn->rhythmGroup());
        checkBarLine();
    } else { // measure duration is less than meter - take notes from the next measure
        m_free += prevDur - newDur;
        qDebug() << debug() << "needs duration" << m_free;
        fill();
    }
  }

  shiftReleased(notesToOut, newNote);
}


void TscoreMeasure::updateRhythmicGroups() {
  if (duration() == 0)
    return; 

  qDebug() << debug() << "Updating rhythmic groups";

  int notePos = 0, grNr = 0, currGr = 0;
  delete[] m_firstInGr;
  m_firstInGr = new qint8[scoreMeter()->groupCount()];
  m_firstInGr[0] = 0; // first note in measure also begins first rhythmic group
  for (int i = 0; i < m_notes.size(); ++i) {
    if (currGr != grNr) {
      m_firstInGr[grNr] = i;
      currGr = grNr;
    }
    m_notes[i]->setRhythmGroup(grNr);
    notePos += m_notes[i]->note()->duration();
    while (grNr < scoreMeter()->groupCount() && notePos >= scoreMeter()->groupPos(grNr))
      grNr++;
  }
  if (currGr < scoreMeter()->groupCount() - 1) { // fill the rest of the array
    for (int gr = currGr + 1; gr < scoreMeter()->groupCount(); ++gr)
      m_firstInGr[gr] = -1; // with '-1' - means no notes yet
  }
  m_free = m_duration - notePos;
}


void TscoreMeasure::resolveBeaming(int firstGroup, int endGroup) {
  if (m_notes.count() < 2)
    return;

  if (endGroup == -1)
    endGroup = scoreMeter()->groupCount() - 1;

  // delete beams in groups affected by rhythm change
  if (!m_beams.isEmpty()) {
    QMutableListIterator<TscoreBeam*> i(m_beams);
    while (i.hasNext()) {
      qint8 rg = i.next()->first()->rhythmGroup();
      if (rg >= firstGroup && rg <= endGroup) {
        delete i.value();
        i.remove();
      }
    }
  }

  // create beams
  while (firstGroup <= endGroup && m_firstInGr[firstGroup] > -1) {
    int noteNr = qMax(1, int(m_firstInGr[firstGroup]) + 1);
    while (noteNr < m_notes.count() && m_notes[noteNr]->rhythmGroup() == firstGroup) {
        auto noteSeg = m_notes[noteNr]; // cache pointer to TscoreNote for multiple reuse
        auto prevSeg = m_notes[noteNr - 1];
        if (!noteSeg->note()->isRest() && !prevSeg->note()->isRest() // not a rest
            && noteSeg->note()->rhythm() > Trhythm::e_quarter // sixteenth or eighth
            && prevSeg->note()->rhythm() > Trhythm::e_quarter)
        {
          if (prevSeg->note()->rtm.beam() == Trhythm::e_noBeam) // start beam group
              m_beams << new TscoreBeam(prevSeg, this);
          if (!m_beams.isEmpty()) {
              noteSeg->note()->rtm.setBeam(Trhythm::e_beamCont);
              m_beams.last()->addNote(noteSeg);
          }
        }
        noteNr++;
    }
    if (!m_beams.isEmpty() && m_beams.last()->last()->note()->rtm.beam() == Trhythm::e_beamCont)
        m_beams.last()->closeBeam();
    firstGroup++;
  }
}


/**
 *  iterate through notes in backward order (right to left), take note by note to release required duration
 *  create a list from taken notes to send it to the next measure
 *  split the latest note (the most right one in the measure) if necessary
 *  half of the duration remains in current measure at the end tied with
 *  a new note that has to be created and push to the beginning of the next measure
 */
int TscoreMeasure::releaseAtEnd(int dur, QList<TscoreNote*>& notesToOut, Tnote& newNote, int endNote) {
  int noteNr = m_notes.count() - 1;
  while (noteNr >= endNote && dur > 0) {
      auto lastN = lastNote();
      int lastDur = lastN->note()->duration();
      if (lastDur > dur) { // last note is longer than required space - split it then create and move the rest of its duration to the next measure
          if (Trhythm(lastDur - dur).rhythm() == Trhythm::e_none) { // subtracting can't be solved by single note
            split(lastN);                                           // then split on two notes
            continue;                                               // and call while loop again
          }
          Trhythm oldRhythm(lastN->note()->rtm); // preserve tie and stem direction
          lastN->setRhythm(Trhythm(lastDur - dur, lastN->note()->isRest()));
          copyRhythmParams(lastN, oldRhythm);
//           lastN->note()->rtm.setStemDown(oldRhythm.stemDown());
          if (oldRhythm.tie())
            lastN->note()->rtm.setTie(Trhythm::e_tieCont);
          newNote = Tnote(*lastN->note(), Trhythm(dur, lastN->note()->isRest()));
//           newNote.rtm.setStemDown(oldRhythm.stemDown());
          lastDur = dur;
      } else { // last note is the same long or smaller than required space - so move it to the next measure
          notesToOut << m_notes.takeLast();
          disconnect(notesToOut.last(), &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
          auto b = notesToOut.last()->beam();
          if (b) {
            m_beams.removeOne(b);
            delete b;
          }
      }
      dur -= lastDur;
      m_free += lastDur; // note was taken out so there is more free space in the measure
  }

  return dur;
}


int TscoreMeasure::takeAtStart(int dur, QList<TscoreNote*>& notesToOut) {
  qDebug() << debug() << "taking" << dur << "from the beginning";
  content(this);
  int noteNr = 0;
  int retDur = 0;
  while (noteNr < m_notes.count() && dur > 0) {
    int firstDur = firstNote()->note()->duration();
    if (firstDur > dur) { // first measure note is longer than required duration - shrink it and create new one
      auto first = firstNote();
      Trhythm oldRhythm(first->note()->rtm);
      if (Trhythm(firstDur - dur).rhythm() == Trhythm::e_none) { // subtracting can't be solved by single note
        split(first);                                            // then split on two notes
        continue;                                                // and call while loop again
      }
      firstNote()->note()->setRhythm(Trhythm(firstDur - dur, first->note()->isRest()));
      copyRhythmParams(first, oldRhythm);
      firstDur = dur;
      retDur = dur;
      dur = 0;
    } else { // first note is the same long or smaller than required space - so move it to the next measure
      notesToOut << m_notes.takeFirst();
      disconnect(notesToOut.last(), &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
      dur -= firstDur;
      auto b = notesToOut.last()->beam();
      if (b) {
        m_beams.removeOne(b);
        delete b;
      }
    }
    m_free += firstDur;
    noteNr++;
  }

  qDebug() << debug() << notesToOut.count() << "taken at start." << "Now measure has" << m_notes.count() << "notes";
  content(this);

  if (m_free)
    fill();

  return retDur;
}


void TscoreMeasure::checkBarLine() {
  if (duration() && m_free == 0) { // update bar line
      if (m_barLine->parentItem() != lastNote()->parentItem()) {
        m_barLine->setParentItem(lastNote());
        QPointF barLinePos = lastNote()->mapFromParent(QPointF(lastNote()->rightX() - 0.2, 0.0));
        m_barLine->setLine(barLinePos.x(), lastNote()->staff()->upperLinePos(), barLinePos.x(), lastNote()->staff()->upperLinePos() + 8.0);
        m_barLine->show();
      }
  } else if (m_free > 0) {
        m_barLine->hide();
  } else // TODO: It should never occur - delete it when tested
      qDebug() << debug() << "ARE YOU MAD? Measure has more notes than the meter allows!";
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################
void TscoreMeasure::addNewNote(Tnote& newNote) {
// prepare tie, if any to restore it after new note will be created
  auto lastN = lastNote();
  auto lastTie = lastN->note()->rtm.tie(); // backup tie state to revert it after new note will be added
//     preserveTie(lastTie, lastN);

// create a new note on the staff
  auto inserted = m_staff->insertNote(lastN->index() + 1, newNote);
  fixStemDirection(lastNote());
  qDebug() << debug() << "creating new note after" << lastN->index() << lastN->note()->toText() << newNote.rtm.string();

  if (!newNote.isRest()) {
    lastN->tieWithNext();
    if (lastTie == Trhythm::e_tieCont || lastTie == Trhythm::e_tieStart)
      inserted->tieWithNext();
  }
}


void TscoreMeasure::shiftReleased(QList<TscoreNote*>& notesToOut, Tnote& newNote) {
  if (!notesToOut.isEmpty())
    m_staff->shiftToMeasure(id() + 1, notesToOut);

  if (newNote.rhythm() != Trhythm::e_none)
    addNewNote(newNote);
}


void TscoreMeasure::fill() {
  QList<TscoreNote*> notesToShift;
  int remainDur = m_staff->shiftFromMeasure(id() + 1, m_free, notesToShift);
  qDebug() << debug() << "fill, remain" << remainDur << "to shift:" << notesToShift.count();
  for (int i = 0; i < notesToShift.size(); ++i) {
    m_notes.append(notesToShift[i]);
    connect(notesToShift[i], &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
  }
  if (remainDur) { // next measure has a part of a new note
      qDebug() << debug() << remainDur << "remained in the next measure";
      auto firstInNext = m_staff->measures()[id() + 1]->firstNote()->note();
      Tnote newNote(*firstInNext, Trhythm(remainDur, firstInNext->isRest()));
      auto inserted = m_staff->insertNote(newNote, lastNoteId() + 1, lastNote()->isReadOnly());
      copyRhythmParams(inserted, firstInNext->rtm);
//       fixStemDirection(inserted);
      insertNote(inserted->index() - firstNoteId(), inserted);
      m_staff->updateNotesPos();
      if (!inserted->note()->isRest()) // add a tie
        inserted->tieWithNext();
  } else {
      updateRhythmicGroups();
      resolveBeaming(0);
      checkBarLine();
  }

  content(this);
}


/**
 * This method is called when subtracting from rhythm value can't be solved in single note.
 * So given 'long' note is split, then calling method tries to subtract from smaller rhythm.
 * Because adding new, split note doesn't change measure duration,
 * this method simplifies process of adding new note to staff and measure.
 * It ties notes as well.
 */
void TscoreMeasure::split(TscoreNote* sn) {
  auto tieCopy = sn->note()->rtm.tie();
//   if (!sn->note()->isRest())
//     preserveTie(tieCopy, sn);

  TrhythmList splitList;
  sn->note()->rtm.spilt(splitList);
  sn->note()->rtm.setRhythm(splitList.first());
  Tnote n2(*sn->note(), splitList.last());
  auto inserted = m_staff->insertNote(n2, sn->index() + 1, sn->isReadOnly());
  fixStemDirection(inserted);
  m_notes.insert(inserted->index() - firstNoteId(), inserted);
  connect(inserted, &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
  inserted->setGroup(sn->group());

  if (!sn->note()->isRest())
    restoreTie(tieCopy, sn);
}


/**
 * Looks for smallest rhythm that given duration divides,
 * then tries to assembly that duration for two notes which are multiplicity of obtained smallest value.
 * Inserts that notes to staff using given @p id index
 */
int TscoreMeasure::splitThenInsert(int dur, int id, const Tnote& note, bool readOnly) {
  qDebug() << debug() << "splitThenInsert" << dur << note.toText() << note.rtm.string();
  int smallestDur = 0;
  for (int i = 1; i < 6; ++i) { // looking for smallest rhythm to divide given duration @p dur
    smallestDur = Trhythm(static_cast<Trhythm::Erhythm>(i)).duration(); // no triplets here
    if (smallestDur < dur) {
      if ((dur / smallestDur) * smallestDur == dur)
        break;
    }
  }
  if (smallestDur) {
      int chunksNr = dur / smallestDur; // number of smallest rhythm values in duration @p dur
      int step = 0;
      Trhythm r1, r2;
      do { // find when rhythms of two divided notes of duration are valid
          step++;
          r1.setRhythm((chunksNr - step) * smallestDur);
          r1.setRest(note.isRest());
          r2.setRhythm(smallestDur * step);
          r2.setRest(note.isRest());
      } while (step < chunksNr - 1 && r1.rhythm() != Trhythm::e_none && r2.rhythm() != Trhythm::e_none);

      if (r1.rhythm() != Trhythm::e_none && r2.rhythm() != Trhythm::e_none) {
        // We found it!
          auto note1 = m_staff->insertNote(id, Tnote(note, r2), readOnly);
          fixStemDirection(note1);
          auto note2 = m_staff->insertNote(note1->index() + 1, Tnote(note, r1), readOnly);
          fixStemDirection(note2);
          if (!note1->note()->isRest())
            note1->tieWithNext();
          // return remained duration
          return r2.duration();
      } else
          qDebug() << debug() << "Can't resolve duration of " << dur << "dividing by" << Trhythm(smallestDur).string();
  } else
      qDebug() << debug() << "Can't resolve duration" << dur << "and split it!";
  return 0;
}


void TscoreMeasure::fixStemDirection(TscoreNote* sn) {
  sn->note()->rtm.setStemDown(sn->notePos() <= 18);
}


void TscoreMeasure::copyRhythmParams(TscoreNote* sn, const Trhythm& r) {
//   sn->setRhythm(r);
  sn->note()->rtm.setTie(r.tie());
  sn->note()->rtm.setStemDown(r.stemDown());
}


/**
 * Depends on @p tieCopy, this methods removes ties from given note and/or from previous one
 * To restore ties call @p restoreTie() with the same value of @p tieCopy and the same @p TscoreNote.
 */
void TscoreMeasure::preserveTie(quint8 tieCopy, TscoreNote* thisNote) {
//   if (tieCopy) {
//     auto prevNote = thisNote->prevNote();
//     if (tieCopy == Trhythm::e_tieCont) {
//         prevNote->setTie(nullptr);
//         thisNote->setTie(nullptr);
//     } else if (tieCopy == Trhythm::e_tieStart) {
//         thisNote->setTie(nullptr);
//     } else { // end of tie
//         prevNote->setTie(nullptr);
//     }
//   }
}


/**
 * Restores ties.
 * It always ties @p thisNote with the next one (if the same)
 * and depends on @p tieCopy, adds ties to previous note and to the next one
 */
void TscoreMeasure::restoreTie(quint8 tieCopy, TscoreNote* thisNote) {
  qDebug() << debug() << "restoring tie of" << thisNote->note()->toText() << thisNote->note()->rtm.string();
  thisNote->tieWithNext();
  auto prevNote = thisNote->prevNote();
  auto nextNote = thisNote->nextNote();
    if (tieCopy == Trhythm::e_tieCont) {
        prevNote->tieWithNext();
        nextNote->tieWithNext();
    } else if (tieCopy == Trhythm::e_tieEnd)
        prevNote->tieWithNext();
    else // tie started
        nextNote->tieWithNext();
}

