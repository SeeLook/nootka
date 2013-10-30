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


#include "texamsettings.h"
#include "widgets/tcolorbutton.h"
#include "texamparams.h"
#include "texpertanswerhelp.h"
#include "widgets/troundedlabel.h"
#include <QtGui>


TexamSettings::TexamSettings(TexamParams* params, QColor* qColor, QColor* aColor, QColor* nbColor, QWidget* parent) :
    QWidget(parent),
    m_params(params),
    m_qColor(qColor),
    m_aColor(aColor),
    m_nbColor(nbColor)
{
// 		QLabel *expertLab = new QLabel(QString("<img src=\"%1\">").arg());
		m_correctChB = new QCheckBox(correctMistakesTxt(), this);
			m_correctChB->setStatusTip(tr("When you will make mistake, the program will show you automatically how a correct answer should be."));
			m_correctChB->setChecked(m_params->showCorrected);
		QLabel *timeLab = new QLabel(tr("preview time of corrected answer"), this);
		m_viewTimeSlider = new QSlider(Qt::Horizontal, this);
			m_viewTimeSlider->setStatusTip(tr("How long time a correct answer will be displayed when next question is asked automatically."));
			m_viewTimeSlider->setMinimum(0);
			m_viewTimeSlider->setMaximum(6000);
			m_viewTimeSlider->setValue(m_params->correctViewDuration);
			m_viewTimeSlider->setSingleStep(500);
			m_viewTimeSlider->setPageStep(1000);
			m_viewTimeSlider->setTickPosition(QSlider::TicksBelow);
			m_viewTimeSlider->setTickInterval(500);
		m_timeLabel = new TroundedLabel(this);
			timePreviewChanged(m_params->correctViewDuration);
		m_suggestExamChB = new QCheckBox(tr("suggest an exam"), this);
			m_suggestExamChB->setStatusTip(tr("Watch exercising progress and when exercising is going well, suggest to start an exam on exercise level."));
			m_suggestExamChB->setChecked(m_params->suggestExam);

    
    m_repeatIncorChB = new QCheckBox(tr("repeat a question when its answer is incorrect"), this);
			m_repeatIncorChB->setChecked(m_params->repeatIncorrect);
			m_repeatIncorChB->setStatusTip(tr("A question with an incorrect answer will be asked once again."));
		m_closeConfirmChB = new QCheckBox(tr("close without confirm"), this);
			m_closeConfirmChB->setStatusTip(tr("If checked, an application will not ask to answer pending question just mark it as wrong, save an exam to file (in directory: %1) and close itself without any confirmation needed.").arg("<b>" + m_params->examsDir + "</b>"));
			m_closeConfirmChB->setChecked(m_params->closeWithoutConfirm);
    
		m_autoNextChB = new QCheckBox(autoNextQuestTxt(), this);
			m_autoNextChB->setChecked(m_params->autoNextQuest);
		m_expertAnswChB = new QCheckBox(expertsAnswerTxt(), this);
			m_expertAnswChB->setChecked(m_params->expertsAnswerEnable);    
    QLabel *nameLab = new QLabel(tr("Student Name:"), this);
    m_nameEdit = new QLineEdit(m_params->studentName, this);
			m_nameEdit->setMaxLength(30);
			m_nameEdit->setStatusTip(tr("Default name for every new exam or exercise."));

    QLabel *questLab = new QLabel(tr("color of questions") + " / " + tr("color of wrong answers"), this);
    m_questColorBut = new TcolorButton(*(m_qColor), this);
    QLabel *answLab = new QLabel(tr("color of answers") + " / " + tr("color of correct answers"), this);
    m_answColorBut = new TcolorButton(*(m_aColor), this);
    QLabel *notBadLab = new QLabel(tr("color of 'not bad' answers"), this);
    m_notBadButt = new TcolorButton(*(m_nbColor), this);

		
		QVBoxLayout *mainLay = new QVBoxLayout;
		QGroupBox *commonGr = new QGroupBox(this);
		QVBoxLayout *commonLay = new QVBoxLayout;
		QHBoxLayout *nameLay = new QHBoxLayout();
			nameLay->addStretch();
			nameLay->addWidget(nameLab);
			nameLay->addStretch();
			nameLay->addWidget(m_nameEdit);
			nameLay->addStretch();
		commonLay->addLayout(nameLay);
			commonLay->addWidget(m_correctChB, 0, Qt::AlignCenter);
			commonLay->addWidget(m_autoNextChB, 0, Qt::AlignCenter);
			commonLay->addWidget(m_expertAnswChB, 0, Qt::AlignCenter);
			commonLay->addStretch();
			commonLay->addStretch();
		QGridLayout *colorsLay = new QGridLayout;
			colorsLay->addWidget(questLab, 0, 0, Qt::AlignCenter);
			colorsLay->addWidget(m_questColorBut, 0, 1, Qt::AlignCenter);
			colorsLay->addWidget(answLab, 1, 0, Qt::AlignCenter);
			colorsLay->addWidget(m_answColorBut, 1, 1, Qt::AlignCenter);
			colorsLay->addWidget(notBadLab, 2, 0, Qt::AlignCenter);
			colorsLay->addWidget(m_notBadButt, 2, 1, Qt::AlignCenter);
    commonLay->addLayout(colorsLay);
		commonGr->setLayout(commonLay);
		mainLay->addWidget(commonGr);
    mainLay->addStretch();
		QGroupBox *exerciseGr = new QGroupBox(tr("exercises"), this);
		QVBoxLayout *exerciseLay = new QVBoxLayout;
		QHBoxLayout *timeLay = new QHBoxLayout;
			timeLay->addWidget(timeLab);
			timeLay->addWidget(m_viewTimeSlider);
			timeLay->addWidget(m_timeLabel);
			exerciseLay->addLayout(timeLay);
			exerciseLay->addWidget(m_suggestExamChB, 0, Qt::AlignCenter);
		exerciseGr->setLayout(exerciseLay);
		mainLay->addWidget(exerciseGr);
		mainLay->addStretch();
		QGroupBox *examGr = new QGroupBox(tr("exams"), this);
		QVBoxLayout *examLay = new QVBoxLayout;
			examLay->addWidget(m_repeatIncorChB);
			examLay->addWidget(m_closeConfirmChB);
		examGr->setLayout(examLay);
		mainLay->addWidget(examGr);
		mainLay->addStretch();
    setLayout(mainLay);
    
    connect(m_expertAnswChB, SIGNAL(clicked(bool)), this, SLOT(expertAnswersChanged(bool)));
		connect(m_viewTimeSlider, SIGNAL(valueChanged(int)), this, SLOT(timePreviewChanged(int)));
}


