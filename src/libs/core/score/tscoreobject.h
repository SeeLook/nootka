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

#ifndef TSCOREOBJECT_H
#define TSCOREOBJECT_H


#include "nootkacoreglobal.h"
#include <QtCore/qobject.h>


/**
 * Describes offset of a note.
 */
class TclefOffset
{
public:
  TclefOffset(qint8 noteOff = 0, qint8 octaveOff = 0) : note(noteOff), octave(octaveOff) {}

  qint8 note;
  qint8 octave;
  int total() { return octave * 7 + note; }
};


class Tnote;
class TnotePair;
class TstaffObject;
class TmeasureObject;
class TnoteObject;
class Tmeter;
class Tnote;


/**
 *
 */
class NOOTKACORE_EXPORT  TscoreObject : public QObject
{

  Q_OBJECT

  Q_PROPERTY(QObject* parent READ parent WRITE setParent)
  Q_PROPERTY(int meter READ meterToInt WRITE setMeter NOTIFY meterChanged)
  Q_PROPERTY(qreal stavesHeight READ stavesHeight NOTIFY stavesHeightChanged)

  friend class TstaffObject;
  friend class TmeasureObject;
  friend class TnoteObject;

public:
  explicit TscoreObject(QObject* parent = nullptr);
  ~TscoreObject();

  void setParent(QObject* p);

  Q_INVOKABLE void addNote(const Tnote& n);
  Q_INVOKABLE void setNote(int staffNr, int noteNr, const Tnote& n);

  bool keySignatureEnabled() const { return m_keySignEnabled; }
  void setKeySignatureEnabled(bool enKey);

  Tmeter* meter() const { return m_meter; }
  void setMeter(int m);
  int meterToInt();

  TnotePair* noteSegment(int id) { return m_segments[id]; }
  TnotePair* firstSegment() { return m_segments.first(); }
  TnotePair* lastSegment() { return m_segments.last(); }

  int measuresCount() { return m_measures.count(); }
  TmeasureObject* measure(int id) { return m_measures[id]; }
  TmeasureObject* firstMeasure() { return m_measures.first(); }
  TmeasureObject* lastMeasure() { return m_measures.last(); }

      /**
       * Total height of all staves
       */
  qreal stavesHeight();

  int stavesCount() { return m_staves.count(); }
  TstaffObject* staff(int id) { return m_staves[id]; }
  TstaffObject* firstStaff() { return m_staves.first(); }
  TstaffObject* lastStaff() { return m_staves.last(); }

      /**
      * Returns duration of given @param grNr group starting from measure beginning
      * Describes grouping (beaming - beam connections) of notes in a single measure for current meter.
      * This is a group of a few int values - each representing duration of the one group:
      * - for 3/8 it is only single 36 value - whole measure under one beam
      * - for 3/4 it is 24, 48, 72) - three groups
      */
  quint8 groupPos(int grNr) { return m_meterGroups[grNr]; }

      /**
       * Number of beaming groups for this meter
       */
  int groupCount() { return m_meterGroups.count(); }

  QList<Tnote>& notes();

signals:
  void meterChanged();
  void staffCreate();
  void stavesHeightChanged();

protected:
  void addStaff(TstaffObject* st);

  TclefOffset clefOffset() const { return m_clefOffset; }

private:
      /**
       * Appends notes to @p m_notes list, creates corresponding @p TnotePair
       * and adds them to @p m_segments list
       */
  void appendNoteList(QList<Tnote>& l);

private:
  Tmeter                           *m_meter;
  bool                              m_keySignEnabled;
  TclefOffset                       m_clefOffset;
  QList<TnotePair*>                 m_segments;
  QList<TstaffObject*>              m_staves;
  QList<TmeasureObject*>            m_measures;
  QList<Tnote>                      m_notes;
  QList<quint8>                     m_meterGroups;

};

#endif // TSCOREOBJECT_H
