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

#include "tguitarsettings.h"
#include "tcolorbutton.h"
#include "ttune.h"
#include "tglobals.h"
#include "tsimplescore.h"
#include <QtGui>



extern Tglobals *gl;

TguitarSettings::TguitarSettings(QWidget *parent) :
        QWidget(parent)
{
    
    QVBoxLayout *mainLay = new QVBoxLayout;
    mainLay->setAlignment(Qt::AlignCenter);

    QHBoxLayout *upLay = new QHBoxLayout;
    m_tuneGroup = new QGroupBox(tr("tune of the guitar"));
    m_tuneGroup->setStatusTip(tr("Select appropirate tune from the list or prepare your own.") + "<br>" + 
				tr("Remember to select appropirate clef in Score settings."));
    QVBoxLayout *tuneLay = new QVBoxLayout;
    tuneLay->setAlignment(Qt::AlignCenter);
    m_tuneCombo = new QComboBox(this);
    tuneLay->addWidget(m_tuneCombo);
    m_tuneView = new TsimpleScore(7, this);
    tuneLay->addWidget(m_tuneView);
// 		m_tuneView->setClefDisabled(true);
    m_tuneView->setClef(gl->Sclef); 
    m_tuneView->setNoteDisabled(6, true); // 7-th is dummy to get more space
   
    m_tuneGroup->setLayout(tuneLay);
    upLay->addWidget(m_tuneGroup);

    QVBoxLayout *guitarLay = new QVBoxLayout;
    m_guitarGroup = new QGroupBox(tr("Guitar:"), this);
	// Selecting guitar type combo
		m_instrumentTypeCombo = new QComboBox(this);
		guitarLay->addWidget(m_instrumentTypeCombo, 0, Qt::AlignCenter);
		m_instrumentTypeCombo->addItem(tr("disabled"));
		QModelIndex in = m_instrumentTypeCombo->model()->index(0, 0);
		QVariant v(0);
		m_instrumentTypeCombo->model()->setData(in, v, Qt::UserRole - 1);
		m_instrumentTypeCombo->addItem(instrumentToText(e_classicalGuitar));
		m_instrumentTypeCombo->addItem(instrumentToText(e_electricGuitar));
		in = m_instrumentTypeCombo->model()->index(2, 0);
// 		QVariant v(0);
		m_instrumentTypeCombo->model()->setData(in, v, Qt::UserRole - 1);
		m_instrumentTypeCombo->addItem(instrumentToText(e_bassGuitar));
		guitarLay->addStretch(1);
	// Righthanded/lefthanded check box
    m_righthandCh = new QCheckBox(tr("guitar for right-handed"),this);
    m_righthandCh->setChecked(gl->GisRightHanded);
    m_righthandCh->setStatusTip(tr("Uncheck this if you are lefthanded<br>and your gitar has changed strings' order"));
    guitarLay->addWidget(m_righthandCh);
    guitarLay->addStretch(1);
	// Number of frets
    m_fretNrLab = new QLabel(tr("number of frets:"),this);
    guitarLay->addWidget(m_fretNrLab);
    m_fretsNrSpin = new QSpinBox(this);
    m_fretsNrSpin->setValue(gl->GfretsNumber);
    m_fretsNrSpin->setMaximum(24);
    m_fretsNrSpin->setMinimum(15);
    guitarLay->addWidget(m_fretsNrSpin);
    guitarLay->addStretch(1);
	// Number of strings
		m_stringNrLab = new QLabel(tr("number of strings:"), this);
		guitarLay->addWidget(m_stringNrLab);
		m_stringNrSpin = new QSpinBox(this);
		m_stringNrSpin->setMaximum(6);
		m_stringNrSpin->setMinimum(3);
		guitarLay->addWidget(m_stringNrSpin);
		guitarLay->addStretch(1);
    upLay->addSpacing(3);
    m_guitarGroup->setLayout(guitarLay);
    upLay->addWidget(m_guitarGroup);

    mainLay->addLayout(upLay);

    QHBoxLayout *downLay = new QHBoxLayout;
    QVBoxLayout *prefLay = new QVBoxLayout;
    m_accidGroup = new QGroupBox(tr("prefered accidentals:"),this);
    m_accidGroup->setStatusTip(tr("Choose which accidentals will be shown in the score."));
    m_prefSharpBut = new QRadioButton(tr("# - sharps"),this);
    m_prefFlatBut = new  QRadioButton(tr("b - flats"),this);
    QButtonGroup *prefGr = new QButtonGroup(this);
    prefGr->addButton(m_prefSharpBut);
    prefGr->addButton(m_prefFlatBut);
    prefLay->addWidget(m_prefSharpBut);
    prefLay->addWidget(m_prefFlatBut);
    m_accidGroup->setLayout(prefLay);
    if (gl->GpreferFlats) m_prefFlatBut->setChecked(true);
    else
        m_prefSharpBut->setChecked(true);
    downLay->addWidget(m_accidGroup);

    m_morePosCh = new QCheckBox(tr("show all possibilities of a note"),this);
    m_morePosCh->setStatusTip(tr("As you know, the same note can be played in few places on a fingerboard.<br>If checked, all of them are showed."));
    downLay->addWidget(m_morePosCh);
    m_morePosCh->setChecked(gl->GshowOtherPos);

    mainLay->addLayout(downLay);
    QGridLayout *colorLay = new QGridLayout;
    m_pointerColorLab = new QLabel(tr("color of string/fret pointer"), this);
    m_pointColorBut = new TcolorButton(gl->GfingerColor, this);
    colorLay->addWidget(m_pointerColorLab, 0, 0, Qt::AlignRight);
    colorLay->addWidget(m_pointColorBut, 0 ,1, Qt::AlignLeft);
    m_selectColorLab = new QLabel(tr("color of selected string/fret"), this);
    m_selColorBut = new TcolorButton(gl->GselectedColor, this);
    colorLay->addWidget(m_selectColorLab, 1, 0, Qt::AlignRight);
    colorLay->addWidget(m_selColorBut, 1, 1, Qt::AlignLeft);
    mainLay->addLayout(colorLay);

    setLayout(mainLay);
		
		updateAmbitus();

    connect(m_tuneCombo, SIGNAL(activated(int)), this, SLOT(tuneSelected(int)));
    connect(m_tuneView, SIGNAL(noteWasChanged(int,Tnote)), this, SLOT(userTune(int,Tnote)));
		connect(m_tuneView, SIGNAL(pianoStaffSwitched()), this, SLOT(switchedToPianoStaff()));
		connect(m_tuneView, SIGNAL(clefChanged(Tclef)), this, SLOT(onClefChanged(Tclef)));
		connect(m_instrumentTypeCombo, SIGNAL(activated(int)), this, SLOT(instrumentTypeChanged(int)));
		connect(m_stringNrSpin, SIGNAL(valueChanged(int)), this, SLOT(stringNrChanged(int)));
		int instrumentIndex = (int)gl->instrument;
		m_instrumentTypeCombo->setCurrentIndex(instrumentIndex);
		instrumentTypeChanged(instrumentIndex);
		setTune(gl->Gtune());
		if (gl->instrument != e_noInstrument) {
				if (*gl->Gtune() == Ttune::stdTune)
						m_tuneCombo->setCurrentIndex(0);
				for (int i = 0; i < 4; i++) {
					if (gl->instrument == e_classicalGuitar) {
						if (*gl->Gtune() == Ttune::tunes[i])
								m_tuneCombo->setCurrentIndex(i + 1);
					} else if (gl->instrument == e_bassGuitar) {
							if (*gl->Gtune() == Ttune::bassTunes[i])
								m_tuneCombo->setCurrentIndex(i);
					}
				}
				QString S = tr("Custom tune");
				if (gl->Gtune()->name == S)
						m_tuneCombo->setCurrentIndex(m_tuneCombo->count() - 1);
		}
}


