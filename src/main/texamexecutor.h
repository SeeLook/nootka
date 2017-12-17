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
class Tcanvas;
class QTimer;
class TexecutorSupply;
class Texam;
class TnameItem;
class Taction;
class TtipData;


#define EXECUTOR TexamExecutor::instance()

/**
 * This class manages exam executing and practicing.
 */
class TexamExecutor : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(QString title READ title NOTIFY titleChanged)
  Q_PROPERTY(TnameItem* nameItem READ nameItem WRITE setNameItem)
  Q_PROPERTY(QVariantList examActions READ examActions NOTIFY examActionsChanged)
  Q_PROPERTY(Tcanvas* tipHandler READ tipHandler NOTIFY tipCreated)

public:
  explicit TexamExecutor(QQuickItem* parent = nullptr);
  ~TexamExecutor() override;

  static TexamExecutor* instance() { return m_instance; }

  QString title() const;

  TnameItem* nameItem() { return m_nameItem; }
  void setNameItem(TnameItem* ni);

  QVariantList examActions() { return m_examActions; }

  Tcanvas* tipHandler();

      /**
       * Describes actions committed by user.
       */
  enum Eactions {
    NoExam = 0, ContinueLastExam, ContinueOtherExam, NewExam, LevelCreator, ContinueExercise, NewExercise
  };
  Q_ENUM(Eactions)

  Q_INVOKABLE bool init(Eactions whatToDo, const QVariant& arg);

  struct TanswerRequire {
      bool octave;
      bool accid;
      bool key;
  };

//   bool closeNootka();
  bool isAnswered() const { return m_isAnswered; }

      /**
       * @p TRUE when exercise or @p FALSE when exam.
       */
  bool isExercise() { return static_cast<bool>(m_exercise); }

signals:
  void titleChanged();
  void examActionsChanged();
  void tipCreated();
  void destroyTips();

protected:
  void deleteExam();

  void askQuestionSlot() { askQuestion(false); }
  void askQuestion(bool isAttempt = false);

      /**
       * If it is called by pressing "check answer" it obviously shows results
       * but if app is closing it only checks answer and save it without displaying results.
       */
  void checkAnswer(bool showResults = true);
  void checkAnswerSlot() { checkAnswer(true); }
//   void stopExamSlot();
//   void stopExerciseSlot();
//   void repeatQuestion();
//   void repeatSound();
//   void playMiddleA();
//   void correctAnswer();
//   void newAttempt();
// 
  void showExamHelp();
  void expertAnswersSlot();
  void startSniffing();     /**< Invokes Tsound::go() */
//   void sniffAfterPlaying(); /**< Starts sniffing when asked note is finished */
//   void rightButtonSlot();
  Q_INVOKABLE void tipLink(const QString& link);
//   void markAnswer(TQAunit* curQ);
//   void delayerTip(); /**< This is QTimer slot invoking m_canvas->whatNextTip(true) method. */
//   void exerciseToExam(); /**< Stops exercising and starts exam. */
  void stopSound(); /**< Common method called by exercises and exams to disable sniffing, lock right button, etc. */

      /**
       * Performs routines after dialog window closed as such as 
       * right mouse button unlocking. If and exam is going to start it calls @p exerciseToExam()
       */
  void suggestDialogClosed(bool startExam);

//   void displayCertificate(); /**< Locks the executor and displays certificate. */
// 
//       /** It sets m_snifferLocked to false (unlocks) and restores capturing right mouse button (installEventFilter) */
//   void unlockAnswerCapturing();
//   void blindQuestion(); /**< Routines for questions with the same answers 'blind' */
// 
//   void noteOfMelodyStarted(const TnoteStruct& n); /**< When user plays a melody as an answer and start of a note was detected. */
//   void noteOfMelodyFinished(const TnoteStruct& n); /**< Played note was finished */
//   void noteOfMelodySelected(int nr); /**< Note of score to play was clicked */
// 
//   void prepareToSettings(); /**< Should be called when main window is going to display settings dialog. */
//   void settingsAccepted(); /**< Should be called when settings (Tglobals) was changed during exam. */
//   void correctNoteOfMelody(int noteNr);
// 
//   void correctionFinished();
// 
private:
      /**
       * Casts @p QVariant @p v to @p Tlevel* and if casted, assigns it to @p m_level and returns @p TRUE
       * otherwise return @p FALSE
       */
  bool castLevelFromQVariant(const QVariant& v);
  void createActions();
  void prepareToExam();
//   void restoreAfterExam();
  void disableWidgets(); /**< Disables score, noteName and guitar*/
  void clearWidgets();
//   void closeExecutor(); /**< Clears canvas and invokes restoreAfterExam() */
  void initializeExecuting(); /**< Performs some initial routines on exam/exercise variables */
// 
//   QString saveExamToFile();

//   void connectPlayingFinished(); /**< Checks @p m_soundTimer and connects @p playingFinished() of @p Tsound */

private:

  static TexamExecutor         *m_instance;
  TexecutorSupply              *m_supp;
  Texam                        *m_exam;
  Tlevel                        m_level; /**< main instance of Tlevel, others are pointers or references to it */
  QList<TQAgroup>               m_questList;

        /**
         * Invokes startSniffing() and stopPlaying() after delay
         * to avoid feedback between played question and listened answer.
         */
  QTimer                      *m_soundTimer, *m_askingTimer;
  Tnote::EnameStyle            m_prevQuestStyle, m_prevAnswStyle;
  TglobalExamStore            *m_glStore;
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
  Tcanvas                     *m_canvas = nullptr;
  Tpenalty                    *m_penalty;
  Texercises                  *m_exercise;
  int                          m_blindCounter; /**< counts occurrences of questions without possible answer */
  TequalRand                  *m_rand;

  QPointer<TexamMelody>        m_melody; /**< Helper class with exam/exercises with melodies */
  TnameItem                   *m_nameItem = nullptr;
  Taction                     *m_helpAct, *m_stopExamAct, *m_nextQuestAct, *m_repeatQuestAct, *m_checkQuestAct;
  Taction                     *m_playAgainAct = nullptr;
  Taction                     *m_correctAct = nullptr;
  Taction                     *m_newAtemptAct = nullptr;
  QVariantList                 m_examActions;

};

#endif // TEXAMEXECUTOR_H
