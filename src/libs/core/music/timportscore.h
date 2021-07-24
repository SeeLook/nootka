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
class QThread;


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
  void setNoteNr(int nr) { m_noteNr = nr; }

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
  TdummyChord* dummyChord() { return m_dummyChord; }

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
  Q_PROPERTY(int unsupported READ unsupported NOTIFY melodyChanged)

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

      /**
       * Returns bit-wise enumerators of @p Tchunk::EimportResults
       * when musicXML part contains score elements
       * which are not handled by Nootka (32nd and more, tuplets, etc. )
       * or just @p 0 when all is supported.
       * But skip @p Tchunk::e_xmlUnsupported
       */
  int unsupported() const { return m_unsupported & ~Tchunk::e_xmlUnsupported; }
  void setUnsupported(int unS) { m_unsupported |= unS; }
  void resetUnsupported() { m_unsupported = 0; }

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
  QList<TalaChord*>      chords;

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
  int                      m_partId = 0;
  int                      m_staffNr = 0;
  int                      m_voiceNr = 0;
  Tmelody                 *m_melody = nullptr;
  TscoreObject            *m_scoreObj = nullptr;
  bool                     m_selected = false;
  int                      m_splitBarNr = 0;
  QString                  m_partName;
  int                      m_unsupported = 0;
};



class TxmlThread;

/**
 * @class TimportScore is a singleton object which existence
 * @class Tmelody detects during reading from musicXML file.
 * @p Tmelody::grabFromMusicXml() is performed in separate thread,
 * so if there are more parts/staves/voices in the XML file
 * @p wantDialog() signal is emitted and
 * all that is written into nested @class TmelodyPart lists.
 * A top is @p parts QList<TmelodyPart*> with part(s),
 * inside it/them is/are staves then voices.
 * Every voice can be divided for snippets.
 * @p partNames are list of parts/instruments read before parts.
 * Due to not all parts contain notes at the end of parsing process
 * @p Tmelody invokes @p sumarize() and signal @p xmlWasRead() is emitted.
 * so @p model() is ready then to parse by QML
 */
class NOOTKACORE_EXPORT TimportScore : public QObject
{

  Q_OBJECT

  friend class TmelodyPart;

public:
  TimportScore(const QString& xmlFileName, Tmelody* melody, QObject *parent = nullptr);
  TimportScore(const QString& xmlFileName, QObject *parent = nullptr);
  ~TimportScore() override;

  static TimportScore* instance() { return m_instance; }

  void runXmlThread();

  void addNote(int partId, int staff, int voice, const Tchunk& note, bool skip = false);

      /**
       * Adds @p note to quasi chord of the latest snippet melody note.
       * The quasi chord is a note list attached to the specific melody note.
       */
  void addChordNote(const Tchunk& note);

  void setUnsupported(int partId, int staff, int voice, int error);

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

  QThread* mainThread() { return m_mainThread; }

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

      /**
       * Any @p QObject inside @p MelodyImport.qml to obtain QML context
       * when new items are created from C++
       */
  QObject* contextObj() { return m_contextObj; }
  void setContextObject(QObject* c);

      /**
       * @p keyChanged(), @p meterChanged() and @p clefChanged()
       * are methods invoked when during reading XML
       * apparent change occurs.
       * Such a change affects all staves so every one is split:
       * in every voice of every staff a new snipped is added,
       * with melody where this particular attribute is changed,
       * so next note by @p addNote() will be appended there.
       */
  void keyChanged(const TkeySignature& newKey);
  void meterChanged(const Tmeter& newMeter);
  void clefChanged(Tclef::EclefType newClef);

      /**
       * @p TRUE only when XML thread was started and finished
       */
  bool xmlReadFinished() const;

signals:
  void importReady();
  void xmlWasRead();
  void wantDialog();

protected:
  Tmelody* newSnippet(TmelodyPart* voicePart, int partId, int staffNr, int voiceNr, Tmelody* melody);
  void setHasMoreParts(bool moreParts);

      /**
       * Called when @p TxmlThread finish parsing music XML data.
       * It moves @p TimportScore back to main thread
       * and moves @p m_parts list to main thread also.
       */
  void musicXmlReadySlot();

private:
  static TimportScore        *m_instance;
  QList<TmelodyPart*>         m_parts;
  QList<QObject*>             m_partsModel;
  bool                        m_hasMoreParts = false;
  Tmelody                    *m_melody; /**< Main melody */
  static int                  m_splitEveryBarNr;
  QStringList                 m_partNames;
  bool                        m_multiselect = false;
  TmelodyPart                *m_lastPart = nullptr; /**< Part where note was added recently */
  QObject                    *m_contextObj = nullptr;
  TxmlThread                 *m_xmlThread = nullptr;
  QThread                    *m_mainThread;
};


/**
 * @p class TxmlThread parses music XML data in separate thread.
 * @p Tmelody::grabFromMusicXml()
 * When @p Tmelody is not set in constructor
 * it creates main melody instance to import,
 * otherwise it uses given pointer to it.
 */
class NOOTKACORE_EXPORT TxmlThread : public QObject
{

  Q_OBJECT

  friend class TimportScore;

public:
  explicit TxmlThread(const QString& xmlFileName, QObject* parent = nullptr);
  explicit TxmlThread(const QString& xmlFileName, Tmelody* m, QObject* parent = nullptr);
  ~TxmlThread() override;

  void start();

  Tmelody* mainMelody() { return m_melody; }

  bool isFinished() const;

signals:
  void musicXmlRead();

private:
  void commonConstructor();

private:
  Tmelody               *m_melody = nullptr;
  QString                m_xmlFileName;
  QThread               *m_thread;
  bool                   m_melodyCreated = false;
};

#endif // TIMPORTSCORE_H