void TguitarSettings::saveSettings() {
    gl->GisRightHanded = m_righthandCh->isChecked();
    gl->GfretsNumber = m_fretsNrSpin->value();
		Ttune tmpTune = Ttune(m_tuneCombo->currentText(), m_tuneView->getNote(5), m_tuneView->getNote(4),
											m_tuneView->getNote(3), m_tuneView->getNote(2), m_tuneView->getNote(1), m_tuneView->getNote(0));
    gl->setTune(tmpTune);
    gl->GshowOtherPos = m_morePosCh->isChecked();
    if (m_prefFlatBut->isChecked()) 
				gl->GpreferFlats = true;
    else 
				gl->GpreferFlats = false;
    gl->GfingerColor = m_pointColorBut->getColor();
    gl->GfingerColor.setAlpha(200);
    gl->GselectedColor = m_selColorBut->getColor();
		gl->instrument = (Einstrument)m_instrumentTypeCombo->currentIndex();
}


void TguitarSettings::restoreDefaults() {
		instrumentTypeChanged(1); // It will restore tune (standard), frets and strings number and clef
		m_righthandCh->setChecked(true);
		m_prefSharpBut->setChecked(true);
		m_morePosCh->setChecked(false);
		m_pointColorBut->setColor(gl->invertColor(palette().highlight().color()));
		m_selColorBut->setColor(palette().highlight().color());
}


