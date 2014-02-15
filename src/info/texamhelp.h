/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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

#ifndef TEXAMHELP_H
#define TEXAMHELP_H


#include "thelpdialogbase.h"

class QCheckBox;
class QLabel;

/** 
 * It shows simple exam help. The parameters are QStrings with colors 
 * in HTML format: 
 * background-color: rgba() 
 * @param path is global path to images
 */
class TexamHelp : public ThelpDialogBase
{
  Q_OBJECT
  
public:
  TexamHelp(QString questColorTxt, QString answColorTxt, bool* showHelp, QWidget* parent = 0);
  
  static QString orRightButtTxt() { return tr("or right mouse button"); } // or right mouse button
  static QString clickSomeButtonTxt(QString imgHtmlTag) 
      { return tr("click %1 button").arg(imgHtmlTag); } // click <button-icon> button
  static QString toGetQuestTxt() { return tr("To get question"); } // To get question
  static QString pressSpaceKey() { return tr("press <b>space</b> key"); } // press <b>space</b> key
  static QString pressEnterKey() { return tr("press <b>Enter</b> key"); } // press <b>Enter</b> key
  static QString orPressEnterKey() { return tr("or press <b>Enter</b> key"); } // or  press <b>Enter</b> key
  static QString orPressBackSpace() { return tr("or <b>Backspace</b> key"); } // or <b>Backspace</b> key
  static QString toStopExamTxt(QString imgHtmlTag)
      { return tr("Click %1 button to stop.").arg(imgHtmlTag); } /** Click %1 button to stop. */
      
			/** Describing exercise mode:
			 * Only recent exercise is stored for further continuation.
			 * When your exercising will go well Nootka will suggest you to start an exam. */
  static QString exerciseFeaturesText();
  
			/** Describing exam mode */
  static QString examFeaturesText();
      
  
  
};

#endif // TEXAMHELP_H
