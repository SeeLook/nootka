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
#include "texamparams.h"
#include "texpertanswerhelp.h"
#include <QtGui>


ExamSettings::ExamSettings(TexamParams* params, QColor* qColor, QColor* aColor, QWidget* parent) :
    QWidget(parent),
    m_params(params),
    m_qColor(qColor),
    m_aColor(aColor)
{
    QVBoxLayout *lay = new QVBoxLayout;

    autoNextChB = new QCheckBox(autoNextQuestTxt(), this);
    lay->addWidget(autoNextChB, 0, Qt::AlignCenter);
    autoNextChB->setChecked(m_params->autoNextQuest);
    repeatIncorChB = new QCheckBox(tr("repeat a question when an answer was incorrect."), this);
    lay->addWidget(repeatIncorChB, 0, Qt::AlignCenter);
    repeatIncorChB->setChecked(m_params->repeatIncorrect);
    repeatIncorChB->setStatusTip(tr("A question with incorrect answer will be asked once again."));
    expertAnswChB = new QCheckBox(expertsAnswerTxt(), this);
    lay->addWidget(expertAnswChB, 0, Qt::AlignCenter);
    lay->addStretch(1);
    
    showHelpChB = new QCheckBox(showHelpWindowTxt(), this);
    showHelpChB->setChecked(m_params->showHelpOnStart);
    lay->addWidget(showHelpChB, 0, Qt::AlignCenter);
    showHelpChB->setStatusTip(tr("Shows window with help when new exam begins."));
    lay->addStretch(1);
    
    QHBoxLayout *nameLay = new QHBoxLayout();
    QLabel *nameLab = new QLabel(tr("student's name:"), this);
    nameLay->addWidget(nameLab);
    nameEdit = new QLineEdit(m_params->studentName, this);
    nameEdit->setMaxLength(30);
    nameLay->addWidget(nameEdit);
    nameEdit->setStatusTip(tr("Default name for every new exam."));
    lay->addLayout(nameLay);
    lay->addStretch(1);

    QGridLayout *colLay = new QGridLayout;
    QLabel *questLab = new QLabel(tr("color of questions"), this);
    questColorBut = new TcolorButton(*(m_qColor), this);
    colLay->addWidget(questLab, 0, 0);
    colLay->addWidget(questColorBut, 0, 1);
    QLabel *answLab = new QLabel(tr("color of answers"), this);
    answColorBut = new TcolorButton(*(m_aColor), this);
    colLay->addWidget(answLab, 1, 0);
    colLay->addWidget(answColorBut, 1, 1);

    lay->addLayout(colLay);
    lay->addStretch(1);

    setLayout(lay);
    
    connect(expertAnswChB, SIGNAL(clicked(bool)), this, SLOT(expertAnswersChanged(bool)));
}


void ExamSettings::saveSettings() {
    m_params->autoNextQuest = autoNextChB->isChecked();
    m_params->repeatIncorrect = repeatIncorChB->isChecked();
    m_params->expertsAnswerEnable = expertAnswChB->isChecked();
    m_params->showHelpOnStart = showHelpChB->isChecked();
    m_params->studentName = nameEdit->text();
        
    *m_qColor = questColorBut->getColor();
    m_qColor->setAlpha(40);
    *m_aColor = answColorBut->getColor();
    m_aColor->setAlpha(40);
}


void ExamSettings::expertAnswersChanged(bool enabled) {
  if (enabled) {
      if (!showExpertAnswersHelpDlg(m_params->askAboutExpert, this, false))
        expertAnswChB->setChecked(false);
  }
}
