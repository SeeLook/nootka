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
#include "tnotepair.h"
#include "music/tmeter.h"
#include "music/tnote.h"

#include <QtCore/qtimer.h>
#include <QtCore/qdebug.h>
#include "checktime.h"


#define WIDTH_CHANGE_DELAY (50) // when score width changes, give 50 ms before staves will be resized


TscoreObject::TscoreObject(QObject* parent) :
  QObject(parent),
  m_keySignEnabled(false),
  m_showExtraAccids(false),
  m_remindAccids(true),
  m_clefOffset(TclefOffset(3, 1)),
  m_width(0.0), m_adjustInProgress(false)
{
  m_meter = new Tmeter(Tmeter::Meter_4_4);
  setMeter(4); // Tmeter::Meter_4_4
  m_measures << new TmeasureObject(0, this);
  m_widthTimer = new QTimer(this);
  m_widthTimer->setSingleShot(true);
  connect(m_widthTimer, &QTimer::timeout, this, &TscoreObject::adjustScoreWidth);

  for (int i = 0; i < 7; i++) // reset accidentals array
    m_accidInKeyArray[i] = 0;
}


TscoreObject::~TscoreObject()
{
  delete m_meter;
  qDebug() << "[TscoreObject] deleted";
}


// TODO: Is it necessary at all?
void TscoreObject::setParent(QObject* p) {
  QObject::setParent(p);
  qDebug() << "[TscoreObject] parent changed to" << p;
}


void TscoreObject::setWidth(qreal w) {
  if (w != m_width) {
    m_width = w;
    if (m_widthTimer->isActive())
      m_widthTimer->stop();
    m_widthTimer->start(WIDTH_CHANGE_DELAY);
  }
}


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
  for (int r = 0; r < outList.count(); ++r) { // add ties (ligatures)
    Trhythm::Etie tie = Trhythm::e_noTie;
    if (r == 0) {
        if (n.rtm.tie()) // there was tie before
          tie = outList.count() == 1 ? Trhythm::e_tieEnd : Trhythm::e_tieCont; // continue it or finish
        else // otherwise start a tie
          tie = Trhythm::e_tieStart;
    } else if (r == outList.count() - 1)
        tie = Trhythm::e_tieEnd;
    else
        tie = Trhythm::e_tieCont;
    outList[r].rtm.setTie(tie);
  }
}

void TscoreObject::addNote(const Tnote& n) {
// CHECKTIME (

  qDebug() << "note" << n.toText();

  auto lastMeasure = m_measures.last();
  if (lastMeasure->free() == 0) { // new measure is needed
    lastMeasure = new TmeasureObject(m_measures.count(), this);
    m_measures << lastMeasure;
    lastStaff()->appendMeasure(lastMeasure);
  }
  int noteDur = n.duration();
  if (noteDur > lastMeasure->free()) { // split note that is adding
      int leftDuration = noteDur - lastMeasure->free();
      int lastNoteId = m_segments.count();

      QList<Tnote> notesToCurrent;
      solveList(n, lastMeasure->free(), notesToCurrent); // solve free duration in current measure
      if (notesToCurrent.isEmpty())
          qDebug() << "[TscoreObject] can't resolve duration of" << lastMeasure->free();
      else {
          appendNoteList(notesToCurrent);
          lastMeasure->appendNewNotes(lastNoteId, notesToCurrent.count());
      }

      QList<Tnote> notesToNext;
      solveList(n, leftDuration, notesToNext); // solve remaining duration for the next measure
      lastNoteId = m_segments.count(); // update id of the last note segment
      if (notesToNext.isEmpty())
          qDebug() << "[TscoreObject] can't resolve duration" << leftDuration;
      else {
          appendNoteList(notesToNext);
          auto newLastMeasure = new TmeasureObject(m_measures.count(), this); // add a new measure
          m_measures << newLastMeasure;
          lastStaff()->appendMeasure(newLastMeasure);
          newLastMeasure->appendNewNotes(lastNoteId, notesToNext.count());
      }
  } else { // just add new note to the last measure
      m_notes << n;
      int lastNoteId = m_segments.count();
      m_segments << new TnotePair(lastNoteId, &m_notes.last());
      lastMeasure->appendNewNotes(lastNoteId, 1);
  }

// )
}


void TscoreObject::setNote(int staffNr, int noteNr, const Tnote& n) {
  if (staffNr < 0 || staffNr >= m_staves.count()) {
    qDebug() << "[TscoreObject] There is no staff number" << staffNr;
    return;
  }
//   m_staves[staffNr]->setNote(noteNr, n);
}



void TscoreObject::setKeySignatureEnabled(bool enKey) {
  m_keySignEnabled = enKey;
}


void TscoreObject::setMeter(int m) {
  // set notes grouping
  m_meter->setMeter(static_cast<Tmeter::Emeter>(m));
  m_meterGroups.clear();
  if (m_meter->lower() == 4) { // simple grouping: one group for each quarter
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
  qDebug() << "[TscoreObject] Meter changed" << m_meterGroups;
}


int TscoreObject::meterToInt() { return static_cast<int>(m_meter->meter()); }


qreal TscoreObject::stavesHeight() {
  if (m_staves.isEmpty())
    return 0.0;
  auto last = lastStaff();
  return last->staffItem()->y() + last->staffItem()->height() * last->scale();
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TscoreObject::addStaff(TstaffObject* st) {
  st->setNumber(stavesCount());
  m_staves.append(st);
  if (m_staves.count() == 1) // initialize first measure of first staff
    st->appendMeasure(m_measures.first());

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
 * More-less it means that staff @p sourceStaff has no space for @p count number of measures measures
 * starting from @p measureNr, but those measures belong to it still
 */
void TscoreObject::startStaffFromMeasure(TstaffObject* sourceStaff, int measureNr, int count) {
  TstaffObject* targetStaff = nullptr;
  if (sourceStaff == lastStaff()) { // create new staff to shift measure(s) there
      emit staffCreate();
      targetStaff = lastStaff();
  } else
      targetStaff = m_staves[sourceStaff->number() + 1];

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
CHECKTIME (
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
)
}


void TscoreObject::onIndentChanged() {
  if (m_keyChanged) {
    m_keyChanged = false;
    adjustScoreWidth();
  }
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void TscoreObject::appendNoteList(QList<Tnote>& l) {
  for (Tnote n : l) {
    m_notes << n;
    m_segments << new TnotePair(m_segments.count(), &m_notes.last());
  }
}