void TexamSettings::saveSettings() {
    m_params->autoNextQuest = m_autoNextChB->isChecked();
    m_params->repeatIncorrect = m_repeatIncorChB->isChecked();
    m_params->expertsAnswerEnable = m_expertAnswChB->isChecked();
    m_params->studentName = m_nameEdit->text();
		m_params->closeWithoutConfirm = m_closeConfirmChB->isChecked();
		m_params->correctViewDuration = m_viewTimeSlider->value();
		m_params->showCorrected = m_correctChB->isChecked();
		m_params->suggestExam = m_suggestExamChB->isChecked();
        
    *m_qColor = m_questColorBut->getColor();
    m_qColor->setAlpha(40);
    *m_aColor = m_answColorBut->getColor();
    m_aColor->setAlpha(40);
    *m_nbColor = m_notBadButt->getColor();
    m_nbColor->setAlpha(40);
}


void TexamSettings::restoreDefaults() {
		m_autoNextChB->setChecked(true);
		m_repeatIncorChB->setChecked(true);
		m_expertAnswChB->setChecked(false);
		m_nameEdit->setText("");
		m_closeConfirmChB->setChecked(false);
		m_viewTimeSlider->setValue(2000);
		m_correctChB->setChecked(true);
		m_suggestExamChB->setChecked(true);
		m_questColorBut->setColor(QColor("red"));
		m_answColorBut->setColor(QColor("green"));
		m_notBadButt->setColor(QColor("#FF8000"));
}



void TexamSettings::expertAnswersChanged(bool enabled) {
  if (enabled) {
      if (!showExpertAnswersHelpDlg(this, &m_params->askAboutExpert, true))
        m_expertAnswChB->setChecked(false);
  }
}


void TexamSettings::timePreviewChanged(int val) {
		if (val < 2000) {
			m_viewTimeSlider->setValue(2000);
			val = 2000;
		}
		m_timeLabel->setText(QString("%1 ms").arg(val));
}



