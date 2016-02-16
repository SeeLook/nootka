/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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

#ifndef TEXPERTANSWERHELP_H
#define TEXPERTANSWERHELP_H


#include "nootkamiscglobal.h"
#include "thelpdialogbase.h"


/** 
 * This function creates showExpertAnswersHelpDlg, 
 * and return user decision.
 * @p askAboutExpert is pointer to global parameter 
 * and @p showAskCheckBox determines does show QCheckBox expressing askAboutExpert state. 
 */
bool NOOTKAMISC_EXPORT showExpertAnswersHelpDlg(QWidget* parent, bool *askAboutExpert, bool showAskCheckBox);

/** It shows simple exam help. The parameters are QStrings with colors */
class TexpertAnswerHelp : public ThelpDialogBase
{
  Q_OBJECT
  
public:
  explicit TexpertAnswerHelp(QWidget* parent, bool* askAboutExpert, bool showAskCheckBox);
  
  
};

#endif // TEXPERTANSWERHELP_H
