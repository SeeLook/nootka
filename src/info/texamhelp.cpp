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


#define PIXICONSIZE (32)

TexamHelp::TexamHelp(QString questColorTxt, QString answColorTxt, 
                     QString& path, bool& showHelp, QWidget* parent) :
    QDialog(parent),
    m_showHelp(showHelp)
{
//   setMaximumSize((parent->width()/3)*2, (parent->height()/3)*2);
  resize((parent->width()/3)*2, (parent->height()/3)*2);
  setWindowTitle(tr("Exam help"));
  
  QVBoxLayout *lay = new QVBoxLayout();
  QTextEdit *ed = new QTextEdit(
    QString("<center><img src=\"%1\"><span style=\"font-size: 18px;\"> ").arg(path+"picts/help.png") +
    tr("How does an exam work ?") +
    QString(" </span><img src=\"%1\"> ").arg(path+"picts/startExam.png") + "<br><br>" +
    "<style type=\"text/css\">img { border-style: solid; border-color: palette(text); border-width: 2px; background-color: palette(window); }</style>" +
    toGetQuestTxt() + ":<br>- " + clickSomeButtonTxt(pixToHtml(path + "picts/nextQuest.png", PIXICONSIZE)) + "<br>- " +
    pressSpaceKey() + "<br>- " + orRightButtTxt() + "<br>" + 
    tr("Select 2nd check box to get the question automatically.") + 
    QString("<br><br><span style=\"%1\">").arg(questColorTxt) +
    tr("Questions are marked with this color and \"?\" mark.") + "</span><br>" + 
    tr("To give an answer, select it on <span style=\"%1\">Nootka's element with that color.</span><br>")
      .arg(answColorTxt) +
    QString("<br><br><img src=\"%1\"><br><br>").arg(path+"picts/scr.png") +
    tr("To check the answer confirm it:") + "<br>- " + 
    clickSomeButtonTxt(pixToHtml(path + "picts/check.png", PIXICONSIZE)) + "<br>- " +
    pressEnterKey() + "<br>- " + orRightButtTxt() + "<br><br>" +
    tr("If you made a mistake and you want to repeat the question:") + "<br>- " + 
    clickSomeButtonTxt(pixToHtml(path+"picts/prevQuest.png", PIXICONSIZE)) + "<br>- " + orPressBkSTxt() + "<br>" +
    tr("By selecting the 3rd check box, answers will be checked immediately without confirmation required.") + "<br><br>" +
    toStopExamTxt(pixToHtml(path + "picts/stopExam.png", PIXICONSIZE)) + "<br><br>" + 
    tr("Exam will be complete when you give answers to all questions.<br>The number of questions depends on the level of the exam.") + "<br>" +
    tr("If you make a mistake, the question will asked two more times and the total number of questions will be increased. These are penalty questions.") + "<br>" +
    tr("When answer is 'not bad', you get one penalty more.") +
    "<br><br><span style=\"font-size: 20px;\"><b>" +
    tr("GOOD LUCK!") + "</b></span>" + "<br><hr><table><tr><th colspan=2>" +
    tr("Experts corner") + "</th></tr><tr><td rowspan=3>" +
    QString("<img style=\"background-color: transparent;\" src=\"%1\">").arg(path+"picts/expertCorner.png") +
    "</td><td><br>1. " + TexamSettings::correctMistakesTxt() + "</td></tr><tr><td><br>2. " + 
    TexamSettings::autoNextQuestTxt() + "</td></tr><tr><td><br>3. " + 
    TexamSettings::expertsAnswerTxt() + "</td></tr></table>", this);
  ed->setReadOnly(true);
//   ed->setFixedSize((parent->width()/3)*2, (parent->height()/5)*3);
  ed->resize((parent->width()/3)*2, (parent->height()/5)*3);
  ed->setAlignment(Qt::AlignCenter);
  lay->addWidget(ed);
  
  showHelpChB = new QCheckBox(TexamSettings::showHelpWindowTxt(), this);
  lay->addWidget(showHelpChB, 0, Qt::AlignCenter);
  showHelpChB->setChecked(m_showHelp);
  
  
  QPushButton *okBut = new QPushButton(tr("OK"),  this);
  lay->addWidget(okBut, 0, Qt::AlignCenter);
  
  setLayout(lay);
  
  connect(okBut, SIGNAL(clicked()), this, SLOT(closeHelp()));

}


void TexamHelp::closeHelp() {
  m_showHelp = showHelpChB->isChecked();
  close();
}





