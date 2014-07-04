/***************************************************************************
 *   Copyright (C) 2014 by Tomasz Bojczuk                                  *
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

#include "tmelodysettings.h"
#include <exam/tlevel.h>
#include <widgets/troundedlabel.h>
#include <tnoofont.h>
#include <QtWidgets>

#include <QDebug>


TmelodySettings::TmelodySettings(TlevelCreatorDlg* creator) :
	TabstractLevelPage(creator)
{
	m_melodyLengthSpin = new QSpinBox(this);
		m_melodyLengthSpin->setMaximum(100);
		m_melodyLengthSpin->setMinimum(1);
		m_melodyLengthSpin->setStatusTip(tr("Number of notes in a melody."));
		
	QLabel *lenghtLab = new QLabel(tr("Melody length"), this);
	
	m_finishOnChB = new QCheckBox(tr("Melody ends on tonic note."), this);
		m_finishOnChB->setStatusTip(tr("Determines the last note of a melody.<br>When set, melody will be finished on tonic note in actual key signature."));
	m_equalTempoChB = new QCheckBox(tr("Require equal tempo"), this);
		m_equalTempoChB->setStatusTip(tr("If set, doesn't matter how fast you will play but melody has to be played whole with the same tempo."));
		
	TroundedLabel *qaLab = new TroundedLabel(tr("Melodies are available for the following questions-answers:") + 
				"<br>" + TnooFont::span("s?", 25) + "<big> -> </big>" + TnooFont::span("n!", 25) +
				"<br>" + TnooFont::span("n?", 25) + "<big> -> </big>" + TnooFont::span("s!", 25) +
				"<br>" + TnooFont::span("s?", 25) + "<big> -> </big>" + TnooFont::span("s!", 25), this);
		qaLab->setAlignment(Qt::AlignCenter);
		
	QVBoxLayout *lay = new QVBoxLayout;
	lay->addStretch();
		QHBoxLayout *melLngLay = new QHBoxLayout;
		melLngLay->addStretch(2);
		melLngLay->addWidget(lenghtLab);
		melLngLay->addStretch(1);
		melLngLay->addWidget(m_melodyLengthSpin);
		melLngLay->addStretch(2);
	lay->addLayout(melLngLay);
	lay->addStretch();
	lay->addWidget(m_finishOnChB);
	lay->addStretch();
	lay->addWidget(m_equalTempoChB);
	lay->addStretch();
	m_melGroup = new QGroupBox(this);
		m_melGroup->setLayout(lay);
	QVBoxLayout *mainLay = new QVBoxLayout;
		mainLay->addWidget(m_melGroup);
		mainLay->addStretch();
		mainLay->addWidget(qaLab);
	setLayout(mainLay);
	
	connect(m_melodyLengthSpin, SIGNAL(valueChanged(int)), this, SLOT(changedLocal()));
	connect(m_equalTempoChB, SIGNAL(clicked()), this, SLOT(changedLocal()));
	connect(m_finishOnChB, SIGNAL(clicked()), this, SLOT(changedLocal()));
}


void TmelodySettings::loadLevel(Tlevel* level) {
	blockSignals(true);
	m_melodyLengthSpin->setValue(level->melodyLen);
	m_finishOnChB->setChecked(level->endsOnTonic);
	m_equalTempoChB->setChecked(level->requireInTempo);
	changedLocal();
	saveLevel(wLevel());
	blockSignals(false);
}


void TmelodySettings::saveLevel(Tlevel* level) {
	level->melodyLen = m_melodyLengthSpin->value();
	level->endsOnTonic = m_finishOnChB->isChecked();
	level->requireInTempo = m_equalTempoChB->isChecked();
}


void TmelodySettings::changed() {
  if ((wLevel()->questionAs.isNote() && wLevel()->answersAs[TQAtype::e_asNote].isSound()) || 
			(wLevel()->questionAs.isSound() && wLevel()->answersAs[TQAtype::e_asSound].isNote()) ||
		  (wLevel()->questionAs.isSound() && wLevel()->answersAs[TQAtype::e_asSound].isSound())) {
		// melodies are supported only for:
		//        - question as note -> answer as sound (play score)
		//        - question as sound -> answer as note (dictation)
		//        - question as sound -> answer as sound (repeat melody)
			m_melGroup->setDisabled(false);
	} else { // otherwise page is disabled
			blockSignals(true);
			m_melodyLengthSpin->setValue(1);
			m_melGroup->setDisabled(true);
			blockSignals(false);
	}
}


void TmelodySettings::changedLocal() {
	if (sender() == m_melodyLengthSpin) {
			if (m_melodyLengthSpin->value() < 3) {
				m_equalTempoChB->setChecked(false);
				m_equalTempoChB->setDisabled(true);
				m_finishOnChB->setChecked(false);
				m_finishOnChB->setDisabled(true);
			} else {
				m_equalTempoChB->setDisabled(false);
				m_finishOnChB->setDisabled(false);
			}
	}
	TabstractLevelPage::changedLocal();
}









