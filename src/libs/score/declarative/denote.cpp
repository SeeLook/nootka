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

#include "denote.h"
#include <tscorenote.h>
#include <tscorestaff.h>
#include <music/tnote.h>

#include <QtCore/QDebug>


DeNote::DeNote(QQuickItem* parent) :
  QQuickItem(parent),
  m_scoreNote(nullptr)
{
  m_note = new Tnote(Tnote(), Trhythm(Trhythm::e_quarter));
}


DeNote::~DeNote()
{
  if (!m_scoreNote)
    delete m_note;
}


char DeNote::pitch() { return m_note->note; }


void DeNote::setPitch(char p) {
  if (m_scoreNote) {
      m_scoreNote->staff()->setNote(m_scoreNote->index(), Tnote(p, m_note->octave, m_note->alter, m_note->rtm));
      parentItem()->update();
  } else {
      m_note->note = p;
  }
  emit pitchChanged();
}


char DeNote::octave() { return m_note->octave; }


void DeNote::setOctave(char o) {
  if (m_scoreNote) {
      m_scoreNote->staff()->setNote(m_scoreNote->index(), Tnote(m_note->note, o, m_note->alter, m_note->rtm));
      parentItem()->update();
  } else {
      m_note->octave = o;
  }
  emit octaveChanged();
}


char DeNote::alter() { return m_note->alter; }


void DeNote::setAlter(char a) {
  if (m_scoreNote) {
      m_scoreNote->staff()->setNote(m_scoreNote->index(), Tnote(m_note->note, m_note->octave, a, m_note->rtm));
      parentItem()->update();
  } else {
      m_note->note = a;
  }
  emit alterChanged();
}


DeNote::Erhythm DeNote::rhythm() { return static_cast<Erhythm>(m_note->rhythm()); }


void DeNote::setRhythm(DeNote::Erhythm r) {
  if (m_scoreNote) {
      m_scoreNote->staff()->setNote(m_scoreNote->index(), Tnote(*m_note, Trhythm(static_cast<Trhythm::Erhythm>(r))));
      qDebug() << "[DeNote rhythm] on the score";
      parentItem()->update();
  } else {
      m_note->setRhythm(Trhythm(static_cast<Trhythm::Erhythm>(r)));
      qDebug() << "[DeNote rhythm] Not set";
  }
  emit rhythmChanged();
}




//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################
void DeNote::setNoteSegment(TscoreNote* ns) {
  if (m_scoreNote) {
    qDebug() << "[DeNote] score note already set!";
    return;
  }

  m_scoreNote = ns;
  delete m_note;
  m_note = m_scoreNote->note();
}

