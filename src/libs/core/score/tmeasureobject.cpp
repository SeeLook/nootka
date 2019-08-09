/***************************************************************************
 *   Copyright (C) 2017-2019 by Tomasz Bojczuk                             *
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

#include "tmeasureobject.h"
#include "tscoreobject.h"
#include "tstaffitem.h"
#include "tnoteitem.h"
#include "tnotepair.h"
#include "tbeamobject.h"
#include "music/tmeter.h"
#include "music/tnote.h"

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtCore/qdebug.h>


void content(TmeasureObject* m) {
  QString c = QStringLiteral("| ");
  for (int n = 0; n < m->noteCount(); ++n)
    c += QString("(%1)").arg(m->note(n)->index()) + QLatin1String("<") + m->note(n)->note()->toText() + QLatin1String(">") + m->note(n)->note()->rtm.string() + QLatin1String(" ");
  c += QStringLiteral("|");
  qDebug() << m->debug() << c;
}


TmeasureObject::TmeasureObject(int nr, TscoreObject* parent) :
  QObject(parent),
  m_number(nr),
  m_score(parent),
  m_staff(nullptr),
  m_firstInGr(new qint8[1]),
  m_barLine(nullptr)
{
  clearAccidState();
  m_duration = m_score->meter()->duration();
  m_free = m_duration;

  connect(qApp, &QGuiApplication::paletteChanged, [=]{
    if (m_barLine)
      m_barLine->setProperty("color", qApp->palette().text().color());
  });
}


TmeasureObject::~TmeasureObject()
{
  if (m_barLine)
    delete m_barLine;
  delete[] m_firstInGr;
//   qDebug() << debug() << "is going delete";
}


void TmeasureObject::setNumber(int nr) {
  m_number = nr;
}


void TmeasureObject::setStaff(TstaffItem* st) {
  if (m_staff != st) {
    m_staff = st;
    for (TnotePair* np : qAsConst(m_notes))
      np->item()->setStaff(m_staff);
  }
}


int TmeasureObject::durationFrom(int id) {
  int dur = m_free;
  if (id < noteCount()) {
    for (int n = id; n < noteCount(); ++n)
      dur += note(n)->note()->duration();
  } else // TODO It should never occur, delete if so
    qDebug() << debug() << "FIXME! This note doesn't belong to this measure or doesn't exist at all!" << id;
  return dur;
}


int TmeasureObject::durationBefore(TnoteItem* it) {
  int dur = 0;
  bool found = false;
  for (int n = 0; n < noteCount(); ++n) {
    if (note(n)->item() != it)
        dur += note(n)->note()->duration();
    else {
      found = true;
      break;
    }
  }
  return found ? dur : 0;
}


void TmeasureObject::appendNewNotes(int segmentId, int count) {
  // so far we are sure there is enough space for whole note list in this measure
//   qDebug() << debug() << "append" << count << "note(s) from" << segmentId << "measure duration" << duration();
  for (int n = segmentId; n < segmentId + count; ++n)
    m_notes.append(m_score->noteSegment(n));
  updateRhythmicGroups();
  int grWithBeam = beamGroup(segmentId);
  for (int n = segmentId; n < segmentId + count; ++n) {
    auto np = m_score->noteSegment(n);
    if (np->item() == nullptr)
      np->setNoteItem(new TnoteItem(m_staff, np));
    else
      np->item()->setStaff(m_staff);
    np->item()->setMeasure(this);
    checkAccidentals();
    np->item()->setNote(*np->note());
    if (m_score->showNoteNames())
      np->item()->setNoteNameVisible(true);
  }
  if (grWithBeam > -1) {
    auto firstInGrId = m_score->noteSegment(firstNoteId() + m_firstInGr[grWithBeam])->index();
    TbeamObject *curBeam = nullptr;
    while (firstInGrId < m_score->notesCount()) {
      auto ns = m_score->noteSegment(firstInGrId);
      if (ns->beam()) {
        if (ns->beam() != curBeam) {
          ns->beam()->prepareBeam();
          curBeam = ns->beam();
        }
      }
      ++firstInGrId;
    }
  }
  refresh();
  m_staff->refresh();
  checkBarLine();
}


void TmeasureObject::insertNotes(Tpairs& nList, int startId) {
  int listDur = TnotePair::pairsDuration(nList);
  Tpairs outNotes;
  if (listDur > m_score->meter()->duration())
    qDebug() << debug() << "FIXME! Ooh, notes to insert are longer than entire measure can contain!" << listDur;

  if (listDur > m_free) {
    int leftDur = releaseAtEnd(listDur - m_free, outNotes, startId);
    if (leftDur) // TODO it should never happen - DELETE IT!
      qDebug() << debug() << "FIXME! Can't insert" << listDur << leftDur << listDur - m_free;
  }
  for (int n = 0; n < nList.count(); ++n) {
    auto noteIn = nList[n];
    m_notes.insert(startId, noteIn);
    if (noteIn->item() == nullptr)
      noteIn->setNoteItem(new TnoteItem(m_staff, noteIn));
    else if (m_staff != noteIn->item()->staff())
      noteIn->item()->setStaff(m_staff);
    noteIn->item()->setMeasure(this);
  }
  update(m_notes[startId]->rhythmGroup()); // nList.first()->rhythmGroup()
  shiftReleased(outNotes);
}


void TmeasureObject::insertNote(TnoteItem* afterItem) {
  if (afterItem) {
    int afterIdInBar = afterItem->index() - afterItem->measure()->firstNoteId();
    int possibleDur = afterItem->measure()->durationFrom(afterIdInBar);
    int workDur = m_score->workRhythm().duration();
    // cut note duration if it is longer than possible measure duration starting from the item
    auto newRtmList = Trhythm::resolve(workDur > possibleDur ? possibleDur : workDur);
    Tpairs nl;
    for (Trhythm rtm : qAsConst(newRtmList)) {
      Tnote newNote(0, 0, 0, Trhythm(rtm.rhythm(), true));
      auto np = m_score->insertSilently(afterItem->index(), newNote, this);
      m_notes.removeAt(afterIdInBar);
      nl << np;
    }
    insertNotes(nl, afterIdInBar);
  }
}


void TmeasureObject::removeNote(TnotePair* n) {
  m_free += n->item()->note()->duration(); // n->note() is already null here
  m_notes.takeAt(n->index() - firstNoteId());
  fill();
}


void TmeasureObject::removeLastNote() {
  if (m_free == 0 && m_barLine) {
    m_barLine->setVisible(false);
    m_barLine->setParentItem(nullptr);
  }
  auto noteToRemove = m_notes.takeLast();
  updateRhythmicGroups();
  // TODO Try to use resolveBeaming here
  if (noteToRemove->beam()) {
    if (noteToRemove->beam()->count() < 3)
      noteToRemove->beam()->deleteBeam();
    else
      noteToRemove->beam()->removeNote(noteToRemove);
    int segId = m_firstInGr[noteToRemove->rhythmGroup()];
    while (segId < m_notes.count()) { // update notes of entire rhythm group
      m_notes[segId]->approve();
      segId++;
    }
  }
  refresh();
  m_staff->refresh();
}


void TmeasureObject::keySignatureChanged() {
  for (int n = 0; n < m_notes.size(); ++n) {
    m_notes[n]->item()->keySignatureChanged();
  }
  refresh();
}


int TmeasureObject::firstNoteId() const {
  return m_notes.isEmpty() ? 0 : m_notes.first()->index();
}


int TmeasureObject::lastNoteId() const {
  return m_notes.isEmpty() ? 0 : m_notes.last()->index();
}


char TmeasureObject::debug() {
  QTextStream o(stdout);
  o << " \033[01;33m[" << QString("%1/%2").arg(number() + 1).arg(m_staff ? m_staff->number() + 1 : -1) << " MEASURE]\033[01;00m";
  return 32; // fake
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmeasureObject::flush() {
  if (m_barLine)
    m_barLine->setVisible(false);
  m_notes.clear();
  setStaff(nullptr);
  m_allNotesWidth = 0.0;
  m_gapsSum = 0.0;
  delete[] m_firstInGr;
  m_firstInGr = new qint8[1];
}


void TmeasureObject::updateRhythmicGroups() {
  if (duration() == 0)
    return; 

  int notePos = 0, grNr = 0, currGr = 0;
  delete[] m_firstInGr;
  m_firstInGr = new qint8[m_score->groupCount()];
  m_firstInGr[0] = 0; // first note in measure also begins first rhythmic group
  for (int i = 0; i < m_notes.size(); ++i) {
    if (currGr != grNr) {
      m_firstInGr[grNr] = i;
      currGr = grNr;
    }
    m_notes[i]->setRhythmGroup(grNr);
    /** We are cheating here: no-rhythm-note gets duration of 1 - such a rhythm doesn't exists */
    notePos += m_notes[i]->note()->rhythm() == Trhythm::NoRhythm ? 1 : m_notes[i]->note()->duration();
    while (grNr < m_score->groupCount() && notePos >= m_score->groupPos(grNr))
      grNr++;
  }
  if (currGr < m_score->groupCount() - 1) { // fill the rest of the array
    for (int gr = currGr + 1; gr < m_score->groupCount(); ++gr)
      m_firstInGr[gr] = -1; // with '-1' - means no notes yet
  }
  m_free = m_duration - notePos;
}


