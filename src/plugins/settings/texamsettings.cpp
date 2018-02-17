/***************************************************************************
 *   Copyright (C) 2011-2018 by Tomasz Bojczuk                             *
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


#include "texamsettings.h"
#include "tcolorbutton.h"
#include <tinitcorelib.h>
#include <texamparams.h>
#include <help/texpertanswerhelp.h>
#include <help/texamhelp.h>
#include <widgets/troundedlabel.h>
#include <QtWidgets/QtWidgets>


TexamSettings::TexamSettings(QWidget* parent, EsettingsMode mode) :
  TtouchArea(parent),
  m_params(Tcore::gl()->E),
  m_qColor(&Tcore::gl()->EquestionColor),
  m_aColor(&Tcore::gl()->EanswerColor),
  m_nbColor(&Tcore::gl()->EnotBadColor),
  m_mode(mode)
{
  m_correctChB = new QCheckBox(TexamHelp::correctMistakesTxt(), this);
    m_correctChB->setStatusTip(tr("When you will make mistake, the program will show you automatically how a correct answer should be."));
    m_correctChB->setChecked(m_params->showCorrected);
  m_correctPreviewSpin = new QSpinBox(this);
    m_correctPreviewSpin->setStatusTip(tr("Preview time of corrected answer when next question is asked automatically."));
    m_correctPreviewSpin->setMinimum(1500);
    m_correctPreviewSpin->setMaximum(6000);
    m_correctPreviewSpin->setValue(m_params->correctPreview);
    m_correctPreviewSpin->setSingleStep(500);
    m_correctPreviewSpin->setSuffix(" ms");
  m_questionDelaySpin = new QSpinBox(this);
    m_questionDelaySpin->setStatusTip(tr("Delay before every next question when it is asked automatically."));
    m_questionDelaySpin->setMaximum(1000);
    m_questionDelaySpin->setMinimum(50);
    m_questionDelaySpin->setSingleStep(50);
    m_questionDelaySpin->setValue(m_params->questionDelay);
    m_questionDelaySpin->setSuffix(" ms");
  m_mistakePreviewSpin = new QSpinBox(this);
    m_mistakePreviewSpin->setStatusTip(tr("A time of displaying incorrect answers."));
    m_mistakePreviewSpin->setMinimum(500);
    m_mistakePreviewSpin->setMaximum(6000);
    m_mistakePreviewSpin->setValue(m_params->mistakePreview);
    m_mistakePreviewSpin->setSingleStep(500);
    m_mistakePreviewSpin->setSuffix(" ms");
  m_suggestExamChB = new QCheckBox(tr("suggest an exam"), this);
    m_suggestExamChB->setStatusTip(tr("Watch exercising progress and when it is going well, suggest to start an exam on the exercise level."));
    m_suggestExamChB->setChecked(m_params->suggestExam);
  m_showDetectedChB = new QCheckBox(tr("show wrong played"), this);
    m_showDetectedChB->setStatusTip(tr("When answer was played (or sung) and it was wrong also the detected wrong note is shown."));
    m_showDetectedChB->setChecked(m_params->showWrongPlayed);
  m_waitForCorrectChB = new QCheckBox(tr("wait for correct note"), this);
    m_waitForCorrectChB->setStatusTip(tr("When melody is played from score, every note is checked immediately and next note can be played only when the previous one was correct. When unset, notes are checked only after playing the last one (or check button was clicked)."));
    m_waitForCorrectChB->setChecked(m_params->waitForCorrect);

  m_repeatIncorChB = new QCheckBox(tr("repeat a question"), this);
    m_repeatIncorChB->setChecked(m_params->repeatIncorrect);
    m_repeatIncorChB->setStatusTip(tr("A question with an incorrect answer will be asked once again."));
  m_closeConfirmChB = new QCheckBox(tr("close without confirm"), this);
    m_closeConfirmChB->setStatusTip(tr("If checked, an application will not ask to answer pending question just mark it as wrong, save an exam to file (in directory: %1) and close itself without any confirmation needed.").arg("<b>" + m_params->examsDir + "</b>"));
    m_closeConfirmChB->setChecked(m_params->closeWithoutConfirm);

  m_autoNextChB = new QCheckBox(TexamHelp::autoNextQuestTxt(), this);
    m_autoNextChB->setChecked(m_params->autoNextQuest);
  m_expertAnswChB = new QCheckBox(TexamHelp::expertsAnswerTxt(), this);
    m_expertAnswChB->setChecked(m_params->expertsAnswerEnable);
  auto *nameLab = new QLabel(tr("Student Name:"), this);
  m_nameEdit = new QLineEdit(m_params->studentName, this);
    m_nameEdit->setMaxLength(30);
    m_nameEdit->setStatusTip(tr("Default name for every new exam or exercise."));

  auto *questLab = new QLabel(tr("questions"), this);
  m_questColorBut = new TcolorButton(*(m_qColor), this);
  m_questColorBut->setStatusTip(tr("color of questions") + " + " + tr("color of wrong answers"));
  auto *answLab = new QLabel(tr("answers"), this);
  m_answColorBut = new TcolorButton(*(m_aColor), this);
  m_answColorBut->setStatusTip(tr("color of answers"));
  auto *notBadLab = new QLabel(tr("'not bad'"), this);
  m_notBadButt = new TcolorButton(*(m_nbColor), this);
  m_notBadButt->setStatusTip(tr("color of 'not bad' answers"));
  m_afterLab = new QLabel(tr("after mistake:"), this);
  m_contRadio = new QRadioButton(tr("continue"), this);
  m_waitRadio = new QRadioButton(tr("wait"), this);
  m_stopRadio = new QRadioButton(tr("stop"), this);
  auto *afterButGr = new QButtonGroup(this);
  afterButGr->addButton(m_contRadio);
  afterButGr->addButton(m_waitRadio);
  afterButGr->addButton(m_stopRadio);
  m_afterLab->setStatusTip(tr("When <b>'ask next question automatically'</b> is checked and you commit a mistake, the program can: <b>continue asking immediately</b>, <b>wait defined period of time</b> or <b>stop questioning</b>, to give you possibility to analyze what was wrong."));
  m_contRadio->setStatusTip(m_afterLab->statusTip());
  m_waitRadio->setStatusTip(m_afterLab->statusTip());
  m_stopRadio->setStatusTip(m_afterLab->statusTip());
  if (m_params->afterMistake == TexamParams::e_continue)
      m_contRadio->setChecked(true);
  else if (m_params->afterMistake == TexamParams::e_wait)
      m_waitRadio->setChecked(true);
  else
      m_stopRadio->setChecked(true);
  autoQuestionSlot(m_params->autoNextQuest);
  m_showNameChB = new QCheckBox(tr("extra names"), this);
    m_showNameChB->setStatusTip(tr("To improve association of note in the score or position on the guitar to note name, Nootka will display names even if neither question nor answer is related to it."));
    m_showNameChB->setChecked(m_params->showNameOfAnswered);

  auto *mainLay = new QVBoxLayout;
  auto *commonGr = new QGroupBox(this);
  auto *commonLay = new QVBoxLayout;
  auto *nameLay = new QHBoxLayout();
    nameLay->addStretch();
    nameLay->addWidget(nameLab);
    nameLay->addStretch();
    nameLay->addWidget(m_nameEdit);
    nameLay->addStretch();
  commonLay->addLayout(nameLay);
#if defined (Q_OS_ANDROID)
    commonLay->addWidget(getLabelFromStatus(m_nameEdit, false), Qt::AlignCenter);
#endif
  auto *expertChBoxesLay = new QVBoxLayout;
    auto *corrLay = new QHBoxLayout;
      corrLay->addWidget(m_correctChB);
      corrLay->addSpacing(15);
      corrLay->addWidget(m_correctPreviewSpin);
      corrLay->addStretch();
    expertChBoxesLay->addLayout(corrLay);
#if defined (Q_OS_ANDROID)
    expertChBoxesLay->addWidget(getLabelFromStatus(m_correctChB, true, true));
#endif
    auto *autoLay = new QHBoxLayout;
      autoLay->addWidget(m_autoNextChB);
      autoLay->addSpacing(15);
      autoLay->addWidget(m_questionDelaySpin);
      autoLay->addStretch();
    expertChBoxesLay->addLayout(autoLay);
#if defined (Q_OS_ANDROID)
    expertChBoxesLay->addWidget(getLabelFromStatus(m_questionDelaySpin, true, true));
#endif
  auto *afterLay = new QHBoxLayout;
    afterLay->addStretch(0);
    afterLay->addWidget(m_afterLab);
    afterLay->addStretch(0);
    afterLay->addWidget(m_contRadio);
    afterLay->addStretch(0);
    afterLay->addWidget(m_waitRadio);
    afterLay->addWidget(m_mistakePreviewSpin);
    afterLay->addStretch(0);
    afterLay->addWidget(m_stopRadio);
    afterLay->addStretch(0);
    expertChBoxesLay->addLayout(afterLay);
#if defined (Q_OS_ANDROID)
    expertChBoxesLay->addWidget(getLabelFromStatus(m_afterLab, true, true));
#endif
    expertChBoxesLay->addWidget(m_expertAnswChB, 0, Qt::AlignLeft);
  QHBoxLayout *expertLay = new QHBoxLayout;
    expertLay->addStretch();
    expertLay->addLayout(expertChBoxesLay);
    expertLay->addStretch();
  commonLay->addLayout(expertLay);

    commonLay->addStretch();
// 		commonLay->addWidget(m_showNameChB, 0, Qt::AlignCenter);
  auto *colorsGr = new QGroupBox(tr("colors"), this);
  auto *colorsLay = new QHBoxLayout;
    colorsLay->addWidget(questLab);
    colorsLay->addWidget(m_questColorBut);
    colorsLay->addSpacing(10);
    colorsLay->addStretch();
    colorsLay->addWidget(answLab);
    colorsLay->addWidget(m_answColorBut);
    colorsLay->addSpacing(10);
    colorsLay->addStretch();
    colorsLay->addWidget(notBadLab);
    colorsLay->addWidget(m_notBadButt);
  colorsGr->setLayout(colorsLay);
  commonLay->addWidget(colorsGr);
  commonGr->setLayout(commonLay);
  mainLay->addWidget(commonGr);
  mainLay->addStretch();
  auto *exerciseGr = new QGroupBox(tr("exercises"), this);
#if defined (Q_OS_ANDROID)
    auto *exerciseLay = new QVBoxLayout;
#else
    auto *exerciseLay = new QGridLayout;
#endif
//     exerciseLay->addStretch();
//     exerciseLay->addWidget(m_suggestExamChB);
#if defined (Q_OS_ANDROID)
    exerciseLay->addWidget(m_suggestExamChB);
    exerciseLay->addWidget(getLabelFromStatus(m_suggestExamChB, true, true));
#else
    exerciseLay->addWidget(m_suggestExamChB, 0, 0);
#endif
//     exerciseLay->addStretch();
//     exerciseLay->addWidget(m_showDetectedChB);
#if defined (Q_OS_ANDROID)
    exerciseLay->addWidget(m_showDetectedChB);
    exerciseLay->addWidget(getLabelFromStatus(m_showDetectedChB, true, true));
#else
    exerciseLay->addWidget(m_showDetectedChB, 0, 1);
#endif
//     exerciseLay->addStretch();
//     exerciseLay->addWidget(m_showNameChB);
#if defined (Q_OS_ANDROID)
    exerciseLay->addWidget(m_showNameChB);
    exerciseLay->addWidget(getLabelFromStatus(m_showNameChB, true, true));
#else
    exerciseLay->addWidget(m_showNameChB, 1, 0);
#endif
#if defined (Q_OS_ANDROID)
    exerciseLay->addWidget(m_waitForCorrectChB);
    exerciseLay->addWidget(getLabelFromStatus(m_waitForCorrectChB, true, true));
#else
    exerciseLay->addWidget(m_waitForCorrectChB, 1, 1);
#endif
//     exerciseLay->addStretch();
  exerciseGr->setLayout(exerciseLay);
  mainLay->addWidget(exerciseGr);
  mainLay->addStretch();
  auto *examGr = new QGroupBox(tr("exams"), this);
#if defined (Q_OS_ANDROID)
  auto *examLay = new QVBoxLayout;
#else
  auto *examLay = new QHBoxLayout;
#endif
    examLay->addStretch();
    examLay->addWidget(m_repeatIncorChB);
#if defined (Q_OS_ANDROID)
    examLay->addWidget(getLabelFromStatus(m_repeatIncorChB, true, true));
#endif
    examLay->addStretch();
    examLay->addWidget(m_closeConfirmChB);
#if defined (Q_OS_ANDROID)
    examLay->addWidget(getLabelFromStatus(m_closeConfirmChB, true, true));
#endif
    examLay->addStretch();
  examGr->setLayout(examLay);
  mainLay->addWidget(examGr);
  mainLay->addStretch();
  setLayout(mainLay);

	if (m_mode != e_settings) {
		colorsGr->hide();
		nameLab->hide();
		m_nameEdit->hide();
		if (m_mode == e_exam) {
			exerciseGr->hide();
			m_correctChB->hide();
			m_correctPreviewSpin->hide();
		} else if (m_mode == e_exercise) {
			examGr->hide();
		}
	}

  connect(m_expertAnswChB, SIGNAL(clicked(bool)), this, SLOT(expertAnswersChanged(bool)));
  connect(m_autoNextChB, SIGNAL(clicked(bool)), this, SLOT(autoQuestionSlot(bool)));
}


void TexamSettings::saveSettings() {
  m_params->autoNextQuest = m_autoNextChB->isChecked();
  m_params->repeatIncorrect = m_repeatIncorChB->isChecked();
  m_params->expertsAnswerEnable = m_expertAnswChB->isChecked();
  m_params->studentName = m_nameEdit->text();
  m_params->closeWithoutConfirm = m_closeConfirmChB->isChecked();
  m_params->mistakePreview = m_mistakePreviewSpin->value();
  m_params->correctPreview = m_correctPreviewSpin->value();
  m_params->questionDelay = m_questionDelaySpin->value();
  m_params->showCorrected = m_correctChB->isChecked();
  m_params->suggestExam = m_suggestExamChB->isChecked();
  m_params->showNameOfAnswered = m_showNameChB->isChecked();
  m_params->showWrongPlayed = m_showDetectedChB->isChecked();
  m_params->waitForCorrect = m_waitForCorrectChB->isChecked();
  if (m_contRadio->isChecked())
    m_params->afterMistake = TexamParams::e_continue;
  else if (m_waitRadio->isChecked())
    m_params->afterMistake = TexamParams::e_wait;
  else
    m_params->afterMistake = TexamParams::e_stop;

  *m_qColor = m_questColorBut->getColor();
  m_qColor->setAlpha(40);
  *m_aColor = m_answColorBut->getColor();
  m_aColor->setAlpha(40);
  *m_nbColor = m_notBadButt->getColor();
  m_nbColor->setAlpha(40);
}


void TexamSettings::restoreDefaults() {
  m_autoNextChB->setChecked(false);
  m_repeatIncorChB->setChecked(true);
  m_expertAnswChB->setChecked(false);
  m_nameEdit->setText("");
  m_closeConfirmChB->setChecked(false);
  m_mistakePreviewSpin->setValue(3000);
  m_correctPreviewSpin->setValue(3000);
  m_questionDelaySpin->setValue(150);
  m_correctChB->setChecked(true);
  m_suggestExamChB->setChecked(true);
  m_waitForCorrectChB->setChecked(true);
  m_questColorBut->setColor(QColor("red"));
  m_answColorBut->setColor(QColor("green"));
  m_notBadButt->setColor(QColor("#FF8000"));
  m_contRadio->setChecked(true);
  m_showNameChB->setChecked(false);
  m_showDetectedChB->setChecked(false);
}


void TexamSettings::expertAnswersChanged(bool enabled) {
  if (enabled) {
    if (!showExpertAnswersHelpDlg(this, &m_params->askAboutExpert, true))
      m_expertAnswChB->setChecked(false);
  }
}


//##########################################################################################
//#######################     PROTECTED       ##############################################
//##########################################################################################

void TexamSettings::autoQuestionSlot(bool state) {
	m_contRadio->setDisabled(!state);
	m_waitRadio->setDisabled(!state);
	m_stopRadio->setDisabled(!state);
	m_mistakePreviewSpin->setDisabled(!state);
  m_afterLab->setDisabled(!state);
  m_correctPreviewSpin->setDisabled(!state);
  m_questionDelaySpin->setDisabled(!state);
}




