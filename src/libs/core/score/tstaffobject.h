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
 * 
 */
class NOOTKACORE_EXPORT  TstaffObject : public QObject
{

  Q_OBJECT

  Q_PROPERTY(TscoreObject* score READ score WRITE setScore)
  Q_PROPERTY(qreal upperLine READ upperLine WRITE setUpperLine NOTIFY upperLineChanged)
  Q_PROPERTY(QQuickItem* staffItem READ staffItem WRITE setStaffItem)
  Q_PROPERTY(qreal notesIndent READ notesIndent WRITE setNotesIndent)
  Q_PROPERTY(int firstMeasureNr READ firstMeasureNr NOTIFY firstMeasureNrChanged)

public:
  explicit TstaffObject(QObject* parent = nullptr);
  ~TstaffObject();

  TscoreObject* score() { return m_score; }
  void setScore(TscoreObject* s);

  void addNote(TnotePair* np);

  void setNote(int noteNr, const Tnote& n);

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
  void setNotesIndent(qreal ni) { m_notesIndent = ni; }

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

signals:
  void upperLineChanged();
  void firstMeasureNrChanged();

protected:
  void fit();
  void updateNotesPos(int startId);

private:
  TscoreObject                  *m_score;
  qreal                          m_upperLine;
  QList<TnotePair*>              m_notes;
  QList<TmeasureObject*>         m_measures;
  QQuickItem                    *m_staffItem;
  qreal                          m_notesIndent;
  qreal                          m_gapFactor;
};

#endif // TSTAFFOBJECT_H
