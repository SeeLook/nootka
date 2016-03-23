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
#include <music/trhythm.h>
#include <QtWidgets/qapplication.h>
#include <QtCore/qdebug.h>
#include <QtGui/qpen.h>
#include <QtGui/qpalette.h>


TscoreMeasure::TscoreMeasure(TmeasPatcher* dispatcher) :
  QObject(dispatcher),
  m_dispatcher(dispatcher)
{
  calcDuration();
  m_free = m_duration;
//   m_barLine = new QGraphicsLineItem(m_staff);
//   m_barLine->hide();
//   m_barLine->setPen(QPen(qApp->palette().text().color(), 0.2));
//   m_barLine->setZValue(50);
}


TscoreMeasure::~TscoreMeasure()
{
  qDebug() << "TscoreMeasure delete";
//   delete m_barLine;
}


void TscoreMeasure::updateMeter() {
  calcDuration();
  // TODO: recalculate or remove beams when no meter
}


void TscoreMeasure::insertNote(int id, TscoreNote* sn) {
  qDebug() << "measure got a note" << sn->rhythm()->xmlType();
  int noteDur = sn->rhythm()->duration();
  if (m_free - noteDur < 0) {
    qDebug() << "[MEASURE] full!!!" << m_free << noteDur;
  }
  m_notes.insert(id, sn);
  connect(sn, &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
  if (m_dispatcher->scoreMeter() && m_dispatcher->scoreMeter()->meter()->meter() != Tmeter::e_none) {
    recalculate(sn);
//     for (TscoreBeam* b : m_beams)
//       b->performBeaming();
  }
}


void TscoreMeasure::removeNote(int noteToRemove) {
  if (noteToRemove >= 0 && noteToRemove < m_notes.count()) {
      disconnect(m_notes[noteToRemove], &TscoreNote::noteGoingToChange, this, &TscoreMeasure::noteChangedSlot);
      m_notes.removeAt(noteToRemove);
      if (m_dispatcher && m_dispatcher->scoreMeter()->meter()->meter() != Tmeter::e_none)
        recalculate(0);
  } else
      qDebug() << "[TscoreMeasure] Tried to remove note out of range";
}


    /**
    * knowing reasons for recalculate() and their effects:
    * 1) rhythm changed:
    *   a| - rhythm value is differ then previous
    *    -- note by note have to be checked
    *    -- remove all beams then
    *   b| - only note was changed to rest or opposite
    *    -- leave other groups (if any) only recalculate that one belongs to changed note
    * 2) pitch changed ????
    * 3) accidental changed ????
    */
void TscoreMeasure::recalculate(TscoreNote* sn) {
  int free = m_duration;
  int notePos = 0;
  if (!sn || sn->rhythmChanged()) {
    /*if (sn && (sn->rhythm()->rhythm() == sn->newRhythm()->rhythm() && sn->rhythm()->hasDot() == sn->newRhythm()->hasDot() &&
               sn->rhythm()->isTriplet() == sn->newRhythm()->isTriplet() && sn->rhythm()->isRest() != sn->newRhythm()->isRest())) {
      qDebug() << "Recalculate only one group";
    } else*/
    { // recalculate entire measure
      qDeleteAll(m_beams);
      m_beams.clear();
      qDebug() << "recalculate entire measure";
      int gr = 0; // current beam group
      for (int i = 0 ; i < m_notes.size(); ++i) {
        int dur = m_notes[i]->newRhythm()->duration(); // more efficient to keep duration in a var then call Trhythm::duration() every time
          if (free - dur < 0) {
              qDebug() << "Measure is full at" << i;
          } else {
            free -= dur;
          }

        if (i > 0) {
          if (i == 1)
              notePos = m_notes[i - 1]->newRhythm()->duration();
      // resolve beaming
          if (m_notes[i]->newRhythm()->rhythm() > Trhythm::e_quarter && m_notes[i - 1]->newRhythm()->rhythm() > Trhythm::e_quarter && // are they 8 or 16
              notePos + dur <= m_dispatcher->scoreMeter()->groupPos(gr)) { // and fit into beam group
                if (m_notes[i - 1]->newRhythm()->beam() == Trhythm::e_noBeam) { // start beam group
                    m_notes[i - 1]->newRhythmToFix()->setBeam(Trhythm::e_beamStart);
                    m_notes[i - 1]->setRhythm(*m_notes[i - 1]->newRhythm());
                    m_beams << new TscoreBeam(m_notes[i - 1], this);
                }
                m_notes[i]->newRhythmToFix()->setBeam(Trhythm::e_beamCont);
                m_beams.last()->addNote(m_notes[i]);
          } else {
              int oldGr = gr;
              while (gr < m_dispatcher->scoreMeter()->groupCount() && notePos + dur > m_dispatcher->scoreMeter()->groupPos(gr))
                  gr++; // switch to next group
              if (m_notes[i - 1]->newRhythm()->beam() == Trhythm::e_beamCont) {
                if (oldGr != gr) // close beam group
                  m_beams.last()->closeBeam();
              }
          }
          notePos += dur;
        }
      }

      emit updateBeams(sn); // TODO update only beams that are changed, restore flags when beam was removed
    }
  } else if (sn->pitchChanged()) {
      sn->newRhythmToFix()->setBeam(sn->rhythm()->beam());
  }

//   qDebug() << "free" << free;
//   if (free == 0) { // show bar line
//       m_barLine->setLine(m_notes.last()->rightX(), m_staff->upperLinePos(),
//                         m_notes.last()->rightX(), m_staff->isPianoStaff() ? m_staff->lowerLinePos() : m_staff->upperLinePos() + 8.0);
//       m_barLine->show();
//   } else
//       m_barLine->hide();
}


int TscoreMeasure::firstNoteId() {
  return m_notes.count() ? m_notes.first()->index() : 0;
}


int TscoreMeasure::lastNoteId() {
  return m_notes.count() ? m_notes.last()->index() : 0;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TscoreMeasure::calcDuration() {
  if (m_dispatcher->scoreMeter())
    m_duration = m_dispatcher->scoreMeter()->meter()->duration();
  else
    m_duration = 0;
}


void TscoreMeasure::noteChangedSlot(TscoreNote* sn) {
  qDebug() << "noteChangedSlot - recalculate" << m_dispatcher->scoreMeter()->groupCount();
  recalculate(sn);
}

//#################################################################################################
//###################              TmeasPatcher        ############################################
//#################################################################################################
TmeasPatcher::TmeasPatcher(QObject* parent) :
  QObject(parent),
  m_scoreMeter(0)
{
//   m_measures << TscoreMeasure(this);
}


TmeasPatcher::~TmeasPatcher() {
  for (TscoreMeasure* m : m_measures)
    delete m;
}


void TmeasPatcher::setScoreMeter(TscoreMeter* sMeter) {
  m_scoreMeter = sMeter;
  // TODO: probably it is enough to call updateMeter() in first measure - it will invoke the rest one by one
  for (int i = 0 ; i < m_measures.size(); ++i)
    m_measures[i]->updateMeter();
}


void TmeasPatcher::addNote(int id, TscoreNote* sn) {
  if (m_measures.isEmpty()) {
      m_measures << new TscoreMeasure(this);
      m_measures.first()->insertNote(id, sn);
  } else {
      for (int m = 0; m < m_measures.size(); ++m) { // add note to appropriate measure
        if (sn->staff() == m_measures[m]->notes().first()->staff()) { // find a staff first
          if (id >= m_measures[m]->firstNoteId()) {
              m_measures[m]->insertNote(id - m_measures[m]->firstNoteId(), sn);
              break;
          }
        }
      }
  }
}


void TmeasPatcher::removeNote(int staffNr, int noteId) {
  for (int m = 0; m < m_measures.size(); ++m) { // add note to appropriate measure
    if (staffNr == m_measures[m]->notes().first()->staff()->number()) { // find a staff first
      if (noteId >= m_measures[m]->firstNoteId()) {
        m_measures[m]->removeNote(noteId - m_measures[m]->firstNoteId());
      }
    }
  }
}




