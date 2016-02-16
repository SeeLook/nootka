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

#include "tguitarsettings.h"
#include "tcolorbutton.h"
#include <tinitcorelib.h>
#include <widgets/tselectinstrument.h>
#include <music/ttune.h>
#include <tsimplescore.h>
#include <tscorescene.h>
#include <tscoreparams.h>
#if defined (Q_OS_ANDROID)
  #include <tmtr.h>
#endif
#include <QtWidgets/QtWidgets>


QString tuningGuitarText, scaleOfInstrText;


TguitarSettings::TguitarSettings(QWidget *parent) :
  TtouchArea(parent),
  m_currentInstr(-1)
{
  m_customTune = new Ttune();
  *m_customTune = *(Tcore::gl()->Gtune());
  m_curentTune = Tcore::gl()->Gtune();
  
  tuningGuitarText = tr("tuning of the guitar");
  scaleOfInstrText = tr("scale of an instrument");

  m_tuneGroup = new QGroupBox(tuningGuitarText, this);
  m_tuneGroup->setStatusTip(tr("Select appropriate tuning from the list or prepare your own.") + "<br>" + 
      tr("Remember to select the appropriate clef in Score settings."));
  m_tuneCombo = new QComboBox(this);
  m_tuneView = new TsimpleScore(6, this);
  m_tuneView->setControllersEnabled(true, false);
  m_tuneView->scoreScene()->setPointedColor(Tcore::gl()->S->pointerColor);
  m_tuneView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_tuneView->setClef(Tcore::gl()->S->clef); 
#if defined (Q_OS_ANDROID)
  m_tuneView->setFixedHeight(qMin(qRound(Tmtr::shortScreenSide() * 0.7), Tmtr::fingerPixels() * 6));
#endif

  m_guitarGroup = new QGroupBox(tr("Instrument") + ":", this);
// Selecting instrument type combo
#if defined (Q_OS_ANDROID)
  m_selectInstr = new TselectInstrument(this, TselectInstrument::e_buttonsOnlyHorizontal);
  m_selectInstr->setGlyphSize(Tmtr::fingerPixels() * 0.8);
#else
  m_selectInstr = new TselectInstrument(this, TselectInstrument::e_buttonsOnlyGrid);
  m_selectInstr->setGlyphSize(40);
#endif
// Right-handed/left-handed check box
#if !defined (Q_OS_ANDROID)
  m_righthandCh = new QCheckBox(tr("right-handed players", "When translation will be too long try to add '\n' - line break between words."), this);
  m_righthandCh->setChecked(Tcore::gl()->GisRightHanded);
  m_righthandCh->setStatusTip(tr("Uncheck this if you are left-handed<br>and your guitar is strung for left-handed playing (changed string order)"));
#endif
// Number of frets
  m_fretNrLab = new QLabel(tr("number of frets:"), this);
  m_fretsNrSpin = new QSpinBox(this);
  m_fretsNrSpin->setMaximum(24);
  m_fretsNrSpin->setMinimum(15);
  m_fretsNrSpin->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
// Number of strings
  m_stringNrLab = new QLabel(tr("number of strings:"), this);
  m_stringNrSpin = new QSpinBox(this);
  m_stringNrSpin->setMaximum(6);
  m_stringNrSpin->setMinimum(3);
  m_stringNrSpin->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

  m_accidGroup = new QGroupBox(tr("preferred accidentals:"),this);
  m_accidGroup->setStatusTip(tr("Choose which accidentals will be shown on the staff."));
  m_prefSharpBut = new QRadioButton(tr("# - sharps"),this);
  m_prefFlatBut = new  QRadioButton(tr("b - flats"),this);
  QButtonGroup *prefGr = new QButtonGroup(this);
  prefGr->addButton(m_prefSharpBut);
  prefGr->addButton(m_prefFlatBut);
  if (Tcore::gl()->GpreferFlats) m_prefFlatBut->setChecked(true);
  else
      m_prefSharpBut->setChecked(true);

  m_fretMarksEdit = new QLineEdit(grabFretsFromList(Tcore::gl()->GmarkedFrets), this);
  m_fretMarksEdit->setMaxLength(25);
  QRegExp rx("([1-2]{0,1}[0-9]{1,2}!{0,1},){0,7}");
  m_fretMarksEdit->setValidator(new QRegExpValidator(rx, 0));
#if defined (Q_OS_ANDROID)
  m_fretMarksEdit->setStatusTip(tr("Put numbers of frets marked with dot. Separate the numbers with comma. Add ! (exclamation mark) after a number to paint a dot twice.").replace(". ", ".<br>"));
#else
  m_fretMarksEdit->setStatusTip(tr("Put numbers of frets marked with dot. Separate the numbers with comma. Add ! (exclamation mark) after a number to paint a dot twice."));
#endif
  QLabel *fretMarksLab = new QLabel(tr("marked frets", "or frets with dots/marks"), this);
  m_disabledWidgets << fretMarksLab;

  m_morePosCh = new QCheckBox(tr("show all possibilities of a note"),this);
  m_morePosCh->setStatusTip(tr("As you know, the same note can be played in several places on the fingerboard.<br>If checked, all of them will be shown."));
  m_morePosCh->setChecked(Tcore::gl()->GshowOtherPos);

  m_pointerColorLab = new QLabel(tr("color of string/fret pointer"), this);
  m_pointColorBut = new TcolorButton(Tcore::gl()->GfingerColor, this);
  m_selectColorLab = new QLabel(tr("color of selected string/fret"), this);
  m_selColorBut = new TcolorButton(Tcore::gl()->GselectedColor, this);

  QVBoxLayout *tuneLay = new QVBoxLayout;
  tuneLay->setAlignment(Qt::AlignCenter);
  tuneLay->addWidget(m_tuneCombo);
  tuneLay->addWidget(m_tuneView);
  m_tuneGroup->setLayout(tuneLay);

  QVBoxLayout *guitarLay = new QVBoxLayout;
    guitarLay->addWidget(m_selectInstr, 0, Qt::AlignCenter);
#if !defined (Q_OS_ANDROID)
    guitarLay->addWidget(m_righthandCh);
#endif
// #if defined (Q_OS_ANDROID)
//     guitarLay->setAlignment(Qt::AlignCenter);
//     m_disabledWidgets << getLabelFromStatus(m_righthandCh);
//     guitarLay->addWidget(m_disabledWidgets.last());
// #endif
    guitarLay->addStretch(1);
#if defined (Q_OS_ANDROID)
    QHBoxLayout *fretsLay = new QHBoxLayout;
      fretsLay->addStretch();
      fretsLay->addWidget(m_fretNrLab);
      fretsLay->addWidget(m_fretsNrSpin);
      fretsLay->addStretch();
    guitarLay->addLayout(fretsLay);
    QHBoxLayout *stringsLay = new QHBoxLayout;
      stringsLay->addStretch();
      stringsLay->addWidget(m_stringNrLab);
      stringsLay->addWidget(m_stringNrSpin);
      stringsLay->addStretch();
    guitarLay->addLayout(stringsLay);
#else
    guitarLay->addWidget(m_fretNrLab, 1, Qt::AlignCenter);
    guitarLay->addStretch(1);
    guitarLay->addWidget(m_fretsNrSpin, 1, Qt::AlignCenter);
    guitarLay->addStretch(1);
    guitarLay->addWidget(m_stringNrLab, 1, Qt::AlignCenter);
    guitarLay->addWidget(m_stringNrSpin, 1, Qt::AlignCenter);
    guitarLay->addStretch(1);
#endif
  m_guitarGroup->setLayout(guitarLay);

#if defined (Q_OS_ANDROID)
  QVBoxLayout *upLay = new QVBoxLayout;
  upLay->setAlignment(Qt::AlignCenter);
#else
  QHBoxLayout *upLay = new QHBoxLayout;
#endif
    upLay->addWidget(m_guitarGroup);
    upLay->addSpacing(3);
    upLay->addWidget(m_tuneGroup);

  QHBoxLayout *prefLay = new QHBoxLayout;
    prefLay->addStretch();
    prefLay->addWidget(m_prefSharpBut);
    prefLay->addSpacing(10);
    prefLay->addWidget(m_prefFlatBut);
    prefLay->addStretch();
#if defined (Q_OS_ANDROID)
    QVBoxLayout *accBoxLay = new QVBoxLayout;
    accBoxLay->addLayout(prefLay);
    m_disabledWidgets << getLabelFromStatus(m_accidGroup, false);
    accBoxLay->addWidget(m_disabledWidgets.last(), 0, Qt::AlignCenter);
    m_accidGroup->setLayout(accBoxLay);
#else
  m_accidGroup->setLayout(prefLay);
#endif

  QHBoxLayout *marksLay = new QHBoxLayout;
    marksLay->addWidget(fretMarksLab);
    marksLay->addWidget(m_fretMarksEdit);
  QVBoxLayout *leftDownLay = new QVBoxLayout;
    leftDownLay->addWidget(m_accidGroup);
    leftDownLay->addLayout(marksLay);
#if defined (Q_OS_ANDROID)
    leftDownLay->setAlignment(Qt::AlignCenter);
    m_disabledWidgets << getLabelFromStatus(m_fretMarksEdit, false);
    leftDownLay->addWidget(m_disabledWidgets.last(), 0, Qt::AlignCenter);
#endif

  QGridLayout *colorLay = new QGridLayout;
    colorLay->addWidget(m_pointerColorLab, 0, 0, Qt::AlignRight);
    colorLay->addWidget(m_pointColorBut, 0 ,1, Qt::AlignLeft);
    colorLay->addWidget(m_selectColorLab, 1, 0, Qt::AlignRight);
    colorLay->addWidget(m_selColorBut, 1, 1, Qt::AlignLeft);

  QVBoxLayout *rightDownLay = new QVBoxLayout;
    rightDownLay->addWidget(m_morePosCh);
#if defined (Q_OS_ANDROID)
    rightDownLay->setAlignment(Qt::AlignCenter);
    m_disabledWidgets << getLabelFromStatus(m_morePosCh, false);
    rightDownLay->addWidget(m_disabledWidgets.last(), 0, Qt::AlignCenter);
#endif
    rightDownLay->addLayout(colorLay);
#if defined (Q_OS_ANDROID)
  QVBoxLayout *downLay = new QVBoxLayout;
#else
  QHBoxLayout *downLay = new QHBoxLayout;
#endif
    downLay->addLayout(leftDownLay);
    downLay->addStretch(1);
    downLay->addLayout(rightDownLay);

  QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);
    mainLay->addLayout(upLay);
    mainLay->addLayout(downLay);

  setLayout(mainLay);

  connect(m_tuneCombo, SIGNAL(activated(int)), this, SLOT(tuneSelected(int)));
  connect(m_tuneView, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(userTune(int, Tnote)));
  connect(m_tuneView, SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
  connect(m_selectInstr, SIGNAL(instrumentChanged(int)), this, SLOT(instrumentTypeChanged(int)));
  connect(m_stringNrSpin, SIGNAL(valueChanged(int)), this, SLOT(stringNrChanged(int)));
  
  m_selectInstr->setInstrument((int)Tcore::gl()->instrument);
  instrumentTypeChanged((int)Tcore::gl()->instrument);
  setTune(Tcore::gl()->Gtune());
  m_fretsNrSpin->setValue(Tcore::gl()->GfretsNumber);
  if (Tcore::gl()->instrument != e_noInstrument) {
      if (*Tcore::gl()->Gtune() == Ttune::stdTune)
          m_tuneCombo->setCurrentIndex(0);
      for (int i = 0; i < 4; i++) {
        if (Tcore::gl()->instrument == e_classicalGuitar) {
          if (*Tcore::gl()->Gtune() == Ttune::tunes[i]) {
              m_tuneCombo->setCurrentIndex(i + 1);
              break;
          }
        } else if (Tcore::gl()->instrument == e_bassGuitar) {
            if (*Tcore::gl()->Gtune() == Ttune::bassTunes[i]) {
              m_tuneCombo->setCurrentIndex(i);
              break;
            }
        }
      }
      QString S = tr("Custom tuning");
      if (Tcore::gl()->Gtune()->name == S)
          m_tuneCombo->setCurrentIndex(m_tuneCombo->count() - 1);
  } else { // Apply instrument scale
    m_tuneView->setClef(Tclef(Tcore::gl()->S->clef));
    m_tuneView->setNote(4, Tcore::gl()->loNote());
    m_tuneView->setNote(5, Tcore::gl()->hiNote());
  }
  updateAmbitus();
#if defined(Q_OS_WIN)
  QTimer::singleShot(5, this, SLOT(delayedBgGlyph()));
#endif
}


