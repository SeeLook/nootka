/***************************************************************************
 *   Copyright (C) 2012-2021 by Tomasz Bojczuk                             *
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
 *  You should have received a copy of the GNU General Public License       *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#ifndef TEXECUTORSUPPLY_H
#define TEXECUTORSUPPLY_H


#include "tnotetoplay.h"
#include <music/tnotestruct.h>
#include <exam/tqaunit.h>
#include <QtCore/qpointer.h>


class TequalRand;
class MainWindow;
class Texam;
class QWidget;
class Tlevel;


/**
 * A purpose of this class is to make the exam executor smaller.
 * Also it has eventFilter() re-implementation to manage right mouse button,
 * witch can not be simply captured by contextMenuEvent from disabled widgets. 
 */
class TexecutorSupply : public QObject
{
  Q_OBJECT

public:
  TexecutorSupply(Tlevel *level, QObject *parent = nullptr);
  virtual ~TexecutorSupply();

  void createQuestionsList(QList<TQAgroup> &list);
  Tnote determineAccid(const Tnote& n);
  void listForRandomNotes(TkeySignature k, QList<TQAgroup> &qaList);

  Tnote forceEnharmAccid(const Tnote& n);

      /**
       * Switches given style to opposite (letters or solfege).
       * If none specified, gives opposite to previous returned.
       */
  Tnote::EnameStyle randomNameStyle(int style = -1);

      /**
       * Returns list with possible positions of a note on given position.
       * Given fingerPos (position) is not included.
       * If strCheck is true it is excluding strings unavailable in a level.
       * @p order determines whether string order is respected.
       */
  void getTheSamePos(TfingerPos &fingerPos, QList<TfingerPos> &posList, bool strCheck = true, bool order = true);

      /**
       * The same as getTheSamePos() but string order is ignored
       */
  void getTheSamePosNoOrder(TfingerPos &fingerPos, QList<TfingerPos> &posList, bool strCheck = true);

      /**
       * Returns randomized number in questions list proper for question and answer on the guitar.
       */
  quint16 getQAnrForGuitarOnly() const { return m_fretFretList[qrand() % m_fretFretList.size()]; }

      /**
       * Returns true when questions and answers as guitar are possible.
       */
  bool isGuitarOnlyPossible() const { return !m_fretFretList.isEmpty(); }

      /**
       * Using level settings calculates number of mandatory questions in exam.
       * It is stored in @p m_totalQuestNr.
       */
  int obligQuestions() const { return m_obligQuestNr; }
  int qaPossibilities() const { return m_qaPossib; }

  void setFinished(bool wasFinished = true) { m_wasFinished = wasFinished;}
  bool wasFinished() const { return m_wasFinished; }

      /**
       * Returns @p True when level and user instrument preferences determine corrected answer to be played.
       * When corrections are possible on guitar it returns @p FALSE
       */
  static bool isCorrectedPlayable() { return m_playCorrections; }

      /**
       * Check. are current guitar fret number and tuning different than exercise or exam level.
       * Adjust globals (tune and fret number have to be stored before) and
       * displays message about changes, if any.
       */
  static void checkGuitarParamsChanged(Texam* exam);

      /**
       * It is true when checkGuitarParamsChanged() obtained message to display.
       * It is used to inform MainWindow that status label is busy by the message.
       */
  static bool paramsChangedMessage() { return m_paramsMessage; }

      /**
       * Returns color depends on answer type.
       */
  static QColor& answerColor(const TQAunit* answer);

      /**
       * Determines color by mistake type.
       */
  static QColor& answerColor(quint32 mistake);

  int loFret() { return m_loFret; }
  int hiFret() { return m_hiFret; }

      /**
       * Checks are given notes matched. Result (mistake(s)) is written into @p curQ.
       * When @p reqOctave is false octaves check are skipped
       * When @p reqAccid is false Cis matches to Des and etc...
       */
  void checkNotes(TQAunit* curQ, Tnote& expectedNote, Tnote& userNote, bool reqOctave, bool reqAccid);

      /**
       * Compares given melodies respecting settings in current level. Saves results into given @p Tattempt.
       * There is no answered notes time so Tattempt::times list remains empty.
       * When notes count are not equal, the biggest number is taken
       * and the rest of 'additional' notes are marked as 'wrong'.
       * The attempt effectiveness is updated at the end.
       */
  void compareMelodies(Tmelody* q, Tmelody* a, Tattempt* att);