Tnote TguitarSettings::lowestNote() {
// 		int lowest = -1;
// 		char loNr = 127;
// 		for (int i = 0; i < 6; i++) {
// 			if (m_tuneView->getNote(i).note) {
// 				if (m_tuneView->getNote(i).getChromaticNrOfNote() < loNr) {
// 					loNr = m_tuneView->getNote(i).getChromaticNrOfNote();
// 					lowest = i;
// 				}
// 			}
// 		}
// 		if (lowest > -1)
// 			return m_tuneView->getNote(lowest);
// 		else
			return m_tuneView->lowestNote();
	/** it should be quite enoght to determine pitch detection range.
	 * For other purposes enable the above code. */
}


//##########################################################################################################
//########################################## PRIVATE #######################################################
//##########################################################################################################

void TguitarSettings::setTune(Ttune* tune) {
    for (int i = 0; i < 6; i++) {
				m_tuneView->setNote(i, tune->str(6 - i));
				m_tuneView->setNoteDisabled(i, !(bool)tune->str(6 - i).note);
				if (tune->str(6 - i).note)
					m_tuneView->setStringNumber(i, 6 - i);
				else
					m_tuneView->clearStringNumber(i);
    }
    m_stringNrSpin->setValue(tune->stringNr());
}


void TguitarSettings::updateAmbitus() {
	for (int i = 0; i < 6; i++)
		m_tuneView->setAmbitus(i, m_tuneView->lowestNote(), 
													 Tnote(m_tuneView->highestNote().getChromaticNrOfNote() - m_fretsNrSpin->value()));
}

//##########################################################################################################
//########################################## PRIVATE SLOTS #################################################
//##########################################################################################################

void TguitarSettings::tuneSelected(int tuneId) {
	if (m_instrumentTypeCombo->currentIndex() == 1) { // classical guitar
    if (tuneId == 0)
        setTune(&Ttune::stdTune);
    else 
			if (tuneId != m_tuneCombo->count() - 1) //the last is custom
						setTune(&Ttune::tunes[tuneId - 1]);
	} else if (m_instrumentTypeCombo->currentIndex() == 3) { // bass guitar
			if (tuneId != m_tuneCombo->count() - 1) //the last is custom
				setTune(&Ttune::bassTunes[tuneId ]);
	}
}


void TguitarSettings::userTune(int, Tnote) {
    m_tuneCombo->setCurrentIndex(m_tuneCombo->count() - 1);
}


void TguitarSettings::onClefChanged(Tclef clef) {
		// this is not piano staff - we don't need updateAmbitus()
		updateNotesState();
		emit clefChanged(clef);
		emit lowestNoteChanged(m_tuneView->lowestNote());
}


void TguitarSettings::switchedToPianoStaff() {
		updateAmbitus();
		updateNotesState();
		emit clefChanged(currentClef());
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
		m_tuneCombo->setCurrentIndex(m_tuneCombo->count() - 1);
}


void TguitarSettings::instrumentTypeChanged(int index) {
	m_tuneCombo->clear();
	if ((Einstrument)index == e_classicalGuitar) {
		m_tuneCombo->addItem(Ttune::stdTune.name);
    for (int i = 0; i < 4; i++) {
        m_tuneCombo->addItem(Ttune::tunes[i].name);
    }
    m_fretsNrSpin->setValue(19);
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
	}
	if ((Einstrument)index != e_noInstrument) {
		if (!m_accidGroup->isEnabled())
				guitarDisabled(false);
		m_tuneCombo->addItem(tr("Custom tune"));
	}
}


void TguitarSettings::guitarDisabled(bool disabled) {
		m_tuneGroup->setDisabled(disabled);
		m_fretsNrSpin->setDisabled(disabled);
		m_fretNrLab->setDisabled(disabled);
		m_stringNrSpin->setDisabled(disabled);
		m_stringNrLab->setDisabled(disabled);
		m_righthandCh->setDisabled(disabled);
		m_accidGroup->setDisabled(disabled);
		m_morePosCh->setDisabled(disabled);
		m_selColorBut->setDisabled(disabled);
		m_selectColorLab->setDisabled(disabled);
		m_pointColorBut->setDisabled(disabled);
		m_pointerColorLab->setDisabled(disabled);
}


void TguitarSettings::updateNotesState() {
		Ttune tmpTune = Ttune(m_tuneCombo->currentText(), m_tuneView->getNote(5), m_tuneView->getNote(4),
											m_tuneView->getNote(3), m_tuneView->getNote(2), m_tuneView->getNote(1), m_tuneView->getNote(0));
		for (int i = 0; i < 6; i++) {
			if (i >= 6 - tmpTune.stringNr()) {
					if (m_tuneView->getNote(i).note == 0) {
						m_tuneView->setNote(i, m_tuneView->lowestNote());
						userTune(0, Tnote());
					}
					m_tuneView->setStringNumber(i, 6 - i);
			} else {
					m_tuneView->setNoteDisabled(i, true);
			}
		}
}



