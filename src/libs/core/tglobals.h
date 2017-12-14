/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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

#ifndef TGLOBALS_H
#define TGLOBALS_H


#include <nootkacoreglobal.h>
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>
#include <QtCore/qrect.h>
#include <QtGui/qcolor.h>
#include <music/tnote.h>
#include <music/tinstrument.h>


class TlayoutParams;
class TscoreParams;
class Ttune;
class QSettings;
class TexamParams;
class TaudioParams;
class TtuneObject;


#define   GLOB     Tglobals::instance()


class NOOTKACORE_EXPORT Tglobals : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QRect geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
  Q_PROPERTY(bool useAnimations READ useAnimations WRITE setUseAnimations NOTIFY useAnimationsChanged)
  Q_PROPERTY(QString lang READ getLang WRITE setLang)
  Q_PROPERTY(qreal scale READ guiScale WRITE setGuiScale NOTIFY guiScaleChanged)
  /* Score switches */
  Q_PROPERTY(bool showEnharmNotes READ showEnharmNotes WRITE setShowEnharmNotes NOTIFY showEnharmNotesChanged)
  Q_PROPERTY(QColor enharmNoteColor READ getEnharmNoteColor WRITE setEnharmNoteColor)
  Q_PROPERTY(QColor noteCursorColor READ getNoteCursorColor WRITE setNoteCursorColor NOTIFY noteCursorColorChanged)
  Q_PROPERTY(bool singleNoteMode READ isSingleNote WRITE setSingleNote NOTIFY singleNoteModeChanged)
  Q_PROPERTY(bool enableDoubleAccids READ enableDoubleAccids WRITE setEnableDoubleAccids NOTIFY enableDoubleAccidsChanged)
  Q_PROPERTY(bool keySignatureEnabled READ keySignatureEnabled WRITE setKeySignatureEnabled NOTIFY enableKeySignatureChanged)
  Q_PROPERTY(bool showKeyName READ showKeyName WRITE setShowKeyName NOTIFY showKeyNameChanged)
  Q_PROPERTY(QString majorKeyNameSufix READ majorKeyNameSufix WRITE setMajorKeyNameSufix)
  Q_PROPERTY(QString minorKeyNameSufix READ minorKeyNameSufix WRITE setMinorKeyNameSufix)
  Q_PROPERTY(int keyNameStyle READ keyNameStyle WRITE setKeyNameStyle)
  Q_PROPERTY(bool rhythmsEnabled READ rhythmsEnabled WRITE setRhythmsEnabled NOTIFY rhythmsEnabledChanged)

  Q_PROPERTY(int clefType READ clefType WRITE setClefType NOTIFY clefTypeChanged)

  /* Note name switches */
  Q_PROPERTY(qreal namesOnScore READ namesOnScore WRITE setNamesOnScore NOTIFY namesOnScoreChanged)
  Q_PROPERTY(int noteNameStyle READ noteNameStyle WRITE setNoteNameStyle NOTIFY noteNameStyleChanged)
  Q_PROPERTY(bool seventhIsB READ seventhIsB WRITE setSeventhIsB NOTIFY seventhIsBChanged)
  Q_PROPERTY(QColor nameColor READ nameColor WRITE setNameColor NOTIFY nameColorChanged)

  /* Instrument switches */
  Q_PROPERTY(Tinstrument instrument READ instrument NOTIFY instrumentChanged)
  Q_PROPERTY(TtuneObject* tuning READ tuning NOTIFY tuningChanged)
  Q_PROPERTY(qreal transposition READ transposition WRITE setTransposition NOTIFY transpositionChanged)

  Q_PROPERTY(QColor fingerColor READ fingerColor WRITE setFingerColor NOTIFY fingerColorChanged)
  Q_PROPERTY(QColor selectedColor READ selectedColor WRITE setSelectedColor NOTIFY selectedColorChanged)
  Q_PROPERTY(bool preferFlats READ preferFlats WRITE setPreferFlats NOTIFY preferFlatsChanged)
  Q_PROPERTY(int fretNumber READ fretNumber NOTIFY guitarParamsChanged)
  Q_PROPERTY(bool showOtherPos READ showOtherPos WRITE setShowOtherPos)
  Q_PROPERTY(QString markedFrets READ markedFrets WRITE setMarkedFrets);

  /* Sound switches */
  Q_PROPERTY(bool audioInEnabled READ audioInEnabled WRITE setAudioInEnabled)
  Q_PROPERTY(int audioInstrument READ audioInstrument WRITE setAudioInstrument)
  Q_PROPERTY(QString inDevName READ inDevName WRITE setInDevName);
  Q_PROPERTY(qreal minDuration READ minDuration WRITE setMinDuration)
  Q_PROPERTY(qreal minVolume READ minVolume WRITE setMinVolume NOTIFY minVolumeChanged)
  Q_PROPERTY(int detectionMethod READ detectionMethod WRITE setDetectionMethod)
  Q_PROPERTY(qreal minSplitVol READ minSplitVol WRITE setMinSplitVol)
  Q_PROPERTY(qreal skipStillerVal READ skipStillerVal WRITE setSkipStillerVal)
  Q_PROPERTY(bool useFilter READ useFilter WRITE setUseFilter)
  Q_PROPERTY(bool audioOutEnabled READ audioOutEnabled WRITE setAudioOutEnabled)
  Q_PROPERTY(QString outDevName READ outDevName WRITE setOutDevName);
  Q_PROPERTY(bool forwardInput READ forwardInput WRITE setForwardInput)

  /* Exam */
  Q_PROPERTY(bool isExam READ isExam WRITE setIsExam NOTIFY isExamChanged)
  Q_PROPERTY(QString student READ student WRITE setStudent NOTIFY studentChanged)
  Q_PROPERTY(QColor correctColor READ correctColor WRITE setCorrectColor NOTIFY correctColorChanged)
  Q_PROPERTY(QColor notBadColor READ notBadColor WRITE setNotBadColor NOTIFY notBadColorChanged)
  Q_PROPERTY(QColor wrongColor READ wrongColor WRITE setWrongColor NOTIFY wrongColorChanged)

