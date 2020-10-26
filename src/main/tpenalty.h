/***************************************************************************
 *   Copyright (C) 2014-2020 by Tomasz Bojczuk                             *
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

#ifndef TPENALTY_H
#define TPENALTY_H


#include <QtCore/qobject.h>


class TexecutorSupply;
class Texam;


/**
 * This class manages penalties during exam execution.
 * Also it takes care about state of @class TexamView
 * @p m_penalStep determines how many 'normal' questions is asked between penalties
 * When it is 0 - penalty is asked every time.
 * @p m_penalCount counts questions and is increased by @p nextQuestion()
 * @p m_blackQuestNr keeps number of question in a black list or it is -1 when 'normal' question.
 * @p m_blackNumber the same as above but for melodies questions
 * 
 * @p nextQuestion() is invoked fromTexam::askQuestion() to increase counter
 * then @p ask() prepares the penalty if its time was come.
 * @p checkAnswer() manages @p m_exam state (summarize the answer) and updates counters of the widgets 
 * and adds penalties if necessary. 
 * Due to melody questions can be continued (new attempts), this method can't determine penalties in this moment.
 * @p newAttempt() prepares to continuation of answering.
 * @p setMelodyPenalties() has to be called when user decided that question is answered/finished.
 * This method is invoked from @class TexamExacutor when new question is created (@p askQUestion() there)
 * @p setBlackQuestion() is called from Texam::repeatQuestion() 
 * to set @p m_blackQuestNr to the last question in the black list and when answer on it is correct
 * this repeated question is removed from the list.
 * @p checkAnswer() and @p setMelodyPenalties() perform certificate check 
 * and when appropriate question number was done @p certificate() signal is emitted.
 */
class Tpenalty : public QObject
{

  Q_OBJECT

public:
  Tpenalty(Texam* exam, TexecutorSupply* supply);

      /**
       * @p TRUE when a question is not a penalty
       */
  bool isNot() { return m_blackQuestNr == -1 && m_blackNumber == -1; }

      /**
       * Check state of counters and if it is time for penalty
       * prepares last question in @p Texam list by copying someone from black list
       * and returns @p TRUE. Otherwise just returns @p FALSE
       */
  bool ask();

  void updatePenalStep();

      /**
       * Increases counter of question, resets @p m_blackQuestNr. Starts timer
       */
  void nextQuestion();

      /**
       * Checks last answer and when not valid adds penalty(s) to black list.
       */
  void checkAnswer();

      /**
       * Prepares @p Texam to continue answering and updates counters.
       */
  void newAttempt();

  void setMelodyPenalties();

      /**
       * If asked question was penalty and answer was correct it removes penalty from black list.
       */
  void releaseBlackList();

      /**
       * Checks could be exam finished and emits @p certificate() when it can.
       */
  void checkForCert();

      /**
       * Sets @p m_blackQuestNr to the last question from the black list
       * because previous answer was wrong or not so bad and it was added at the end of blacList
       * When an answer will be correct the list will be decreased.
       */
  void setBlackQuestion();

      /**
       * Pauses exam view timers.
       * Forwarded functionality of @p TexamView and @p TprogressWidget classes
       */
  void pauseTime();

      /**
       * Continues exam view timers.
       * Forwarded functionality of @p TexamView and @p TprogressWidget classes
       */
  void continueTime();

      /**
       * Updates exam variables with already elapsed times.
       * Forwarded functionality of @p TexamView and @p TprogressWidget classes
       */
  void updateExamTimes();

      /**
       * Stops refreshing elapsing time on the exam view labels.
       * Forwarded functionality of @p TexamView and @p TprogressWidget classes
       */
  void stopTimeView();

      /**
       * Initializes counting time for a new question.
       * Forwarded functionality of @p TexamView and @p TprogressWidget classes
       */
  void startQuestionTime();

      /**
       * Stops counting time of the current question and updates counters and labels.
       * Forwarded functionality of @p TexamView and @p TprogressWidget classes
       */
  void stopQuestionTime();

      /**
       * Elapsed time of current question.
       * Forwarded functionality of @p TexamView and @p TprogressWidget classes
       */
  quint32 elapsedTime();

signals:
      /**
       * Emitted when last mandatory question was correct, so certificate can be displayed.
       */
  void certificate();

private:
  Texam                    *m_exam;
  TexecutorSupply          *m_supply;
  int                       m_blackQuestNr; /**< -1 if no black, otherwise points question in blackList list. */
  int                       m_blackNumber; /**< Points number is black numbers list or -1 for none */
  int                       m_penalStep; /**< Interval of questions, after it penalty question is asked */
  int                       m_penalCount; /**< Counts questions to ask penalties one. */
};

#endif // TPENALTY_H