void TmeasureObject::checkBarLine() {
  if (m_free == 0 && m_score->meter()->meter() != Tmeter::NoMeter) {
//     qDebug() << debug() << "check bar line";
    auto lastNote = last()->item();
    if (!m_barLine) {
      m_staff->score()->component()->setData("import QtQuick 2.9; Rectangle { width: 0.3 }", QUrl());
      m_barLine = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
      m_barLine->setProperty("color", qApp->palette().text().color());
      m_barLine->setParentItem(lastNote);
    }
    m_barLine->setParentItem(lastNote);
    m_barLine->setVisible(true);
    qreal xOff = lastNote == m_staff->lastMeasure()->last()->item() ? 0.2 : 0.0; // fit line at the staff end
    m_barLine->setX(lastNote->rightX() - lastNote->x() + xOff);
    m_barLine->setY(m_staff->upperLine());
    m_barLine->setHeight(m_score->isPianoStaff() ? 30.0 : 8.0);
  }
}


void TmeasureObject::meterChanged() {
  m_duration = m_score->meter()->duration();
  updateRhythmicGroups();
}


void TmeasureObject::refresh() {
  m_gapsSum = 0.0;
  m_allNotesWidth = 0.0;
  for (int n = 0; n < m_notes.size(); ++n) {
    auto noteObj = note(n)->item();
    m_gapsSum += noteObj->rhythmFactor();
    m_allNotesWidth += noteObj->width();
  }
}


