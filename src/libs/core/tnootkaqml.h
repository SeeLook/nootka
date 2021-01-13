/***************************************************************************
 *   Copyright (C) 2017-2021 by Tomasz Bojczuk                             *
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

#ifndef TNOOTKAQML_H
#define TNOOTKAQML_H


#include <nootkacoreglobal.h>
#include "music/tmeter.h"
#include "music/tclef.h"
#include "music/ttune.h"
#include "music/tinstrument.h"

#include <QtCore/qobject.h>
#include <QtGui/qcolor.h>


class TcommonInstrument;
class TscoreObject;
class QQuickItem;
class QQmlEngine;
class Taction;
class QTimer;


#define   NOO   TnootkaQML::instance()

/**
 * Singleton object to manage (create) custom types from QML
 * In constructor it registers types accessible from QML in Nootka
 * Access it through @p Noo object
 * Also it provides many helper functions through @p Noo
 *
 * Also it acts as node to tie note changes of sound, instrument and score 
 */
class NOOTKACORE_EXPORT TnootkaQML : public QObject
{

  Q_OBJECT

  Q_PROPERTY(Taction* settingsAct READ settingsAct NOTIFY dummyActionSignal)
  Q_PROPERTY(Taction* scoreAct READ scoreAct NOTIFY dummyActionSignal)
  Q_PROPERTY(Taction* melodyAct READ melodyAct NOTIFY dummyActionSignal)
  Q_PROPERTY(Taction* levelAct READ levelAct NOTIFY dummyActionSignal)
  Q_PROPERTY(Taction* chartsAct READ chartsAct NOTIFY dummyActionSignal)
  Q_PROPERTY(Taction* examAct READ examAct NOTIFY dummyActionSignal)
  Q_PROPERTY(Taction* aboutAct READ aboutAct NOTIFY dummyActionSignal)

  Q_PROPERTY(QQuickItem* mainScore READ mainScore WRITE setMainScore)
  Q_PROPERTY(TcommonInstrument* instrument READ instrument WRITE setInstrument)

  Q_PROPERTY(QColor messageColor READ messageColor WRITE setMessageColor NOTIFY messageColorChanged)

public:
  explicit TnootkaQML(QObject* parent = nullptr);
  ~TnootkaQML() override;

  static TnootkaQML* instance() { return m_instance; }

  Taction* settingsAct() { return m_settingsAct; }
  Taction* scoreAct() { return m_scoreAct; }
  Taction* melodyAct() { return m_melodyAct; }
  Taction* levelAct() { return m_levelAct; }
  Taction* chartsAct() { return m_chartsAct; }
  Taction* examAct() { return m_examAct; }
  Taction* aboutAct() { return m_aboutAct; }

  QColor messageColor() { return m_messageColor; }
  void setMessageColor(const QColor& mc);

      /**
       * Dialogues recognized by main QML Dialog instance of main window
       * @p NoDialog only creates 'dialog loader' instance but does nothing
       */
  enum Edialogs {
    NoDialog = 0, Settings = 1, About = 2, LevelCreator = 3, ExamStart = 4, ExamSummary = 5, Charts = 6, Tuner = 7, MelodyGenerator = 8
  };
  Q_ENUM(Edialogs)

  enum EnameStyle {
    Norsk_Hb = 0,     /**< for letters with signs f.e. C# Cx or Cb    !! THIS IS DEFAULT !! */
    Deutsch_His = 1,  /**< for letters with names f.e. Cis Cisis or Ces H and B (H with flat) */
    Italiano_Si = 2,  /**< for classical Do Re Mi Fa Sol La Si */
    English_Bb = 3,   /**< like @p e_norsk_Hb but with B and Bb (B flat) */
    Nederl_Bis = 4,   /**< like @p e_deutsch_His but with B ens Bes */
    Russian_Ci = 5    /**< classical but in Russian: До Ре Ми Фа Соль Ля Си */
  };
  Q_ENUM(EnameStyle)

