/***************************************************************************
 *   Copyright (C) 2021 by Tomasz Bojczuk                                  *
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

#ifndef TIMPORTSCORE_H
#define TIMPORTSCORE_H


#include <nootkacoreglobal.h>
#include "music/tmelody.h"
#include <QtCore/qobject.h>


#define IMPORT_SCORE TimportScore::instance()


class Tchunk;
class TscoreObject;
class TmelodyPart;
class TdummyChord;
class QQuickItem;


/**
 * class @class TalaChord those are notes of a chord read from musicXML.
 * They are stored in @p Tmelody @p notes() member
 * and @p noteNr() is number of the note with this chord inside
 * @p TmelodyPart @p oart.
 * NOTICE: @p TalaChord::notes() contains all chord notes
 * including note in @p part->melody() - this one is first (ID 0)
 *
 * @class TdummyChord instance has to be set during initialization.
 * It gives kind of a connection between @p TmelodyPart changes
 * and @p DummyChord QML control.
 * Needed during @p TmelodyPart transformations (split/merge, transpose)
 */
class NOOTKACORE_EXPORT TalaChord {

public:
  TalaChord(TmelodyPart* mp);

      /**
       * Number of a note in melody with this chord notes.
       */
  int noteNr() const { return m_noteNr; }

      /**
       * Notes of the chord stored in @p Tmelody
       */
  Tmelody* notes() { return &m_notes; }

  TmelodyPart    *part = nullptr;

  int count() const { return m_notes.length(); }
  void add(const Tchunk& n) { m_notes.addNote(n); }

      /**
       * Store pointer to QML @p TdummyChord instance
       */
  void setDummyChord(TdummyChord* dCh);

      /**
       * Reset parent of @p TdummyChord
       * which must be the @p TnoteItem
       */
  void setChordParent(QQuickItem* noteItem);

private:
  Tmelody         m_notes;
  int             m_noteNr = -1;
  TdummyChord    *m_dummyChord = nullptr; // QML item displaying this chord
};


/**
 * @class TmelodyPart is nested container for @class Tmelody
 * and list of score parts (staves, voices, snippets)
 */
class NOOTKACORE_EXPORT TmelodyPart : public QObject
{

  Q_OBJECT

  Q_PROPERTY(bool selected READ selected WRITE setSelected NOTIFY selectedChanged)
  Q_PROPERTY(int part READ part NOTIFY melodyChanged)
  Q_PROPERTY(int staff READ staff NOTIFY melodyChanged)
  Q_PROPERTY(int voice READ voice NOTIFY melodyChanged)
  Q_PROPERTY(int splitBarNr READ splitBarNr WRITE setSplitBarNr NOTIFY splitBarNrChanged)
  Q_PROPERTY(QList<QObject*> snippets READ snippets NOTIFY melodyChanged)
  Q_PROPERTY(QString partName READ partName NOTIFY melodyChanged)
  Q_PROPERTY(int key READ key WRITE setKey NOTIFY melodyChanged)
  Q_PROPERTY(int count READ count NOTIFY melodyChanged)

public:
  explicit TmelodyPart (TmelodyPart* parent = nullptr, int partId = 0, int staffNr = 0, int voiceNr = 0);
    ~TmelodyPart() override;

  bool selected() const { return m_selected; }
  void setSelected(bool sel);
  int part() const { return m_partId; }
  int staff() const { return m_staffNr; }
  int voice() const { return m_voiceNr; }
  int splitBarNr() const { return m_splitBarNr; }
  void setSplitBarNr(int splitNr);

  Tmelody* melody() { return m_melody; }
  void setMelody(Tmelody* m);

  int count() const { return parts.count(); }

  QList<TmelodyPart*>    parts;

      /**
       * HACK
       * This is @p parts list of @p TmelodyPart
       * converted to list of @p QObject for backward compatibility with Qt 5.12.
       * TODO: Use @p parts here when Qt 5.15 will be minimal.
       */
  QList<QObject*>        snippets();
  QList<TalaChord>       chords;