TguitarSettings::~TguitarSettings() {
	delete m_customTune;
}


void TguitarSettings::saveSettings() {
	Tcore::gl()->instrument = (Einstrument)m_selectInstr->instrument();
#if !defined (Q_OS_ANDROID)
	Tcore::gl()->GisRightHanded = m_righthandCh->isChecked();
#endif
	Tcore::gl()->GfretsNumber = m_fretsNrSpin->value();
	Ttune *tmpTune = new Ttune();
	if (Tcore::gl()->instrument != e_noInstrument)
			grabTuneFromScore(tmpTune);
// 			tmpTune = new Ttune(m_tuneCombo->currentText(), m_tuneView->getNote(5), m_tuneView->getNote(4),
// 											m_tuneView->getNote(3), m_tuneView->getNote(2), m_tuneView->getNote(1), m_tuneView->getNote(0));
	else { // instrument scale taken from note segments 4 & 5
		Tnote hiN, loN; // fix notes order
		if (m_tuneView->getNote(5).chromatic() < m_tuneView->getNote(4).chromatic()) {
			hiN = m_tuneView->getNote(4);
			loN = m_tuneView->getNote(5);
		} else {
			hiN = m_tuneView->getNote(5);
			loN = m_tuneView->getNote(4);
		}
		*tmpTune = Ttune("scale", 
												Tnote(hiN.chromatic() - m_fretsNrSpin->value()), loN,
												Tnote(0, 0, 0), Tnote(0, 0, 0), Tnote(0, 0, 0), Tnote(0, 0, 0)	);
	}
	Tcore::gl()->setTune(*tmpTune);
	delete tmpTune;
	Tcore::gl()->GshowOtherPos = m_morePosCh->isChecked();
	if (m_prefFlatBut->isChecked()) 
			Tcore::gl()->GpreferFlats = true;
	else 
			Tcore::gl()->GpreferFlats = false;
	Tcore::gl()->GfingerColor = m_pointColorBut->getColor();
	Tcore::gl()->GfingerColor.setAlpha(200);
	Tcore::gl()->GselectedColor = m_selColorBut->getColor();
	checkFretsAndStore(Tcore::gl()->GmarkedFrets);
}