  Q_INVOKABLE QString version();
  Q_INVOKABLE Tclef clef(int type);
  Q_INVOKABLE Tmeter meter(int m);
  Q_INVOKABLE Tnote note(int pitch, int octave, int alter, int rhythm = 3, bool rest = false, bool dot = false);
  Q_INVOKABLE Tnote note(const Tnote& n, int rhythm = 3, bool rest = false, bool dot = false);
  Q_INVOKABLE Tnote note(const Tnote& n, const Trhythm& r);
  Q_INVOKABLE Tnote note(int chroma, bool sharp = true);
  Q_INVOKABLE Tnote emptyNote();
  Q_INVOKABLE int octave(const Tnote& n) { return static_cast<int>(n.octave()); }
  Q_INVOKABLE int pitch(const Tnote& n) { return static_cast<int>(n.note()); }
  Q_INVOKABLE int alter(const Tnote& n) { return static_cast<int>(n.alter()); }
  Q_INVOKABLE Tnote transpose(Tnote n, int semitones);
  Q_INVOKABLE Trhythm rhythm(int rtm, bool rest, bool dot, bool triplet);
  Q_INVOKABLE QString rhythmText(const Trhythm& r);
  Q_INVOKABLE QString noteName(const Tnote& n, int style, bool showOctave = true);
  Q_INVOKABLE QString majorKeyName(int key);
  Q_INVOKABLE QString minorKeyName(int key);
  Q_INVOKABLE QString majAndMinKeyName(int key);
  Q_INVOKABLE bool isAndroid();
  Q_INVOKABLE bool isWindows();
  Q_INVOKABLE bool isMac();
  Q_INVOKABLE QStringList guitarTunings();
  Q_INVOKABLE QStringList bassTunings();
  Q_INVOKABLE Ttune tuning(int tuningType);
  Q_INVOKABLE Ttune tuning(const Tnote& s1, const Tnote& s2, const Tnote& s3, const Tnote& s4, const Tnote& s5, const Tnote& s6);
  Q_INVOKABLE Tinstrument instr(int type);
  Q_INVOKABLE QString getXmlToOpen();
  Q_INVOKABLE QString getXmlToSave(const QString& fileName = QString());
  Q_INVOKABLE QString pix(const QString& imageFileName);
  Q_INVOKABLE QString TR(const QString& context, const QString& text, const QString& disambiguation = QString(), int n = -1);

      /**
       * Returns given color @p c with alpha channel @p a
       */
  Q_INVOKABLE QColor alpha(const QColor& c, int a);

      /**
       * Returns randomized color, @p alpha is alpha level
       * @p level (220 by default) determines maximal value of color [0 - 255].
       * Using smaller value avoids generating dark colors
       */
  Q_INVOKABLE QColor randomColor(int alpha = 255, int level = 220);
  Q_INVOKABLE QColor invert(const QColor& c);

  Q_INVOKABLE qreal hue(const QColor& c) const;
  Q_INVOKABLE qreal saturation(const QColor& c) const;
  Q_INVOKABLE qreal lightness(const QColor& c) const;

  Q_INVOKABLE int fontSize();
  Q_INVOKABLE QString fontFamily();
  Q_INVOKABLE int fingerPixels();

      /**
       * height or width of a screen (less of them)
       */
  Q_INVOKABLE int shortScreenSide();

      /**
       * height or width of a screen (bigger of them)
       */
  Q_INVOKABLE int longScreenSide();

  Q_INVOKABLE QString pixToHtml(const QString& pixName, int height = 0);

      /**
       * Calls @p QDesktopServices::openUrl with Nootka site doc at given @p hash
       */
  Q_INVOKABLE void openHelpLink(const QString& hash);

  Q_INVOKABLE qreal bound(qreal min, qreal val, qreal max);

  bool resetConfig() const { return m_resetConfig; }
  Q_INVOKABLE void setResetConfig(bool rc) { m_resetConfig = rc; }

      /**
       * Main @p QQmlEngine created at very beginning of Nootka launch
       */
  QQmlEngine* qmlEngine() { return m_qmlEngine; }
  void setQmlEngine(QQmlEngine* e);

