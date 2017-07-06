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
#include <QtCore/qpoint.h>
#include <QtGui/qcolor.h>
#include "music/tclef.h"
#include "music/tnote.h"


/**
 * @class TclefOffset describes offset of a note depends on clef
 */
class TclefOffset
{
public:
  TclefOffset(qint8 noteOff = 0, qint8 octaveOff = 0) : note(noteOff), octave(octaveOff) {}

  qint8 note;
  qint8 octave;
  int total() { return octave * 7 + note; }
  void set(qint8 nOff, qint8 oOff) { note = nOff; octave = oOff; }
};


class QQmlEngine;
class QQmlComponent;
class QTimer;
class TnotePair;
class TstaffObject;
class TmeasureObject;
class TnoteObject;
class Tmeter;


/**
 * Implementation of score supplementing Score.qml
 */
class NOOTKACORE_EXPORT  TscoreObject : public QObject
{

  Q_OBJECT

                        /* Musical parameters */
  Q_PROPERTY(int meter READ meterToInt WRITE setMeter NOTIFY meterChanged)
  Q_PROPERTY(Tclef::EclefType clefType READ clefType WRITE setClefType NOTIFY clefTypeChanged)
  Q_PROPERTY(int keySignature READ keySignature WRITE setKeySignature NOTIFY keySignatureChanged)
  Q_PROPERTY(int notesCount READ notesCount)
  Q_PROPERTY(int cursorAlter READ cursorAlter WRITE setCursorAlter)
                        /* Score switches */
  Q_PROPERTY(bool keySignatureEnabled READ keySignatureEnabled WRITE setKeySignatureEnabled NOTIFY keySignatureEnabledChanged)
  Q_PROPERTY(bool enableDoubleAccidentals READ enableDoubleAccidentals WRITE setEnableDoubleAccids)
  Q_PROPERTY(bool showNoteNames READ showNoteNames WRITE setShowNoteNames)
  Q_PROPERTY(QColor nameColor READ nameColor WRITE setNameColor)
  Q_PROPERTY(int nameStyle READ nameStyle WRITE setNameStyle)
  Q_PROPERTY(bool readOnly READ readOnly WRITE setReadOnly)
                        /* Helper variables */
  Q_PROPERTY(qreal stavesHeight READ stavesHeight NOTIFY stavesHeightChanged)
  Q_PROPERTY(qreal width READ width WRITE setWidth)
                        /* Note cursor */
  Q_PROPERTY(TnoteObject* activeNote READ activeNote NOTIFY activeNoteChanged)
  Q_PROPERTY(TnoteObject* lastNote READ lastNote NOTIFY lastNoteChanged)
  Q_PROPERTY(qreal activeYpos READ activeYpos NOTIFY activeYposChanged)
  Q_PROPERTY(qreal upperLine READ upperLine NOTIFY upperLineChanged)
  Q_PROPERTY(qreal xFirstInActivBar READ xFirstInActivBar NOTIFY activeBarChanged)
  Q_PROPERTY(qreal xLastInActivBar READ xLastInActivBar NOTIFY activeBarChanged)
  Q_PROPERTY(bool allowAdding READ allowAdding WRITE setAllowAdding NOTIFY allowAddingChanged)
  Q_PROPERTY(Trhythm workRhythm READ workRhythm WRITE setWorkRhythm)

  friend class TstaffObject;
  friend class TmeasureObject;
  friend class TnoteObject;

public:
  explicit TscoreObject(QObject* parent = nullptr);
  ~TscoreObject() override;

  /* ------------------ Musical parameters ------------------ */

  Tmeter* meter() const { return m_meter; }
  Q_INVOKABLE void setMeter(int m);
  int meterToInt() const; /**< Small helper for QML property that converts meter enumerator into int */

      /**
       * In most cases clef change is managed by QML (Staff & Clef) - means outside.
       * But when melody is set QML has to be updated from here.
       * It changes note positions according to clef or converts note into rest if the note is out of score range
       */
  Tclef::EclefType clefType() const { return m_clefType; }
  void setClefType(Tclef::EclefType ct);

  int keySignature() const { return static_cast<int>(m_keySignature); }
  void setKeySignature(int k);

  Q_INVOKABLE void addNote(const Tnote& n);
  Q_INVOKABLE void setNote(TnoteObject* no, const Tnote& n);

      /**
       * Returns a note item of @p TnoteObject
       */
  Q_INVOKABLE TnoteObject* note(int noteId);

  Q_INVOKABLE void noteClicked(qreal yPos);

