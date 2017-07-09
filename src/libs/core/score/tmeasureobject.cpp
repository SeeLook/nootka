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

#include "tmeasureobject.h"
#include "tscoreobject.h"
#include "tstaffobject.h"
#include "tnoteobject.h"
#include "tnotepair.h"
#include "tbeamobject.h"
#include "music/tmeter.h"
#include "music/tnote.h"

#include <QtQml/qqmlcomponent.h>
#include <QtCore/qdebug.h>


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


void TmeasureObject::setStaff(TstaffObject* st) {
  if (m_staff != st) {
    m_staff = st;
    for (TnotePair* np : qAsConst(m_notes))
      np->item()->setStaff(m_staff);
  }
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
      np->setNoteObject(new TnoteObject(m_staff, np));
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
    while (firstInGrId < m_score->notesCount()) {
      auto ns = m_score->noteSegment(firstInGrId);
      TbeamObject *curBeam = nullptr;
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

//   qDebug() << debug() << "Updating rhythmic groups" << m_free;
}


void TmeasureObject::checkBarLine() {
  if (m_free == 0 && m_score->meter()->meter() != Tmeter::NoMeter) {
    auto lastNote = last()->item();
    if (!m_barLine) {
      m_staff->score()->component()->setData("import QtQuick 2.9; Rectangle { width: 0.3 }", QUrl());
      m_barLine = qobject_cast<QQuickItem*>(m_staff->score()->component()->create());
      m_barLine->setParentItem(lastNote);
      m_barLine->setProperty("color", lastNote->color());
    }
    qreal xOff = lastNote == m_staff->lastMeasure()->last()->item() ? 0.2 : 0.0; // fit line at the staff end
    m_barLine->setX(lastNote->rightX() - lastNote->x() + xOff);
    m_barLine->setY(m_staff->upperLine());
    m_barLine->setHeight(m_score->isPianoStaff() ? 22.0 : 8.0);
  }
}


void TmeasureObject::meterChanged() {
  m_duration = m_score->meter()->duration();
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
      m_accidsState[np->note()->note - 1] = np->note()->alter; // register accidental of a note
  }
}


int TmeasureObject::beamGroup(int segmentId) {
  int currGr = m_score->noteSegment(segmentId)->rhythmGroup();
  int segId = m_firstInGr[currGr] + 1;
  int grWithBeam = -1;

//   QList<QList<int>> beamLists;
//   QList<int> l1;
//   beamLists << l1;
//   for (int bb = segId - 1; bb < m_notes.count() && m_notes[bb]->rhythmGroup() == currGr; ++bb) {
//     auto prevSeg = m_notes[bb];
//     if (!prevSeg->note()->isRest() && prevSeg->note()->rhythm() > Trhythm::Quarter) {
//       auto lastBeam = beamLists.last();
//       if (!lastBeam.isEmpty()) {
//         if (lastBeam.last() < bb - 1) { // a new beam if there was a rest in between notes in current rhythm group
//           QList<int> newBeam;
//           beamLists << newBeam;
//         }
//       }
//       beamLists.last() << bb;
//     }
//   }
//   for (int b = 0; b < beamLists.size(); ++b)
//     qDebug() << debug() << "beam" << b << ":" << beamLists[b];

  while (segId < m_notes.count() && m_notes[segId]->rhythmGroup() == currGr) {
    auto noteSeg = m_notes[segId];
    auto prevSeg = m_notes[segId - 1];
    if (!noteSeg->note()->isRest() && !prevSeg->note()->isRest() // not a rest
      && noteSeg->note()->rhythm() > Trhythm::Quarter // sixteenth or eighth
      && prevSeg->note()->rhythm() > Trhythm::Quarter)
    {
        if (prevSeg->note()->rtm.beam() == Trhythm::e_noBeam) // start beam group
          prevSeg->setBeam(new TbeamObject(prevSeg, this));
        auto beam = prevSeg->beam();
        if (noteSeg->beam() == nullptr)
          beam->addNote(noteSeg);
        grWithBeam = currGr;
    }
    segId++;
  }
  return grWithBeam;
}

//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TmeasureObject::clearAccidState() {
  for (int i = 0; i < 7; ++i)
    m_accidsState[i] = 100; // note doesn't occur in a measure
}
