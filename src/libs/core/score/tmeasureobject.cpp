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
#include "music/tmeter.h"
#include "music/tnote.h"

#include <QtQml/qqmlengine.h>
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
  m_duration = m_score->meter()->duration();
  m_free = m_duration;
}


TmeasureObject::~TmeasureObject()
{
  qDebug() << debug() << "is going delete";
}


void TmeasureObject::setNumber(int nr) {
  m_number = nr;
}


void TmeasureObject::setStaff(TstaffObject* st) {
  if (m_staff != st) {
    m_staff = st;
    for (TnotePair* np : m_notes)
      np->object()->setStaff(m_staff);
  }
}


void TmeasureObject::appendNewNotes(int segmentId, int count) {
  // so far we are sure there is enough space for whole note list in this measure
//   qDebug() << debug() << "append" << count << "note(s) from" << segmentId;
  for (int n = segmentId; n < segmentId + count; ++n)
    m_notes.append(m_score->noteSegment(n));
  updateRhythmicGroups();
  // resolve beaming
  for (int n = segmentId; n < segmentId + count; ++n) {
    auto np = m_score->noteSegment(n);
    auto noteObject = new TnoteObject(m_staff);
    noteObject->setIndex(np->index());
    noteObject->setMeasure(this);
    noteObject->setNote(*np->note());
    np->setNoteObject(noteObject);
  }
  refresh();
  m_staff->refresh();
  checkBarLine();
}


void TmeasureObject::insertNote(int id, TnotePair* np) {
//   qDebug() << debug() << "inserting note at" << id;

  m_notes.append(np);
  updateRhythmicGroups();
//   if (np->object() == nullptr) {
//     auto noteObject = new TnoteObject(m_staff);
//     noteObject->setMeasure(this);
//     noteObject->setNote(*np->note());
//     np->setNoteObject(noteObject);
//     m_staff->addNote(np);
//   }
}


int TmeasureObject::firstNoteId() const {
  return m_notes.count() ? m_notes.first()->object()->index() : 0;
}


int TmeasureObject::lastNoteId() const {
  return m_notes.count() ? m_notes.last()->object()->index() : 0;
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
    notePos += m_notes[i]->note()->duration();
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
  if (m_free == 0) {
    auto lastNote = last()->object();
    if (!m_barLine) {
      QQmlEngine engine;
      QQmlComponent comp(&engine, this);
      comp.setData("import QtQuick 2.7; Rectangle { width: 0.3; height: 8 }", QUrl());
      m_barLine = qobject_cast<QQuickItem*>(comp.create());
      m_barLine->setParentItem(lastNote);
      m_barLine->setProperty("color", lastNote->color());
      m_barLine->setY(m_staff->upperLine());
    }
    qreal xOff = lastNote == m_staff->lastMeasure()->last()->object() ? 0.2 : 0.0; // fit line at the staff end
    m_barLine->setX(lastNote->rightX() - lastNote->x() + xOff);
  }
}


void TmeasureObject::refresh() {
  m_gapsSum = 0.0;
  m_allNotesWidth = 0.0;
  for (int n = 0; n < m_notes.size(); ++n) {
    auto noteObj = note(n)->object();
    m_gapsSum += noteObj->rhythmFactor();
    m_allNotesWidth += noteObj->width();
  }
}