void TmeasureObject::checkAccidentals() {
  clearAccidState();
  for (int n = 0; n < m_notes.size(); ++n) {
    auto np = note(n);
    if (np->note()->isValid() && !np->note()->isRest())
      m_accidsState[np->note()->note() - 1] = np->note()->alter(); // register accidental of a note
  }
}


int TmeasureObject::beamGroup(int segmentId) {
  int currGr = m_score->noteSegment(segmentId)->rhythmGroup();
  int segId = m_firstInGr[currGr] + 1;
  if (segId < 0) { // TODO it should never happen - DELETE IT!
    qDebug() << debug() << "FIXME! beamGroup()";
    return -1;
  }
  int grWithBeam = -1;
  while (segId < m_notes.count() && m_notes[segId]->rhythmGroup() == currGr) {
    auto noteSeg = m_notes[segId];
    auto prevSeg = m_notes[segId - 1];
    if (!noteSeg->note()->isRest() && !prevSeg->note()->isRest() // not a rest
      && noteSeg->note()->rhythm() > Trhythm::Quarter // sixteenth or eighth
      && prevSeg->note()->rhythm() > Trhythm::Quarter
      && (!m_score->isPianoStaff() || noteSeg->note()->onUpperStaff() == prevSeg->note()->onUpperStaff())
    )
    {
        if (prevSeg->note()->rtm.beam() == Trhythm::e_noBeam) // start beam group
          prevSeg->setBeam(m_score->getBeam(prevSeg, this));
        auto beam = prevSeg->beam();
        if (noteSeg->beam() == nullptr)
          beam->addNote(noteSeg);
        grWithBeam = currGr;
    }
    segId++;
  }
  return grWithBeam;
}


