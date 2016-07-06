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
#include "tscorescene.h"
#include <music/tmeter.h>
#include <music/tnote.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>
#include <QtGui/qpen.h>
#include <QtGui/qpalette.h>


/** For debug purposes: displays content of given measure */
void content (TscoreMeasure* m) {
  QString c;;
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
  m_number(nr),
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
  qDebug() << "TscoreMeasure delete";
  delete[] m_firstInGr;
  delete m_barLine;
}


Tmeter* TscoreMeasure::meter() const {
  return m_staff->scoreScene()->scoreMeter()->meter();
}


TscoreMeter* TscoreMeasure::scoreMeter() const {
  return m_staff->scoreScene()->scoreMeter();
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
  qDebug() << debug() << "duration" << duration() << "remained" << m_free;
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


void TscoreMeasure::insertNote(int id, TscoreNote* sn) {
//   qDebug() << debug() << sn->note()->rtm.xmlType() << "inserting id" << id << "count" << m_notes.count() << "free" << m_free;
  int noteDur = sn->note()->duration();

  QList<TscoreNote*> notesToOut;
  Tnote newNote; // new note that has to be created when rhythm duration is split

  if (sn->note()->rhythm() != Trhythm::e_none && m_free - noteDur < 0) { // move notes to the next measure if not enough space
    int toRelease = releaseAtEnd(noteDur - m_free, notesToOut, newNote, id);
    if (toRelease > 0) { // There is still not enough space
      if (m_free > 0) { // measure has some free space for the new note but not for entire
          sn->setRhythm(Trhythm(m_free, sn->note()->isRest()));
          if (newNote.rhythm() != Trhythm::e_none) // TODO: remove when tested
            qDebug() << "NEW NOTE is already created!!!";
          newNote = Tnote(*sn->note(), Trhythm(noteDur - m_free, sn->note()->isRest()));
          noteDur = m_free;
      } else { // measure has not at all space
          qDebug() << debug() << "move entire note";
          notesToOut << sn;
          m_staff->shiftToMeasure(number() + 1, notesToOut);
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

  content(this);
}


void TscoreMeasure::removeNote(int noteToRemove) {
  if (noteToRemove >= 0 && noteToRemove < m_notes.count()) {
      disconnect(m_notes[noteToRemove], &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
      m_notes.removeAt(noteToRemove);
      // TODO: insert notes from next measure to fulfill removed rhythm
  } else
      qDebug() << "[TscoreMeasure] Tried to remove note out of range";
}


qreal TscoreMeasure::notesWidth() {
  qreal w = 0.0;
  for (TscoreNote* sn : m_notes)
    w += sn->width();
  return w;
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


QString TscoreMeasure::debug() {
  return QString("[MEASURE %1]").arg(number());
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
    qDebug() << "rhythm changed from" << prevDur << "to" << newDur << "free" << m_free;
    if (m_free - (newDur - prevDur) < 0) {
        qDebug() << debug() << "recalculate() needs release measure about" << newDur - prevDur - m_free;
        int rel = releaseAtEnd(newDur - prevDur - m_free, notesToOut, newNote, sn->index() - firstNoteId());
        qDebug() << "RECALCULATED, remained" << rel;
        updateRhythmicGroups();
        resolveBeaming(sn->rhythmGroup());
        checkBarLine();
        // TODO: Rather perform beaming after staff will change notes positions
//         for (TscoreBeam* b : m_beams) { // update beams but only after this note group
//           if (b->first()->rhythmGroup() > sn->rhythmGroup())
//             b->performBeaming();
//         }
    } else if (newDur == prevDur) {
        if (sn->note()->isRest() != sn->rhythm()->isRest())
          qDebug() << "note" << sn->index() << "changed to/from rest";
        resolveBeaming(sn->rhythmGroup(), sn->rhythmGroup());
        checkBarLine();
        // TODO: Rather perform beaming after staff will change notes positions
//         for (TscoreBeam* b : m_beams) { // update beams but only after this note group
//           if (b->first()->rhythmGroup() == sn->rhythmGroup())
//             b->performBeaming();
//         }
    }
  }

  shiftReleased(notesToOut, newNote);
}


void TscoreMeasure::updateRhythmicGroups() {
  if (duration() == 0)
    return; 

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
        if (!m_notes[noteNr]->note()->isRest() && !m_notes[noteNr - 1]->note()->isRest() // not a rest
            && m_notes[noteNr]->note()->rhythm() > Trhythm::e_quarter // sixteenth or eighth
            && m_notes[noteNr - 1]->note()->rhythm() > Trhythm::e_quarter)
        {
          if (m_notes[noteNr - 1]->note()->rtm.beam() == Trhythm::e_noBeam) // start beam group
              m_beams << new TscoreBeam(m_notes[noteNr - 1], this);
          if (!m_beams.isEmpty()) {
              m_notes[noteNr]->note()->rtm.setBeam(Trhythm::e_beamCont);
              m_beams.last()->addNote(m_notes[noteNr]);
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
int TscoreMeasure::releaseAtEnd(int dur, QList<TscoreNote*>& notesToOut, Tnote& newNote, int endNote)
{
  int noteNr = m_notes.count() - 1;
  while (noteNr >= endNote && dur > 0) {
      int lastDur = lastNote()->note()->duration();
      if (lastDur > dur) { // last note is longer than required space - split it then create and move the rest of its duration to the next measure
          lastNote()->setRhythm(Trhythm(lastDur - dur, lastNote()->note()->isRest()));
          newNote = Tnote(*lastNote()->note(), Trhythm(dur, lastNote()->note()->isRest()));
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


void TscoreMeasure::checkBarLine() {
  if (duration() && m_free == 0) { // update bar line
      if (m_barLine->parentItem() != lastNote()->parentItem()) {
        m_barLine->setParentItem(lastNote());
        QPointF barLinePos = lastNote()->mapFromParent(QPointF(lastNote()->rightX() - 0.2, 0.0));
        m_barLine->setLine(barLinePos.x(), lastNote()->staff()->upperLinePos(), barLinePos.x(), lastNote()->staff()->upperLinePos() + 8.0);
        m_barLine->show();
      }
  } else if (m_free > 0)
      m_barLine->hide();
  else // TODO: It should never occur - delete it when tested
      qDebug() << debug() << "ARE YOU MAD? Measure has more notes than the meter allows!";
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################
void TscoreMeasure::deleteBeams(int firstB) {
  int beamsCount = m_beams.count();
  for (int b = firstB; b < beamsCount; ++b)
    delete m_beams.takeAt(firstB);
}


void TscoreMeasure::addNewNote(Tnote& newNote) {
  qDebug() << debug() << "creating new note" << lastNote()->index() << newNote.rtm.xmlType();
  if (!m_staff->hasSpaceFor(newNote))
      qDebug() << "Such a kerfuffle! Staff has no space for new, split note";
  lastNote()->note()->rtm.setTie(Trhythm::e_tieStart);
  newNote.rtm.setTie(Trhythm::e_tieEnd);
  m_staff->insertNote(lastNote()->index() + 1, newNote);
}


void TscoreMeasure::shiftReleased(QList<TscoreNote*>& notesToOut, Tnote& newNote) {
  if (!notesToOut.isEmpty())
    m_staff->shiftToMeasure(number() + 1, notesToOut);

  if (newNote.rhythm() != Trhythm::e_none)
    addNewNote(newNote);
}




