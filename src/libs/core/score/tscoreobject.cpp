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
#include "music/tmeter.h"
#include "music/tnote.h"
#include <QtCore/qdebug.h>



TscoreObject::TscoreObject(QObject* parent) :
  QObject(parent),
  m_keySignEnabled(false),
  m_clefOffset(TclefOffset(3, 1))
{
  m_meter = new Tmeter(Tmeter::Meter_4_4);
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
  m_staves.last()->addNote(n);
}


void TscoreObject::setKeySignatureEnabled(bool enKey) {
  m_keySignEnabled = enKey;
}


//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void TscoreObject::addStaff(TstaffObject* st) {
  m_staves.append(st);
}