      /**
       * Compares given melody with list of played notes.
       * @p Tattempt::times is filled up and wrong intonation is detected if level requires it.
       * When notes count are not equal, the biggest number is taken
       * and the rest of 'additional' notes are marked as 'wrong'
       * The attempt effectiveness is updated at the end.
       */
  void compareMelodies(Tmelody* q, QList<TnoteStruct>& a, Tattempt* att);

      /**
       * Compares two given melodies (@p Tmelody pointers)
       * - first one (question) was played - so it could be transposed
       * - second one (answer) was written in the score
       */
  void compareWrittenFromPlayed(Tmelody* q, Tmelody * a, Tattempt* attempt, int transposition);

  void comparePlayedFromScore(Tmelody* q, QVector<TnoteToPlay>& toPlay, QVector<TnoteStruct>& a, Tattempt* att, int transposition);

  quint32 comparePitches(int p1, int p2, bool requireOctave);

      /**
       * Returns a key signature depend on a current level settings.
       * Lo level key when single key level but also it can random it for multiple keys levels.
       * In those cases given note may be adjusted (changed) to randomized key (accidental may be converted)
       */
  TkeySignature getKey(Tnote& note);

      /**
       * Resets key signature random. Used when an exercise goes into an exam.
       */
  void resetKeyRandom();

      /**
       * Returns random time signature (integer from @p Tmeter::Emeter) from the meter list defined in level.
       * It uses @p TequalRand class for it if there are more time signatures
       */
  int randomMeter();
  void resetMeterRandom();

      /**
       * Gives number of measures for randomized melody.
       * It is calculated when bar number is variable and the value increases as exam/exercise is progressing
       */
  int getBarNumber(int questNr, int penallNr);

      /**
       * Looks for shortest rhythmic value in given melody to determine optimal quantization value.
       * So far supported are 6 (16th) and 12 (8th)
       */
  int findQuantization(Tmelody* m);

signals:
  void rightButtonClicked();

protected:
#if !defined (Q_OS_ANDROID)
  virtual bool eventFilter(QObject* obj, QEvent* event);
#endif

      /**
       * Calculates how many question/answer combinations are possible for different types enabled in a level.
       */
  void calcQAPossibleCount();

      /**
       * Determines should be corrections played or shown on the guitar
       */
  static void checkPlayCorrected(Tlevel* level);


private:
      /**
       * Checks is given note in level key signature (if single key) 
       * or in some key in key signature range
       */
  bool isNoteInKey(Tnote &n);

      /**
       * Appends given note and technical data to question list.
       */
  void addToList(QList<TQAgroup> &list, Tnote &n, Ttechnical &t);

      /**
       * Returns number of string - itself (realNr - 1) when strings order is not respected
       * or pitch-ordered taken from gl->strOrder()
       */
  quint8 strNr(quint8 str0to6, bool ordered = true);

private:
  Tlevel                   *m_level;
  int                       m_obligQuestNr; /**< Total number of questions to answer in exam (without penalties) */
  int                       m_qaPossib;
  Tnote::Ealter             m_prevAccid; /**< Previous accidental used. */

      /**
       * intervals between asking about double accidentals.
       * By default every forth question is with double accidentals.
       */
  int                     m_dblAccidsCntr;

      /**
       * It counts occurrence of e, f, b and c notes
       * to avoid asking about eis fes his and ces to often 
       * witch occurs in scores rarely.
       */
  int                     m_eisCesCntr;

        /**
         * to switch naming style between "do re mi" and "c d e"
         * when question and answer are note name.
         */
  bool                     m_isSolfege;

      /**
       * List of numbers of questions in the questions list that can be 
       * either question as position or answer. 
       * The list is initialized in createQuestionsList() method if level has that settings.
       */
  QList<quint16>           m_fretFretList;

  bool                     m_wasFinished; /**< True when message about finished exam was shown. */
  static bool              m_playCorrections; /**< Corrected answers will be played (True) or shown on the guitar (False) */
  static bool              m_paramsMessage; /**< True when message was displayed */

      /**
       * Temporary stored fret range of a level.
       * Values may be differ than originally in level when level no needs guitar
       * but to show fret range they are adjusted.
       * LEVEL VALUES REMAINED UNTOUCHED
       */
  int                      m_loFret, m_hiFret;
  TequalRand              *m_randKey = nullptr;
  TequalRand              *m_randMeter = nullptr;
  QList<int>               m_meterList; /**< List of time signatures available in level  */

};

#endif // TEXECUTORSUPPLY_H
