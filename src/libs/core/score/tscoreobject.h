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


class QTimer;
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
  Q_PROPERTY(qreal width READ width WRITE setWidth)
  Q_PROPERTY(int keySignature READ keySignature WRITE setKeySignature)
  Q_PROPERTY(bool keySignatureEnabled READ keySignatureEnabled WRITE setKeySignatureEnabled)

  friend class TstaffObject;
  friend class TmeasureObject;
  friend class TnoteObject;

public:
  explicit TscoreObject(QObject* parent = nullptr);
  ~TscoreObject();

  void setParent(QObject* p);

  qreal width() { return m_width; }
  void setWidth(qreal w);

  Q_INVOKABLE void addNote(const Tnote& n);
  Q_INVOKABLE void setNote(int staffNr, int noteNr, const Tnote& n);

  bool keySignatureEnabled() const { return m_keySignEnabled; }
  void setKeySignatureEnabled(bool enKey);

  int keySignature() { return static_cast<int>(m_keySignature); }
  void setKeySignature(int k);

  bool showExtraAccids() { return m_showExtraAccids; }
  void setShowExtraAccids(bool accShow);

      /**
       * If set, reminds about accidentals changes occurred in previous measure
       */
  bool remindAccids() { return m_remindAccids; }
  void setRemindAccids(bool doRemaind);

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

      /**
       * Asks Score.qml about create new staff
       */
  void staffCreate();
  
      /**
       * Informs Score.qml that content widget height has to be adjusted to all staves height
       */
  void stavesHeightChanged();

protected:
  void addStaff(TstaffObject* st);

  TclefOffset clefOffset() const { return m_clefOffset; }

      /**
       * Shifts @p count measures starting from @p measureNr by setting first and last ids
       * of the next measure to given @p sourceStaff
       * Also it manages ownership of initial (source) staff shifted notes and sets parent of every such note to target staff.
       */
  void startStaffFromMeasure(TstaffObject* sourceStaff, int measureNr, int count = 1);

  void deleteStaff(TstaffObject* st);

      /**
       * Score width is handled by Score.qml, but it has to be known here.
       * During window scaling width of score changes a few times before it is stabilized.
       * To avoid scaling staves more times than one for width change,
       * a timer guards the width change and call @p adjustScoreWidth() method only after delay
       */
  void adjustScoreWidth();

  bool adjustInProgress() { return m_adjustInProgress; }

  void updateStavesPos();

  void onIndentChanged();

      /**
       * This array keeps values (-1, 0 or 1) for accidentals in key sign.
       */
  qint8 accidInKey(int k) { return m_accidInKeyArray[k]; }

private:
      /**
       * Appends notes to @p m_notes list, creates corresponding @p TnotePair
       * and adds them to @p m_segments list
       */
  void appendNoteList(QList<Tnote>& l);

private:
  Tmeter                           *m_meter;
  bool                              m_keySignEnabled;
  bool                              m_showExtraAccids;
  bool                              m_remindAccids;
  TclefOffset                       m_clefOffset;
  qreal                             m_width;
  bool                              m_adjustInProgress;
  bool                              m_keyChanged = false;
  QList<TnotePair*>                 m_segments;
  QList<TstaffObject*>              m_staves;
  QList<TmeasureObject*>            m_measures;
  QList<Tnote>                      m_notes;
  QList<quint8>                     m_meterGroups;
  QTimer                           *m_widthTimer;
  qint8                             m_keySignature = 0;
  qint8                             m_accidInKeyArray[7];

};

#endif // TSCOREOBJECT_H