void TmeasureObject::noteGoingRest(TnotePair* np) {
  if (np->beam()) {
    if (np->beam()->count() > 2) {
      if (np->beam()->removeNote(np))
        np->beam()->deleteBeam();
    } else
        np->beam()->deleteBeam();
    int segId = m_firstInGr[np->rhythmGroup()];
    while (segId < m_notes.count() && m_notes[segId]->rhythmGroup() == np->rhythmGroup()) { // update notes of entire rhythm group
      m_notes[segId]->approve();
      segId++;
    }
  }
}


void TmeasureObject::restGoingNote(TnotePair* np) {
  QList<QList<int>> beamLists;
  QList<int> l1;
  beamLists << l1;
  for (int bb = m_firstInGr[np->rhythmGroup()]; bb < m_notes.count() && m_notes[bb]->rhythmGroup() == np->rhythmGroup(); ++bb) {
    auto prevSeg = m_notes[bb];
    if (!prevSeg->note()->isRest() && prevSeg->note()->rhythm() > Trhythm::Quarter) {
      auto lastBeam = beamLists.last();
      if (!lastBeam.isEmpty()) {
        if (lastBeam.last() < bb - 1) { // a new beam if there was a rest in between notes in current rhythm group
          QList<int> newBeam;
          beamLists << newBeam;
        }
      }
      beamLists.last() << bb;
    }
  }
  int noteId = np->index() - firstNoteId();
  for (int bl = 0; bl < beamLists.size(); ++bl) {
    QList<int>& beam = beamLists[bl];
    if (beam.size() > 1 && noteId >= beam.first() && noteId <= beam.last()) {
      TbeamObject *newBeam = nullptr;
      for (int b = beam.first(); b <= beam.last(); ++b) {
        auto noteInBeam = m_notes[b];
        if (noteInBeam->beam())
          delete noteInBeam->beam();
        if (newBeam)
          newBeam->addNote(noteInBeam);
        else
          newBeam = m_score->getBeam(noteInBeam, this);
      }
      newBeam->prepareBeam();
      newBeam->drawBeam();
    }
  }
}


void TmeasureObject::changeNoteDuration(TnotePair* np, const Tnote& newNote) {
  int prevDur = np->note()->duration();
  int newDur = newNote.duration();
  if (prevDur == newDur) {
    qDebug() << debug() << "FIXME! calling changeNoteDuration() when duration doesn't changed";
    return;
  }
  Tnote nn = newNote;
  Tpairs notesToOut;
  if (m_free - (newDur - prevDur) < 0) { // There is not enough space for new note - its duration is longer than possible free space in the measure
    /** 1. Try to release measure (move notes after this @p np one to the next measure) */
      int leftDur = releaseAtEnd(newDur - prevDur - m_free, notesToOut, np->index() - firstNoteId() + 1);
      if (leftDur) {
    /** 2. There is still not enough space for new duration - so cut duration of this @p np note */
        auto thisBarRtms = Trhythm::resolve(m_free + prevDur);
        nn.setRhythm(thisBarRtms.first());
        for (int r = 1; r < thisBarRtms.count(); ++r) {
          if (!newNote.isRest())
            thisBarRtms[r].setTie(Trhythm::e_tieCont);
          m_score->insertSilently(np->index() + r, Tnote(newNote, thisBarRtms[r]), this);
        }
      }
      np->setNote(nn);

      update(np->rhythmGroup());
      checkBarLine();
  } else { // measure duration is less than meter - take notes from the next measure
      m_free += prevDur - newDur;
      np->setNote(nn);
      fill(); // it updates measure
  }
  shiftReleased(notesToOut);
}


