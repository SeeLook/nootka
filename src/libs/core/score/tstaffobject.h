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

  friend class TscoreObject;

public:
  explicit TstaffObject(QObject* parent = nullptr);
  ~TstaffObject();

  TscoreObject* score() { return m_score; }
  void setScore(TscoreObject* s);

      /**
       * Refreshes gap factor and note positions
       */
  void refresh();

      /**
       * Y coordinate of upper staff line
       */
  qreal upperLine() { return m_upperLine; }
  void setUpperLine(qreal upLine);

  QQuickItem* staffItem() { return m_staffItem; }
  void setStaffItem(QQuickItem* si);

      /**
       * X coordinate of first note (or width of clef + key sign. + meter)
       */
  qreal notesIndent() { return m_notesIndent; }
  void setNotesIndent(qreal ni);

  int firstMeasureNr();

      /**
       * Number of this staff. It is set by QML staff item
       */
  int number() const;

  TmeasureObject* firstMeasure() { return m_measures.first(); }
  TmeasureObject* lastMeasure() { return m_measures.last(); }

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
  qreal allNotesWidth() { return m_allNotesWidth; }

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
  void loNotePosChanged(int staffNr, qreal offset);
  void hiNotePosChanged(int staffNr, qreal offset);

protected:
  void fit();
  void updateNotesPos(int startMeasure = 0);

      /**
       * Checks positions of all notes to find lowest and highest.
       * @p doEmit determines whether this method sends appropriate signals
       */
  void checkNotesRange(bool doEmit = true);

  void appendMeasure(TmeasureObject* m);
  void takeMeasure(int id);

private:
  void findLowestNote(); /**< Checks all Y positions of staff notes to find lowest one */
  void findHighestNote(); /**< Checks all Y positions of staff notes to find highest one */

private:
  TscoreObject                  *m_score;
  qreal                          m_upperLine;
  QList<TmeasureObject*>         m_measures;
  QQuickItem                    *m_staffItem;
  qreal                          m_notesIndent;
  qreal                          m_gapFactor;
  qreal                          m_loNotePos, m_hiNotePos;
  qreal                          m_allNotesWidth = 0.0;
};

#endif // TSTAFFOBJECT_H
