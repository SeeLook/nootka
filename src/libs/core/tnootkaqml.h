/***************************************************************************
 *   Copyright (C) 2017-2018 by Tomasz Bojczuk                             *
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

  Q_PROPERTY(QQuickItem* mainScore READ mainScore WRITE setMainScore)
  Q_PROPERTY(TcommonInstrument* instrument READ instrument WRITE setInstrument)

public:
  explicit TnootkaQML(QObject* parent = nullptr);
  ~TnootkaQML() override;

  static TnootkaQML* instance() { return m_instance; }

      /**
       * Dialogues recognized by main QML Dialog instance of main window
       */
  enum Edialogs {
    Settings = 1, About = 2, LevelCreator = 3, ExamStart = 4, ExamSummary = 5, Charts = 6, QuickAudio = 7
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
  Q_INVOKABLE QString getLicense();
  Q_INVOKABLE QString getChanges();
  Q_INVOKABLE bool isAndroid();
  Q_INVOKABLE QStringList guitarTunings();
  Q_INVOKABLE QStringList bassTunings();
  Q_INVOKABLE Ttune tuning(int tuningType);
  Q_INVOKABLE Ttune tuning(const Tnote& s1, const Tnote& s2, const Tnote& s3, const Tnote& s4, const Tnote& s5, const Tnote& s6);
  Q_INVOKABLE Tinstrument instr(int type);
  Q_INVOKABLE QString getXmlToOpen();
  Q_INVOKABLE QString getXmlToSave();
  Q_INVOKABLE QString aboutQt();
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

  Q_INVOKABLE int fontSize();

  Q_INVOKABLE QString pixToHtml(const QString& pixName, int height = 0);

      /** Calls @p QDesktopServices::openUrl with Nootka site doc at given @p hash */
  Q_INVOKABLE  void openHelpLink(const QString& hash);

  Q_INVOKABLE qreal bound(qreal min, qreal val, qreal max);

  bool resetConfig() const { return m_resetConfig; }
  Q_INVOKABLE void setResetConfig(bool rc) { m_resetConfig = rc; }

/**
 * All stuff below is responsible for handling note changes in score, instrument and sound in/out.
 * @p TnootkaQML has score and instrument pointers to handle theirs signals when note is changed,
 * but sound is managed outside (through @p Tsound) due to all audio stuff is in sound library layer above.
 * So @p playNote() signal is emitted apparently and @p noteStarted(), @p noteFinished() methods are invoked by @p Tscound
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
};

#endif // TNOOTKAQML_H