void TguitarSettings::restoreDefaults() {
		instrumentTypeChanged(1); // It will restore tune (standard), frets and strings number and clef
#if !defined (Q_OS_ANDROID)
		m_righthandCh->setChecked(true);
#endif
		m_prefSharpBut->setChecked(true);
		m_morePosCh->setChecked(false);
		m_pointColorBut->setColor(QColor(255, 0, 127, 200));
		m_selColorBut->setColor(QColor(51, 153, 255));
		m_fretMarksEdit->setText("5,7,9,12!,15,19");
}


Tnote TguitarSettings::lowestNote() {
// 		int lowest = -1;
// 		char loNr = 127;
// 		for (int i = 0; i < 6; i++) {
// 			if (m_tuneView->getNote(i).note) {
// 				if (m_tuneView->getNote(i).chromatic() < loNr) {
// 					loNr = m_tuneView->getNote(i).chromatic();
// 					lowest = i;
// 				}
// 			}
// 		}
// 		if (lowest > -1)
// 			return m_tuneView->getNote(lowest);
// 		else
			return m_tuneView->lowestNote();
	/** it should be quite enough to determine pitch detection range.
	 * For other purposes enable the above code. */
}


//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################

void TguitarSettings::setTune(Ttune* tune) {
    for (int i = 0; i < 6; i++) {
				m_tuneView->setNote(i, tune->str(6 - i));
				m_tuneView->setNoteDisabled(i, !(bool)tune->str(6 - i).note);
				if (m_selectInstr->instrument() != 0 && tune->str(6 - i).note)
					m_tuneView->setStringNumber(i, 6 - i);
				else
					m_tuneView->clearStringNumber(i);
    }
    m_stringNrSpin->setValue(tune->stringNr());
		m_curentTune = tune;
		emit tuneChanged(m_curentTune);
}


