/***************************************************************************
 *   Copyright (C) 2014-2016 by Tomasz Bojczuk                             *
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

#include "tmelodysettings.h"
#include <exam/tlevel.h>
#include <exam/textrans.h>
#include <widgets/troundedlabel.h>
#include <qtr.h>
#include <tinitcorelib.h>
#include <tmultiscore.h>
// #include <tscorescene.h>
// #include <tnotecontrol.h>
#include <tscoreparams.h>
#include <tnoofont.h>
#include <QtWidgets/QtWidgets>

#include <QtCore/qdebug.h>


TmelodySettings::TmelodySettings(TlevelCreatorDlg* creator) :
  TabstractLevelPage(creator)
{
  auto questionsLab = new QLabel(qTR("TlevelCreatorDlg", "Questions") + QLatin1String(":"), this);
  int nootFontSize = fontMetrics().boundingRect("A").height() * 2;
  m_playMelodyChB = new QCheckBox(TexTrans::playMelodyTxt(), this);
    m_playMelodyChB->setStatusTip(tableTip(TexTrans::playDescTxt(), TQAtype::e_asNote, TQAtype::e_asSound, nootFontSize));
  m_writeMelodyChB = new QCheckBox(TexTrans::writeMelodyTxt(), this);
    m_writeMelodyChB->setStatusTip(tableTip(TexTrans::writeDescTxt(), TQAtype::e_asSound, TQAtype::e_asNote, nootFontSize));

  QString nooColor = QString("color: %1").arg(palette().highlight().color().name());
  auto melodyLab = new QLabel(TnooFont::span("m", nootFontSize * 2, nooColor), this);
  melodyLab->setAlignment(Qt::AlignCenter);
  melodyLab->setMaximumWidth(melodyLab->sizeHint().width());

  m_melodyLengthSpin = new QSpinBox(this);
    m_melodyLengthSpin->setMaximum(50);
    m_melodyLengthSpin->setMinimum(1);
    m_melodyLengthSpin->setStatusTip(tr("Number of notes in a melody."));
    m_melodyLengthSpin->setStatusTip(tr("Maximum number of notes in a melody. Melody length is random value between 70% and 100% of that number."));

  QLabel *lenghtLab = new QLabel(tr("Melody length"), this);
  
  m_finishOnChB = new QCheckBox(tr("Melody ends on tonic note"), this);
    m_finishOnChB->setStatusTip(tr("Determines the last note of a melody.<br>When set, melody will be finished on tonic note in actual key signature."));
//   m_equalTempoChB = new QCheckBox(tr("Require equal tempo"), this);
//     m_equalTempoChB->setStatusTip(tr("If set, doesn't matter how fast you will play but entire melody has to be played with the same tempo."));

  auto randomLab = new QLabel(tr("Random melody") + QLatin1String(":"), this);
  m_randomRadio = new QRadioButton(tr("in selected range"), this);
  m_listRadio = new QRadioButton(tr("from notes below"), this);
  auto randButtGroup = new QButtonGroup(this);
    randButtGroup->addButton(m_randomRadio);
    randButtGroup->addButton(m_listRadio);
  m_randomRadio->setChecked(true);

  m_score = new TmultiScore(nullptr, this);
  m_score->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_score->setAmbitus(Tnote(Tcore::gl()->loString().chromatic()), Tnote(Tcore::gl()->hiNote().chromatic()));
//   m_score->addBGglyph(int(Tcore::gl()->instrument)); // TODO: make glyph centered or delete it at all
  m_score->setClef(Tclef(Tcore::gl()->S->clef));
  m_score->setScoreDisabled(true);

  auto rightLay = new QVBoxLayout;
    auto questLay = new QHBoxLayout;
      questLay->addStretch(2);
      questLay->addWidget(questionsLab);
      questLay->addStretch(1);
      questLay->addWidget(m_playMelodyChB);
      questLay->addStretch(1);
      questLay->addWidget(m_writeMelodyChB);
      questLay->addStretch(2);
  rightLay->addLayout(questLay);
    auto melLengthLay = new QHBoxLayout;
      melLengthLay->addStretch(2);
      melLengthLay->addWidget(lenghtLab);
      melLengthLay->addStretch(1);
      melLengthLay->addWidget(m_melodyLengthSpin);
      melLengthLay->addStretch(2);
      melLengthLay->addWidget(m_finishOnChB);
      melLengthLay->addStretch(2);
  rightLay->addLayout(melLengthLay);
    auto randRadiosLay = new QVBoxLayout;
      randRadiosLay->addWidget(m_randomRadio);
      randRadiosLay->addWidget(m_listRadio);
    auto randomLay = new QHBoxLayout;
      randomLay->addStretch();
      randomLay->addWidget(randomLab);
      randomLay->addLayout(randRadiosLay);
      randomLay->addStretch();
  rightLay->addLayout(randomLay);
//   rightLay->addWidget(m_equalTempoChB);
  auto upperLay = new QHBoxLayout;
    upperLay->addWidget(melodyLab);
    upperLay->addLayout(rightLay);
  auto lay = new QVBoxLayout;
    lay->addLayout(upperLay);
    lay->addWidget(m_score);

  m_melGroup = new QGroupBox(qTR("TmelMan", "Melody"), this);
    m_melGroup->setCheckable(true);
    m_melGroup->setLayout(lay);
  auto mainLay = new QVBoxLayout;
    mainLay->addWidget(m_melGroup);
  setLayout(mainLay);

  connect(m_melodyLengthSpin, SIGNAL(valueChanged(int)), this, SLOT(changedLocal()));
//   connect(m_equalTempoChB, SIGNAL(clicked()), this, SLOT(changedLocal()));
  connect(m_finishOnChB, &QCheckBox::clicked, this, &TmelodySettings::changedLocal);
  connect(m_playMelodyChB, &QCheckBox::clicked, this, &TmelodySettings::changedLocal);
  connect(m_writeMelodyChB, &QCheckBox::clicked, this, &TmelodySettings::changedLocal);
  connect(m_melGroup, &QGroupBox::clicked, this, &TmelodySettings::changedLocal);
  connect(m_randomRadio, &QRadioButton::clicked, this, &TmelodySettings::changedLocal);
  connect(m_listRadio, &QRadioButton::clicked, this, &TmelodySettings::changedLocal);
//   connect(m_score, &TmultiScore::noteWasChanged, this, &TmelodySettings::changedLocal);
}


void TmelodySettings::loadLevel(Tlevel* level) {
  const QSignalBlocker blocker(this);
  m_melodyLengthSpin->setValue(level->melodyLen);
  m_finishOnChB->setChecked(level->endsOnTonic);
  m_randomRadio->setChecked(level->randMelody == Tlevel::e_randFromRange);
  m_listRadio->setChecked(level->randMelody == Tlevel::e_randFromList);
  m_playMelodyChB->setChecked(level->canBeMelody() && level->questionAs.isNote() && level->answersAs[TQAtype::e_asNote].isSound());
  m_writeMelodyChB->setChecked(level->canBeMelody() && level->questionAs.isSound() && level->answersAs[TQAtype::e_asSound].isNote());
  m_score->deleteNotes();
//   m_score->scoreScene()->right()->enableToAddNotes(false);
  if (level->randMelody == Tlevel::e_randFromList) {
    m_score->setNotes(level->notesList);
    m_score->setEnableKeySign(level->useKeySign);
    if (level->useKeySign)
      m_score->setKeySignature(level->keyOfrandList);
  }
//   m_equalTempoChB->setChecked(level->requireInTempo);
  m_melGroup->setChecked(level->canBeMelody());
  m_score->setScoreDisabled(level->randMelody == Tlevel::e_randFromRange);
  saveLevel(workLevel());
  if (level != workLevel())
    changed();
//   emit levelChanged();
}


void TmelodySettings::saveLevel(Tlevel* level) {
  qDebug() << "[TmelodySettings] saving level" << (level == workLevel() ? QStringLiteral("work") : level->name);
  if (m_melGroup->isChecked()) {
      level->melodyLen = static_cast<quint16>(m_melodyLengthSpin->value());
      level->endsOnTonic = m_finishOnChB->isChecked();
      level->randMelody = m_randomRadio->isChecked() ? Tlevel::e_randFromRange : Tlevel::e_randFromList;
      level->notesList.clear();
      if (level->randMelody == Tlevel::e_randFromList) {
        level->keyOfrandList = m_score->keySignature();
        int loNoteId = 0, hiNoteId = 0;
        for (int n = 0; n < m_score->notesCount(); ++n) {
          auto note = m_score->getNote(n);
          if (note.isValid()) // skip empty notes
            level->notesList << note;
          auto chromatic = note.chromatic();
          if (chromatic > level->notesList[hiNoteId].chromatic())
              hiNoteId = n;
          else if (chromatic < level->notesList[loNoteId].chromatic())
            loNoteId = n;
        }
        // overwrite fret range and note range
        level->loFret = 0;
        level->hiFret = Tcore::gl()->GfretsNumber;
        if (level->isSingleKey) {
            // narrow range when transposition will not be performed
            level->loNote = level->notesList[loNoteId];
            level->hiNote = level->notesList[hiNoteId];
        } else {
            // when the list will be transposed, set note range the same as whole instrument scale,
            // otherwise ambitus of pitch detection would be not contain transposed notes
            level->loNote = Tcore::gl()->loNote();
            level->hiNote = Tcore::gl()->hiNote();
        }
      }
//       fillLevelQA(level);
    //   level->requireInTempo = m_equalTempoChB->isChecked();
  }
//   else // single note mode
//       level->melodyLen = 1;
}


void TmelodySettings::changed() {
  const QSignalBlocker blocker(this);
  if (workLevel()->melodyLen > 1
//       && ((workLevel()->questionAs.isNote() && workLevel()->answersAs[TQAtype::e_asNote].isSound())
//       || (workLevel()->questionAs.isSound() && workLevel()->answersAs[TQAtype::e_asSound].isNote())
//       || (workLevel()->questionAs.isSound() && workLevel()->answersAs[TQAtype::e_asSound].isSound()))
     ) {
      // melodies are supported only for:
      //        - question as note -> answer as sound (play score)
      //        - question as sound -> answer as note (dictation)
      //        - question as sound -> answer as sound (repeat melody)
      m_melGroup->setChecked(true);
      m_melodyLengthSpin->setValue(workLevel()->melodyLen);
      if (workLevel()->melodyLen < 3) {
          // m_equalTempoChB->setChecked(false);
          // m_equalTempoChB->setDisabled(true);
          m_finishOnChB->setChecked(false);
          m_finishOnChB->setDisabled(true);
      } else {
          // m_equalTempoChB->setDisabled(false);
          m_finishOnChB->setDisabled(false);
      }
      fillLevelQA(workLevel());
      if (workLevel()->randMelody == Tlevel::e_randFromRange)
        m_finishOnChB->setChecked(false);
      m_finishOnChB->setDisabled(workLevel()->randMelody != Tlevel::e_randFromRange);
      m_score->setScoreDisabled(workLevel()->randMelody == Tlevel::e_randFromRange);
  } else { // otherwise page is disabled
      m_melodyLengthSpin->setValue(1);
      m_melGroup->setChecked(false);
  }
  m_score->setEnableKeySign(workLevel()->useKeySign);
  if (workLevel()->useKeySign) {
    if (workLevel()->isSingleKey) { // force the key that was set in accidentals page
        m_score->setKeySignature(workLevel()->loKey);
        m_score->setMaxKeySign(workLevel()->loKey.value()); // lock key signature change
        m_score->setMinKeySign(workLevel()->loKey.value());
    } else {
        m_score->setMaxKeySign(workLevel()->hiKey.value());
        m_score->setMinKeySign(workLevel()->loKey.value());
    }
  }
  m_score->setEnabledDblAccid(workLevel()->withDblAcc);
//   loadLevel(workLevel());
//   changedLocal();
}

//#################################################################################################
//###################              PROTECTED           ############################################
//#################################################################################################

void TmelodySettings::changedLocal() {
  if (signalsBlocked())
    return;

//   if (sender() == m_score) { // score is enabled here
//     if (m_score->isKeySignEnabled() && workLevel()->onlyCurrKey) { // Check is changed note in current key
//       if (m_score->currentIndex() > -1) {
//         QColor mC = -1;
//         if (!m_score->keySignature().inKey(m_score->getNote(m_score->currentIndex())).isValid())
//           mC = Qt::red;
//         m_score->noteFromId(m_score->currentIndex())->markNote(mC);
//       }
//     }
//   }

  const QSignalBlocker blocker(this);
  if (sender() == m_melGroup)
    m_melodyLengthSpin->setValue(m_melGroup->isChecked() ? 2 : 1);
//   if (sender() == m_melodyLengthSpin) {
//       if (m_melodyLengthSpin->value() < 3) {
// //         m_equalTempoChB->setChecked(false);
// //         m_equalTempoChB->setDisabled(true);
//         m_finishOnChB->setChecked(false);
//         m_finishOnChB->setDisabled(true);
//       } else {
// //         m_equalTempoChB->setDisabled(false);
//         m_finishOnChB->setDisabled(false);
//       }
//   }
//   if (sender() == m_playMelodyChB || sender() == m_writeMelodyChB) {
//     if (m_melGroup->isChecked())
//       fillLevelQA(workLevel());
//   }
//   if (sender() == m_randomRadio || sender() == m_listRadio) {
//     if (m_listRadio->isChecked())
//       m_finishOnChB->setChecked(false);
//     m_finishOnChB->setDisabled(m_listRadio->isChecked());
//     m_score->setScoreDisabled(m_randomRadio->isChecked());
//   }
  TabstractLevelPage::changedLocal();
  changed();
}


void TmelodySettings::fillLevelQA(Tlevel* l) {
  if (m_melGroup->isChecked()) {
    l->questionAs.setAsNote(m_playMelodyChB->isChecked());
    l->answersAs[TQAtype::e_asNote] = TQAtype(false, false, false, m_playMelodyChB->isChecked());
    l->questionAs.setAsSound(m_writeMelodyChB->isChecked());
    l->answersAs[TQAtype::e_asSound] = TQAtype(m_writeMelodyChB->isChecked(), false, false, false);
    l->questionAs.setAsFret(false);
    l->answersAs[TQAtype::e_asFretPos] = TQAtype(false, false, false, false);
    l->questionAs.setAsName(false);
    l->answersAs[TQAtype::e_asName] = TQAtype(false, false, false, false);
  }
}









