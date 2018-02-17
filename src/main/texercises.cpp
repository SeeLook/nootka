/***************************************************************************
 *   Copyright (C) 2013-2018 by Tomasz Bojczuk                             *
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

#include "texercises.h"
#include <tglobals.h>
#include <exam/texam.h>
#include <QtWidgets/qradiobutton.h>
#include <QtWidgets/qbuttongroup.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qstyle.h>
#include <QtWidgets/qboxlayout.h>
#include <QtWidgets/qgroupbox.h>
#include <QtWidgets/qlabel.h>


Texercises::Texercises(Texam* exam, QObject* parent) :
  QObject(parent),
  m_exam(exam)
{
  m_exam->setExercise();
}


void Texercises::setSuggestionEnabled(int qaPosibilities, bool melody) {
  if (qaPosibilities > 0) {
      m_max = qMax(qaPosibilities, melody ? 5 : 10); // not less than 10 or 5 if melodies
      m_checkNow = true;
      m_checkInFuture = true; // global setting is true when it comes here
      m_currentGood = 0;
      m_prevMistake = 0;
  } else {
      m_checkNow = false;
      m_checkInFuture = false;
  }
}


void Texercises::checkAnswer() {
  if (!m_checkNow || !m_exam->curQ()->answered())
    return;

  if (m_exam->curQ()->isCorrect()) {
      m_currentGood++;
      m_prevMistake++;
  } else {
      if (m_currentGood > m_max / 2 && m_prevMistake > m_max / 2) // when there were enough good answers
        m_currentGood = m_max / 2; // forgive single mistake
      else
        m_currentGood = 0;
      m_prevMistake = 0; // but reset mistake counter, so after next wrong answer whole cycle will be required
  }

  if (m_currentGood >= m_max) {
    /** Show suggestion dialog when amount of proper answers is sufficient. */
    TsuggestExam *suggExam = new TsuggestExam();
    emit messageDisplayed();
    TsuggestExam::Esuggest what = suggExam->suggest(GLOB->waitForCorrect() && m_exam->melodies());
    if (suggExam->result() == QDialog::Accepted) {
        switch(what) {
          case TsuggestExam::e_readyToExam:
            m_readyToExam = true;
            break;
          case TsuggestExam::e_forAmoment:
            m_currentGood = 0;
            break;
          case TsuggestExam::e_checkEntireMelody:
            GLOB->setWaitForCorrect(false);
            m_currentGood = 0;
            break;
          case TsuggestExam::e_notThisTime:
            m_checkNow = false;
            break;
          case TsuggestExam::e_neverEver:
            m_checkInFuture = false;
            m_checkNow = false;
            break;
        }
    } else
        m_currentGood = 0;
    delete suggExam;
    emit messageClosed(m_readyToExam);
  }
}


//####################################################################################################################
//##################################### TsuggestExam #################################################################
//####################################################################################################################
TsuggestExam::TsuggestExam() :
  QDialog()
{
#if defined (Q_OS_ANDROID)
  showMaximized();
#else
  setWindowTitle(tr("Start an exam"));
#endif
  auto mainLab = new QLabel(QStringLiteral("<h3>") +
  tr("You are very good in this exercise!<br>Would you like to pass an exam on the same level and got a certificate?") +
  QStringLiteral("</h3>"), this);
  mainLab->setAlignment(Qt::AlignCenter);
  m_readyExamRadio = new QRadioButton(tr("Sure! Lets start an exam!"), this);
  m_notNowRadio = new QRadioButton(tr("Not now, ask me for a moment."), this);
  m_checkEntireRadio = new QRadioButton(tr("No thanks, but check entire melody from now."), this);
  m_notThisExRadio = new QRadioButton(tr("Not in this exercise."), this);
  m_neverAskRadio = new QRadioButton(tr("Never more suggest me to start an exam!"), this);

  QButtonGroup *buttonGr = new QButtonGroup(this);
  buttonGr->addButton(m_readyExamRadio);
  buttonGr->addButton(m_notNowRadio);
  buttonGr->addButton(m_checkEntireRadio);
  buttonGr->addButton(m_notThisExRadio);
  buttonGr->addButton(m_neverAskRadio);
  m_readyExamRadio->setChecked(true);

  QPushButton *okButt = new QPushButton(tr("OK"), this);
  okButt->setIcon(QIcon(style()->standardIcon(QStyle::SP_DialogOkButton)));

  QVBoxLayout *lay = new QVBoxLayout;
  lay->addWidget(mainLab);
  QGroupBox *radioBox = new QGroupBox(this);
  QVBoxLayout *radioLay = new QVBoxLayout;
    radioLay->addWidget(m_checkEntireRadio, 1, Qt::AlignCenter);
    radioLay->addWidget(m_readyExamRadio, 1, Qt::AlignCenter);
    radioLay->addWidget(m_notNowRadio, 1, Qt::AlignCenter);
    radioLay->addWidget(m_notThisExRadio, 1, Qt::AlignCenter);
    radioLay->addWidget(m_neverAskRadio, 1, Qt::AlignCenter);
  radioBox->setLayout(radioLay);
  lay->addWidget(radioBox);
  lay->addStretch(1);
  lay->addWidget(okButt, 1, Qt::AlignCenter);
  setLayout(lay);

  connect(okButt, SIGNAL(clicked()), this, SLOT(accept()));
}


TsuggestExam::Esuggest TsuggestExam::suggest(bool entireVisible) {
  m_checkEntireRadio->setVisible(entireVisible);
  exec();
  if (m_readyExamRadio->isChecked())
    return e_readyToExam;
  if (m_notNowRadio->isChecked())
    return e_forAmoment;
  if (m_checkEntireRadio->isChecked())
    return e_checkEntireMelody;
  if (m_notThisExRadio->isChecked())
    return e_notThisTime;
  if (m_neverAskRadio->isChecked()) {
    return e_neverEver;
  }
  return e_readyToExam;
}