void TguitarSettings::updateAmbitus() {
	Tnote highest = Tnote(m_tuneView->highestNote().chromatic() - m_fretsNrSpin->value());
	if (m_selectInstr->instrument() == 0) // other instrument has no frets
		highest = m_tuneView->highestNote();
	for (int i = 0; i < 6; i++)
		m_tuneView->setAmbitus(i, m_tuneView->lowestNote(), highest);
}


void TguitarSettings::grabTuneFromScore(Ttune* tune) {
		Tnote nn[6];
		int stringNr = 0;
		for (int i = 0; i < 6; i++) {
			nn[i] = fixEmptyNote(i);
			if (nn[i].note !=0)
					stringNr++;
		}
		QString tuneName;
		if (stringNr > 2)
				tuneName = m_tuneCombo->currentText();
		else {
				tuneName = "scale";
				nn[5] = Tnote(nn[5].chromatic() - m_fretsNrSpin->value());
		}
		*tune = Ttune(tuneName, nn[5], nn[4], nn[3], nn[2], nn[1], nn[0]);
}


//##########################################################################################################
//########################################## PRIVATE SLOTS #################################################
//##########################################################################################################

void TguitarSettings::tuneSelected(int tuneId) {
	disconnect(m_stringNrSpin, SIGNAL(valueChanged(int)), this, SLOT(stringNrChanged(int)));
	if (m_selectInstr->instrument() == 1 || m_selectInstr->instrument() == 2) { // classical guitar
		if (tuneId < m_tuneCombo->count() - 1) // set default clef for defined tunes
				m_tuneView->setClef(Tclef(Tclef::e_treble_G_8down));
    if (tuneId == 0)
        setTune(&Ttune::stdTune);
    else if (tuneId != m_tuneCombo->count() - 1) //the last is custom
				setTune(&Ttune::tunes[tuneId - 1]);
	} else if (m_selectInstr->instrument() == 3) { // bass guitar
			if (tuneId != m_tuneCombo->count() - 1) { //the last is custom
				m_tuneView->setClef(Tclef(Tclef::e_bass_F_8down));
				setTune(&Ttune::bassTunes[tuneId]);
			}
	}
	connect(m_stringNrSpin, SIGNAL(valueChanged(int)), this, SLOT(stringNrChanged(int)));
}