  int cursorAlter() const { return m_cursorAlter; }
  void setCursorAlter(int curAlt) { m_cursorAlter = curAlt; }

  Q_INVOKABLE void openMusicXml(const QString& musicFile);
  Q_INVOKABLE void saveMusicXml(const QString& musicFile);

  /* ------------------ Score switches ------------------ */

  bool keySignatureEnabled() const { return m_keySignEnabled; }
  void setKeySignatureEnabled(bool enKey);

  bool showExtraAccids() const { return m_showExtraAccids; }
  void setShowExtraAccids(bool accShow);

  bool enableDoubleAccidentals() { return m_enableDoubleAccids; }
  void setEnableDoubleAccids(bool dblEnabled);

      /**
       * If set, reminds about accidental changes occurred in previous measure
       */
  bool remindAccids() const { return m_remindAccids; }
  void setRemindAccids(bool doRemaind);

  bool showNoteNames() { return m_showNoteNames; }
  void setShowNoteNames(bool showNames);

  QColor nameColor() const { return m_nameColor; }
  void setNameColor(const QColor& nameC);

  int nameStyle() const { return m_nameStyle; }
  void setNameStyle(int nameS);

  bool readOnly() const { return m_readOnly; }
  void setReadOnly(bool ro);

  /* ------------------ Lists with score content (staves, measures notes) ------------------ */

  int notesCount() const { return m_notes.count(); }
  TnotePair* noteSegment(int id) { return m_segments[id]; }
  TnotePair* firstSegment() { return m_segments.first(); }
  TnotePair* lastSegment() { return m_segments.last(); }

  int measuresCount() const { return m_measures.count(); }
  TmeasureObject* measure(int id) { return m_measures[id]; }
  TmeasureObject* firstMeasure() { return m_measures.first(); }
  TmeasureObject* lastMeasure() { return m_measures.last(); }

  int stavesCount() const { return m_staves.count(); }
  TstaffObject* staff(int id) { return m_staves[id]; }
  TstaffObject* firstStaff() { return m_staves.first(); }
  TstaffObject* lastStaff() { return m_staves.last(); }

  /* ------------------ Other helpers ------------------ */

  qreal width() { return m_width; }
  void setWidth(qreal w);

      /**
       * Total height of all staves
       */
  qreal stavesHeight();

      /**
       * Returns duration of given @param grNr group starting from measure beginning
       * Describes grouping (beaming - beam connections) of notes in a single measure for current meter.
       * This is a group of a few int values - each representing duration of the one group:
       * - for 3/8 it is only single 36 value - whole measure under one beam
       * - for 3/4 it is 24, 48, 72) - three groups
       */
  quint8 groupPos(int grNr) const { return m_meterGroups[grNr]; }

      /**
       * Number of beaming groups for this meter
       */
  int groupCount() const { return m_meterGroups.count(); }

  qreal upperLine();

      /**
       * Returns highest possible note on the staff in current clef
       */
  Q_INVOKABLE Tnote highestNote();

      /**
       * Returns lowest possible note on the staff in current clef
       */
  Q_INVOKABLE Tnote lowestNote();

  bool isPianoStaff() { return m_clefType == Tclef::PianoStaffClefs; }

/* ------------------ Note cursor ------------------ */
  TnoteObject* activeNote() { return m_activeNote; }
  qreal xFirstInActivBar();
  qreal xLastInActivBar();
  qreal activeYpos() const { return m_activeYpos; }

  Trhythm workRhythm() const;
  void setWorkRhythm(const Trhythm& r);

  Q_INVOKABLE Tnote posToNote(qreal yPos);

      /**
       * A switch that triggers displaying controls for adding notes to the score.
       * When @p TRUE, the last staff width is smaller about 'note add' control width (4.0).
       * QML uses @p lastNote() position to place that control at the score end.
       */
  bool allowAdding() const { return m_allowAdding; }
  void setAllowAdding(bool allow);

      /**
       * Last note item (@p TnoteObject) or null if score is empty
       */
  TnoteObject* lastNote();

signals:
  void meterChanged();

  void clefTypeChanged();

      /**
       * Asks Score.qml about create new staff
       */
  void staffCreate();

      /**
       * Usually those props are managed outside of @p TscoreObject - by QML
       * but in case @p keySignatureEnabled() and @p keySignature() are changed 'internally'
       * QML has to react on that change, so @p clefTypeChanged(), @p keySignatureEnabledChanged() and @p keySignatureChanged() are emitted
       */
  void keySignatureEnabledChanged();
  void keySignatureChanged();

