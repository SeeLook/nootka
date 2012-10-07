/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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


#include "examsettings.h"
#include "tcolorbutton.h"
#include "texamparams.h"
#include "texpertanswerhelp.h"
#include <QtGui>


ExamSettings::ExamSettings(TexamParams* params, QColor* qColor, QColor* aColor, QColor* nbColor, QWidget* parent) :
    QWidget(parent),
    m_params(params),
    m_qColor(qColor),
    m_aColor(aColor),
    m_nbColor(nbColor)
{
    QVBoxLayout *lay = new QVBoxLayout;

    m_autoNextChB = new QCheckBox(autoNextQuestTxt(), this);
    lay->addWidget(m_autoNextChB, 0, Qt::AlignCenter);
    m_autoNextChB->setChecked(m_params->autoNextQuest);
    m_repeatIncorChB = new QCheckBox(tr("repeat a question when an answer was incorrect."), this);
    lay->addWidget(m_repeatIncorChB, 0, Qt::AlignCenter);
    m_repeatIncorChB->setChecked(m_params->repeatIncorrect);
    m_repeatIncorChB->setStatusTip(tr("A question with incorrect answer will be asked once again."));
    m_expertAnswChB = new QCheckBox(expertsAnswerTxt(), this);
    m_expertAnswChB->setChecked(m_params->expertsAnswerEnable);
    lay->addWidget(m_expertAnswChB, 0, Qt::AlignCenter);
    lay->addStretch(1);
    
    m_showHelpChB = new QCheckBox(showHelpWindowTxt(), this);
    m_showHelpChB->setChecked(m_params->showHelpOnStart);
    lay->addWidget(m_showHelpChB, 0, Qt::AlignCenter);
    m_showHelpChB->setStatusTip(tr("Shows window with help when new exam begins."));
    lay->addStretch(1);
    
    QHBoxLayout *nameLay = new QHBoxLayout();
    QLabel *nameLab = new QLabel(tr("student's name:"), this);
    nameLay->addWidget(nameLab);
    m_nameEdit = new QLineEdit(m_params->studentName, this);
    m_nameEdit->setMaxLength(30);
    nameLay->addWidget(m_nameEdit);
    m_nameEdit->setStatusTip(tr("Default name for every new exam."));
    lay->addLayout(nameLay);
    lay->addStretch(1);

    QGridLayout *colLay = new QGridLayout;
    QLabel *questLab = new QLabel(tr("color of questions") + " / " + tr("color of wrong answers"), this);
    m_questColorBut = new TcolorButton(*(m_qColor), this);
    colLay->addWidget(questLab, 0, 0);
    colLay->addWidget(m_questColorBut, 0, 1);
    QLabel *answLab = new QLabel(tr("color of answers") + " / " + tr("color of correct answers"), this);
    m_answColorBut = new TcolorButton(*(m_aColor), this);
    colLay->addWidget(answLab, 1, 0);
    colLay->addWidget(m_answColorBut, 1, 1);
    QLabel *notBadLab = new QLabel(tr("color of 'not so bad' answers"), this);
    m_notBadButt = new TcolorButton(*(m_nbColor), this);
    colLay->addWidget(notBadLab, 2, 0);
    colLay->addWidget(m_notBadButt, 2, 1);

    lay->addLayout(colLay);
    lay->addStretch(1);

    setLayout(lay);
    
    connect(m_expertAnswChB, SIGNAL(clicked(bool)), this, SLOT(expertAnswersChanged(bool)));
}


void ExamSettings::saveSettings() {
    m_params->autoNextQuest = m_autoNextChB->isChecked();
    m_params->repeatIncorrect = m_repeatIncorChB->isChecked();
    m_params->expertsAnswerEnable = m_expertAnswChB->isChecked();
    m_params->showHelpOnStart = m_showHelpChB->isChecked();
    m_params->studentName = m_nameEdit->text();
        
    *m_qColor = m_questColorBut->getColor();
    m_qColor->setAlpha(40);
    *m_aColor = m_answColorBut->getColor();
    m_aColor->setAlpha(40);
    *m_nbColor = m_notBadButt->getColor();
    m_nbColor->setAlpha(40);
}


void ExamSettings::expertAnswersChanged(bool enabled) {
  if (enabled) {
      if (!showExpertAnswersHelpDlg(m_params->askAboutExpert, this, false))
        m_expertAnswChB->setChecked(false);
  }
}
