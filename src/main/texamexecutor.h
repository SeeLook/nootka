/***************************************************************************
 *   Copyright (C) 2011-2020 by Tomasz Bojczuk                             *
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


#ifndef TEXAMEXECUTOR_H
#define TEXAMEXECUTOR_H

#include <exam/tqaunit.h>
#include <exam/tlevel.h>

#include <QtQuick/qquickitem.h>
#include <QtCore/qpointer.h>
#include <QtGui/qcolor.h>


class TnoteStruct;
class TexamMelody;
class Tpenalty;
class TequalRand;
class Tsound;
class Texercises;
class TglobalExamStore;
class TtipHandler;
class QTimer;
class TexecutorSupply;
class Texam;
class Taction;


#define EXECUTOR TexamExecutor::instance()

/**
 * This class manages exam executing and practicing.
 */
class TexamExecutor : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(QList<QObject*> examActions READ examActions NOTIFY examActionsChanged)
  Q_PROPERTY(TtipHandler* tipHandler READ tipHandler NOTIFY tipHandlerCreated)
  Q_PROPERTY(bool isExercise READ isExercise)
  Q_PROPERTY(bool showPitchView READ showPitchView NOTIFY questionChanged)
  Q_PROPERTY(bool showRtmView READ showRtmView NOTIFY questionChanged)

  friend class TexamSummary;
  friend class TnootkaCertificate;

public:
  explicit TexamExecutor(QQuickItem* parent = nullptr);
  ~TexamExecutor() override;

  static TexamExecutor* instance() { return m_instance; }

  QString title() const;

  QList<QObject*> examActions() { return m_examActions; }

  TtipHandler* tipHandler();

  bool showPitchView() const;
  bool showRtmView() const;

      /**
       * Describes reason of starting executor
       */
  enum EexecOrigin {
    NoExam = 0, ContinueExam = 1, NewExam = 2, LevelCreator = 3, StartExercise = 4
  };
  Q_ENUM(EexecOrigin)

      /**
       * Reasons (so far three) why summary dialog was invoked
       */
  enum EsummaryReason {
    NoReason = 0, SumContExam, SumFinishExer, SumFinishExam
  };
  Q_ENUM(EsummaryReason)

  EsummaryReason summaryReason() const { return m_summaryReason; }

  Q_INVOKABLE bool init(TexamExecutor::EexecOrigin whatToDo, const QVariant& arg);

  bool isInitialized() const { return m_supp != nullptr && m_tipHandler != nullptr; }

  struct TanswerRequire {
      bool octave;
      bool accid;
      bool key;
  };

  Q_INVOKABLE bool closeNootka();

  bool isAnswered() const { return m_isAnswered; }

      /**
       * @p TRUE when exercise or @p FALSE when exam.
       */
  bool isExercise() { return static_cast<bool>(m_exercise); }

      /**
       * Should be called when settings (Tglobals) was changed during exam.
       */
  Q_INVOKABLE void settingsAccepted();

  Taction* checkQuestAct() { return m_checkQuestAct; }
  Taction* nextQuestAct() { return m_nextQuestAct; }
  Taction* newAtemptAct() { return m_newAtemptAct; }
  Taction* repeatQuestAct() { return m_repeatQuestAct; }
  Taction* correctAct() { return m_correctAct; }
  Taction* stopExamAct() { return m_stopExamAct; }
  Taction* playAgainAct() { return m_playAgainAct; }
  Taction* tuningForkAct() { return m_tuningForkAct; }
  Taction* clearScoreAct(); // redirected from main score

  static QString examHelpText();

signals:
  void titleChanged();
  void examActionsChanged();
  void tipHandlerCreated();
  void examSummary();
  void showSettings();
  void questionChanged();

protected:

      /**
       * This is "second" part of exam executor initialization @p init(),
       * when previous exam is continued and its summary is displayed, it is invoked by @p TexamSummary::continueExam()
       * in other cases it is called directly in @p init() method
       */
  bool continueInit();

  Texam* exam() { return m_exam; }
  void deleteExam();

  void askQuestionSlot() { askQuestion(false); }
  void askQuestion(bool isAttempt = false);

      /**
       * If it is called by pressing "check answer" it obviously shows results
       * but if app is closing it only checks answer and save it without displaying results.
       */
  void checkAnswer(bool showResults = true);
  void checkAnswerSlot() { checkAnswer(true); }
  void stopExamSlot();
  void stopExerciseSlot();
  void restoreExerciseAfterSummary();
  void continueExercise();
  void finishExerciseAfterSummary();
  void repeatQuestion();
  void repeatSound();
  void playMiddleA();
  void correctAnswer();
  void newAttempt();

  void showExamHelp();
  void expertAnswersSlot();

      /**
       * Invokes @p Tsound::startListen()
       */
  void startSniffing();

      /**
       * Starts sniffing when asked note is finished
       */
  void sniffAfterPlaying();
