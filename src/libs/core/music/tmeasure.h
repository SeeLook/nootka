/***************************************************************************
 *   Copyright (C) 2014-2021 by Tomasz Bojczuk                             *
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

#ifndef TMEASURE_H
#define TMEASURE_H


#include "tmeter.h"


class Tchunk;

/**
 * A musical measure
 */
class NOOTKACORE_EXPORT Tmeasure
{

public:
  Tmeasure(int nr, Tmeter::Emeter m = Tmeter::NoMeter);

  Tmeter& meter() { return m_meter; }

  int number() { return m_number; }

      /**
       * Adds a note.
       */
  void addNote(const Tchunk& n);
  Tchunk& note(int index) { return m_notes[index]; } /** Returns given note in this measure */
  Tchunk& lastNote() { return m_notes.last(); }

      /**
       * Current duration of the measure
       */
  int duration() const { return m_duration; }

      /**
       * @p TRUE when measure is full - notes duration is equal meter duration
       */
  bool isFull();

  void removeLastNote();

  int count() { return m_notes.size(); }

      /**
       * Replaces note @p noteNr with notes in the list.
       * List of notes must have the same duration like replacing note.
       */
  void swapWithNotes(int noteNr, const QList<Tchunk>& notes);

private:
  int                 m_number;
  Tmeter              m_meter;
  QList<Tchunk>       m_notes;
  int                 m_duration;
};

#endif // TMEASURE_H
