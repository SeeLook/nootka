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
  QColor questionColor;
  QColor answerColor;
  bool autoNextQuest; // Next quetion in en exam is given automatically after correct answer
  bool repeatIncorrect; // If EautoNextQuest is true incorrect questions are asked again once
//   bool repeatUntilcorrect; /** or until correct answer will be given. */
  bool expertsAnswerEnable;
  bool askAboutExpert; // shows confirm dialog when expertsAnswerEnable is going to be changed
  bool showHelpOnStart; // shows dialog with help on start en exam
  QString studentName;

};


#endif // TEXAMPARAMS_H