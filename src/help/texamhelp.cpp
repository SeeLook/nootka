/***************************************************************************
 *   Copyright (C) 2011-2021 by Tomasz Bojczuk                             *
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
#include <tnootkaqml.h>
#include <tpath.h>

QString br = QLatin1String("<br>");
QString br_ = QLatin1String("<br>- ");

QString TexamHelp::exerciseFeaturesText()
{
    return tr("When your exercising will go well Nootka will suggest you to start an exam.");
}

QString TexamHelp::examFeaturesText()
{
    return tr("Exams can be saved to files and continued later. Moreover, exam files can be opened to analyze.") + br
        + tr("Exam will be completed when you give answers to all questions.<br>The number of questions depends on the level of the exam.") + br
        + tr("If you make a mistake, the question will asked two more times and the total number of questions will be increased. These are penalty questions.")
        + br + tr("When answer is 'not bad', you get one penalty more.");
}

QString TexamHelp::getQuestionText()
{
    const int iconsSize = NOO->factor() * (NOO->isAndroid() ? 1.8 : 3);

    return toGetQuestTxt() + QLatin1String(":") + br + br_
#if defined(Q_OS_ANDROID)
        + tapIconTxt(NOO->pix("nextQuest", iconsSize));
#else
        + clickSomeButtonTxt(NOO->pix("nextQuest", iconsSize)) + br_ + pressSpaceKey(); // + br_ + orRightButtTxt()
#endif
}

QString TexamHelp::confirmText()
{
    const int iconsSize = NOO->factor() * (NOO->isAndroid() ? 1.8 : 3);

    return tr("To check the answer confirm it:") + br + br_
#if defined(Q_OS_ANDROID)
        + tapIconTxt(NOO->pix("check", iconsSize));
#else
        + clickSomeButtonTxt(NOO->pix("check", iconsSize)) + br_ + pressEnterKey(); // br_ + orRightButtTxt()
#endif
}

QString TexamHelp::practiceText()
{
    const int iconsSize = NOO->factor() * (NOO->isAndroid() ? 1.8 : 3);
    return tr("If you made a mistake during an exercise, Nootka can show you correct answer. To see it:") + br + br_
#if defined(Q_OS_ANDROID)
        + tapIconTxt(NOO->pix("correct", iconsSize));
#else
        + clickSomeButtonTxt(NOO->pix("correct", iconsSize)) + br_ + orPressEnterKey();
#endif
}

QString TexamHelp::practiceExplained()
{
    const int iconsSize = NOO->factor() * (NOO->isAndroid() ? 1.8 : 3);
    return tr("You can every time click button %1 to pause or stop exercising and to see your results.").arg(NOO->pix("stopExam", iconsSize)) + br
        + exerciseFeaturesText();
}

QString TexamHelp::examText()
{
    const int iconsSize = NOO->factor() * (NOO->isAndroid() ? 1.8 : 3);

    return tr("If you made a mistake during an exam and you want to repeat the question:") + br + br_
#if defined(Q_OS_ANDROID)
        + tapIconTxt(NOO->pix("prevQuest", iconsSize));
#else
        + clickSomeButtonTxt(NOO->pix("prevQuest", iconsSize)) + br_ + orPressBackSpace();
#endif
}

QString TexamHelp::examExplained()
{
    const int iconsSize = NOO->factor() * (NOO->isAndroid() ? 1.8 : 3);
    return toStopExamTxt(NOO->pix("stopExam", iconsSize)) + br + examFeaturesText();
}

QString TexamHelp::optionsText()
{
    return tr("Just click this settings button to adjust an exercise or an exam to your current needs.");
}
