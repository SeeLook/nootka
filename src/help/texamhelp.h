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

#ifndef TEXAMHELP_H
#define TEXAMHELP_H

#include <QtCore/qobject.h>

class TexamHelp : public QObject
{
    Q_OBJECT

public:
    TexamHelp(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    static QString getQuestionText();
    static QString confirmText();
    static QString practiceText();
    static QString practiceExplained();
    static QString examText();
    static QString examExplained();
    static QString optionsText();

    /**
     * check answers without requiring confirmation
     */
    static QString expertsAnswerTxt() { return tr("check answers without requiring confirmation"); }

    /**
     * correct mistakes (exercises only)
     */
    static QString correctMistakesTxt() { return tr("correct mistakes (exercises only)"); }

    /**
     * ask next question automatically
     */
    static QString autoNextQuestTxt() { return tr("ask next question automatically"); }

    /**
     * or right mouse button
     */
    static QString orRightButtTxt() { return tr("or right mouse button"); }

    /**
     * click /button-icon/ button
     */
    static QString clickSomeButtonTxt(const QString &imgHtmlTag) { return tr("click %1 button").arg(imgHtmlTag); }

    /**
     * tap (icon image)
     */
    static QString tapIconTxt(const QString &imgHtmlTag) { return tr("tap %1", "tap <icon image follows>").arg(imgHtmlTag); }

    /**
     * To get question
     */
    static QString toGetQuestTxt() { return tr("To get question"); }

    /**
     * press space key
     */
    static QString pressSpaceKey() { return tr("press <b>space</b> key"); }

    /**
     * press Enter key
     */
    static QString pressEnterKey() { return tr("press <b>Enter</b> key"); }

    /**
     * or  press Enter key
     */
    static QString orPressEnterKey() { return tr("or press <b>Enter</b> key"); }

    /**
     * or Backspace key
     */
    static QString orPressBackSpace() { return tr("or <b>Backspace</b> key"); }

    /**
     * Click %1 button to stop.
     */
    static QString toStopExamTxt(const QString &imgHtmlTag) { return tr("Click %1 button to stop.").arg(imgHtmlTag); }

    /**
     * Click %1 button to stop.
     */
    static QString toStopExamImageTxt(const QString &imgHtmlTag)
    {
        return tr("Click %1 to stop.", "click <icon-image> to stop (exam or exercise)").arg(imgHtmlTag);
    }

    /**
     * Describing exercise mode:
     * Only recent exercise is stored for further continuation.
     * When your exercising will go well Nootka will suggest you to start an exam.
     */
    static QString exerciseFeaturesText();

    /**
     * Describing exam mode
     */
    static QString examFeaturesText();
};

#endif // TEXAMHELP_H
