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

#include <QtGui/qguiapplication.h>
#include <QtGui/qpalette.h>
#include <QtQml/qqmlengine.h>
#include <QtCore/qdebug.h>


TstaffObject::TstaffObject(QObject* parent) :
  QObject(parent),
  m_score(nullptr),
  m_upperLine(16.0),
  m_staffItem(nullptr),
  m_loNotePos(28.0), m_hiNotePos(12.0),
  m_number(-1),
  m_firstMeasureId(0), m_lastMeasureId(-1)
{
}


TstaffObject::~TstaffObject() {
  qDebug() << "[TstaffObject] is going delete";
}


void TstaffObject::setScore(TscoreObject* s) {
  m_score = s;
  m_score->addStaff(this);
}


TmeasureObject* TstaffObject::firstMeasure() { return m_score->measure(m_firstMeasureId); }

TmeasureObject* TstaffObject::lastMeasure() { return m_score->measure(m_lastMeasureId); }


void TstaffObject::refresh() {
  fit();
  checkNotesRange();
}


void TstaffObject::setUpperLine(qreal upLine) {
  m_upperLine = upLine;
  emit upperLineChanged();
}


void TstaffObject::setStaffItem(QQuickItem* si) {
  m_staffItem = si;
  if (m_score->stavesCount() > 1) { // initial staff position, depends on lowest note in the previous staff
    auto prevStaff = m_score->staff(m_score->stavesCount() - 2);
    m_staffItem->setY(prevStaff->staffItem()->y() + (prevStaff->loNotePos() - hiNotePos() + 4.0) * prevStaff->scale()); // TODO hasScordature
  }
}


int TstaffObject::firstMeasureNr() {
  return m_lastMeasureId == -1 ? 0 : (m_firstMeasureId < m_score->measuresCount() ? m_score->measure(m_firstMeasureId)->number() : 0);
}


/**
 * Indent changes when key signature accidentals are added. It changes gap factor, can cause measure shifting.
 * For multiple staves on the score, last staff gets indent change at the end of queue.
 * Only then gap factor can be calculated.
 */
void TstaffObject::setNotesIndent(qreal ni) {
  if (m_notesIndent != ni) {
    m_notesIndent = ni;
    if (this == m_score->lastStaff())
      m_score->onIndentChanged();
  }
}


qreal TstaffObject::scale() { return m_staffItem ? staffItem()->property("scale").toReal() : 1.0; }


