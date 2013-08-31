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


#include "questionssettings.h"
#include "tquestionaswdg.h"
#include "tkeysigncombobox.h"
#include <QtGui>

extern bool isNotSaved;

questionsSettings::questionsSettings(QWidget *parent) :
    QWidget(parent)
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
    m_questLab = new QLabel("<b>" + TquestionAsWdg::questionTxt().toUpper() + "</b>", this);
    qaLay->addWidget(m_questLab, 0, 0, Qt::AlignBottom | Qt::AlignRight);
    m_answLab = new TverticalLabel(TquestionAsWdg::answerTxt().toUpper() + "  ", this);
    QFont f = font();
    f.setBold(true);
    m_answLab->setFont(f);
    qaLay->addWidget(m_answLab, 0, 1, Qt::AlignBottom);
    QLabel *asNoteLab = new QLabel(TquestionAsWdg::asNoteTxt().replace(" ", "<br>"), this);
    asNoteLab->setAlignment(Qt::AlignCenter);
    qaLay->addWidget(asNoteLab, 0, 2, Qt::AlignBottom);
    QLabel *asNameLab = new QLabel(TquestionAsWdg::asNameTxt().replace(" ", "<br>"), this);
    asNameLab->setAlignment(Qt::AlignCenter);
    qaLay->addWidget(asNameLab, 0, 3, Qt::AlignBottom);
    QLabel *asFretLab = new QLabel(TquestionAsWdg::asFretPosTxt().replace(" ", "<br>"), this);
    asFretLab->setAlignment(Qt::AlignCenter);
    qaLay->addWidget(asFretLab, 0, 4, Qt::AlignBottom);
    asSoundLab = new QLabel(TquestionAsWdg::asSoundTxt().replace(" ", "<br>"), this);
    asSoundLab->setAlignment(Qt::AlignCenter);
    qaLay->addWidget(asSoundLab, 0, 5, Qt::AlignBottom);
  // CheckBoxes with types of answers for every kind of question
    asNoteWdg = new TquestionAsWdg(TQAtype::e_asNote, qaLay, 1, this);
    asNameWdg = new TquestionAsWdg(TQAtype::e_asName, qaLay, 2, this);
    asFretPosWdg = new TquestionAsWdg(TQAtype::e_asFretPos, qaLay, 3, this);
    asSoundWdg = new TquestionAsWdg(TQAtype::e_asSound, qaLay, 4, this);
  // Labels on the right side of the table with symbols of types - related to questions
    QLabel *scoreNooLab = new QLabel("s?", this);
    QFont nf("nootka", fontMetrics().boundingRect("A").height());
#if defined(Q_OS_MACX)
    nf.setPointSize(fontMetrics().boundingRect("A").height() * 2);
