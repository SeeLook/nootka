/***************************************************************************
 *   Copyright (C) 2013-2015 by Tomasz Bojczuk                             *
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
#include "thelpdialogbase.h"
#include <graphics/tnotepixmap.h>
#include <tpath.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qscroller.h>



/*static*/
#if defined (Q_OS_ANDROID)
  inline int getPixSize() { return qApp->fontMetrics().height() * 1.2; }
#else
  inline int getPixSize() { return qApp->fontMetrics().height() * 1.5; }
#endif


QString TmainHelp::exerciseAndExamText() {
	QString helpTxt;
  QString br = QStringLiteral("<br>");
	helpTxt = youWillLearnText() + " ";
	helpTxt += QApplication::translate("TmainHelp", "Much depends on the chosen level. Press %1 button to create your own questions set (a level).").
			arg(QLatin1String(" &nbsp; ") + pixToHtml(Tpath::img("levelCreator"), getPixSize()) + QLatin1String(" &nbsp; "));
  helpTxt += br + QApplication::translate("TmainHelp", "Select a level and warm up yourself exercising. Then try to pass an exam.");
	helpTxt += br + duringExercisingText();
	helpTxt += br + duringExamsText();
	return helpTxt;
}


QString TmainHelp::youWillLearnText() {
	return QApplication::translate("TmainHelp",
                                 "You will learn by answering questions. To answer, you can play, sing, put in the name of a note, and so on.");
}


QString TmainHelp::duringExercisingText() {
  QString nbsp3 = QLatin1String(" &nbsp; ");
	return QApplication::translate("TmainHelp", "During exercising %1 the program will be your understanding and friendly teacher - it will show you corrected answers if you miss.").arg(nbsp3 + pixToHtml(Tpath::img("practice"), getPixSize() * 1.3) + nbsp3);
}


QString TmainHelp::duringExamsText() {
  QString nbsp3 = QLatin1String(" &nbsp; ");
	return QApplication::translate("TmainHelp", "During exams %1 Nootka will be your strict and &quot;old school&quot; master. Any mistake will be penalized with additional questions...<br>When you pass an exam you got a certificate!").arg(nbsp3 + pixToHtml(Tpath::img("exam"), getPixSize() * 1.3) + nbsp3);
}



//#######################################################################################################


TmainHelp::TmainHelp(QWidget* parent) :
  QWidget(parent)
{
  int pixSize = getPixSize();
  QString bbrEnd = QLatin1String("</b><br>");
  QString nbsp3 = QLatin1String(" &nbsp; ");
  QString helpTxt = QLatin1String("<br><b>") + QApplication::translate("TmainHelp", "Using Nootka may be divided into three stages:", "Don't try to translate the entries in this context/section too strict, rather use some nice words into your language to describe it. The statements like '%1' are images(icons) inside the text");
  helpTxt += QLatin1String("</b><table><tr><td>&nbsp;</td><td><hr><b>");

  helpTxt += QApplication::translate("TmainHelp", "I. Discovering") + bbrEnd;
  helpTxt += QApplication::translate("TmainHelp", "Exploring the interface of Nootka and how musical scores work. Just click on elements of the interface to see and get to know Nootka. Also, you can play or sing if you have a mic or web-cam.") + "<br>";
  helpTxt += QApplication::translate("TmainHelp", "Press %1 buttons to see help and %2 button to adjust Nootka to your preference.").
      arg(nbsp3 + pixToHtml(Tpath::img("logo"), pixSize * 2.2) + QLatin1String(" <span style=\"font-size: x-large;\"> + </span> ")
                            + pixToHtml(Tpath::img("help"), pixSize)  + nbsp3).
      arg(nbsp3 + pixToHtml(Tpath::img("systemsettings"), pixSize) + nbsp3);
  helpTxt += ThelpDialogBase::onlineDocP("getting-started");

  helpTxt += "<hr><b>" + QApplication::translate("TmainHelp", "II. Exercises and exams") + bbrEnd;
	helpTxt += exerciseAndExamText();
  helpTxt += ThelpDialogBase::onlineDocP("exercises");

  helpTxt += QLatin1String("<hr><b>") + QApplication::translate("TmainHelp", "III. Analyzing") + bbrEnd;
#if defined (Q_OS_ANDROID)
  helpTxt += QLatin1String("This feature isn't ready yet in Android version.<br><b>You may transfer Nootka files to desktop computer and see the results in Nootka version there.");
#else
  helpTxt += QApplication::translate("TmainHelp", "Nootka will tell you about what you've been thinking for so long... and about the progress you've been making so far....<br>Press %1 button to see and to analyze the results of your exams, find your weak points, and improve.").
    arg(nbsp3 + pixToHtml(Tpath::img("charts"), pixSize) + nbsp3);
#endif
  helpTxt += ThelpDialogBase::onlineDocP("analyze");
  helpTxt += QLatin1String("</td></tr></table>");

  helpTxt += QLatin1String("<hr><b><span style=\"font-size: xx-large;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;")
          + QApplication::translate("TmainHelp", "Have fun!") + QLatin1String("</span></b>");

  QTextBrowser *helpEdit = new QTextBrowser(this);
  helpEdit->setHtml(helpTxt);
	helpEdit->setReadOnly(true);
  helpEdit->setOpenExternalLinks(true);
  helpEdit->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
  QScroller::grabGesture(helpEdit->viewport(), QScroller::LeftMouseButtonGesture);
#if defined (Q_OS_ANDROID)
  helpEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  helpEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#endif
// 	qDebug() << helpEdit->toHtml();

  auto lay = new QVBoxLayout;
  lay->addWidget(helpEdit);
  setLayout(lay);
}

 