char TstaffObject::debug() {
  QTextStream o(stdout);
  o << "\033[01;34m[" << number() + 1 << " STAFF]\033[01;00m";
  return 32; // fake
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

#define BARLINE_OFFSET (2.0)

void TstaffObject::fit() {
  if ((m_number == 0 && m_score->measure(m_firstMeasureId)->isEmpty()) || m_lastMeasureId == -1 || measuresCount() < 1) {
    qDebug() << debug() << "Empty staff - nothing to fit";
    return;
  }

  qreal factor = 2.5;
  m_gapsSum = 0.0;
  m_allNotesWidth = 0.0;
  qreal availableWidth;

  for (int m = m_firstMeasureId; m <= m_lastMeasureId; ++m) {
    auto measure = m_score->measure(m);
    m_allNotesWidth += measure->allNotesWidth() + (m > m_firstMeasureId ? BARLINE_OFFSET : 0.0); // add bar line space
    m_gapsSum += measure->gapsSum();
    availableWidth = m_score->width() - m_notesIndent - m_allNotesWidth - 1.0;
    factor = availableWidth / m_gapsSum;
    if (factor < 0.8) { // shift current measure and the next ones
      if (m == m_firstMeasureId) { // first measure in the staff
          qDebug() << debug() << "!!!!!! Split this measure among staves !!!!!";
          break;
      } else {
          m_gapsSum -= measure->gapsSum();
          m_allNotesWidth -= measure->allNotesWidth();
          if (m > m_firstMeasureId)
            m_allNotesWidth -= BARLINE_OFFSET;
          m_gapFactor = (m_score->width() - m_notesIndent - m_allNotesWidth - 1.0) / m_gapsSum;  // allow factor bigger than 2.5
          createExtraTie(measure->first()->item());
          m_score->startStaffFromMeasure(this, m, m_lastMeasureId - (m - 1));
          m_lastMeasureId = m - 1;
          updateNotesPos();
          checkNotesRange();
          if (!m_score->adjustInProgress())
            m_score->staff(m_number + 1)->refresh();
          m_score->updateStavesPos();
          return;
      }
    }
  }

  if (factor > 1.5 && this != m_score->lastStaff()) {
    int m = m_lastMeasureId + 1;
    if (m >= m_score->measuresCount()) { // TODO delete debug message if not occurs
        qDebug() << debug() << "Next staff exists but there are no more measures. IT SHOULD NEVER HAPPEN!";
        return;
    } else {
      auto nextMeasure = m_score->measure(m);
      auto nextStaff = m_score->staff(m_number + 1);
      qreal tempGapSum = m_gapsSum;
      availableWidth -= nextMeasure->allNotesWidth();
      tempGapSum += nextMeasure->gapsSum();
      if (availableWidth / tempGapSum > 0.8) {
        m_lastMeasureId = m;
        nextMeasure->setStaff(this);
        deleteExtraTie();
        nextStaff->setFirstMeasureId(m + 1); // if there is not next measure - next staff will be deleted
        if (nextStaff->measuresCount() < 1)
          m_score->deleteStaff(nextStaff);
        else
          createExtraTie(nextStaff->firstMeasure()->first()->item());
        fit();
        checkNotesRange();
        return;
      }
    }
  }

  m_gapFactor = qBound(0.5, factor, 2.5); // notes in this staff are ready to positioning
  updateNotesPos();
}


void TstaffObject::updateNotesPos(int startMeasure) {
  auto firstMeas = firstMeasure();
  if (firstMeas->isEmpty())
    return;

  qDebug() << debug() << "updating notes positions from" << startMeasure << "measure among number" << measuresCount()
            << "gap factor" << m_gapFactor << "notes count" << lastMeasure()->last()->index() - firstMeasure()->first()->index() + 1;
  TnoteObject* prevNote = nullptr;
  if (startMeasure == 0)
    firstMeas->first()->item()->setX(m_notesIndent);
  else
    prevNote = m_score->measure(startMeasure - 1)->last()->item();

  for (int m = m_firstMeasureId; m <= m_lastMeasureId; ++m) {
    auto measure = m_score->measure(m);
    if (measure->staff() != this) { // TODO delete debug message if not occurs
      qDebug() << debug() << "Something went wrong, measure" << measure->number() << "doesn't belong to staff" << m_number << "FIXING!";
      measure->setStaff(this);
    }
    qreal barOffset = m > 0 ? BARLINE_OFFSET : 0.0; // offset for first note after bar line
    for (int n = 0; n < measure->noteCount(); ++n) {
      auto note = measure->note(n)->item();
      if (prevNote)
        note->setX(prevNote->rightX() + barOffset);
      prevNote = note;
      barOffset = 0.0;
    }
    measure->checkBarLine();
  }
}


void TstaffObject::checkNotesRange(bool doEmit) {
  qreal oldHi = m_hiNotePos, oldLo = m_loNotePos;
  findHighestNote();
  findLowestNote();
  if (m_number > 0 && doEmit && oldHi != m_hiNotePos) // never do not emit for first staff
    emit hiNotePosChanged(m_number, (oldHi - m_hiNotePos) * scale());
  if (doEmit && oldLo != m_loNotePos)
    emit loNotePosChanged(m_number, (m_loNotePos - oldLo) * scale());
  return;
}


void TstaffObject::appendMeasure(TmeasureObject* m) {
  m_lastMeasureId = m->number();
  m->setStaff(this);
}


void TstaffObject::insertMeasure(int index, TmeasureObject* m) {
  qDebug() << debug() << "Inserting measure nr" << m->number() + 1 << "at" << index;
  if (index < m_firstMeasureId) {
    m_firstMeasureId = index;
    emit firstMeasureNrChanged();
  }
  if (index > m_lastMeasureId)
    m_lastMeasureId = index;
  m->setStaff(this);
}


void TstaffObject::deleteExtraTie() {
  if (m_extraTie) {
    delete m_extraTie;
    m_extraTie = nullptr;
  }
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TstaffObject::findHighestNote() {
  m_hiNotePos = upperLine() - 4.0;
  for (int m = m_firstMeasureId; m <= m_lastMeasureId; ++m) {
    auto measure = m_score->measure(m);
    for (int n = 0; n < measure->noteCount(); ++n) {
      auto noteSeg = measure->note(n);
      if (noteSeg->item()->notePosY()) // is visible
        m_hiNotePos = qMin(qreal(noteSeg->item()->notePosY() - (noteSeg->note()->rtm.stemDown() ? 2.0 : 4.0)), m_hiNotePos);
    }
  }
}


void TstaffObject::findLowestNote() {
//   if (hasScordature()) { TODO
//     m_loNotePos = height();
//     return;
//   }
  m_loNotePos =  upperLine() + 14.0; // TODO (isPianoStaff() ? lowerLinePos(): upperLinePos()) + 13.0;
  for (int m = m_firstMeasureId; m <= m_lastMeasureId; ++m) {
    auto measure = m_score->measure(m);
    for (int n = 0; n < measure->noteCount(); ++n) {
      auto noteSeg = measure->note(n);
      m_loNotePos = qMax(qreal(noteSeg->item()->notePosY() + (noteSeg->note()->rtm.stemDown() ? 4 : 2)), m_loNotePos);
    }
  }
}


void TstaffObject::createExtraTie(TnoteObject* parent) {
  if (parent->note()->rtm.tie() == Trhythm::e_tieCont || parent->note()->rtm.tie() == Trhythm::e_tieEnd) {
      if (!m_extraTie) {
        QQmlEngine engine;
        QQmlComponent comp(&engine, this);
        comp.setData("import QtQuick 2.7; Text { font { family: \"Scorek\"; pixelSize: 7 }}", QUrl());
        m_extraTie = qobject_cast<QQuickItem*>(comp.create());
        m_extraTie->setX(-2.446875); // 2.546875 tie glyph width
        m_extraTie->setProperty("color", qApp->palette().text().color());
      }
      m_extraTie->setParentItem(parent->head());
      m_extraTie->setProperty("text", parent->note()->rtm.stemDown() ? QStringLiteral("\ue204") : QStringLiteral("\ue1fd"));
      m_extraTie->setY(parent->note()->rtm.stemDown() ? -1.0 : 0.0);
  } else {
      deleteExtraTie();
  }
}