void TmeasureObject::resolveBeaming(int firstGroup, int endGroup) {
  if (noteCount() < 2)
    return;

  if (endGroup == -1)
    endGroup = m_score->groupCount() - 1;

  // delete beams in group range of [firstGroup to endGroup]
  int noteId = m_firstInGr[firstGroup];
  if (noteId < 0) { // TODO it should never happen - DELETE IT!
    qDebug() << debug() << "FIXME! wrong grouping";
    return;
  }
  int grId = firstGroup;
  while (noteId < noteCount() && grId <= endGroup) {
    if (m_notes[noteId]->beam())
      m_notes[noteId]->beam()->deleteBeam();
    noteId++;
    if (noteId < noteCount())
      grId = m_notes[noteId]->rhythmGroup();
  }

  for (int g = firstGroup; g <= endGroup; ++g) {
    if (m_firstInGr[g] < 0 || m_firstInGr[g] > noteCount() - 1)
      break;
    int gr = beamGroup(m_firstInGr[g] + firstNoteId());
    if (gr > -1 && gr < m_score->groupCount()) {
      auto beam = m_notes[m_firstInGr[gr]]->beam();
      if (beam) {
        beam->prepareBeam(); // TODO what about more beams in the same group??
      }
    }
  }
  // approve beam changes for all eighths and sixteenths that have no beam just flag - to revert the flag
  if (m_firstInGr[firstGroup] >= 0 && m_firstInGr[firstGroup] < noteCount()) {
    for (int n = m_firstInGr[firstGroup]; n < noteCount(); ++n) {
      auto note = m_notes[n];
      if (!note->beam() && !note->note()->isRest() && note->note()->rhythm() > Trhythm::Quarter)
        note->approve();
    }
  }
}


/**
 *  - iterate through notes in backward order (right to left), take note by note to release required duration
 *  - create a list from taken notes to send it to the next measure
 *  - split the latest note (the most right one in the measure) if necessary
 *  - half of the duration remains in current measure at the end tied with
 *    a new note that has to be created and push to the beginning of the next measure
 */
int TmeasureObject::releaseAtEnd(int dur, Tpairs& notesToOut, int endNote) {
  int noteNr = m_notes.count() - 1;
  while (noteNr >= endNote && dur > 0) {
    auto lastNote = last();
    int lastDur = lastNote->note()->duration();
    if (lastDur > dur) { // last note is longer than required space - split it and create and move the rest of its duration to the next measure
        auto rList = Trhythm::resolve(lastDur - dur);
        auto lastTie = lastNote->note()->rtm.tie();
        if (!lastNote->note()->isRest()) {
          rList.first().setTie(lastTie > Trhythm::e_tieStart ? Trhythm::e_tieCont : Trhythm::e_tieStart);
        }
        for (int r = 1; r < rList.count(); ++r) {
          if (!lastNote->note()->isRest())
            rList[r].setTie(Trhythm::e_tieCont);
          m_score->insertSilently(lastNote->index() + r, Tnote(*lastNote->note(), rList[r]), this);
        }
        lastNote->setNote(Tnote(*lastNote->note(), rList.first()));
        // remaining part of the note that goes to next measure
        auto rtmToNext = Trhythm::resolve(dur);
        int indexToInsert = rtmToNext.count() > 1 ? 0 : notesToOut.count();
        for (int r = 0; r < rtmToNext.count(); ++r) {
          if (!lastNote->note()->isRest()) {
            if (r < rtmToNext.count() - 1)
              rtmToNext[r].setTie(Trhythm::e_tieCont);
            else
              rtmToNext[r].setTie(lastTie == Trhythm::e_tieCont ? lastTie : Trhythm::e_tieEnd);
          }
          m_score->insertSilently(last()->index() + r + 1, Tnote(*lastNote->note(), rtmToNext[r]), this);
          notesToOut.insert(indexToInsert, m_notes.takeLast());
        }
        lastDur = dur; // instead of: dur = 0; m_free += lastDur; lastDur = 0;
    } else { // last note is the same long or smaller than required space - so move it to the next measure
        notesToOut << m_notes.takeLast();
        // TODO maybe clear beams here
    }
    dur -= lastDur;
    m_free += lastDur; // note was taken out so there is more free space in the measure
    noteNr--;
  }

  return dur;
}