public:

      /** If @p true, setting are loaded from temporary config file */
  Tglobals(QObject* parent = nullptr);
  ~Tglobals() override;

      /**
       * Instance (single for whole Nootka) of Tglobals class.
       * Also available through @p GLOB macro
       */
  static Tglobals* instance() { return m_instance; }

      /** Geometry of main Nootka window */
  QRect geometry() const { return m_geometry; }
  void setGeometry(const QRect& g) { m_geometry = g; }

      /** To show GUI animations. */
  bool useAnimations() const { return m_useAnimations; }
  void setUseAnimations(bool use);

  QString getLang() const { return lang; }
  void setLang(const QString& l) { lang = l; }

  qreal guiScale() const { return m_guiScale; }
  void setGuiScale(qreal sc);

  /* ------------------ Score switches ------------------ */
  bool showEnharmNotes() const;
  void setShowEnharmNotes(bool showEnharm);

  QColor getEnharmNoteColor() const;
  void setEnharmNoteColor(const QColor& c);

  QColor getNoteCursorColor() const;
  void setNoteCursorColor(const QColor& c);

  bool isSingleNote() const;
  void setSingleNote(bool sn);

  bool namesOnScore() const;
  void setNamesOnScore(bool showNames);

  QColor nameColor() const;
  void setNameColor(const QColor& nameC);

  bool enableDoubleAccids() const;
  void setEnableDoubleAccids(bool dblAcc);

  bool keySignatureEnabled() const;
  void setKeySignatureEnabled(bool enKey);

  int clefType() const;
  void setClefType(int clType);

  bool showKeyName() const;
  void setShowKeyName(bool showKey);

  QString majorKeyNameSufix() const;
  void setMajorKeyNameSufix(const QString& mkns);

  QString minorKeyNameSufix() const;
  void setMinorKeyNameSufix(const QString& mkns);

  int keyNameStyle() const;
  void setKeyNameStyle(int keyStyle);

  bool rhythmsEnabled() const;
  void setRhythmsEnabled(bool enR);

  /* ------------------ Note name switches ------------------ */
  bool seventhIsB() const;
  void setSeventhIsB(bool isB);

  int noteNameStyle() const;
  void setNoteNameStyle(int nameStyle);

  /* ------------------ Instrument switches ------------------ */
  QColor fingerColor() const { return GfingerColor; }
  void setFingerColor(const QColor& fc);

  QColor selectedColor() const { return GselectedColor; }
  void setSelectedColor(const QColor& sc);

  bool preferFlats() const { return GpreferFlats; }
  void setPreferFlats(bool prefFlat) { GpreferFlats = prefFlat; }

  bool showOtherPos() const { return GshowOtherPos; }
  void setShowOtherPos(bool show) { GshowOtherPos = show; }

  int transposition() const;
  void setTransposition(int t);

  QString markedFrets() const;
  void setMarkedFrets(const QString& frets);

  /* ------------------ Sound switches ------------------ */
  bool audioInEnabled() const;
  void setAudioInEnabled(bool inEnabled);

  int audioInstrument() const;
  void setAudioInstrument(int ai);

  QString inDevName() const;
  void setInDevName(const QString& inName);

  qreal minDuration() const;
  void setMinDuration(qreal md);

  qreal minVolume() const;
  void setMinVolume(qreal mv);

  int detectionMethod() const;
  void setDetectionMethod(int m);

  qreal minSplitVol() const;
  void setMinSplitVol(qreal msv);

  qreal skipStillerVal() const;
  void setSkipStillerVal(qreal ssv);

  bool useFilter() const;
  void setUseFilter(bool use);

  bool audioOutEnabled() const;
  void setAudioOutEnabled(bool outEnabled);

  QString outDevName() const;
  void setOutDevName(const QString& odn);

  bool forwardInput() const;
  void setForwardInput(bool fi);

  /* ------------------ Exam switches ------------------ */
  bool isExam() const { return m_isExam; }
  void setIsExam(bool is);

  QString student() const;
  void setStudent(const QString& st);

  QColor correctColor() const { return EanswerColor; }
  void setCorrectColor(const QColor& c);

  QColor notBadColor() const { return EnotBadColor; }
  void setNotBadColor(const QColor& c);

  QColor wrongColor() const { return EquestionColor; }
  void setWrongColor(const QColor& c);

      /**
       * Updates key signature names according to name style and major/minor suffixes.
       * Emits @p keyNameChanged() to inform MainScore.qml
       */
  Q_INVOKABLE void updateKeySignatureNames();

      /** This method return application install path - path from where Nootka was started. */
  static QString getInstPath(QString appInstPath);
  static QString& path; /**< Reference to Tpath::main - Nootka resources path */

  void storeSettings(QSettings* cfg); /**< Saves settings stored in @p cfg */
  void loadSettings(QSettings* cfg); /**< Loads Nootka settings from file to @p cfg */

  QString version;
  bool isFirstRun; /**< to show or skip first run wizard*/
  QString lang; /**< Application language. If empty - selected from system info*/
  QSettings *config; /**< Pointer to QSettings instance of Nootka */

        /**
         * Let's have a convention:
         * globals settings for @class TnoteName will started from 'N' letter
         * for @class TscoreWidget and @class TscoreWidgetSimple
         * and for guitar (@class TfingerBoard) from 'G' letter.
         * For sound and exam there are pointers to appropirate classes with theirs parameters.
         */

  //============ score settings =============================================================
  TscoreParams *S; /**< Score parameters */


    /** Guessing solfege name style from current locale setting. F.e.: ru is e_russian_Ci */
  Tnote::EnameStyle getSolfegeStyle();

  QString lastXmlDir() const;
  void setLastXmlDir(const QString& lastXml);

  //============ guitar settings =============================================================
  Tinstrument instrument() { return m_instrument; }
  Q_INVOKABLE void setInstrument(Tinstrument::Etype t);
  Q_INVOKABLE void setInstrument(int t) { setInstrument(t >= 0 && t < INSTR_COUNT ? static_cast<Tinstrument::Etype>(t) : Tinstrument::NoInstrument); }
  int fretNumber() const { return GfretsNumber; }
  unsigned  GfretsNumber; /**< default 19 */
  bool GisRightHanded; /**< default true */
  bool GshowOtherPos; /**< Shows other possibilities of note (sound) on the fretboard (default true) */
  QColor GfingerColor; /**< rules the same like in S->enharmNotesColor */
  QColor GselectedColor;
      /**
       * Guitar params like guitar type or fret/string numbers, fret marks - affects guitar repainting.
       * Keep their change in single call then.
       */
  Q_INVOKABLE void setGuitarParams(int fretNr, const Ttune& tun /*TODO , bool rightHand */);

      /**
       * Actual tune of the guitar also with information about strings number
       * available by Ttune::stringsNr()
       */
  Ttune *Gtune() { return m_tune; }
  Q_INVOKABLE void setTune(const Ttune &t);
  TtuneObject* tuning() { return m_tuneObject; }
  Q_INVOKABLE int stringNumber();

      /**
       * It returns real string number (0 - 5) when @param strNr
       * is sorted number from highest (0) to lowest (5)
       */
  qint8 strOrder(qint8 strNr) { return m_order[strNr]; }
  Tnote hiString(); /**< Returns the highest (usually first - 0) string. */
  Tnote loString(); /**< Returns the lowest (usually last) string */

      /** The highest available note in current tune with current fret number */
  Tnote hiNote() { return Tnote(hiString().chromatic() + GfretsNumber); }
  Tnote loNote() { return loString(); } // the same as loString()

      /** It says witch accidentals are preferred while user clicks guitar
       * and note is calculated. Default are sharps*/
  bool GpreferFlats; // default false
  QList<QVariant> GmarkedFrets;


  TexamParams *E; /**< Exam parameters */
  QColor EquestionColor; /**< red */
  QColor EanswerColor; /**< green */
  QColor EnotBadColor; /**< #ff8000 (orange) */

  TaudioParams *A;  /**< Audio parameters */
  TlayoutParams *L; /**< Main window Layout params. */

      /**
       * Returns system user name (log-in name)
       */
  static QString systemUserName();

