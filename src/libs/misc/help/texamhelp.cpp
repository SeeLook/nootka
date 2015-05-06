/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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

#include "texamhelp.h"



QString TexamHelp::exerciseFeaturesText() {
	return tr("When your exercising will go well Nootka will suggest you to start an exam.");
}


QString TexamHelp::examFeaturesText() {
	return tr("Exams can be saved to files and continued later. Moreover, exam files can be opened to analyze.") + "<br>" +
    tr("Exam will be completed when you give answers to all questions.<br>The number of questions depends on the level of the exam.") + "<br>" +
    tr("If you make a mistake, the question will asked two more times and the total number of questions will be increased. These are penalty questions.") + "<br>" +
    tr("When answer is 'not bad', you get one penalty more.");
}


TexamHelp::TexamHelp(QString questColorTxt, QString answColorTxt, bool* showHelp, QWidget* parent) :
    ThelpDialogBase(parent, 0)
{
  resize((parent->width() / 3) * 2, (parent->height() / 3) * 2);
  const int iconsSize = fontMetrics().boundingRect("A").height() * 2;
  const int bigIconSize = iconsSize * 2;
  helpText()->setHtml(
    QString("<center><h2>%1").arg(pix("help", bigIconSize)) + "<br>" +
    tr("How does an exercise or an exam work?") + "</h2><br>" +
		pix("practice", bigIconSize) + "&nbsp;&nbsp;&nbsp;&nbsp;" + pix("exam", bigIconSize) + "<br>" +
		tr(" Briefly: Nootka give you a question and you give an answer...") + "<br><br><br><br>" +
    toGetQuestTxt() + ":<br>- " + clickSomeButtonTxt(pix("nextQuest", iconsSize)) + "<br>- " +
    pressSpaceKey() + "<br>- " + orRightButtTxt() + "<br>" +
		
    QString("<br><br><span style=\"%1\">").arg(questColorTxt) +
    tr("Questions are marked with this color and \"?\" mark.") + "</span><br>" + 
    tr("To give an answer, select it on <span style=\"%1\">Nootka's element with that color.</span><br>")
      .arg(answColorTxt) +
    QString("<br><br>%1<br><br>").arg(pix("scr", parent->width() * 0.6)) +
    tr("To check the answer confirm it:") + "<br>- " + 
    clickSomeButtonTxt(pix("check", iconsSize)) + "<br>- " +
    pressEnterKey() + "<br>- " + orRightButtTxt() + "<br><br>" +
		    
		"<hr><table><tr><td valign=\"middle\" align=\"center\">" + pix("practice", bigIconSize) + "<br>" + tr("Exercises") +
		"</td><td align=\"center\">" +
		tr("If you made a mistake during an exercise, Nootka can show you correct answer. To see it:") + 
		"<br>- " + clickSomeButtonTxt(pix("correct", iconsSize)) + "<br>- " + orPressEnterKey() + "<br><br>" +
		tr("You can every time click button %1 to pause or stop exercising and to see your results.").arg(pix("stopExam", iconsSize)) +
		"<br>" + exerciseFeaturesText() +	"</td></tr></table><br><br>" +
		
		"<hr><table><tr><td  valign=\"middle\" align=\"center\">" + pix("exam", bigIconSize) + "<br>" + tr("Exams") +
		"</td><td align=\"center\">" +
		tr("If you made a mistake during an exam and you want to repeat the question:") + "<br>- " + 
    clickSomeButtonTxt(pix("prevQuest", iconsSize)) + "<br>- " + orPressBackSpace() + "<br><br>" +
		toStopExamTxt(pix("stopExam", iconsSize)) + "<br>" + 
		examFeaturesText() + "</td></tr></table></center><center>" +
		
		"<hr><table><tr><td valign=\"middle\" align=\"center\">" + pix("exam-settings", bigIconSize) + "<br>" + tr("Settings") +
		"</td><td align=\"center\"><br><br>" +
		tr("Just click this settings button to adjust an exercise or an exam to your current needs.") +	"</td></tr></table><br><br>" +
		
    "<hr><br><br><span style=\"font-size: xx-large;\"><b>" +
    tr("GOOD LUCK!") + "</b></span>"
		"<br></center>");
  
  helpText()->resize((parent->width() / 3) * 2, (parent->height() / 5) * 3);
	showCheckBox(showHelp);
}






