/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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


#include "questionssettings.h"
#include "tkeysigncombobox.h"
#include <widgets/tquestionaswdg.h>
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <tnoofont.h>
#include <tmtr.h>
#include <widgets/tintonationview.h>
#include <QtWidgets/QtWidgets>


// TODO make order with layout code!!!
// It works fine: more horizontal for desktop and more vertical for mobile
// but it is hard to read and understand

questionsSettings::questionsSettings(TlevelCreatorDlg* creator) :
    TabstractLevelPage(creator)
{
	QVBoxLayout *mainLay = new QVBoxLayout;
	mainLay->addStretch();

	int nootFontSize = fontMetrics().boundingRect("A").height() * 2;
	QString nooColor = QString("color: %1").arg(palette().highlight().color().name());
	m_tableWdg = new QWidget(this);
	QHBoxLayout *tabLay = new QHBoxLayout;
	tabLay->addWidget(m_tableWdg);
	QGridLayout *qaLay = new QGridLayout(); // Questions & Answers table
	qaLay->setAlignment(Qt::AlignCenter);
	qaLay->setSpacing(10);
// Labels describing answers types
	QFont f = font();
	f.setBold(true);
	QLabel *newQuestLab = new QLabel(TquestionAsWdg::answerTxt().toUpper(), this);
	newQuestLab->setFont(f);
	qaLay->addWidget(newQuestLab, 0, 2, 0, 4, Qt::AlignHCenter | Qt::AlignTop);
	m_questLab = new QLabel(TnooFont::span("n", nootFontSize * 1.5, nooColor) + "<br><br>" + TquestionAsWdg::questionTxt().toUpper(), this);
	m_questLab->setAlignment(Qt::AlignCenter);
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
#elif defined (Q_OS_ANDROID)
//   nf.setPointSize(qMax<int>(Tmtr::fingerPixels() / 3, fontMetrics().boundingRect("A").height()));
  nf.setPointSize(Tmtr::fingerPixels() / 3);
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
  m_paintHandler = new TpaintHandler(widget());
  m_paintHandler->setLayout(tabLay);
	m_singleGr = new QGroupBox(tr("single note"), this);
			m_singleGr->setCheckable(true);
      QVBoxLayout *outTabLay = new QVBoxLayout;
      outTabLay->addWidget(m_paintHandler);
      outTabLay->setContentsMargins(0, 0, 0, 0);
			m_singleGr->setLayout(outTabLay);


#if defined (Q_OS_ANDROID)
    auto androidMelLay = new QHBoxLayout;
    melodyLab->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    androidMelLay->addWidget(melodyLab);
    androidMelLay->addLayout(melLay);
    m_melodiesGr->setLayout(androidMelLay);
    QVBoxLayout *grBoxLay = new QVBoxLayout;
#else
    auto grBoxLay = new QHBoxLayout;
#endif
		grBoxLay->addStretch();
		grBoxLay->addWidget(m_singleGr);
		grBoxLay->addStretch();
		grBoxLay->addStretch();

	mainLay->addLayout(grBoxLay);
	mainLay->addStretch();

// some checkBoxes
#if defined (Q_OS_ANDROID)
  auto chLay = new QVBoxLayout;
#else
	auto chLay = new QGridLayout;
#endif
	octaveRequiredChB = new QCheckBox(tr("require octave"),this);
	octaveRequiredChB->setStatusTip(tr("if checked, selecting of valid octave is required"));
#if defined (Q_OS_ANDROID)
  chLay->addWidget(octaveRequiredChB, 0, Qt::AlignLeft);
  chLay->addWidget(getLabelFromStatus(octaveRequiredChB, true, true));
#else
	chLay->addWidget(octaveRequiredChB, 0, 0, Qt::AlignLeft);
#endif

	styleRequiredChB = new QCheckBox(tr("use different naming styles"),this);
	styleRequiredChB->setStatusTip(tr("if checked, note names are switched between letters and solfege."));
#if defined (Q_OS_ANDROID)
  chLay->addWidget(styleRequiredChB, 0, Qt::AlignLeft);
  chLay->addWidget(getLabelFromStatus(styleRequiredChB, true, true));
#else
	chLay->addWidget(styleRequiredChB, 1, 0, Qt::AlignLeft);
#endif

	showStrNrChB = new QCheckBox(tr("show string number in questions"), this);
	showStrNrChB->setStatusTip(
      tr("Shows on which string an answer has to be given.<br>Be careful, sometimes it is needed and sometimes it makes no sense."));
#if defined (Q_OS_ANDROID)
  chLay->addWidget(showStrNrChB, 0, Qt::AlignLeft);
  chLay->addWidget(getLabelFromStatus(showStrNrChB, true, true));
#else
	chLay->addWidget(showStrNrChB, 0, 1, Qt::AlignLeft);
#endif

  lowPosOnlyChBox = new QCheckBox(tr("notes in the lowest position only"),this);
  lowPosOnlyChBox->setStatusTip(tr("if checked, the lowest position in selected fret range is required,<br>otherwise all possible positions of the note are acceptable.<br>To use this, all strings have to be available!"));
#if defined (Q_OS_ANDROID)
  chLay->addWidget(lowPosOnlyChBox, 0, Qt::AlignLeft);
  chLay->addWidget(getLabelFromStatus(lowPosOnlyChBox, true, true));
#else
  chLay->addWidget(lowPosOnlyChBox, 1, 1, Qt::AlignLeft);
#endif

	mainLay->addLayout(chLay);

	TintonationCombo *intoCombo = new TintonationCombo(this);
	m_intonationCombo = intoCombo->accuracyCombo; // we need only combo box (label is not necessary)
	mainLay->addWidget(intoCombo, 0, Qt::AlignCenter);

#if defined (Q_OS_ANDROID) // add space at the bottom to allow touching lower boxes and combo
  mainLay->addSpacing(Tmtr::fingerPixels() / 2);
  mainLay->setContentsMargins(0, 10, 0, 10);
#endif
	setLayout(mainLay);

  connect(m_singleGr, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
	connect(asNoteWdg, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
	connect(asNameWdg, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
	connect(asFretPosWdg, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
	connect(asSoundWdg, SIGNAL(answerStateChanged()), this, SLOT(whenParamsChanged()));
	
	connect(octaveRequiredChB , SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
	connect(styleRequiredChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
	connect(showStrNrChB, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
	connect(lowPosOnlyChBox, SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
	connect(m_intonationCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(whenParamsChanged()));
	connect(m_paintHandler, &TpaintHandler::paintMe, this, &questionsSettings::paintSlot);
}

//#################################################################################################
//###################                PUBLIC            ############################################
//#################################################################################################

void questionsSettings::loadLevel(Tlevel* level) {
  blockSignals(true);
    asNoteWdg->setAnswers(level->answersAs[TQAtype::e_asNote]);
    asNoteWdg->setChecked(level->questionAs.isNote()); // when it is false it cleans all checkBoxes to false
    asNameWdg->setAnswers(level->answersAs[TQAtype::e_asName]);
    asNameWdg->setChecked(level->questionAs.isName());
    asFretPosWdg->setAnswers(level->answersAs[TQAtype::e_asFretPos]);
    asFretPosWdg->setChecked(level->questionAs.isFret());
    asSoundWdg->setAnswers(level->answersAs[TQAtype::e_asSound]);
    asSoundWdg->setChecked(level->questionAs.isSound());

    octaveRequiredChB->setChecked(level->requireOctave);
    styleRequiredChB->setChecked(level->requireStyle);
    showStrNrChB->setChecked(level->showStrNr);
    lowPosOnlyChBox->setChecked(level->onlyLowPos);
    m_intonationCombo->setCurrentIndex(level->intonation);
    saveLevel(wLevel());
    m_singleGr->setChecked(!wLevel()->canBeMelody());
  blockSignals(false);
  adjustToLevel();
}


void questionsSettings::saveLevel(Tlevel* level) {
  level->questionAs.setAsNote(asNoteWdg->isChecked());
  level->answersAs[TQAtype::e_asNote] = asNoteWdg->getAnswers();
  level->questionAs.setAsName(asNameWdg->isChecked());
  level->answersAs[TQAtype::e_asName] = asNameWdg->getAnswers();
  level->questionAs.setAsFret(asFretPosWdg->isChecked());
  level->answersAs[TQAtype::e_asFretPos] = asFretPosWdg->getAnswers();
  level->questionAs.setAsSound(asSoundWdg->isChecked());
  level->answersAs[TQAtype::e_asSound] = asSoundWdg->getAnswers();

  level->requireOctave = octaveRequiredChB->isChecked();
  level->requireStyle = styleRequiredChB->isChecked();
  level->showStrNr = showStrNrChB->isChecked();
  level->onlyLowPos = lowPosOnlyChBox->isChecked();
  level->intonation = m_intonationCombo->currentIndex();
}


void questionsSettings::setMelodiesEnabled(bool enableMelodies) {
  m_singleGr->blockSignals(true);
  m_singleGr->setChecked(!enableMelodies);
  if (enableMelodies) {
    asNameWdg->setChecked(false);
    asFretPosWdg->setChecked(false);
  }
  asSoundWdg->setChecked(false); // reset it either for melodies or for single note
  asNoteWdg->setChecked(false); // reset it either for melodies or for single note
  m_singleGr->blockSignals(false);
}


void questionsSettings::changed() {
  loadLevel(wLevel());
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void questionsSettings::adjustToLevel() {
// Disable 'show string' and 'lowest position only' when no answers as guitar and/or sound
  if (lowPosOnlyChBox->isVisible()) {
    bool lowDisabled = false;
    if (!asNoteWdg->answerAsPos() && !asNameWdg->answerAsPos() &&
              !asFretPosWdg->answerAsPos() && !asSoundWdg->answerAsPos() &&
              !asNoteWdg->answerAsSound() && !asNameWdg->answerAsSound() &&
              !asFretPosWdg->answerAsSound() && !asSoundWdg->answerAsSound())
        lowDisabled = true;
// Disable showStrNrChB & lowPosOnlyChBox  if question and answer are on guitar
    if (asFretPosWdg->isChecked() && asFretPosWdg->answerAsPos()) {
      showStrNrChB->setChecked(true);
      lowPosOnlyChBox->setChecked(false);
      lowDisabled = true;
    }
    lowPosOnlyChBox->setDisabled(lowDisabled);
    showStrNrChB->setDisabled(lowDisabled);
  }
// Is sound input enabled to allow intonation check
  if (asNoteWdg->answerAsSound() || asNameWdg->answerAsSound() || asFretPosWdg->answerAsSound() || asSoundWdg->answerAsSound())
      m_intonationCombo->setDisabled(false);
  else
      m_intonationCombo->setDisabled(true);
// Disable name styles when no name as question or answer
  if (!(asNameWdg->isChecked() || asNoteWdg->answerAsName() || asFretPosWdg->answerAsName() || asSoundWdg->answerAsName())) {
      styleRequiredChB->setChecked(false);
      styleRequiredChB->setDisabled(true);
  } else {
      styleRequiredChB->setDisabled(false);
  }
}



void questionsSettings::whenParamsChanged() {
  if (!signalsBlocked()) {
    if (sender() == m_singleGr) {
      if (m_singleGr->isChecked())
        wLevel()->melodyLen = 1;
      else
        wLevel()->melodyLen = 2;
    }
    adjustToLevel();
    changedLocal();
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


void questionsSettings::paintSlot( ) {
  QPainter painter(m_paintHandler);
  QPen pen = painter.pen();
	if (m_singleGr->isChecked())
		pen.setColor(palette().color(QPalette::Active, QPalette::Text));
	else
		pen.setColor(palette().color(QPalette::Disabled, QPalette::Text));
  pen.setWidth(1);
	pen.setStyle(Qt::DashLine);
  painter.setPen(pen);
  int vertLineUpY = m_questLab->geometry().y() + m_questLab->geometry().height() + 14;
  painter.drawLine(10, vertLineUpY, m_singleGr->contentsRect().width() - 20, vertLineUpY);
  int vertLineDownY =asSoundWdg->enableChBox->geometry().y() + asSoundWdg->enableChBox->geometry().height() + 14;
  painter.drawLine(10, vertLineDownY, m_singleGr->contentsRect().width() - 20, vertLineDownY);
  int horLineLeftX = asNoteWdg->enableChBox->x()	+ asNoteWdg->enableChBox->width() + 14;
  painter.drawLine(horLineLeftX,10, horLineLeftX,m_singleGr->contentsRect().height() - 20);
  int horLineRightX = m_asSoundLab->geometry().x() + m_asSoundLab->geometry().width() + 14;
  painter.drawLine(horLineRightX,10, horLineRightX,m_singleGr->contentsRect().height() - 20);
	
}


void questionsSettings::stringsCheckedSlot(bool checked) {
  if (checked)
    lowPosOnlyChBox->setDisabled(false);
  else {
    lowPosOnlyChBox->setChecked(false);
    lowPosOnlyChBox->setDisabled(true);    
  }
}


void questionsSettings::melodyQuestionSlot() {
// 	if (m_playMelodyChB->isChecked()) {
// 			asNoteWdg->setAnswers(TQAtype(false, false, false, true));
// 			asNoteWdg->setChecked(true);
// 	} else {
// 			asNoteWdg->setChecked(false);
// 	}
// 	if (m_writeMelodyChB->isChecked()) {
// 			asSoundWdg->setAnswers(TQAtype(true, false, false, false));
// 			asSoundWdg->setChecked(true);
// 	} else {
// 			asSoundWdg->setChecked(false);
// 	}
}









