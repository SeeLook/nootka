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
#include <QtCore/qdebug.h>
#include "tscoreobject.h"
#include "tnoteobject.h"
#include "music/tnote.h"


TstaffObject::TstaffObject(QObject* parent) :
  QObject(parent),
  m_score(nullptr),
  m_upperLine(16.0),
  m_staffItem(nullptr)
{
}


TstaffObject::~TstaffObject() { qDebug() << "[TstaffObject] is going delete"; }


void TstaffObject::setScore(TscoreObject* s) {
  m_score = s;
  setParent(s);
  qDebug() << "TstaffObject got a score parent" << s;
  m_score->addStaff(this);
}


void TstaffObject::addNote(const Tnote& n) {
  auto noteObj = new TnoteObject(this, m_staffItem);
  noteObj->setIndex(m_notes.count());
  noteObj->setNote(n);
  noteObj->setX(notesIndent() + noteObj->index() * 7.0);
  m_notes.append(noteObj);
  emit noteAdded(noteObj);
}


void TstaffObject::setUpperLine(qreal upLine) {
  m_upperLine = upLine;
  emit upperLineChanged();
}


void TstaffObject::setStaffItem(QQuickItem* si) {
  m_staffItem = si;
}


