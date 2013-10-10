/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
 *   tomaszbojczuk@gmail.com                                               *
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

#include <QString>
#include <QColor>


  /** class describes en exam parameters changeable by user. */
class TexamParams 
{
public:
// They are very global and Tglobals has them directly
//   QColor questionColor;
//   QColor answerColor;


  bool autoNextQuest; /** Next quetion in en exam is given automatically after correct answer @def false */
  bool repeatIncorrect; /** If EautoNextQuest is true incorrect questions are asked again once @def true */
  bool expertsAnswerEnable; /** Enable checking without confirm @def false */
  bool askAboutExpert; /** shows confirm dialog when expertsAnswerEnable is going to be changed @def true */
  bool showHelpOnStart; /** shows dialog with help on start en exam @def true */
  QString studentName; /** @def empty */
  QString examsDir; /** Path to dir with recent opened exam file. @def system_home_path */
  QString levelsDir; /** Path to dir with recent opened level file. @def system_home_path */
  bool closeWithoutConfirm; /** Do not ask anything when Nootka is closing. */
  
// PRACTICE/EXERCISING
  bool showCorrected; /** When answer was wrong, corrected one will be shown */
  //   bool repeatUntilcorrect; /** or until correct answer will be given. */
};


#endif // TEXAMPARAMS_H