void TguitarSettings::userTune(int, Tnote) {
    m_tuneCombo->setCurrentIndex(m_tuneCombo->count() - 1);
		grabTuneFromScore(m_customTune);
		m_curentTune = m_customTune;
		emit tuneChanged(m_customTune);
}


void TguitarSettings::onClefChanged(Tclef clef) {
  updateAmbitus();
  updateNotesState();
  if (m_selectInstr->instrument() == (int)e_noInstrument && clef.type() == Tclef::e_pianoStaff) { // workaround for piano staff ambitus
    if (m_tuneView->getNote(5) == m_tuneView->lowestNote()) // set highest note of piano staff as a non guitar scale
      m_tuneView->setNote(5, m_tuneView->highestNote());
  }
  emit clefChanged(clef);
  emit lowestNoteChanged(m_tuneView->lowestNote());
}


Tclef TguitarSettings::currentClef() {
		return m_tuneView->clef();
}


void TguitarSettings::stringNrChanged(int strNr) {
		for (int i = 0; i < 6; i++) {
			if (m_tuneView->getNote(i).note) {
				if (i < 6 - strNr) {
					m_tuneView->setNote(i ,Tnote(0, 0, 0));
					m_tuneView->clearStringNumber(i);
					m_tuneView->setNoteDisabled(i, true);
				}
			} else {
				if (i >= 6 - strNr) {
					m_tuneView->setNote(i, m_tuneView->lowestNote());
					m_tuneView->setStringNumber(i, 6 - i);
					m_tuneView->setNoteDisabled(i, false);
				}
			}
		}
		userTune(0, Tnote()); // values in params are unused
}


