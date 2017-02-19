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
#include "tnotepair.h"
#include "music/tmeter.h"
#include "music/tnote.h"
#include <QtCore/qdebug.h>



TscoreObject::TscoreObject(QObject* parent) :
  QObject(parent),
  m_keySignEnabled(false),
  m_clefOffset(TclefOffset(3, 1))
{
  m_meter = new Tmeter(Tmeter::Meter_4_4);
  setMeter(4); // Tmeter::Meter_4_4
  m_measures << new TmeasureObject(this);
}


TscoreObject::~TscoreObject()
{
  delete m_meter;
  qDebug() << "[TscoreObject] deleted";
}


void TscoreObject::setParent(QObject* p) {
  QObject::setParent(p);
  qDebug() << "[TscoreObject] parent changed to" << p;
}


void TscoreObject::addNote(const Tnote& n) {
  m_notes << n;
  auto newSeg = new TnotePair(&m_notes.last());
  m_segments << newSeg;
  auto lastMeasure = m_measures.last();
  lastMeasure->insertNote(lastMeasure->lastNoteId() - lastMeasure->firstNoteId(), newSeg);
}


void TscoreObject::setNote(int staffNr, int noteNr, const Tnote& n) {
  if (staffNr < 0 || staffNr >= m_staves.count()) {
    qDebug() << "[TscoreObject] There is no staff number" << staffNr;
    return;
  }
  m_staves[staffNr]->setNote(noteNr, n);
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


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TscoreObject::addStaff(TstaffObject* st) {
  m_staves.append(st);
  if (st->number() == 0) // initialize first measure staff
    m_measures.first()->setStaff(st);
}