void TmeasureObject::releaseAtStart(int dur, Tpairs& notesToOut) {
  int retDur = 0;
  TnotePair* firstNote;
  Trhythm::Etie firstTie;
  while (!m_notes.isEmpty() && dur > 0) {
    firstNote = first();
    int firstDur = firstNote->note()->duration();
    if (firstDur > dur) { // first measure note is longer than required duration - shrink it and create new one
        auto rList = Trhythm::resolve(firstDur - dur);
        firstTie = firstNote->note()->rtm.tie();
        if (!firstNote->note()->isRest())
          rList.first().setTie(firstTie > Trhythm::e_tieStart ? Trhythm::e_tieCont : Trhythm::e_tieEnd);
        firstNote->setNote(Tnote(*firstNote->note(), rList.first()));
        for (int r = 1; r < rList.count(); ++r) {
          if (!firstNote->note()->isRest())
            rList[r].setTie(Trhythm::e_tieCont);
          m_score->insertSilently(firstNoteId(), Tnote(*firstNote->note(), rList[r]), this);
        }
        firstDur = dur;
        retDur = dur;
        dur = 0;
    } else { // first note is the same long or smaller than required space - so move it to the next measure
        notesToOut << m_notes.takeFirst();
        dur -= firstDur;
        // TODO maybe clear beams here
    }
    m_free += firstDur;
  }


  if (m_free)
    fill();

  if (retDur) {
    // remaining part of the note that goes to previous measure
    auto rtmToPrev = Trhythm::resolve(retDur);
    for (int r = 0; r < rtmToPrev.count(); ++r) {
      if (!firstNote->note()->isRest()) {
        if (r < rtmToPrev.count() - 1)
          rtmToPrev[r].setTie(Trhythm::e_tieCont);
        else
          rtmToPrev[r].setTie(firstTie == Trhythm::e_tieCont ? firstTie : Trhythm::e_tieStart);
      }
      m_score->insertSilently(firstNoteId(), Tnote(*firstNote->note(), rtmToPrev[r]), this);
      notesToOut.append(m_notes.takeFirst());
    }
  }

  if (m_free && m_barLine) {
    m_barLine->setVisible(false);
    m_barLine->setParentItem(nullptr);
  }
}


void TmeasureObject::insertSilently(int id, TnotePair* np) {
  m_notes.insert(id, np);
  if (np->item() == nullptr)
    np->setNoteItem(new TnoteItem(m_staff, np));
  else if (m_staff != np->item()->staff())
    np->item()->setStaff(m_staff);
  np->item()->setMeasure(this);
  np->item()->setNote(*np->note());
  if (m_score->showNoteNames())
    np->item()->setNoteNameVisible(true);
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TmeasureObject::clearAccidState() {
  for (int i = 0; i < 7; ++i)
    m_accidsState[i] = 100; // note doesn't occur in a measure
}


void TmeasureObject::shiftReleased(Tpairs& notesToOut) {
  if (!notesToOut.isEmpty())
    m_staff->shiftToMeasure(m_number + 1, notesToOut);
}


void TmeasureObject::fill() {
  Tpairs notesToShift;
  m_staff->shiftFromMeasure(m_number + 1, m_free, notesToShift);
  int lastId = lastNoteId() + 1;

  for (int i = 0; i < notesToShift.count(); ++i)
    insertSilently(lastId + i, notesToShift[i]);

  update();
  if (m_free && m_barLine) {
    m_barLine->setVisible(false);
    m_barLine->setParentItem(nullptr);
  }
}


void TmeasureObject::update(int beamGrToResolve) {
  updateRhythmicGroups();
  checkAccidentals();
  resolveBeaming(beamGrToResolve);
  refresh();
}
