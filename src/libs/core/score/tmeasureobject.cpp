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
#include <QtCore/qdebug.h>


TmeasureObject::TmeasureObject(TscoreObject* parent) :
  QObject(parent),
  m_score(parent),
  m_staff(nullptr),
  m_number(-1) // undefined
{
}


void TmeasureObject::setNumber(int nr) {
  m_number = nr;
}


void TmeasureObject::setStaff(TstaffObject* st) {
  if (m_staff != st) {
    m_staff = st;
  }
}


void TmeasureObject::insertNote(int id, TnotePair* np) {
  qDebug() << "[TmeasureObject] inserting note at" << id;
  if (np->object() == nullptr) {
    auto noteObject = new TnoteObject(m_staff);
    noteObject->setMeasure(this);
    noteObject->setNote(*np->note());
    np->setNoteObject(noteObject);
    m_notes.append(np);
    m_staff->addNote(np);
  }
}


int TmeasureObject::firstNoteId() const {
  return m_notes.count() ? m_notes.first()->object()->index() : 0;
}


int TmeasureObject::lastNoteId() const {
  return m_notes.count() ? m_notes.last()->object()->index() : 0;
}



