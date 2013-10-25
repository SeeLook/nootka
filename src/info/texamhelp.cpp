/***************************************************************************
 *   Copyright (C) 2011-2013 by Tomasz Bojczuk                             *
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
#include "tpixmaker.h"
#include "texamsettings.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QCheckBox>
#include <QDebug>


#define PIXICONSIZE (32)
#define BIGPIXSIZE (64)

TexamHelp::TexamHelp(QString questColorTxt, QString answColorTxt, bool* showHelp, QWidget* parent) :
    ThelpDialogBase(parent, 0)
{
  resize((parent->width() / 3) * 2, (parent->height() / 3) * 2);
//   setWindowTitle(tr("Exam help"));
  helpText()->setHtml(
    QString("<center><h2>%1 &nbsp;&nbsp;").arg(pix("help", BIGPIXSIZE)) +
    tr("How does an exercise or an exam work?") +
    QString("&nbsp;&nbsp; %1").arg(pix("practice", BIGPIXSIZE) + " " + pix("nootka-exam", BIGPIXSIZE)) + "</h2>" +
//     "<style type=\"text/css\">img { border-style: solid; border-color: palette(text); border-width: 2px; background-color: palette(window); }</style>" +
		tr(" Briefly: Nootka give you a question and you give an answer...") + "<br><br><br><br>" +
    toGetQuestTxt() + ":<br>- " + clickSomeButtonTxt(pix("nextQuest", PIXICONSIZE)) + "<br>- " +
    pressSpaceKey() + "<br>- " + orRightButtTxt() + "<br>" + 
    tr("Select 2nd check box to get the question automatically.") + 
		
    QString("<br><br><span style=\"%1\">").arg(questColorTxt) +
    tr("Questions are marked with this color and \"?\" mark.") + "</span><br>" + 
    tr("To give an answer, select it on <span style=\"%1\">Nootka's element with that color.</span><br>")
      .arg(answColorTxt) +
    QString("<br><br>%1<br><br>").arg(pix("scr", 0)) +
    tr("To check the answer confirm it:") + "<br>- " + 
    clickSomeButtonTxt(pix("check", PIXICONSIZE)) + "<br>- " +
    pressEnterKey() + "<br>- " + orRightButtTxt() + "<br>" +
		tr("By selecting the 3rd check box, answers will be checked immediately without confirmation required.") + "<br><br>" +
		    
		"<hr><table><tr><td valign=\"middle\" align=\"center\">" + pix("practice", BIGPIXSIZE) + "<br>" + tr("Exercises") +
		"</td><td align=\"center\">" +
		tr("If you made a mistake during an exercise, Nootka can show you correct answer. To see it:") + 
		"<br>- " + clickSomeButtonTxt(pix("correct", PIXICONSIZE)) + "<br>- " + orPressEnterKey() + "<br>" +
		tr("Select 1st check box to see corrected answer for every mistake.") + "<br><br>" +
		tr("You can every time click button %1 to pause or stop exercising and to see your results.<br>Only recent exercise is stored for further continuation.<br>When your exercising will go well Nootka will suggest you to start an exam.").arg(pix("stopExam", PIXICONSIZE)) +
		"</td></tr></table><br><br>" +
		
		"<hr><table><tr><td  valign=\"middle\" align=\"center\">" + pix("nootka-exam", BIGPIXSIZE) + "<br>" + tr("Exams") +
		"</td><td align=\"center\">" +
		tr("If you made a mistake during an exam and you want to repeat the question:") + "<br>- " + 
    clickSomeButtonTxt(pix("prevQuest", PIXICONSIZE)) + "<br>- " + orPressBackSpace() + "<br><br>" +
		toStopExamTxt(pix("stopExam", PIXICONSIZE)) + "<br>" + 
		tr("Then you will be able to analyze its results and save exam to file for further continue.") + "<br>" +
    tr("Exam will be completed when you give answers to all questions.<br>The number of questions depends on the level of the exam.") + "<br>" +
    tr("If you make a mistake, the question will asked two more times and the total number of questions will be increased. These are penalty questions.") + "<br>" +
    tr("When answer is 'not bad', you get one penalty more.") +
		"</td></tr></table></center><center>" +
    "<hr><br><br><span style=\"font-size: 20px;\"><b>" +
    tr("GOOD LUCK!") + "</b></span>" + "<br><hr><table><tr><th colspan=2>" +
    tr("Experts corner") + "</th></tr><tr><td rowspan=3>" +
//     QString("<img style=\"background-color: transparent;\" src=\"%1\">").arg(pix("expertCorner"))+
		pix("expertCorner", 0) +
    "</td><td><br>1. " + TexamSettings::correctMistakesTxt() + "</td></tr><tr><td><br>2. " + 
    TexamSettings::autoNextQuestTxt() + "</td></tr><tr><td><br>3. " + 
    TexamSettings::expertsAnswerTxt() + "</td></tr></table></center>");
  
  helpText()->resize((parent->width() / 3) * 2, (parent->height() / 5) * 3);
  qDebug() << helpText()->toHtml();
	showCheckBox(showHelp);
}






