	/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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


#include "questionssettings.h"
#include "tquestionaswdg.h"
#include "tkeysigncombobox.h"
#include <tintonationview.h>
#include <QtGui>

extern bool isNotSaved;


questionsSettings::questionsSettings(QWidget *parent) :
    QWidget(parent),
    m_levelIsLoading(false)
{
    QVBoxLayout *mainLay = new QVBoxLayout;    
    mainLay->addStretch();

    m_tableWdg = new QWidget(this);
    QHBoxLayout *tabLay = new QHBoxLayout;
    tabLay->addStretch();
    tabLay->addWidget(m_tableWdg);
    tabLay->addStretch();
    QGridLayout *qaLay = new QGridLayout(); // Questions & Answers table
    qaLay->setAlignment(Qt::AlignCenter);
    qaLay->setSpacing(10);
  // Labels describing answers types
		QFont f = font();
    f.setBold(true);
		QLabel *newQuestLab = new QLabel(TquestionAsWdg::answerTxt().toUpper(), this);
		newQuestLab->setFont(f);
		qaLay->addWidget(newQuestLab, 0, 2, 0, 4, Qt::AlignHCenter | Qt::AlignTop);
    m_questLab = new QLabel(TquestionAsWdg::questionTxt().toUpper(), this);
		m_questLab->setFont(f);
    qaLay->addWidget(m_questLab, 1, 0, Qt::AlignBottom | Qt::AlignHCenter);
    m_answLab = new QLabel("  ", this);
    m_answLab->setFont(f);
    qaLay->addWidget(m_answLab, 1, 1, Qt::AlignBottom);
    QLabel *asNoteLab = new QLabel(" <br>" + TquestionAsWdg::asNoteTxt().replace(" ", "<br>"), this);
    asNoteLab->setAlignment(Qt::AlignCenter);
    qaLay->addWidget(asNoteLab, 1, 2, Qt::AlignBottom);
    QLabel *asNameLab = new QLabel(" <br>" + TquestionAsWdg::asNameTxt().replace(" ", "<br>"), this);
    asNameLab->setAlignment(Qt::AlignCenter);
    qaLay->addWidget(asNameLab, 1, 3, Qt::AlignBottom);
    m_asFretLab = new QLabel(" <br>" + TquestionAsWdg::asFretPosTxt().replace(" ", "<br>"), this);
    m_asFretLab->setAlignment(Qt::AlignCenter);
    qaLay->addWidget(m_asFretLab, 1, 4, Qt::AlignBottom);
    m_asSoundLab = new QLabel(" <br>" + TquestionAsWdg::asSoundTxt().replace(" ", "<br>"), this);
    m_asSoundLab->setAlignment(Qt::AlignCenter);
    qaLay->addWidget(m_asSoundLab, 1, 5, Qt::AlignBottom);
  // CheckBoxes with types of answers for every kind of question
    asNoteWdg = new TquestionAsWdg(TQAtype::e_asNote, qaLay, 2, this);
    asNameWdg = new TquestionAsWdg(TQAtype::e_asName, qaLay, 3, this);
    asFretPosWdg = new TquestionAsWdg(TQAtype::e_asFretPos, qaLay, 4, this);
    asSoundWdg = new TquestionAsWdg(TQAtype::e_asSound, qaLay, 5, this);
  // Labels on the right side of the table with symbols of types - related to questions
    QLabel *scoreNooLab = new QLabel("s?", this);
    QFont nf("nootka", fontMetrics().boundingRect("A").height());
#if defined(Q_OS_MACX)
    nf.setPointSize(fontMetrics().boundingRect("A").height() * 2);
#endif
    scoreNooLab->setFont(nf);
    qaLay->addWidget(scoreNooLab, 2, 6, Qt::AlignCenter);
    QLabel *nameNooLab = new QLabel("c?", this);
    nameNooLab->setFont(nf);
    qaLay->addWidget(nameNooLab, 3, 6, Qt::AlignCenter);
    m_guitarNooLab = new QLabel("g?", this);
    m_guitarNooLab->setFont(nf);
    qaLay->addWidget(m_guitarNooLab, 4, 6, Qt::AlignCenter);
    m_soundNooLab = new QLabel("n?", this);
    m_soundNooLab->setFont(nf);
    qaLay->addWidget(m_soundNooLab, 5, 6);
  // Labels on the bottom side of the table with symbols of types - related to answers  
    QLabel *qScoreNooLab = new QLabel("s!", this);
    qScoreNooLab->setFont(nf);
    qaLay->addWidget(qScoreNooLab, 6, 2, Qt::AlignCenter);
    QLabel *qNmeNooLab = new QLabel("c!", this);
    qNmeNooLab->setFont(nf);
    qaLay->addWidget(qNmeNooLab, 6, 3, Qt::AlignCenter);
    m_qGuitarNooLab = new QLabel("g!", this);
    m_qGuitarNooLab->setFont(nf);
    qaLay->addWidget(m_qGuitarNooLab, 6, 4, Qt::AlignCenter);
    m_qSoundNooLab = new QLabel("n!", this);
    m_qSoundNooLab->setFont(nf);
    qaLay->addWidget(m_qSoundNooLab, 6, 5);
    
    m_tableWdg->setLayout(qaLay);
    mainLay->addLayout(tabLay);
    mainLay->addStretch();
    
  // some checkBoxes
    QGridLayout *chLay = new QGridLayout;
    octaveRequiredChB = new QCheckBox(tr("require octave"),this);
    octaveRequiredChB->setStatusTip(tr("if checked, selecting of valid octave is required"));
    chLay->addWidget(octaveRequiredChB, 0, 0, Qt::AlignLeft);
    
    forceAccChB = new QCheckBox(tr("force using appropriate accidental"),this);
    forceAccChB->setStatusTip(tr("if checked, it is possible to select a note<br>with given accidental only."));
    chLay->addWidget(forceAccChB, 1, 0, Qt::AlignLeft);
    
    styleRequiredChB = new QCheckBox(tr("use different naming styles"),this);
    styleRequiredChB->setStatusTip(tr("if checked, note names are switched between letters and solfege."));
    chLay->addWidget(styleRequiredChB, 2, 0, Qt::AlignLeft);
    
    showStrNrChB = new QCheckBox(tr("show string number in questions"), this);
    showStrNrChB->setStatusTip(tr("Shows on which string an answer has to be given.<br>Be careful, sometimes it is needed and sometimes it makes no sense."));
    chLay->addWidget(showStrNrChB, 0, 1, Qt::AlignLeft);
    mainLay->addLayout(chLay);
    
    lowPosOnlyChBox = new QCheckBox(tr("notes in the lowest position only"),this);
    lowPosOnlyChBox->setStatusTip(tr("if checked, the lowest position in selected fret range is required,<br>otherwise all possible positions of the note are acceptable.<br>To use this, all strings have to be available!"));
    chLay->addWidget(lowPosOnlyChBox, 1, 1, Qt::AlignLeft);
    
    currKeySignChBox = new QCheckBox(tr("notes in current key signature only"),this);
    currKeySignChBox->setStatusTip(tr("Only notes from current key signature are taken.<br>If key signature is disabled accidentals are not used."));
    chLay->addWidget(currKeySignChBox, 2, 1, Qt::AlignLeft);
		
		TintonationCombo *intoCombo = new TintonationCombo(this);
		m_intonationCombo = intoCombo->accuracyCombo; // we need only combo box (label is not necessary)
		mainLay->addWidget(intoCombo, 0, Qt::AlignCenter);
      
    setLayout(mainLay);

    connect(asNoteWdg, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
    connect(asNameWdg, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
    connect(asFretPosWdg, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
    connect(asSoundWdg, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
    
    connect(octaveRequiredChB , SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(forceAccChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(styleRequiredChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(showStrNrChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(lowPosOnlyChBox, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
    connect(currKeySignChBox, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
		connect(m_intonationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(whenParamsChanged()));
}


void questionsSettings::loadLevel(Tlevel& level) {
	m_levelIsLoading = true;
    asNoteWdg->setAnswers(level.answersAs[TQAtype::e_asNote]);
    asNoteWdg->setChecked(level.questionAs.isNote()); // when it is false it cleans all checkBoxes to false
    asNameWdg->setAnswers(level.answersAs[TQAtype::e_asName]);
    asNameWdg->setChecked(level.questionAs.isName());
    asFretPosWdg->setAnswers(level.answersAs[TQAtype::e_asFretPos]);
    asFretPosWdg->setChecked(level.questionAs.isFret());
    asSoundWdg->setAnswers(level.answersAs[TQAtype::e_asSound]);
    asSoundWdg->setChecked(level.questionAs.isSound());
    
    octaveRequiredChB->setChecked(level.requireOctave);
    forceAccChB->setChecked(level.forceAccids);
    styleRequiredChB->setChecked(level.requireStyle);
    showStrNrChB->setChecked(level.showStrNr);
    lowPosOnlyChBox->setChecked(level.onlyLowPos);
    currKeySignChBox->setChecked(level.onlyCurrKey);
		m_intonationCombo->setCurrentIndex(level.intonation);
	m_levelIsLoading = false;
}


void questionsSettings::whenParamsChanged() {
	if (m_levelIsLoading)
		return;
	
  // disable showStrNrChB & lowPosOnlyChBox  if question and answer are on guitar
    if (asFretPosWdg->isChecked() && asFretPosWdg->answerAsPos()) {
      showStrNrChB->setChecked(true);
      showStrNrChB->setDisabled(true);
      lowPosOnlyChBox->setChecked(false);
      lowPosOnlyChBox->setDisabled(true);
    } else {
        showStrNrChB->setDisabled(false);
        lowPosOnlyChBox->setDisabled(false);
    }      
  // Is score enabled in a level
    if (!asNoteWdg->isChecked() && !asNameWdg->answerAsNote() && !asFretPosWdg->answerAsNote() && !asSoundWdg->answerAsNote())
        emit scoreEnabled(false);
    else
        emit scoreEnabled(true);
  // Are score and names enabled
    if (!asNameWdg->isChecked() && !asNoteWdg->answerAsName() && !asFretPosWdg->answerAsName() && 
          !asSoundWdg->answerAsName() && !asNoteWdg->isChecked() && !asNameWdg->answerAsNote() && 
          !asFretPosWdg->answerAsNote() && !asSoundWdg->answerAsNote()) {
              emit accidEnabled(false);    
    }
    else
          emit accidEnabled(true);
	// Is sound input enabled to enable intonation
		if (asNoteWdg->answerAsSound() || asNameWdg->answerAsSound() || asFretPosWdg->answerAsSound() || asSoundWdg->answerAsSound())
				m_intonationCombo->setDisabled(false);
		else
				m_intonationCombo->setDisabled(true);
    if (!isNotSaved) {
        isNotSaved = true;
        emit questSettChanged();
    }
}


void questionsSettings::hideGuitarRelated() {
		asFretPosWdg->enableChBox->hide();
		asFretPosWdg->asNoteChB->hide();
		asFretPosWdg->asNameChB->hide();
		asFretPosWdg->asFretPosChB->hide();
		asFretPosWdg->asSoundChB->hide();
		asFretPosWdg->questLab->hide();
		asNoteWdg->asFretPosChB->hide();
		asNameWdg->asFretPosChB->hide();
		asSoundWdg->asFretPosChB->hide();
		showStrNrChB->hide();
		lowPosOnlyChBox->hide();
		m_asFretLab->hide();
		m_qGuitarNooLab->hide();
		m_guitarNooLab->hide();
}


void questionsSettings::saveLevel(Tlevel &level) {
    level.questionAs.setAsNote(asNoteWdg->isChecked());
    level.answersAs[TQAtype::e_asNote] = asNoteWdg->getAnswers();
    level.questionAs.setAsName(asNameWdg->isChecked());
    level.answersAs[TQAtype::e_asName] = asNameWdg->getAnswers();
    level.questionAs.setAsFret(asFretPosWdg->isChecked());
    level.answersAs[TQAtype::e_asFretPos] = asFretPosWdg->getAnswers();
    level.questionAs.setAsSound(asSoundWdg->isChecked());
    level.answersAs[TQAtype::e_asSound] = asSoundWdg->getAnswers();
    
    level.requireOctave = octaveRequiredChB->isChecked();
    level.forceAccids = forceAccChB->isChecked();
    level.requireStyle = styleRequiredChB->isChecked();
    level.showStrNr = showStrNrChB->isChecked();
    level.onlyLowPos = lowPosOnlyChBox->isChecked();
    level.onlyCurrKey = currKeySignChBox->isChecked();
		level.intonation = m_intonationCombo->currentIndex();
}


void questionsSettings::paintEvent(QPaintEvent* ) {
  QPainter painter(this);
  QPen pen = painter.pen();
  pen.setColor(palette().text().color());
  pen.setWidth(2);
  painter.setPen(pen);
  int vertLineUpY = m_tableWdg->geometry().y() + m_questLab->geometry().bottom() + 
      (asNoteWdg->enableChBox->geometry().top() - m_questLab->geometry().bottom()) / 2;
  painter.drawLine(m_tableWdg->geometry().left(), vertLineUpY, m_tableWdg->geometry().right(), vertLineUpY);
  int vertLineDownY = m_tableWdg->geometry().y() +  asSoundWdg->enableChBox->geometry().bottom() + 
      (m_qSoundNooLab->geometry().top() - asSoundWdg->enableChBox->geometry().bottom()) / 2;
  painter.drawLine(m_tableWdg->geometry().left(), vertLineDownY, m_tableWdg->geometry().right(), vertLineDownY);
  int horLineLeftX = m_tableWdg->geometry().x() + asNoteWdg->enableChBox->geometry().right() + 6;
  painter.drawLine(horLineLeftX, m_tableWdg->geometry().top(), horLineLeftX, m_tableWdg->geometry().bottom());
  int horLineRightX = m_tableWdg->geometry().x() + m_asSoundLab->geometry().right() + 
      (m_soundNooLab->geometry().left() - m_asSoundLab->geometry().right()) / 2;
  painter.drawLine(horLineRightX , m_tableWdg->geometry().top(), horLineRightX, m_tableWdg->geometry().bottom());
}


void questionsSettings::stringsCheckedSlot(bool checked) {
  if (checked)
    lowPosOnlyChBox->setDisabled(false);
  else {
    lowPosOnlyChBox->setChecked(false);
    lowPosOnlyChBox->setDisabled(true);    
  }
}




