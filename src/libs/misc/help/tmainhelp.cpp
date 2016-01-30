/***************************************************************************
 *   Copyright (C) 2013-2016 by Tomasz Bojczuk                             *
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


#include "tmainhelp.h"
#include "thelpdialogbase.h"
#include <graphics/tnotepixmap.h>
#include <tpath.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qscroller.h>


/*static*/
inline int getPixSize() { return qApp->fontMetrics().height() * 1.5; }

QString TmainHelp::exerciseAndExamText() {
	QString helpTxt;
	helpTxt = youWillLearnText() + " ";
	helpTxt += QApplication::translate("TmainHelp", "Much depends on the chosen level. Press %1 button to create your own questions set (a level).").
			arg(" &nbsp; " + pixToHtml(Tpath::img("levelCreator"), getPixSize()) + " &nbsp; ");
  helpTxt += "<br>" + QApplication::translate("TmainHelp", "Select a level and warm up yourself exercising. Then try to pass an exam.");
	helpTxt += "<br>" + duringExercisingText();
	helpTxt += "<br>" + duringExamsText();
	return helpTxt;
}


QString TmainHelp::youWillLearnText() {
	return QApplication::translate("TmainHelp", "You will learn by answering questions. To answer, you can play, sing, put in the name of a note, and so on.");
}


QString TmainHelp::duringExercisingText() {
	return QApplication::translate("TmainHelp", "During exercising %1 the program will be your understanding and friendly teacher - it will show you corrected answers if you miss.").arg(" &nbsp; " + pixToHtml(Tpath::img("practice"), 32) + " &nbsp; ");
}


QString TmainHelp::duringExamsText() {
	return QApplication::translate("TmainHelp", "During exams %1 Nootka will be your strict and &quot;old school&quot; master. Any mistake will be penalized with additional questions...<br>When you pass an exam you got a certificate!").arg(" &nbsp; " + pixToHtml(Tpath::img("exam"), 32) + " &nbsp; ");
}



//#######################################################################################################


TmainHelp::TmainHelp(QWidget* parent) :
    QWidget(parent)
{ 
  QVBoxLayout *lay = new QVBoxLayout;
  QString helpTxt = "<br><b>" + QApplication::translate("TmainHelp", "Using Nootka may be divided into three stages:", "Don't try to translate the entries in this context/section too strict, rather use some nice words into your language to describe it. The statements like '%1' are images(icons) inside the text");
  helpTxt += "</b><table><tr><td>&nbsp;</td><td><hr><b>";
  
  helpTxt += QApplication::translate("TmainHelp", "I. Discovering") + "</b><br>";
  helpTxt += QApplication::translate("TmainHelp", "Exploring the interface of Nootka and how musical scores work. Just click on elements of the interface to see and get to know Nootka. Also, you can play or sing if you have a mic or web-cam.") + "<br>";
  helpTxt += QApplication::translate("TmainHelp", "Press %1 buttons to see help and %2 button to adjust Nootka to your preference.").
      arg(" &nbsp; " + pixToHtml(Tpath::img("logo"), getPixSize() * 2.2) + " <span style=\"font-size: x-large;\"> + </span> " + pixToHtml(Tpath::img("help"), getPixSize())  + " &nbsp; ").
      arg(" &nbsp; " + pixToHtml(Tpath::img("systemsettings"), getPixSize()) + " &nbsp; ");
  helpTxt += ThelpDialogBase::onlineDocP("getting-started");
  
  helpTxt += "<hr><b>" + QApplication::translate("TmainHelp", "II. Exercises and exams") + "</b><br>";
	helpTxt += exerciseAndExamText();
  helpTxt += ThelpDialogBase::onlineDocP("exercises");
	
  helpTxt += "<hr><b>" + QApplication::translate("TmainHelp", "III. Analyzing") + "</b><br>";
  helpTxt += QApplication::translate("TmainHelp", "Nootka will tell you about what you've been thinking for so long... and about the progress you've been making so far....<br>Press %1 button to see and to analyze the results of your exams, find your weak points, and improve.").
    arg(" &nbsp; " + pixToHtml(Tpath::img("charts"), getPixSize()) + " &nbsp; ");
  helpTxt += ThelpDialogBase::onlineDocP("analyze");
  helpTxt += "</td></tr></table>";
  
  helpTxt += "<hr><b><span style=\"font-size: xx-large;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + QApplication::translate("TmainHelp", "Have fun!") + "</span></b>";
  QTextBrowser *helpEdit = new QTextBrowser(this);
  helpEdit->setHtml(helpTxt);
	helpEdit->setReadOnly(true);
  helpEdit->setOpenExternalLinks(true);
  helpEdit->setTextInteractionFlags(Qt::LinksAccessibleByKeyboard | Qt::LinksAccessibleByMouse);
  QScroller::grabGesture(helpEdit->viewport(), QScroller::LeftMouseButtonGesture);
// 	qDebug() << helpEdit->toHtml();
  lay->addWidget(helpEdit);
  setLayout(lay);
}

 
