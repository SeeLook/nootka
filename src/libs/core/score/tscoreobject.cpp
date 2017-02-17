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


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TscoreObject::addStaff(TstaffObject* st) {
  m_staves.append(st);
  if (st->number() == 0) // initialize first measure staff
    m_measures.first()->setStaff(st);
}