//   void rightButtonSlot();
  Q_INVOKABLE void tipLink(const QString& link);
  void markAnswer(TQAunit* curQ);

      /**
       * Stops exercising and starts exam.
       */
  void exerciseToExam();

      /**
       * Common method called by exercises and exams to disable sniffing, lock right button, etc.
       */
  void stopSound();

      /**
       * Performs routines after dialog window closed as such as 
       * right mouse button unlocking. If and exam is going to start it calls @p exerciseToExam()
       */
  void suggestDialogClosed(bool startExam);

      /**
       * Locks the executor and displays certificate.
       */
  void displayCertificate();

      /**
       * It sets m_snifferLocked to false (unlocks) and restores capturing right mouse button (installEventFilter)
       */
  void unlockAnswerCapturing();

      /**
       * Routines for questions with the same answers 'blind'
       */
  void blindQuestion();

      /**
       * When user plays a melody as an answer and start of a note was detected.
       */
  void noteOfMelodyStarted(const TnoteStruct& n);

      /**
       * Played note was finished
       */
  void noteOfMelodyFinished(const TnoteStruct& n);

      /**
       * Note of score to play was clicked
       */
  void noteOfMelodySelected(int nr);

      /**
       * Should be called when settings dialog is going to be displayed.
       */
  void prepareToSettings();
  void correctNoteOfMelody(int noteNr);

protected slots:
  void correctionFinishedSlot();


private:
  void createActions();
  void prepareToExam();
  void restoreAfterExam();

      /**
       * Disables controls of score, note name and instrument
       */
  void disableWidgets();
  void clearWidgets();

      /**
       * closing routines and invokes @p restoreAfterExam()
       */
  void closeExecutor();

      /**
       * Performs some initial routines on exam/exercise variables
       */
  void initializeExecuting();

  QString saveExamToFile();

      /**
       * Checks @p m_soundTimer and connects @p playingFinished() of @p Tsound
       */
  void connectPlayingFinished();

      /**
       * Casts @p QVariant @p v to @p Tlevel* and if casted, assigns it to @p m_level and returns @p TRUE
       * otherwise return @p FALSE
       */
  bool castLevelFromQVariant(const QVariant& v);

private:

  static TexamExecutor         *m_instance;
  TexecutorSupply              *m_supp = nullptr;
  Texam                        *m_exam;
  Tlevel                        m_level; /**< main instance of Tlevel, others are pointers or references to it */
  QList<TQAgroup>               m_questList;

        /**
         * Invokes startSniffing() and stopPlaying() after delay
         * to avoid feedback between played question and listened answer.
         */
  QTimer                      *m_soundTimer, *m_askingTimer;
  Tnote::EnameStyle            m_prevQuestStyle, m_prevAnswStyle;
  TglobalExamStore            *m_glStore = nullptr;
  TanswerRequire               m_answRequire;

      /**
       * Indicates when sniffing has to be ignored, 
       * because some dialog window exist over exam.
       */
  bool                         m_snifferLocked;
  bool                         m_shouldBeTerminated, m_isAnswered, m_incorrectRepeated;

        /**
         * If it is sets to @p TRUE locks invoking event of right mouse button.
         * It has to be set before @p singleShot() method called on askQuestion() 
         * to avoid user click button and call @p askQuestion() again during time of delay.
         */
  bool                         m_lockRightButt;
  bool                         m_goingClosed; /**< It becomes true when user wants close Nootka during an exam or exercise.*/

        /** stores note if question and answer are Note Name to restore it if question is repeated
        It is to restore buttons state in NoteName widget witch are unchecked by disableWidget() */
  Tnote                        m_prevNoteIfName;
  TtipHandler                 *m_tipHandler = nullptr;
  Tpenalty                    *m_penalty;
  Texercises                  *m_exercise;
  int                          m_blindCounter; /**< counts occurrences of questions without possible answer */
  TequalRand                  *m_rand;

  QPointer<TexamMelody>        m_melody; /**< Helper class with exam/exercises with melodies */
  Taction                     *m_settAct, *m_helpAct, *m_stopExamAct, *m_nextQuestAct, *m_repeatQuestAct, *m_checkQuestAct;
  Taction                     *m_playAgainAct = nullptr;
  Taction                     *m_correctAct = nullptr;
  Taction                     *m_newAtemptAct = nullptr;
  Taction                     *m_tuningForkAct = nullptr;
  QList<QObject*>              m_examActions;
  EsummaryReason               m_summaryReason = NoReason;
  Tnote::EnameStyle            m_exerciseTmpStyle;
  bool                         m_askAfterSummary = false;
  bool                         m_disconnectAfterAnim = false;
  int                          m_melodySelectionIndex = 0;

};

#endif // TEXAMEXECUTOR_H
