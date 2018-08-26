/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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

#ifndef TMEASUREOBJECT_H
#define TMEASUREOBJECT_H


#include "nootkacoreglobal.h"
#include <QtCore/qobject.h>


class QQuickItem;
class TscoreObject;
class TstaffItem;
class TnoteItem;
class TnotePair;
class TbeamObject;


/**
 * @class TmeasureObject is an implementation of the measure
 */
class NOOTKACORE_EXPORT TmeasureObject : public QObject
{
  Q_OBJECT

  friend class TscoreObject;
  friend class TstaffItem;
  friend class TnoteItem;

public:

  explicit TmeasureObject(int nr = -1, TscoreObject* parent = nullptr);
  ~TmeasureObject() override;

  int number() const { return m_number; }
  void setNumber(int nr);

      /**
       * Actual duration of all notes in the measure
       */
  int duration() const { return m_duration; }

      /**
       * Free 'rhythm space' remained in the measure
       */
  int free() const { return m_free; }

      /**
       * Adds list of @p count notes at the measure end starting from @p segmentId note number in the score
       * Measure has to have already space for the whole list!
       */
  void appendNewNotes(int segmentId, int count);

  void removeLastNote();

  TscoreObject* score() { return m_score; }

  TstaffItem* staff() { return m_staff; }
  void setStaff(TstaffItem* st);

  int noteCount() { return m_notes.count(); }
  TnotePair* note(int nr) { return m_notes[nr]; }
  TnotePair* first() { return m_notes.first(); }
  TnotePair* last() { return m_notes.last(); }
  bool isEmpty() { return m_notes.isEmpty(); }

      /**
       * Staff index of the first measure note
       */
  int firstNoteId() const;

      /**
       * Staff index of the last measure note
       */
  int lastNoteId() const;

      /**
       * Summarized width of all notes in this measure
       */
  qreal allNotesWidth() { return m_allNotesWidth; }

      /**
       * Sum of all rhythm spaces depending on rhythm values after every measure note
       */
  qreal gapsSum() { return m_gapsSum; }

      /**
       * Prints debug message with [number MEASURE]
       */
  char debug();

protected:

  void flush();

      /**
       * Sets appropriate @p setRhythmGroup of every note in the measure.
       */
  void updateRhythmicGroups();
  void checkBarLine();

  void meterChanged();

      /**
       * Updates @p m_allNotesWidth @p m_allGaps according to current measure content
       */
  void refresh();

  void checkAccidentals();

  void keySignatureChanged();

  qint8 accidState(int noteNr) { return m_accidsState[noteNr]; }

      /**
       * Checks notes rhythms of group @p segmentId belongs to
       * for 8ths and 16ths and crates beams (@p TbeamObject) if they occurs
       * It can be called before @p TnoteObject creation.
       * When beam was set or note was added to it, it returns number of that group
       * or -1 if no beams were added
       */
  int beamGroup(int segmentId);

      /**
       * Fixes a beam if given note has it.
       */
  void noteGoingRest(TnotePair* np);

      /**
       * Beams given note with surrounding notes. WARNING: Given note has to be 8th or 16th
       */
  void restGoingNote(TnotePair* np);

private:
  void clearAccidState();

private:
  int                            m_number;
  int                            m_duration;
  TscoreObject                  *m_score;
  TstaffItem                    *m_staff;
  int                            m_free;
  QList<TnotePair*>              m_notes;
  qint8                         *m_firstInGr; /**< qint8 is sufficient - measure never has more than 127 notes */
  QQuickItem                    *m_barLine;
  qreal                          m_allNotesWidth = 0.0;
  qreal                          m_gapsSum = 0.0;
  qint8                          m_accidsState[7];
};

#endif // TMEASUREOBJECT_H
