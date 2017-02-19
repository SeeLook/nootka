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

#include "tstaffobject.h"
#include "tscoreobject.h"
#include "tmeasureobject.h"
#include "tnoteobject.h"
#include "tnotepair.h"
#include "music/tnote.h"

#include <QtCore/qdebug.h>


TstaffObject::TstaffObject(QObject* parent) :
  QObject(parent),
  m_score(nullptr),
  m_upperLine(16.0),
  m_staffItem(nullptr)
{
}


TstaffObject::~TstaffObject() { 
  qDebug() << "[TstaffObject] is going delete";
}


void TstaffObject::setScore(TscoreObject* s) {
  m_score = s;
  setParent(s);
  qDebug() << "TstaffObject got a score parent" << s;
  m_score->addStaff(this);
}


void TstaffObject::addNote(TnotePair* np) {
  np->object()->setIndex(m_notes.count());
  m_notes.append(np);
  fit();
}


void TstaffObject::setNote(int noteNr, const Tnote& n) {
  if (noteNr < 0 || noteNr >= m_notes.count()) {
    qDebug() << "[TstaffObject] There is no note with number" << noteNr;
    return;
  }
//   m_notes[noteNr]->setNote(n);
}


void TstaffObject::setUpperLine(qreal upLine) {
  m_upperLine = upLine;
  emit upperLineChanged();
}


void TstaffObject::setStaffItem(QQuickItem* si) {
  m_staffItem = si;
}


int TstaffObject::firstMeasureNr() {
  return m_measures.first()->number();
}


int TstaffObject::number() const {
  return m_staffItem ? m_staffItem->property("number").toInt() : 0;
}


void TstaffObject::setNotesIndent(qreal ni) {
  if (m_notesIndent != ni) {
    m_notesIndent = ni;
    fit();
  }
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TstaffObject::fit() {
  if (m_notes.isEmpty()) {
    qDebug() << "[TstaffObject] Empty staff - nothing to fit";
    return;
  }

  qreal factor = 2.0;
  qreal m_gapsSum = 0.0;
  qreal m_allNotesWidth = 0.0;

  for (int n = 0; n < m_notes.size(); ++n) {
    m_gapsSum += m_notes[n]->object()->rhythmFactor();
    m_allNotesWidth += m_notes[n]->object()->width();
    if (n > 1) {
      factor = (m_staffItem->width() - m_notesIndent - m_allNotesWidth) / m_gapsSum;
//       if (factor < 1.0) { // shift current measure and the next ones
//         needShift = true;
//         break; // rest of the notes goes to the next staff
//       }
    }
  }
  m_gapFactor = qBound(1.0, factor, 2.0); // notes in this staff are ready to positioning
  updateNotesPos(0);
}


void TstaffObject::updateNotesPos(int startId) {
  qDebug() << "updating notes positions from" << startId;

  if (startId == 0)
    m_notes[0]->object()->setX(m_notesIndent);
  else
    m_notes[startId]->object()->setX(m_notes[startId - 1]->object()->rightX());

  for (int n = startId + 1; n < m_notes.size(); ++n) {
    m_notes[n]->object()->setX(m_notes[n - 1]->object()->rightX());
  }
}
