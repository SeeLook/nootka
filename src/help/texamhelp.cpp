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

#include "texamhelp.h"
#include <tglobals.h>


#define SCR_FACTOR (0.9)


QString br = QLatin1String("<br>");
QString br_ = QLatin1String("<br>- ");

QString TexamHelp::exerciseFeaturesText() {
 return tr("When your exercising will go well Nootka will suggest you to start an exam.");
}


QString TexamHelp::examFeaturesText() {
 return tr("Exams can be saved to files and continued later. Moreover, exam files can be opened to analyze.") + br +
    tr("Exam will be completed when you give answers to all questions.<br>The number of questions depends on the level of the exam.") + br +
    tr("If you make a mistake, the question will asked two more times and the total number of questions will be increased. These are penalty questions.") + br + tr("When answer is 'not bad', you get one penalty more.");
}


TexamHelp::TexamHelp(const QString& questColorTxt, const QString& answColorTxt, bool* showHelp, QWidget* parent) :
    ThelpDialogBase(parent, 0)
{
#if !defined (Q_OS_ANDROID)
  resize((GLOB->geometry().width() / 10) * 8, (GLOB->geometry().height() / 10) * 8);
#endif
  const int iconsSize = fontMetrics().boundingRect("A").height() * 2;
  const int bigIconSize = (iconsSize * 3) / 2;
  helpText()->setHtml(
    QString("<center><h2>%1").arg(pix("help", bigIconSize)) + br +
    tr("How does an exercise or an exam work?") + QLatin1String("</h2>") + br
    + pix("practice", bigIconSize) + QLatin1String("&nbsp;&nbsp;&nbsp;&nbsp;") + pix("exam", bigIconSize) + br
    + tr(" Briefly: Nootka give you a question and you give an answer...") + br + br + br + br
    + toGetQuestTxt() + QLatin1String(":") + br_
#if defined (Q_OS_ANDROID)
    + tapIconTxt(pix("nextQuest", iconsSize))
#else
    + clickSomeButtonTxt(pix("nextQuest", iconsSize)) + br_ + pressSpaceKey() + br_ + orRightButtTxt()
#endif

    + br + br + br + QString("<span style=\"%1\">").arg(questColorTxt) +
    tr("Questions are marked with this color and \"?\" mark.") + QLatin1String("</span>") +br +
    tr("To give an answer, select it on <span style=\"%1\">Nootka's element with that color.</span><br>")
    .arg(answColorTxt) + br + br + QString("%1").arg(pix("scr", qRound(static_cast<qreal>(width()) * 0.4013104013104013  * SCR_FACTOR))) + br + br +
    tr("To check the answer confirm it:") + br_
#if defined (Q_OS_ANDROID)
    + tapIconTxt(pix("check", iconsSize))
#else
    + clickSomeButtonTxt(pix("check", iconsSize)) + br_ +
    pressEnterKey() + br_ + orRightButtTxt() + br + br
#endif

    + QLatin1String("<hr><table><tr><td valign=\"middle\" align=\"center\">") + pix("practice", bigIconSize) + br + tr("Exercises")
    + QLatin1String("</td><td align=\"center\">") +
    tr("If you made a mistake during an exercise, Nootka can show you correct answer. To see it:") + br_
#if defined (Q_OS_ANDROID)
    + tapIconTxt(pix("correct", iconsSize))
#else
    + clickSomeButtonTxt(pix("correct", iconsSize)) + br_ + orPressEnterKey()
#endif
    + br + br + tr("You can every time click button %1 to pause or stop exercising and to see your results.").arg(pix("stopExam", iconsSize)) +
    br + exerciseFeaturesText() + QLatin1String("</td></tr></table>") + br + br +

    QLatin1String("<hr><table><tr><td  valign=\"middle\" align=\"center\">") + pix("exam", bigIconSize) + br + tr("Exams")
    + QLatin1String("</td><td align=\"center\">") +
    tr("If you made a mistake during an exam and you want to repeat the question:") + br_
#if defined (Q_OS_ANDROID)
    + tapIconTxt(pix("prevQuest", iconsSize))
#else
    + clickSomeButtonTxt(pix("prevQuest", iconsSize)) + br_ + orPressBackSpace()
#endif
    + br + br + toStopExamTxt(pix("stopExam", iconsSize)) + br +
    examFeaturesText() + QLatin1String("</td></tr></table></center><center>") +

    QLatin1String("<hr><table><tr><td valign=\"middle\" align=\"center\">") + pix("exam-settings", bigIconSize) + br + tr("Settings") +
    QLatin1String("</td><td align=\"center\">") + br + br +
    tr("Just click this settings button to adjust an exercise or an exam to your current needs.")
    + QLatin1String("</td></tr></table>") + br + br

    + QLatin1String("<hr><br><br><span style=\"font-size: xx-large;\"><b>") +
    tr("GOOD LUCK!") + "</b></span>" +
    onlineDocP("exercises") + br
    + QLatin1String("</center>"));

  helpText()->resize((width() / 3) * 2, (height() / 5) * 3);
  showCheckBox(showHelp);
}






