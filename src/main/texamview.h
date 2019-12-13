/***************************************************************************
 *   Copyright (C) 2011-2019 by Tomasz Bojczuk                             *
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


#ifndef TEXAMVIEW_H
#define TEXAMVIEW_H


#include <QtQuick/qquickitem.h>
#include <QtCore/qelapsedtimer.h>
#include <QtCore/qtimer.h>


class Texam;
class TQAunit;

#define    RESULTS    TexamView::instance()

#define    REFRESH_TIME (250)


/**
 * A @class TexamView represents status of exam.
 * It displays times and numbers of valid/invalid questions.
 * @p startExam(Texam) Initializes it.
 * @p questionStart() starts timer for a new question
 * @p questionStop() stops timer and writes elapsed time to @p Texam
 * @p answered() summarizes and displays counters numbers on labels
 *
 * It has single instance available through @p instance()
 * defined also as a macro @p RESULTS
 */
class TexamView : public QQuickItem
{

  Q_OBJECT

  Q_PROPERTY(int correctAnswers READ correctAnswers NOTIFY answersNumberChanged)
  Q_PROPERTY(int halfAnswers READ halfAnswers NOTIFY answersNumberChanged)
  Q_PROPERTY(int wrongAnswers READ wrongAnswers NOTIFY answersNumberChanged)
  Q_PROPERTY(QString effectiveness READ effectiveness NOTIFY effectivenessChanged)
  Q_PROPERTY(QString totalTimeText READ totalTimeText NOTIFY totalTimeTextChanged)
  Q_PROPERTY(QString averText READ averText NOTIFY averTextChanged)
  Q_PROPERTY(QString reactText READ reactText NOTIFY reactTextChanged)

  Q_PROPERTY(QString answersText READ answersText NOTIFY valuesUpdated)
  Q_PROPERTY(QString totalText READ totalText NOTIFY valuesUpdated)
  Q_PROPERTY(int progressValue READ progressValue NOTIFY valuesUpdated)
  Q_PROPERTY(int progressMax READ progressMax NOTIFY valuesUpdated)

  Q_PROPERTY(bool isExercise READ isExercise NOTIFY isExerciseChanged)

public:
  explicit TexamView(QQuickItem *parent = nullptr);
  ~TexamView() override;

  int correctAnswers() const;
  int halfAnswers() const;
  int wrongAnswers() const;

  QString effectiveness() const;
  QString totalTimeText() const;
  QString averText() const;
  QString reactText() const;

  QString answersText() const { return m_answersText; }
  QString totalText() const { return m_totalText; }
  int progressValue() const { return m_progressValue; }
  int progressMax() const { return m_progressMax; }

  bool isExercise() const;

  static TexamView* instance() { return m_instance; }

    /** Returns time given in &milliseconds in format h:mm:ss */
  static QString formatedTotalTime(int t) { return QString("%1:%2:%3")
          .arg(t/3600000)
          .arg((t%3600000)/60000, 2, 'f', 0, '0')
          .arg((t%60000)/1000, 2, 'f', 0, '0'); }

  void startExam(Texam *exam, int totalNr); /**< Initialization */
  void questionStart(); /**< Starts counting time for a new question */
  void questionStop(); /**< Stops question timer and updates time in the last exam TQAunit. */
  void answered(); /**< Displays updated counters after answer */

  quint16 questionTime(); /**< Elapsed time for of current question [seconds * 10] */

  void pause(); /**< Stops counting time of answer */
  void go(); /**< Continues counting time of answer */

  void stopExam() { m_timer->stop(); }

  void doNotdisplayTime() { m_timer->stop(); } /**< Stops displaying pending time */
  void displayTime() {   m_timer->start(REFRESH_TIME); } /**< Starts Refreshing elapsing time every 100 ms  */

      /** Total time of the exam. */
  quint32 totalTime() {return quint32(m_startExamTime) + quint32(m_totalTime.elapsed() / 1000); }
  void updateExam(); /**< Updates elapsed time and average time in current @p Texam */

  void reactTimesUpdate(); /**< Updates reaction time and average reaction time of to the last answer time. */
  void effectUpdate(); /**< Updates effectiveness value of an exam. */
  void questionCountUpdate(); /**< Updates mistakes, correct, and 'not bad' numbers to current exam values */

  static QString progressExamTxt() { return tr("Progress of the exam"); } // Progress of the exam
  static QString examFinishedTxt() { return tr("Exam was finished"); } // Exam was finished

  void progress() { updateLabels(); }
  void terminate();

  Q_INVOKABLE QString answersHint() const;
  Q_INVOKABLE QString summaryHint() const;
  Q_INVOKABLE QString progressHint() const;
  Q_INVOKABLE QString correctHint() const;
  Q_INVOKABLE QString halfHint() const;
  Q_INVOKABLE QString wrongHint() const;
  Q_INVOKABLE QString effectHint() const;
  Q_INVOKABLE QString averageHint() const;
  Q_INVOKABLE QString answerTimeTxt() const;
  Q_INVOKABLE QString examTimeTxt() const;
  Q_INVOKABLE QString resultsTxt() const;

signals:
  void answersNumberChanged();
  void effectivenessChanged();
  void totalTimeTextChanged();
  void reactTextChanged();
  void averTextChanged();
  void valuesUpdated();
  void isExerciseChanged();

protected:
  void countTime();
  void updateLabels();

private:
  bool                 m_showReact; /**< switches whether displays pending question time counter */
  QString              m_effectivenessText;
  QElapsedTimer        m_questionTime; /**< Elapsing time of a question - started with questionStart() and stopped with questionStop() */
  int                  m_startExamTime; /**< Elapsed time from previous exam sessions */
  QTimer              *m_timer;
  QElapsedTimer        m_totalTime; /**< Total time of an exam */
  int                  m_answerDuration = 0; /**< Duration time of current answer, it can be paused during exercise, so it is combined after answer resume. */
  Texam               *m_exam;
  static TexamView    *m_instance;

  int                  m_totalNr;
  QString              m_answersText, m_totalText;
  int                  m_progressValue, m_progressMax;

};

#endif // TEXAMVIEW_H