signals:
  void geometryChanged(); /**< It is never emitted :(  */
  void useAnimationsChanged();
  void guiScaleChanged();
  void noteCursorColorChanged();
  void singleNoteModeChanged();
  void showEnharmNotesChanged();
  void namesOnScoreChanged();
  void enableDoubleAccidsChanged();
  void enableKeySignatureChanged();
  void clefTypeChanged();
  void nameColorChanged();
  void noteNameStyleChanged();
  void seventhIsBChanged();
  void showKeyNameChanged();
  void keyNameChanged();
  void rhythmsEnabledChanged();
  void instrumentChanged();
  void tuningChanged();
  void fingerColorChanged();
  void selectedColorChanged();
  void preferFlatsChanged(); /**< Fake, this option doesn't affect QML */
  void guitarParamsChanged();
  void transpositionChanged();
  void minVolumeChanged();
  void isExamChanged();
  void studentChanged();
  void correctColorChanged();
  void notBadColorChanged();
  void wrongColorChanged();

private:
  static Tglobals           *m_instance;
  Ttune                     *m_tune; /**< current guitar tune */
  TtuneObject               *m_tuneObject;
  qint8                      m_order[6]; /**< Strings order is determined in @param setTune() method */
  QRect                      m_geometry;
  bool                       m_useAnimations;
  Tinstrument                m_instrument;
  qreal                      m_guiScale;
  bool                       m_isExam = false;

};
#endif // TGLOBALS_H