#endif
    scoreNooLab->setFont(nf);
    qaLay->addWidget(scoreNooLab, 1, 6, Qt::AlignCenter);
    QLabel *nameNooLab = new QLabel("c?", this);
    nameNooLab->setFont(nf);
    qaLay->addWidget(nameNooLab, 2, 6, Qt::AlignCenter);
    QLabel *guitarNooLab = new QLabel("g?", this);
    guitarNooLab->setFont(nf);
    qaLay->addWidget(guitarNooLab, 3, 6, Qt::AlignCenter);
    soundNooLab = new QLabel("n?", this);
    soundNooLab->setFont(nf);
    qaLay->addWidget(soundNooLab, 4, 6);
  // Labels on the bottom side of the table with symbols of types - related to answers  
    QLabel *qScoreNooLab = new QLabel("s!", this);
    qScoreNooLab->setFont(nf);
    qaLay->addWidget(qScoreNooLab, 5, 2, Qt::AlignCenter);
    QLabel *qNmeNooLab = new QLabel("c!", this);
    qNmeNooLab->setFont(nf);
    qaLay->addWidget(qNmeNooLab, 5, 3, Qt::AlignCenter);
    QLabel *qGuitarNooLab = new QLabel("g!", this);
    qGuitarNooLab->setFont(nf);
    qaLay->addWidget(qGuitarNooLab, 5, 4, Qt::AlignCenter);
    qSoundNooLab = new QLabel("n!", this);
    qSoundNooLab->setFont(nf);
    qaLay->addWidget(qSoundNooLab, 5, 5);
    
    m_tableWdg->setLayout(qaLay);
    mainLay->addLayout(tabLay);
    mainLay->addStretch();
    
  // some checkBoxes
    QGridLayout *chLay = new QGridLayout;
    octaveRequiredChB = new QCheckBox(tr("require octave"),this);
    octaveRequiredChB->setStatusTip(tr("if checked, selecting of valid octave is required"));
    chLay->addWidget(octaveRequiredChB, 0, 0, Qt::AlignLeft);
    
    forceAccChB = new QCheckBox(tr("force using appropriate accidental"),this);
    forceAccChB->setStatusTip(tr("if checked, is possible to select a note<br>with given accidental only."));
    chLay->addWidget(forceAccChB, 1, 0, Qt::AlignLeft);
    
    styleRequiredChB = new QCheckBox(tr("use different naming styles"),this);
    styleRequiredChB->setStatusTip(tr("if checked, note names are switched between letters and solfege."));
    chLay->addWidget(styleRequiredChB, 2, 0, Qt::AlignLeft);
    
    showStrNrChB = new QCheckBox(tr("show string number in questions"), this);
    showStrNrChB->setStatusTip(tr("Shows on which string an answer has to be given.<br>Be careful, when it is needed and when it has no sense"));
    chLay->addWidget(showStrNrChB, 0, 1, Qt::AlignLeft);
    mainLay->addLayout(chLay);
    
    lowPosOnlyChBox = new QCheckBox(tr("notes in the lowest position only"),this);
    lowPosOnlyChBox->setStatusTip(tr("if checked, the lowest position in selected frets' range are required,<br>otherwise all possible positions of the note are taken.<br>To use this, all strings have to be available !!"));
    chLay->addWidget(lowPosOnlyChBox, 1, 1, Qt::AlignLeft);
    
    currKeySignChBox = new QCheckBox(tr("notes in current key signature only"),this);
    currKeySignChBox->setStatusTip(tr("Only notes from current key signature are taken.<br>If key signature is disabled accidentals are not used."));
    chLay->addWidget(currKeySignChBox, 2, 1, Qt::AlignLeft);
      
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
}

void questionsSettings::loadLevel(TexamLevel& level) {
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
}


void questionsSettings::whenParamsChanged() {
//     if (asNameWdg->answerAsName() && asNameWdg->isChecked()) {
//         styleRequiredChB->setChecked(true);
//         styleRequiredChB->setDisabled(true);
//     } else 
//         styleRequiredChB->setDisabled(false);
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
    
    
    if (!isNotSaved) {
        isNotSaved = true;
        emit questSettChanged();
    }
}


void questionsSettings::saveLevel(TexamLevel &level) {
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
      (qSoundNooLab->geometry().top() - asSoundWdg->enableChBox->geometry().bottom()) / 2;
  painter.drawLine(m_tableWdg->geometry().left(), vertLineDownY, m_tableWdg->geometry().right(), vertLineDownY);
  int horLineLeftX = m_tableWdg->geometry().x() + asNoteWdg->enableChBox->geometry().right() + 
      (asNoteWdg->asNoteChB->geometry().left() - asNoteWdg->enableChBox->geometry().right()) / 2;
  painter.drawLine(horLineLeftX, m_tableWdg->geometry().top(), horLineLeftX, m_tableWdg->geometry().bottom());
  int horLineRightX = m_tableWdg->geometry().x() + asSoundLab->geometry().right() + 
      (soundNooLab->geometry().left() - asSoundLab->geometry().right()) / 2;
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


//############################################################################
 //#####################   TverticalLabel ####################################
 //###########################################################################

TverticalLabel::TverticalLabel(QString text, QWidget* parent) :
  QWidget(parent),
  m_text(text)
{
    QFontMetrics metrics = fontMetrics();
    m_rect = metrics.boundingRect(m_text);
    setFixedSize(m_rect.height() * 2.5, m_rect.width() * 1.2);
}


void TverticalLabel::paintEvent(QPaintEvent* ) {
    QPainter painter(this);
    painter.save();
    painter.translate(width() / 2, height() / 2);
    painter.rotate(270); // angle
    painter.drawText( height() / (-2), 0 , m_text);
    painter.restore();
}



