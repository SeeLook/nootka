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


#include "rangesettings.h"
#if defined (Q_OS_ANDROID)
  #include <tmtr.h>
#endif
#include <level/tlevelpreview.h>
#include <tsimplescore.h>
#include <tscorescene.h>
#include <tnoofont.h>
#include <exam/tlevel.h>
#include <music/ttune.h>
#include <tscoreparams.h>
#include <tinitcorelib.h>
#include <QtWidgets/QtWidgets>


rangeSettings::rangeSettings(TlevelCreatorDlg* creator) :
    TabstractLevelPage(creator)
{
  QVBoxLayout *mainLay = new QVBoxLayout;
  mainLay->setAlignment(Qt::AlignCenter);

  QHBoxLayout *allLay = new QHBoxLayout;

  QVBoxLayout *scoreLay = new QVBoxLayout;
  m_scoreRang = new TsimpleScore(2, this);
    m_scoreRang->setClef(Tclef(Tcore::gl()->S->clef));
    m_scoreRang->setAmbitus(Tnote(Tcore::gl()->loString().chromatic()), Tnote(Tcore::gl()->hiNote().chromatic()));
    m_scoreRang->setNote(0, Tnote(1, 0));
    m_scoreRang->setNote(1, Tnote(1, 1));
    m_scoreRang->addBGglyph(int(Tcore::gl()->instrument));
    m_scoreRang->setControllersEnabled(true, false);
    m_scoreRang->scoreScene()->setPointedColor(Tcore::gl()->S->pointerColor);
#if defined (Q_OS_ANDROID)
    m_scoreRang->setFixedHeight(qRound(Tmtr::shortScreenSide() * 0.7));
#endif
  m_fretAdjustButt = new QPushButton(tr("adjust fret range"), this);
    m_fretAdjustButt->setStatusTip(tr("Adjust fret range in a level to currently selected note range"));
  auto notesRangGr = new QGroupBox(TlevelPreview::notesRangeTxt(), this);
  scoreLay->addWidget(m_scoreRang);
  scoreLay->addWidget(m_fretAdjustButt);
#if defined (Q_OS_ANDROID)
  m_fretAdjLabel = getLabelFromStatus(m_fretAdjustButt, false, true);
  scoreLay->addWidget(m_fretAdjLabel);
#endif

  notesRangGr->setLayout(scoreLay);
  allLay->addWidget(notesRangGr);

  QVBoxLayout *guitLay = new QVBoxLayout;
  m_fretGr = new QGroupBox(TlevelPreview::fretsRangeTxt(), this);
  QHBoxLayout *fretLay = new QHBoxLayout;
  QLabel *fromLab = new QLabel(tr("from"),this);
  m_fromSpinB = new QSpinBox(this);
  m_fromSpinB->setMaximum(Tcore::gl()->GfretsNumber);
  QLabel *toLab = new QLabel(tr("to"),this);
  m_toSpinB = new QSpinBox(this);
  m_toSpinB->setMaximum(Tcore::gl()->GfretsNumber);
  m_toSpinB->setValue(3);
  fretLay->addStretch(2);
  fretLay->addWidget(fromLab);
  fretLay->addWidget(m_fromSpinB);
  fretLay->addStretch(1);
  fretLay->addWidget(toLab);
  fretLay->addWidget(m_toSpinB);
  fretLay->addStretch(2);
  m_noteAdjustButt = new QPushButton(tr("adjust note range"), this);
  m_noteAdjustButt->setStatusTip(tr("Adjust note range in a level to currently selected fret range"));
  m_fretGr->setLayout(fretLay);
  guitLay->addWidget(m_fretGr);
  guitLay->addWidget(m_noteAdjustButt, 1, Qt::AlignCenter);
#if defined (Q_OS_ANDROID)
  m_noteAdjLabel = getLabelFromStatus(m_noteAdjustButt, false, true);
  guitLay->addWidget(m_noteAdjLabel);
#endif
  guitLay->addStretch(1);

  m_stringsGr = new QGroupBox(tr("available strings:"),this);
  m_stringsGr->setStatusTip(tr("Uncheck strings if you want to skip them in an exam."));
  QGridLayout *strLay = new QGridLayout;
  for (int i = 0; i < 6; i++) {
      m_stringBut[i] = new QCheckBox(QString("%1").arg(i + 1),this);
  #if defined (Q_OS_ANDROID)
      m_stringBut[i]->setFont(TnooFont(qRound(Tmtr::fingerPixels() * 0.5)));
  #else
      m_stringBut[i]->setFont(TnooFont(qRound(font().pointSize() * 2.5)));
  #endif
      m_stringBut[i]->setChecked(true);
      connect(m_stringBut[i], SIGNAL(clicked()), this, SLOT(stringSelected()));
      connect(m_stringBut[i], SIGNAL(clicked()), this, SLOT(whenParamsChanged()));
      if (i < 3)
          strLay->addWidget(m_stringBut[i], 1, i + 1, 0);
      else
          strLay->addWidget(m_stringBut[i], 2, i - 2, 0);
      if (i >= Tcore::gl()->Gtune()->stringNr())
        m_stringBut[i]->hide();
  }
  m_stringsGr->setLayout(strLay);
  guitLay->addWidget(m_stringsGr);
#if defined (Q_OS_ANDROID)
  m_strGrLabel = getLabelFromStatus(m_stringsGr, false, true);
  guitLay->addWidget(m_strGrLabel);
#endif
  guitLay->addStretch(1);

  allLay->addLayout(guitLay);
  mainLay->addLayout(allLay);
  setLayout(mainLay);

  if (Tcore::gl()->instrument == e_noInstrument) {
    m_fretGr->hide();
    m_stringsGr->hide();
    m_fretAdjustButt->hide();
    m_noteAdjustButt->hide();
#if defined (Q_OS_ANDROID)
    m_strGrLabel->hide();
    m_fretAdjLabel->hide();
    m_noteAdjLabel->hide();
#endif
  }

  connect(m_scoreRang, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(whenParamsChanged()));
  connect(m_scoreRang, SIGNAL(clefChanged(Tclef)), this, SLOT(whenParamsChanged()));
  connect(m_fromSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
  connect(m_toSpinB, SIGNAL(valueChanged(int)), this, SLOT(whenParamsChanged()));
  connect(m_fretAdjustButt, SIGNAL(clicked()), this, SLOT(adjustFrets()));
  connect(m_noteAdjustButt, SIGNAL(clicked()), this, SLOT(adjustNotes()));
}


void rangeSettings::stringSelected() {
  if ( !m_stringBut[0]->isChecked() && !m_stringBut[1]->isChecked()
      && !m_stringBut[2]->isChecked() && !m_stringBut[3]->isChecked()
      && !m_stringBut[4]->isChecked() && !m_stringBut[5]->isChecked() ) {
      m_stringBut[0]->setChecked(true);
  }
}


void rangeSettings::loadLevel(Tlevel* level) {
  const QSignalBlocker blocker(this);
  m_scoreRang->setClef(level->clef);
  m_scoreRang->setAmbitus(Tnote(Tcore::gl()->loString().chromatic()), Tnote(Tcore::gl()->hiNote().chromatic()));
  m_scoreRang->setNote(0, level->loNote);
  m_scoreRang->setNote(1, level->hiNote);
  m_fromSpinB->setValue(level->loFret);
  m_toSpinB->setValue(level->hiFret);
  for (int i = 0; i < Tcore::gl()->Gtune()->stringNr(); i++)
      m_stringBut[i]->setChecked(level->usedStrings[i]);
  stringSelected();
  setDisabled(level->randMelody == Tlevel::e_randFromList);
  saveLevel(workLevel());
}


void rangeSettings::whenParamsChanged() {
  m_scoreRang->setAmbitus(Tnote(Tcore::gl()->loString().chromatic()), Tnote(Tcore::gl()->hiNote().chromatic()));
  if (!m_stringBut[0]->isChecked() || !m_stringBut[1]->isChecked()
      || !m_stringBut[2]->isChecked() || !m_stringBut[3]->isChecked()
      || !m_stringBut[4]->isChecked() || !m_stringBut[5]->isChecked() )
    emit allStringsChecked(false);
  else
    emit allStringsChecked(true);
      /** It has two reasons:
        1. when questions list is created there is no conditions to check
          unavailable (unchecked) strings
        2. in level validation method is hard to determine dependency unchecked
          strings between range of frets and notes. */
  changedLocal();
}


void rangeSettings::saveLevel(Tlevel* level) {
  if (level->randMelody != Tlevel::e_randFromRange) {
    // Fixing empty notes
		if (m_scoreRang->getNote(0).note == 0)
				m_scoreRang->setNote(0, 
										Tnote(qMax(Tcore::gl()->loString().chromatic(), m_scoreRang->lowestNote().chromatic())));
		if (m_scoreRang->getNote(1).note == 0)
				m_scoreRang->setNote(1, 
										Tnote(qMin(Tcore::gl()->hiNote().chromatic(), m_scoreRang->highestNote().chromatic())));

    if (m_scoreRang->getNote(0).chromatic() <= m_scoreRang->getNote(1).chromatic()) {
				level->loNote = m_scoreRang->getNote(0);
				level->hiNote = m_scoreRang->getNote(1);
		} else {
        level->loNote = m_scoreRang->getNote(1);
        level->hiNote = m_scoreRang->getNote(0);
    }
    if (m_fromSpinB->value() <= m_toSpinB->value()) {
        level->loFret = m_fromSpinB->value();
        level->hiFret = m_toSpinB->value();
    } else {
        level->loFret = m_toSpinB->value();
        level->hiFret = m_fromSpinB->value();
    }
    for (int i = 0; i < Tcore::gl()->Gtune()->stringNr(); i++)
        level->usedStrings[i] = m_stringBut[i]->isChecked();
		level->clef = m_scoreRang->clef();
  }
}


void rangeSettings::changed() {
  const QSignalBlocker blocker(this);
  if (workLevel()->canBeGuitar() || Tcore::gl()->instrument != e_noInstrument) {
        m_fretGr->setDisabled(false);
        m_stringsGr->setDisabled(false);
        m_fretAdjustButt->setDisabled(false);
        m_noteAdjustButt->setDisabled(false);
  } else {
      m_fretGr->setDisabled(true);
      m_stringsGr->setDisabled(true);
      m_fretAdjustButt->setDisabled(true);
      m_noteAdjustButt->setDisabled(true);
  }
  loadLevel(workLevel());
  changedLocal();
}


void rangeSettings::changedLocal() {
  TabstractLevelPage::changedLocal();
}



void rangeSettings::adjustFrets() {
	char loF, hiF;
	Tlevel lev;
	saveLevel(&lev);
	if (!lev.loNote.alter && !lev.hiNote.alter) { // when range doesn't use accidentals
			lev.withFlats = workLevel()->withFlats; // maybe actual working level doses
			lev.withSharps = workLevel()->withSharps;
	} // checking routine requires it
	if (lev.adjustFretsToScale(loF, hiF)) {
			m_fromSpinB->setValue(loF);
			m_toSpinB->setValue(hiF);
			emit rangeChanged();
	} else {
			qDebug() << "Can't adjust fret range!";
	}
}


void rangeSettings::adjustNotes() {
	m_scoreRang->setNote(0, Tnote(Tcore::gl()->loString().chromatic() + m_fromSpinB->value()));
	m_scoreRang->setNote(1, Tnote(Tcore::gl()->hiString().chromatic() + m_toSpinB->value()));
	emit rangeChanged();
}









