/***************************************************************************
 *   Copyright (C) 2013 by Tomasz Bojczuk                                  *
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
#include "tpixmaker.h"
#include <QBoxLayout>
#include <QTextEdit>
#include <QApplication>

#define PIXSIZE (28)


/*static*/
QString TmainHelp::exerciseAndExamText(const QString& path) {
	QString helpTxt;
	helpTxt = youWillLearnText() + " ";
	helpTxt += QApplication::translate("TmainHelp", "Much depends on the chosen level. Press %1 button to create your own questions set (a level).").
			arg(" &nbsp; " + pixToHtml(path + "picts/levelCreator.png", PIXSIZE) + " &nbsp; ");
  helpTxt += "<br>" + QApplication::translate("TmainHelp", "Select a level and warm up yourself exercising. Then try to pass an exam.");
	helpTxt += "<br>" + duringExamsText(path);
  helpTxt += "<br>" + duringExercisingText(path);
	return helpTxt;
}


QString TmainHelp::youWillLearnText() {
	return QApplication::translate("TmainHelp", "You will learn by answering questions. To answer, you can play, sing, put in the name of a note, and so on.");
}


QString TmainHelp::duringExercisingText(const QString& path) {
	return QApplication::translate("TmainHelp", "During exercising %1 the program will be your understanding and friendly teacher - it will show you corrected answers if you miss.").arg(" &nbsp; " + pixToHtml(path+"picts/practice.png", 32) + " &nbsp; ");
}


QString TmainHelp::duringExamsText(const QString& path) {
	return QApplication::translate("TmainHelp", "During exams %1 Nootka will be your strict and &quot;old school&quot; master. Any mistake will be penalized with additional questions...<br>When you pass an exam you got a certificate!").arg(" &nbsp; " + pixToHtml(path + "picts/nootka-exam.png", 32) + " &nbsp; ");
}



//#######################################################################################################

TmainHelp::TmainHelp(const QString& path, QWidget* parent) :
    QWidget(parent)
{ 
  QVBoxLayout *lay = new QVBoxLayout;
  QString helpTxt = "<br><b>" + QApplication::translate("TmainHelp", "Using Nootka may be divided into three stages:", "Don't try to translate the entries in this context/section too strict, rather use some nice words into your language to describe it. The statements like '%1' are images(icons) inside the text");
  helpTxt += "</b><table><tr><td>&nbsp;</td><td><hr><b>";
  
  helpTxt += QApplication::translate("TmainHelp", "I. Discovering") + "</b><br>";
  helpTxt += QApplication::translate("TmainHelp", "Exploring the interface of Nootka and how musical scores work. Just click on elements of the interface to see and get to know Nootka. Also, you can play or sing if you have a mic or web-cam.") + "<br>";
  helpTxt += QApplication::translate("TmainHelp", "Press %1 buttons to see help and %2 button to adjust Nootka to your preference.").
      arg(" &nbsp; " + pixToHtml(path + "picts/about.png", PIXSIZE) + " <span style=\"font-size: 25px;\"> + </span> " + pixToHtml(path + "picts/help.png", PIXSIZE)  + " &nbsp; ").
      arg(" &nbsp; " + pixToHtml(path + "picts/systemsettings.png", PIXSIZE) + " &nbsp; ");
      
  helpTxt += "<hr><b>" + QApplication::translate("TmainHelp", "II. Exercises and exams") + "</b><br>";
	helpTxt += exerciseAndExamText(path);
  
	
  helpTxt += "<hr><b>" + QApplication::translate("TmainHelp", "III. Analyzing") + "</b><br>";
  helpTxt += QApplication::translate("TmainHelp", "Nootka will tell you about what you've been thinking for so long... and about the progress you've been making so far....<br>Press %1 button to see and to analyze the results of your exams, find your weak points, and improve.").
    arg(" &nbsp; " + pixToHtml(path + "picts/charts.png", PIXSIZE) + " &nbsp; ");;
  helpTxt += "</td></tr></table>";
  helpTxt += "<hr><b><span style=\"font-size: 25px;\">&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" + QApplication::translate("TmainHelp", "Have fun!") + "</span></b>";
  QTextEdit *helpEdit = new QTextEdit(this);
  helpEdit->setHtml(helpTxt);
	helpEdit->setReadOnly(true);
// 	qDebug() << helpEdit->toHtml();
  lay->addWidget(helpEdit);
  setLayout(lay);
}

 
