/***************************************************************************
 *   Copyright (C) 2013-2021 by Tomasz Bojczuk                             *
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

#include "tmainhelp.h"
#include <tnootkaqml.h>


#if defined (Q_OS_ANDROID)
  inline int getPixSize() { return NOO->factor() * 1.2; }
#else
  inline int getPixSize() {
    return qRound(static_cast<qreal>(NOO->factor()) * 3.0);
  }
#endif


QString TmainHelp::exerciseAndExamText() {
  QString br = QStringLiteral("<br>");
  return youWillLearnText() + QLatin1String(" ")
      + tr("Much depends on the chosen level. Press %1 button to create your own questions set (a level).").arg(QLatin1String(" &nbsp; ")
      + NOO->pix("levelCreator", getPixSize()) + QLatin1String(" &nbsp; "))
      + br + tr("Select a level and warm up yourself exercising. Then try to pass an exam.") + br
      + QLatin1String("<table><tr><td valign=\"middle\">") + NOO->pix("practice", getPixSize())
      + QLatin1String(" </td><td valign=\"middle\"> ") + duringExercisingText() + QLatin1String("</td></tr>")
      + QLatin1String("<tr><td><br></td><td><br></td></tr>")
      + QLatin1String("<tr><td valign=\"middle\">") + NOO->pix("exam", getPixSize())
      + QLatin1String(" </td><td valign=\"middle\"> ") + duringExamsText() + QLatin1String("</td></tr></table>");
}


QString TmainHelp::youWillLearnText() {
  return tr("You will learn by answering questions. To answer, you can play, sing, put in the name of a note, and so on.");
}


QString TmainHelp::duringExercisingText() {
  return tr("During exercising %1 the program will be your understanding and friendly teacher - it will show you corrected answers if you miss.").arg(QString());
}


QString TmainHelp::duringExamsText() {
  return tr("During exams %1 Nootka will be your strict and &quot;old school&quot; master. Any mistake will be penalized with additional questions...<br>When you pass an exam you got a certificate!").arg(QString());
}

QString TmainHelp::mainHelp() {
  int pixHeight = getPixSize();
  QString bbrEnd = QLatin1String("</b><br>");
  QString nbsp3 = QLatin1String(" &nbsp; ");
  QString helpTxt = QLatin1String("<br><b>") + tr("Using Nootka may be divided into three stages:", "Don't try to translate the entries in this context/section too strict, rather use some nice words into your language to describe it. The statements like '%1' are images(icons) inside the text");
  helpTxt += QLatin1String("</b><table><tr><td>&nbsp;</td><td><hr><b>");

  helpTxt += tr("I. Discovering") + bbrEnd;
  helpTxt += tr("Exploring the interface of Nootka and how musical scores work. Just click on elements of the interface to see and get to know Nootka. Also, you can play or sing if you have a mic or web-cam.") + "<br>";
  helpTxt += tr("Press %1 buttons to see help and %2 button to adjust Nootka to your preference.").
      arg(nbsp3 + NOO->pix("logo", pixHeight) + QLatin1String(" <span style=\"font-size: x-large;\"> + </span> ")
                            + NOO->pix("help", pixHeight)  + nbsp3,
          nbsp3 + NOO->pix("systemsettings", pixHeight) + nbsp3);
  helpTxt += NOO->onlineDocP("getting-started");

  helpTxt += QLatin1String("<hr><b>") + tr("II. Exercises and exams") + bbrEnd;
  helpTxt += exerciseAndExamText();
  helpTxt += NOO->onlineDocP("exercises");

  helpTxt += QLatin1String("<hr><b>") + tr("III. Analyzing") + bbrEnd;
#if defined (Q_OS_ANDROID)
  helpTxt += QLatin1String("This feature isn't ready yet in Android version.<br><b>You may transfer Nootka files to desktop computer and see the results in Nootka version there.</b>");
#else
  helpTxt += tr("Nootka will tell you about what you've been thinking for so long... and about the progress you've been making so far....<br>Press %1 button to see and to analyze the results of your exams, find your weak points, and improve.").
    arg(nbsp3 + NOO->pix("charts", pixHeight) + nbsp3);
#endif
  helpTxt += NOO->onlineDocP("analyze");
  helpTxt += QLatin1String("</td></tr></table>");

  helpTxt += QLatin1String("<hr><b><span style=\"font-size: xx-large;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")
          + tr("Have fun!") + QLatin1String("</span></b>");
  return helpTxt;
}


