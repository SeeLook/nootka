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

#ifndef TSTAFFOBJECT_H
#define TSTAFFOBJECT_H


#include "nootkacoreglobal.h"
#include <QtCore/qobject.h>


class QQuickItem;
class QTimer;
class TscoreObject;
class TmeasureObject;
class TnoteObject;
class TnotePair;
class Tnote;


/**
 * @class TstaffObject is C++ logic of Staff.qml
 */
class NOOTKACORE_EXPORT  TstaffObject : public QObject
{

  Q_OBJECT

  Q_PROPERTY(TscoreObject* score READ score WRITE setScore)
  Q_PROPERTY(qreal upperLine READ upperLine WRITE setUpperLine NOTIFY upperLineChanged)
  Q_PROPERTY(QQuickItem* staffItem READ staffItem WRITE setStaffItem)
  Q_PROPERTY(qreal notesIndent READ notesIndent WRITE setNotesIndent)
  Q_PROPERTY(int firstMeasureNr READ firstMeasureNr NOTIFY firstMeasureNrChanged)
  Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)

  friend class TscoreObject;
  friend class TnotePair;

public:
  explicit TstaffObject(QObject* parent = nullptr);
  ~TstaffObject() override;

  int number() const { return m_number; }
  void setNumber(int nr) { m_number = nr; emit numberChanged(); }

  TscoreObject* score() { return m_score; }
  void setScore(TscoreObject* s);

      /**
       * Refreshes gap factor and note positions
       */
  void refresh();

      /**
       * Y coordinate of upper staff line
       */
  qreal upperLine() const { return m_upperLine; }
  void setUpperLine(qreal upLine);

  QQuickItem* staffItem() { return m_staffItem; }
  void setStaffItem(QQuickItem* si);

      /**
       * X coordinate of first note (or width of clef + key sign. + meter)
       */
  qreal notesIndent() const { return m_notesIndent; }
  void setNotesIndent(qreal ni);

  int firstMeasureNr();

  int measuresCount() { return m_lastMeasureId - m_firstMeasureId + 1; }
  TmeasureObject* firstMeasure();
  TmeasureObject* lastMeasure();
//   TmeasureObject* measure(int id) { return m_measures[id]; }

  TnotePair* firstNote();
  TnotePair* lastNote();

      /**
       * Multiplexer of rhythm gaps between notes.
       * It changes to place all notes nicely over entire staff width
       */
  qreal gapFactor() { return m_gapFactor; }

      /**
       * Y position of lowest note on the staff
       */
  qreal loNotePos() const { return m_loNotePos; }

      /**
       * Y position of highest note on the staff
       */
  qreal hiNotePos() const { return m_hiNotePos; }

      /**
       * Minimal height of the staff to display all its notes.
       */
  qreal minHeight() const { return m_loNotePos - (number() == 0 ? 0.0 : m_hiNotePos); }

      /**
       * Width of all notes on the staff
       */
  qreal allNotesWidth() const { return m_allNotesWidth; }

  qreal gapsSum() const { return m_gapsSum; }

      /**
       * Scaling factor of the staff
       */
  qreal scale();

      /**
       * Prints to std out debug info about this staff: [nr STAFF] in color
       */
  char debug();

signals:
  void upperLineChanged();
  void firstMeasureNrChanged();
  void numberChanged();
  void loNotePosChanged(int staffNr, qreal offset);
  void hiNotePosChanged(int staffNr, qreal offset);

protected:
  void fit();
  void updateNotesPos(int startMeasure = 0);

  int firstMeasureId() const { return m_firstMeasureId; }
  void setFirstMeasureId(int firstId) { m_firstMeasureId = firstId; emit firstMeasureNrChanged(); }
  int lastMeasureId() const { return m_lastMeasureId; }
  void setLastMeasureId(int lastId) { m_lastMeasureId = lastId; }

      /**
       * Checks positions of all notes to find lowest and highest.
       * @p doEmit determines whether this method sends appropriate signals
       */
  void checkNotesRange(bool doEmit = true);

  void appendMeasure(TmeasureObject* m);

      /**
       * Inserts given @p m measure at @p index position
       * which is in current staff measures numerology
       */
  void insertMeasure(int index, TmeasureObject* m);

      /**
       * Staff manages extra tie text when tie is broken among staves.
       * It creates QML Text item and adds it to first note head in the staff.
       * Whenever first note changes (measures are shifting), it check is that extra tie necessary
       */
  void createExtraTie(TnoteObject* parent);
  void deleteExtraTie();

private:
  void findLowestNote(); /**< Checks all Y positions of staff notes to find lowest one */
  void findHighestNote(); /**< Checks all Y positions of staff notes to find highest one */

private:
  TscoreObject                  *m_score;
  qreal                          m_upperLine;
  QQuickItem                    *m_staffItem;
  qreal                          m_notesIndent;
  qreal                          m_gapFactor;
  qreal                          m_loNotePos, m_hiNotePos;
  int                            m_number;
  int                            m_firstMeasureId, m_lastMeasureId;
  qreal                          m_allNotesWidth = 0.0;
  qreal                          m_gapsSum = 0.0;
  QQuickItem                    *m_extraTie = nullptr; /**< Tie at the staff beginning */
};

#endif // TSTAFFOBJECT_H
