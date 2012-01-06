/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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
#include "examsettings.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QCheckBox>

TexamHelp::TexamHelp(QString questColorTxt, QString answColorTxt, 
                     QString& path, bool& showHelp, QWidget* parent) :
    QDialog(parent),
    m_showHelp(showHelp)
{
  setMaximumSize((parent->width()/3)*2, (parent->height()/3)*2);
  setWindowTitle(tr("Exam's help"));
  
  QVBoxLayout *lay = new QVBoxLayout();
  QTextEdit *ed = new QTextEdit(
    QString("<center><img src=\"%1\"><span style=\"font-size: 18px;\"> ").arg(path+"picts/help.png") +
    tr("How does an exam work ?") +
    QString(" </span><img src=\"%1\"> ").arg(path+"picts/startExam.png") + "<br><br>" +
    "<style type=\"text/css\">img { border-style: solid; border-color: palette(text); border-width: 2px; background-color: palette(window); }</style>" +
    tr("To get question:") + "<br>- " + tr("click <img src=\"%1\"> button").arg(path+"picts/next-icon.png") + "<br>- " +
    tr("press <b>space</b> key") + "<br>- " + orRightButtTxt() + "<br>" + 
    tr("Select 2-nd check box to get the question automaticaly.") + 
    QString("<br><br><span style=\"%1\">").arg(questColorTxt) +
    tr("Questions are marked with this color and \"?\" mark.") + "</span><br>" + 
    tr("To give an answer, select it on <span style=\"%1\">Nootka's element pointed with that color.</span><br>")
      .arg(answColorTxt) +
    QString("<br><br><img src=\"%1\"><br><br>").arg(path+"picts/scr.png") +
    tr("To check the answer confirm it:") + "<br>- " + 
    tr("click <img src=\"%1\"> button").arg(path+"picts/check-icon.png") + "<br>- " +
    tr("press <b>Enter</b> key") + "<br>- " + orRightButtTxt() + "<br><br>" +
    tr("If You make mistake, You can use:<br>- <img src=\"%1\"> button<br>- <b>Backspace</b> key<br>to repeat the question.<br>").arg(path+"picts/prev-icon.png") +
    tr("By selecting 3-rd check box, the answers will be checking immediately without confirmation.") + "<br><br>" +
    tr("To stop the exam click <img src=\"%1\"> button.").arg(path+"picts/stopExam-icon.png") + 
    "<br><br><span style=\"font-size: 20px;\"><b>" +
    tr("GOOD LUCK !!!") + "</b></span>" + "<br><hr><table><tr><th colspan=2>" +
    tr("Experts' corner") + "</th></tr><tr><td rowspan=3>" +
    QString("<img style=\"background-color: transparent;\" src=\"%1\">").arg(path+"picts/expertCorner.png") +
    "</td><td><br>1. " + tr("show or hide the hints") + "</td></tr><tr><td><br>2. " + 
    ExamSettings::autoNextQuestTxt() + "</td></tr><tr><td><br>3. " + 
    ExamSettings::expertsAnswerTxt() + "</td></tr></table>", this);
  ed->setReadOnly(true);
  ed->setFixedSize((parent->width()/3)*2, (parent->height()/5)*3);
  ed->setAlignment(Qt::AlignCenter);
  lay->addWidget(ed);
  
  showHelpChB = new QCheckBox(ExamSettings::showHelpWindowTxt(), this);
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