  Q_INVOKABLE void setScoreObject(TscoreObject* sObj);

  QString partName() const { return m_partName; }
  void setPartName(const QString& pn) { m_partName = pn; }

      /**
       * Key signature is only valid when this @p TmelodyPart has melody
       * or some of its @p parts has.
       * Otherwise 0 is returned
       */
  int key() const;

      /**
       * It will set key in this melody @p m_melody if exists
       * or in all melodies of @p parts.
       * Due to purpose of this method is just to prepare melody
       * for further transposition - it doesn't refresh corresponding score.
       * To do so @p melodyChanged() signal has to be emitted.
       */
  void setKey(int k);

  TscoreObject* score() { return m_scoreObj; }

  void addChordNote(TmelodyPart* part, const Tchunk& n);

signals:
  void melodyChanged();
  void selectedChanged();
  void splitBarNrChanged();

private:
  int                   m_partId = 0;
  int                   m_staffNr = 0;
  int                   m_voiceNr = 0;
  int                   m_snippet = 0;
  Tmelody              *m_melody = nullptr;
  TscoreObject         *m_scoreObj = nullptr;
  bool                  m_selected = false;
  int                   m_splitBarNr = 0;
  QString               m_partName;
};


/**
 * @class TimportScore is a singleton object which existence
 * @class Tmelody detects during reading from musicXML file.
 * If there are more parts/staves/voices in that file
 * all that is written into nested @class TmelodyPart lists.
 * A top is @p parts QList<TmelodyPart*> with part(s),
 * inside it/them is/are staves then voices.
 * Every voice can be divided for snippets.
 * @p partNames are list of parts/instruments read before parts.
 * Due to not all parts contain notes
 * @p sumarize() has to be invoked after processing XML.
 * It will prepare @p model() ready to parse by QML
 */
class NOOTKACORE_EXPORT TimportScore : public QObject
{

  Q_OBJECT

  friend class TmelodyPart;

public:
  TimportScore(Tmelody* melody, QObject *parent = nullptr);
  ~TimportScore();

  static TimportScore* instance() { return m_instance; }

  void addNote(int partId, int staff, int voice, const Tchunk& note, bool skip = false);

      /**
       * Adds @p note to quasi chord of the latest snippet melody note.
       * The quasi chord is a note list attached to the specific melody note.
       */
  void addChordNote(const Tchunk& note);

      /**
       * Prepares @p model() with not empty parts
       */
  void sumarize();

      /**
       * @p Tmelody instance which performs XML reading.
       * It keeps melody parameters like meter, key, clef, title and so,
       * but it is valid only inside @p addNote() method.
       */
  Tmelody* mainMelody() { return m_melody; }

  QList<TmelodyPart*>* parts() { return &m_parts; }

  QList<QObject*> model() { return m_partsModel; }

      /**
       * @p TRUE when there are more melodies to import
       */
  bool hasMoreParts() const { return m_hasMoreParts; }

  static int splitEveryBarNr() { return m_splitEveryBarNr; }
  static void setSplitBarNr(int splitNr);

      /**
       * List of part names from <part-list><score-part><part-name>.
       * They correspond with further <part> tags.
       */
  QStringList& partNames() { return m_partNames; }
  void addPartName(const QString& pn);

  bool multiSelect() const { return m_multiselect; }
  void setMultiSelect(bool ms) { m_multiselect = ms; }

signals:
  void importReady();

private:
  static TimportScore        *m_instance;
  QList<TmelodyPart*>         m_parts;
  QList<QObject*>             m_partsModel;
  bool                        m_hasMoreParts = false;
  Tmelody                    *m_melody; /**< Main melody */
  static int                  m_splitEveryBarNr;
  QStringList                 m_partNames;
  bool                        m_multiselect = false;
  TmelodyPart                *m_lastPart = nullptr;
};

#endif // TIMPORTSCORE_H
