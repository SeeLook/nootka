/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#include "tnotetoplay.h"
#include <music/tmelody.h>
#include <QtCore/qdebug.h>


TnoteToPlay::TnoteToPlay(const Tnote &note, int index, int transposition) :
  m_duration(note.duration()),
  m_index(index)
{
  if (!note.isRest())
    m_pitch = note.chromatic() + transposition;
}


QVector<TnoteToPlay> TnoteToPlay::fromMelody(Tmelody *m, int transposition) {
  QVector<TnoteToPlay> prList;
  bool wasRest = false;
  for (int n = 0; n < m->length(); ++n) {
    if (m->note(n)->p().isRest()) { // treat a multiple rests as a single duration
        if (wasRest)
            prList.last().append(m->note(n)->p());
        else {
            wasRest = true;
            prList << TnoteToPlay(m->note(n)->p(), n);
        }
        continue;
    } else
        wasRest = false;

    if (m->note(n)->p().rtm.tie() < Trhythm::e_tieCont) // no tie or tie starts
        prList << TnoteToPlay(m->note(n)->p(), n, transposition);
    else { // note with tie is continued or ends
        if (prList.isEmpty()) { // TODO: it should never happened, so delete it after testing
          qDebug() << "[TnoteToPlay] wrong tie in first note of melody";
          break;
        }
        if (prList.last().pitch() != m->note(n)->p().chromatic() + transposition) { // TODO: it should never happened, so delete it after testing
          qDebug() << "[TnoteToPlay] tied notes are different!" << n << m->note(n)->p().toText();
          continue;
        }
        prList.last().append(m->note(n)->p());
    }
  }
  return prList;
}