  void openFile(const QString& runArg);

      /**
       * Emits signal with given text to QML to display status message on Nootka window at given position
       * @p tipPos value represents QQuickItem::TransformOrigin enumerator.
       * @p default 1 is top center of window
       */
  Q_INVOKABLE void setStatusTip(const QString& statusText, int tipPos = 1);

      /**
       * Displays status message in declared period @p time
       * at position @p pos - top center by default (see @p QQuickItem::TransformOrigin)
       */
  void showTimeMessage(const QString& message, int time, int pos = 1);
  bool messageTimerActive() const;

      /**
       * Returns text of question/ answer kind depends on given value of @p qaType.
       * It could be:
       * @li 0 - as note on the staff
       * @li 1 - as note name
       * @li 2 - on instrument
       * @li 3 - as played sound
       */
  Q_INVOKABLE QString qaTypeText(int qaType);

      /**
       * Name of seventh note according to localization.
       * Returns 'b' or 'h' depends on translation.
       */
  Q_INVOKABLE QString note7translated() const;

      /**
       * Default style grab from translation 'letters' or 'solfege'
       */
  Q_INVOKABLE QString keyNameTranslated() const;

/**
 * All stuff below is responsible for handling note changes in score, instrument and sound in/out.
 * @p TnootkaQML has score and instrument pointers to handle theirs signals when note is changed,
 * but sound is managed outside (through @p Tsound) due to all audio stuff is in sound library layer above.
 * So @p playNote() signal is emitted apparently and @p noteStarted(), @p noteFinished() methods are invoked by @p Tscound
 *
 * TRANSPOSITION
 * Only notes to be played have to be transposed according @p GLOB->transposition()
 * to sound properly for current transposing instrument.
 * Pitch detected notes are already transposed back to score visual notation.
 */
  QQuickItem* mainScore() { return m_mainScore; }
  void setMainScore(QQuickItem* ms);

  TscoreObject* scoreObj() { return m_scoreObject; }

  TcommonInstrument* instrument() { return m_instrument; }
  void setInstrument(TcommonInstrument* ci);

  void noteStarted(const Tnote& n);
  void noteFinished(const Tnote& n);

      /**
       * Id (number) of selected note on the score or @p -1 if none
       */
  int selectedNoteId() const;

      /**
       * This method is invoked by @p Tsound when playing note changes,
       * and it is used to select corresponding note on the score and on the instrument
       */
  void selectPlayingNote(int id);

      /**
       * Number of notes on the score
       */
  int scoreNotesCount() const;

      /**
       * Reference to the notes list on the score
       */
  QList<Tnote>& scoreNoteList() const;

signals:
  void playNote(const Tnote&);
  void dummyActionSignal();
  void settingsActTriggered();
  void scoreActTriggered();
  void melodyActTriggered();
  void levelActTriggered();
  void chartsActTriggered();
  void examActTriggered();
  void aboutActTriggered();
  void statusTip(const QString& statusText, int tipPos);
  void messageColorChanged();

protected:
  void connectInstrument();
  void instrumentChangesNoteSlot();
  void examStartStop();
  void scoreChangedNoteSlot();

  int getTechicalFromScore();

private:
  static TnootkaQML             *m_instance;

  TcommonInstrument             *m_instrument = nullptr;
  QQuickItem                    *m_mainScore = nullptr;
  TscoreObject                  *m_scoreObject = nullptr;
  bool                           m_nodeConnected = false;
  bool                           m_ignoreScore = false; /**< Becomes @p TRUE to ignore @p scoreChangedNote() slot  */
  bool                           m_resetConfig = false;
  QQmlEngine                    *m_qmlEngine = nullptr;
  Taction                       *m_scoreAct = nullptr;
  Taction                       *m_settingsAct, *m_levelAct, *m_examAct, *m_melodyAct, *m_chartsAct, *m_aboutAct;

  QColor                         m_messageColor;
  QTimer                        *m_messageTimer = nullptr;
  int                            m_messagePos;
};

#endif // TNOOTKAQML_H
