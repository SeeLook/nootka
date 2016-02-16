/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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

#include "texpertanswerhelp.h"


TexpertAnswerHelp::TexpertAnswerHelp(QWidget* parent, bool* askAboutExpert, bool showAskCheckBox) :
    ThelpDialogBase(parent, 0)
{
#if !defined (Q_OS_ANDROID)
  setFixedSize(600, 380);
  setWindowTitle(tr("Experts mode"));
  helpText()->document()->setTextWidth(fontMetrics().boundingRect("w").width() * 80);
#endif
  
  helpText()->setHtml("<center><big>" + tr("You are about to enter expert mode.<br> In this mode you don't need to confirm every answer,<br><b>but remember the following:") + "</b></big><ul><li>" + 
    tr("Selecting a note on the score or a position on the fingerboard invokes automatic checking of your answer, so select a key signature first, if required.") + "<br></li><li>" +
    tr("When an answer is the name of a note <b>first select a proper accidental and an octave</b> and then click a note button - this automatically invokes checking.") + "<br></li><li>" +
    tr("When you have to play a note as an answer - the first detected sound will be taken, so be sure that your input device captures exactly what you want.") + "<br></li><li>" +
    tr("When the last note of question that is a melody was played - checking starts.") + "<br></li></ul><br></center>");
  
		if (showAskCheckBox)
				showCheckBox(tr("Always remind me about this"), askAboutExpert);
		
		showButtons(true, true);
}


bool showExpertAnswersHelpDlg(QWidget* parent, bool* askAboutExpert, bool showAskCheckBox) {
  TexpertAnswerHelp *exHlp = new TexpertAnswerHelp(parent, askAboutExpert, showAskCheckBox);
  bool res = false;
  exHlp->exec();
  if (exHlp->result() == QDialog::Accepted)
      res = true;    
  delete exHlp;
  return res;
}
