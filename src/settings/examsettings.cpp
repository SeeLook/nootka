/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "examsettings.h"
#include "tcolorbutton.h"
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;

ExamSettings::ExamSettings(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;

    autoNextChB = new QCheckBox(autoNextQuestTxt(), this);
    lay->addWidget(autoNextChB, 0, Qt::AlignCenter);
    autoNextChB->setChecked(gl->EautoNextQuest);
    repeatIncorChB = new QCheckBox(tr("repeat a question when an answer was incorrect."), this);
    lay->addWidget(repeatIncorChB, 0, Qt::AlignCenter);
    repeatIncorChB->setChecked(gl->ErepeatIncorrect);
    repeatIncorChB->setStatusTip(tr("A question with incorrect answer will be asked once again."));
    expertAnswChB = new QCheckBox(expertsAnswerTxt(), this);
    lay->addWidget(expertAnswChB, 0, Qt::AlignCenter);
    lay->addStretch(1);
    
    showHelpChB = new QCheckBox(tr("show help when exam starts"), this);
    lay->addWidget(showHelpChB, 0, Qt::AlignCenter);
    showHelpChB->setStatusTip(tr("Shows window with help when new exam begins."));
    lay->addStretch(1);
    QLabel *nameLab = new QLabel(tr("student's name:"), this);
    lay->addWidget(nameLab, 0, Qt::AlignCenter);
    nameEdit = new QLineEdit(this);
    nameEdit->setMaxLength(20);
    lay->addWidget(nameEdit, 0, Qt::AlignCenter);
    nameEdit->setStatusTip(tr("Default name for every new exam."));
    lay->addStretch(1);

    QGridLayout *colLay = new QGridLayout;
    QLabel *questLab = new QLabel(tr("color of questions"), this);
    questColorBut = new TcolorButton(gl->EquestionColor, this);
    colLay->addWidget(questLab, 0, 0);
    colLay->addWidget(questColorBut, 0, 1);
    QLabel *answLab = new QLabel(tr("color of answers"), this);
    answColorBut = new TcolorButton(gl->EanswerColor, this);
    colLay->addWidget(answLab, 1, 0);
    colLay->addWidget(answColorBut, 1, 1);

    lay->addLayout(colLay);
    lay->addStretch(1);

    setLayout(lay);

}


void ExamSettings::saveSettings() {
    gl->EautoNextQuest = autoNextChB->isChecked();
    gl->ErepeatIncorrect = repeatIncorChB->isChecked();
    gl->EquestionColor = questColorBut->getColor();
    gl->EquestionColor.setAlpha(40);
    gl->EanswerColor = answColorBut->getColor();
    gl->EanswerColor.setAlpha(40);
}
