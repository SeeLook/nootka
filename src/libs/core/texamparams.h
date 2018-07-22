/***************************************************************************
 *   Copyright (C) 2011-2018 by Tomasz Bojczuk                             *
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


#ifndef TEXAMPARAMS_H
#define TEXAMPARAMS_H

#include <nootkacoreglobal.h>
#include <QString>
#include <QColor>


#define RECENT_EXAMS_LIMIT (15) // max number of recent exams to remember

  /** class describes en exam parameters changeable by user. */
class NOOTKACORE_EXPORT TexamParams
{
// They are very global and Tglobals has them directly
//   QColor questionColor;
//   QColor answerColor;

public:

      /** Actions after user mistake when next question is asked automatically. */
  enum EafterMistake {
    e_continue = 0, /** next question is asked immediately */
    e_wait = 1, /** next question is asked after delay defined in @p previewDuration */
    e_stop = 2 /** questioning is stopped, to get next one user has to click */
  };

  bool autoNextQuest; /**< Next question in en exam is given automatically after correct answer @def false */
  bool repeatIncorrect; /**< If EautoNextQuest is true incorrect questions are asked again once @def true */
  EafterMistake afterMistake; /**< Actions after user mistake when next question is set to ask automatically. @def e_continue */
  bool expertsAnswerEnable; /**< Enable checking without confirm @def false */
  QString studentName; /**< @def empty */
  QString examsDir; /**< Path to dir with recent opened exam file. @def system_home_path */
  QString levelsDir; /**< Path to dir with recent opened level file. @def system_home_path */
  bool closeWithoutConfirm; /**< Do not ask anything when Nootka is closing. @def false */
  bool showNameOfAnswered; /**< Displays note name of answered (if possible). @def false */

// PRACTICE/EXERCISING
  bool showCorrected; /**< When answer was wrong, corrected one will be shown */
  bool suggestExam; /**< Check is exercising going well and suggest to start an exam on that level. @def true */
  bool showWrongPlayed; /**< Displays what pitch Nootka detected when played answer was wrong. @def false  */
  bool waitForCorrect; /**< When exercise melody is played it locks current note index until correct note is played. @def true  */

  int mistakePreview; /**< How long mistakes are exposed to ask auto next question. @def 3000ms */
  int questionDelay; /**< Wait time before every next question. */
  int correctPreview; /**< Preview of answer correction.  */

// Displaying dialogues check box state
  bool askAboutExpert; /**< shows confirm dialog when expertsAnswerEnable is going to be changed @def true */
  bool showHelpOnStart; /**< shows dialog with help on start en exam or exercise @def true */
  bool showVeryBeginHelp; /**< When start exam or exercise dialog appears @def true */
};


#endif // TEXAMPARAMS_H
