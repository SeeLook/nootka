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

  m_melGroup = new QGroupBox(tr("melodies"), this);
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
}


void TmelodySettings::loadLevel(Tlevel* level) {
  QSignalBlocker(this);
  m_melodyLengthSpin->setValue(level->melodyLen);
  m_finishOnChB->setChecked(level->endsOnTonic);
  if (m_listRadio->isChecked()) {
    // TODO save notes from score to level list, produce warning when score has less notes than two
  }
//   m_equalTempoChB->setChecked(level->requireInTempo);
  changedLocal();
  saveLevel(wLevel());
}


void TmelodySettings::saveLevel(Tlevel* level) {
  level->melodyLen = m_melodyLengthSpin->value();
  level->endsOnTonic = m_finishOnChB->isChecked();
//   level->requireInTempo = m_equalTempoChB->isChecked();
}


void TmelodySettings::changed() {
  QSignalBlocker(this);
  if (wLevel()->melodyLen > 1
      && ((wLevel()->questionAs.isNote() && wLevel()->answersAs[TQAtype::e_asNote].isSound())
      || (wLevel()->questionAs.isSound() && wLevel()->answersAs[TQAtype::e_asSound].isNote())
      || (wLevel()->questionAs.isSound() && wLevel()->answersAs[TQAtype::e_asSound].isSound()))) {
    // melodies are supported only for:
    //        - question as note -> answer as sound (play score)
    //        - question as sound -> answer as note (dictation)
    //        - question as sound -> answer as sound (repeat melody)
      m_melGroup->setChecked(true);
      m_melodyLengthSpin->setValue(wLevel()->melodyLen);
  } else { // otherwise page is disabled
      m_melodyLengthSpin->setValue(1);
      m_melGroup->setChecked(false);
  }
  m_score->setEnableKeySign(wLevel()->useKeySign);
  if (wLevel()->useKeySign) {
    if (wLevel()->isSingleKey) { // force the key that was set in accidentals page
        m_score->setKeySignature(wLevel()->loKey);
        m_score->setMaxKeySign(wLevel()->loKey.value()); // lock key signature change
        m_score->setMinKeySign(wLevel()->loKey.value());
    } else {
        m_score->setMaxKeySign(wLevel()->hiKey.value());
        m_score->setMinKeySign(wLevel()->loKey.value());
    }
  }
}


void TmelodySettings::changedLocal() {
  QSignalBlocker(this);
  if (sender() == m_melGroup) {
    if (m_melGroup->isChecked()) {
        m_melodyLengthSpin->setValue(2);
    } else {
        m_melodyLengthSpin->setValue(1);
    }
  }
  if (sender() == m_melodyLengthSpin) {
      if (m_melodyLengthSpin->value() < 3) {
//         m_equalTempoChB->setChecked(false);
//         m_equalTempoChB->setDisabled(true);
        m_finishOnChB->setChecked(false);
        m_finishOnChB->setDisabled(true);
      } else {
//         m_equalTempoChB->setDisabled(false);
        m_finishOnChB->setDisabled(false);
      }
  }
  if (sender() == m_playMelodyChB || sender() == m_writeMelodyChB) {
    if (m_melGroup->isChecked()) {
      wLevel()->questionAs.setAsNote(m_playMelodyChB->isChecked());
      wLevel()->answersAs[TQAtype::e_asNote] = TQAtype(false, false, false, m_playMelodyChB->isChecked());
      wLevel()->questionAs.setAsSound(m_writeMelodyChB->isChecked());
      wLevel()->answersAs[TQAtype::e_asSound] = TQAtype(m_writeMelodyChB->isChecked(), false, false, false);
      wLevel()->questionAs.setAsFret(false);
      wLevel()->answersAs[TQAtype::e_asFretPos] = TQAtype(false, false, false, false);
      wLevel()->questionAs.setAsName(false);
      wLevel()->answersAs[TQAtype::e_asName] = TQAtype(false, false, false, false);
    }
  }
  TabstractLevelPage::changedLocal();
}









