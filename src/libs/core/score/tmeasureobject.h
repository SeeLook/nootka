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

#ifndef TMEASUREOBJECT_H
#define TMEASUREOBJECT_H


#include "nootkacoreglobal.h"
#include <QtCore/qobject.h>


class QQuickItem;
class TscoreObject;
class TstaffObject;
class TnoteObject;
class TnotePair;


/**
 * @class TmeasureObject is an implementation of the measure
 */
class NOOTKACORE_EXPORT TmeasureObject : public QObject
{
  Q_OBJECT

  friend class TstaffObject;

public:

  explicit TmeasureObject(int nr = -1, TscoreObject* parent = nullptr);
  ~TmeasureObject();

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

  void insertNote(int id, TnotePair* np);

  TscoreObject* score() { return m_score; }

  TstaffObject* staff() { return m_staff; }
  void setStaff(TstaffObject* st);

  int noteCount() { return m_notes.count(); }
  TnotePair* note(int nr) { return m_notes[nr]; }
  TnotePair* first() { return m_notes.first(); }
  TnotePair* last() { return m_notes.last(); }
  bool isEmpty() { m_notes.isEmpty(); }

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
      /**
       * Sets appropriate @p setRhythmGroup of every note in the measure.
       */
  void updateRhythmicGroups();
  void checkBarLine();

      /**
       * Updates @p m_allNotesWidth @p m_allGaps according to current measure content
       */
  void refresh();

private:
  int                            m_number;
  int                            m_duration;
  int                            m_id;
  TscoreObject                  *m_score;
  TstaffObject                  *m_staff;
  int                            m_free;
  QList<TnotePair*>              m_notes;
  qint8                         *m_firstInGr; /**< qint8 is sufficient - measure never has more than 127 notes */
  QQuickItem                    *m_barLine;
  qreal                          m_allNotesWidth = 0.0;
  qreal                          m_gapsSum = 0.0;
};

#endif // TMEASUREOBJECT_H