void TguitarSettings::instrumentTypeChanged(int index) {
	if (m_currentInstr == index)
		return;
	
	m_tuneCombo->clear();
	m_currentInstr = index;
	if ((Einstrument)index == e_classicalGuitar || (Einstrument)index == e_electricGuitar) {
			m_tuneCombo->addItem(Ttune::stdTune.name);
			for (int i = 0; i < 4; i++) {
					m_tuneCombo->addItem(Ttune::tunes[i].name);
			}
			if ((Einstrument)index == e_classicalGuitar)
					m_fretsNrSpin->setValue(19);
			else
					m_fretsNrSpin->setValue(23);
			m_tuneView->setClef(Tclef(Tclef::e_treble_G_8down));
			setTune(&Ttune::stdTune);
			m_tuneCombo->setCurrentIndex(0);
			m_stringNrSpin->setValue(Ttune::tunes[0].stringNr());
	} else if ((Einstrument)index == e_bassGuitar) { // bass guitar
			for (int i = 0; i < 4; i++) {
        m_tuneCombo->addItem(Ttune::bassTunes[i].name);
			}
			m_fretsNrSpin->setValue(20);
			m_tuneView->setClef(Tclef(Tclef::e_bass_F_8down));
			setTune(&Ttune::bassTunes[0]);
			m_tuneCombo->setCurrentIndex(0);
			m_stringNrSpin->setValue(Ttune::bassTunes[0].stringNr());
	} else {
			guitarDisabled(true);
			m_stringNrSpin->setValue(2); // fake two strings
			m_tuneView->setClef(Tclef(Tclef::e_treble_G));
			m_fretsNrSpin->setValue(19); // no need but keep it default
			for (int i = 0; i < 6; i++) {
				if (i < 4)
					m_tuneView->setNoteDisabled(i, true);
				m_tuneView->clearNote(i);
				m_tuneView->clearStringNumber(i);
			}
			updateAmbitus();
			m_tuneView->setNote(4, m_tuneView->lowestNote());
			m_tuneView->setNote(5, m_tuneView->highestNote());
			grabTuneFromScore(m_curentTune);
			emit tuneChanged(currentTune());
	}
	if ((Einstrument)index != e_noInstrument) {
		if (!m_accidGroup->isEnabled())
				guitarDisabled(false);
		m_tuneCombo->addItem(tr("Custom tuning"));
	}
  m_tuneView->addBGglyph(index);
	emit instrumentChanged(index);
	emit clefChanged(m_tuneView->clef());
}


void TguitarSettings::guitarDisabled(bool disabled) {
  if (disabled)
    m_tuneGroup->setTitle(scaleOfInstrText);
  else
    m_tuneGroup->setTitle(tuningGuitarText);
  m_tuneCombo->setDisabled(disabled);
  m_fretsNrSpin->setDisabled(disabled);
  m_fretNrLab->setDisabled(disabled);
  m_stringNrSpin->setDisabled(disabled);
  m_stringNrLab->setDisabled(disabled);
#if !defined (Q_OS_ANDROID)
  m_righthandCh->setDisabled(disabled);
#endif
  m_accidGroup->setDisabled(disabled);
  m_morePosCh->setDisabled(disabled);
  m_selColorBut->setDisabled(disabled);
  m_selectColorLab->setDisabled(disabled);
  m_pointColorBut->setDisabled(disabled);
  m_pointerColorLab->setDisabled(disabled);
  m_fretMarksEdit->setDisabled(disabled);
  foreach(QWidget *w, m_disabledWidgets)
    w->setDisabled(disabled);
}


void TguitarSettings::updateNotesState() {
		Ttune *tmpTune = new Ttune();
		grabTuneFromScore(tmpTune);
		for (int i = 0; i < 6; i++) {
			if (i >= 6 - tmpTune->stringNr()) {
        if (m_tuneView->getNote(i).note == 0) {
          m_tuneView->setNote(i, m_tuneView->lowestNote());
          userTune(0, Tnote());
        }
        if (m_selectInstr->instrument())
          m_tuneView->setStringNumber(i, 6 - i);
        else
          m_tuneView->clearStringNumber(i);
			}
		}
		delete tmpTune;
}


Tnote TguitarSettings::fixEmptyNote(int noteSegm) {
	Tnote nn = m_tuneView->getNote(noteSegm);
	if (m_tuneView->isNoteDisabled(noteSegm))
			nn = Tnote(); // empty because disabled
	else if (nn.note == 0) // empty because stupid
			nn = Tnote(m_tuneView->lowestNote().chromatic() + noteSegm);
	return nn;
}


void TguitarSettings::checkFretsAndStore(QList<QVariant>& fretList) {
	fretList.clear();
	QStringList fr = m_fretMarksEdit->text().split(",");
	for (int i = 0; i < fr.size(); ++i) {
		QString exMark = "";
		if (fr[i].contains("!")) {
			exMark = "!";
			fr[i].replace("!", "");
		}
		bool ok;
		int frNr = fr[i].toInt(&ok);
		if (ok && frNr > 0 && frNr <= m_fretsNrSpin->value())
			fretList << fr[i] + exMark;
	}
}


QString TguitarSettings::grabFretsFromList(const QList<QVariant>& fretList) {
	QString fretText;
	for (int i = 0; i < fretList.size(); ++i) {
		fretText.append(fretList.at(i).toString());
		if (i < fretList.size() - 1)
			fretText.append(",");
	}
	return fretText;
}


#if defined(Q_OS_WIN)
void TguitarSettings::delayedBgGlyph() {
  m_tuneView->addBGglyph(m_currentInstr);
}
#endif