      /**
       * Informs Score.qml that content widget height has to be adjusted to all staves height
       */
  void stavesHeightChanged();

  void activeNoteChanged();
  void activeYposChanged();
  void upperLineChanged();

      /**
       * When active note was clicked
       */
  void clicked();

      /**
       * Emitted when number of active measure changes, but also when measure moves (note positions are changed)
       */
  void activeBarChanged();
  void allowAddingChanged();

      /**
       * It is emitted whenever last note changes due to score refactoring 
       * (i.e. when meter changes and all notes are removed and added again)
       * QML score depends on it
       */
  void lastNoteChanged();

protected:
      /**
       * By keeping those objects available we are reducing QML components creation time when called from C++
       */
  QQmlComponent* component() { return m_qmlComponent; }
  QQmlEngine* qmlEngine() { return m_qmlEngine; }

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
  qint8 accidInKey(int k) const { return m_accidInKeyArray[k]; }

      /**
       * Returns number of note that starts a tie through @p x()
       * and number of note that ends the tie through @p y()
       */
  QPoint tieRange(TnoteObject* n);

/* ------------------ Note cursor ------------------ */

      /**
       * Sets note over which one cursor appears or null if none
       */
  void changeActiveNote(TnoteObject* aNote);
  void setActiveNotePos(qreal yPos);

      /**
       * Delays hiding note cursor when touch was released
       */
  QTimer* touchHideTimer() { return m_touchHideTimer; }

  TnoteObject* hoveredNote() { return m_hoveredNote; }
  void setHoveredNote(TnoteObject* hn) { m_hoveredNote = hn; }

  TnoteObject* pressedNote() { return m_presseddNote; }
  void setPressedNote(TnoteObject* pn) { m_presseddNote = pn; }

      /**
       * Positioning notes in a staff invokes it to keep position of first/last notes in a bar actual
       */
  void emitActiveBarChanged() { emit activeBarChanged(); }

      /**
       * emitting @p lastNoteChanged() signal updates position of 'note add' QML control, if note adding is allowed
       */
  void emitLastNote() { if (m_allowAdding) emit lastNoteChanged(); }

private:
      /**
       * Appends notes to @p m_notes list, creates corresponding @p TnotePair
       * and adds them to @p m_segments list
       */
  void appendToNoteList(QList<Tnote>& l);
  void updateClefOffset();

      /**
       * Returns @p TnotePair - a segment that can be either created or taken from @p m_spareSegments
       */
  TnotePair* getSegment(int noteNr, Tnote* n);

      /**
       * According to actual clef, returns global note number (independent on octave) from given note position
       */
  int globalNoteNr(qreal yPos);

  void clearScore();

  void enterTimeElapsed();
  void leaveTimeElapsed();

private:
                              /* Musical parameters */
  Tclef::EclefType                  m_clefType = Tclef::Treble_G;
  Tmeter                           *m_meter;
  qint8                             m_keySignature = 0;
                              /* Score switches */
  bool                              m_keySignEnabled;
  bool                              m_showExtraAccids;
  bool                              m_remindAccids;
  bool                              m_enableDoubleAccids;
  bool                              m_showNoteNames;
  bool                              m_readOnly = false;
                              /* Lists with notes, measures, staves, meter groups */
  QList<TnotePair*>                 m_segments;
  QList<TnotePair*>                 m_spareSegments;
  QList<TstaffObject*>              m_staves;
  QList<TmeasureObject*>            m_measures;
  QList<Tnote>                      m_notes;
  QList<quint8>                     m_meterGroups;
                              /* Helper variables */
  TclefOffset                       m_clefOffset;
  qreal                             m_width;
  bool                              m_adjustInProgress;
  bool                              m_keyChanged = false;
  QTimer                           *m_widthTimer;
  qint8                             m_accidInKeyArray[7];
  QQmlEngine                       *m_qmlEngine;
  QQmlComponent                    *m_qmlComponent;
  QColor                            m_nameColor;
  int                               m_nameStyle;
                              /* Note cursor */
  TnoteObject                      *m_activeNote = nullptr;
  qreal                             m_activeYpos = 0.0;
  QTimer                           *m_touchHideTimer;
  TnoteObject                      *m_hoveredNote = nullptr;
  TnoteObject                      *m_presseddNote = nullptr;
  int                               m_cursorAlter = 0;
  int                               m_activeBarNr = -1;
  Trhythm                          *m_workRhythm;
  bool                              m_allowAdding;
  QTimer                           *m_enterTimer, *m_leaveTimer;

};

#endif // TSCOREOBJECT_H
