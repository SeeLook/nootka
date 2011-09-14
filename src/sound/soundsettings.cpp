/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "soundsettings.h"
#include "tplayer.h"
#include "tglobals.h"
#include <QtGui>

extern Tglobals *gl;

SoundSettings::SoundSettings(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout;

    audioOutEnableGr = new QGroupBox(tr("play sound"), this);
    audioOutEnableGr->setStatusTip(tr("Selected notes and guitar positions will be played."));
    audioOutEnableGr->setCheckable(true);
    audioOutEnableGr->setChecked(gl->AoutSoundEnabled);
	
	QVBoxLayout *audioOutLay = new QVBoxLayout;
	
	audioRadioButt = new QRadioButton(tr("real audio playback"), this);
	audioOutLay->addWidget(audioRadioButt);
	realAGr = new QGroupBox(this);	
    QVBoxLayout *reaALay = new QVBoxLayout;
    QLabel *outDevLab = new QLabel(tr("audio device for output"), this);
    reaALay->addWidget(outDevLab);
    audioOutDevListCombo = new QComboBox(this);
    reaALay->addWidget(audioOutDevListCombo);
    audioOutDevListCombo->addItems(Tplayer::getAudioDevicesList());
    if (audioOutDevListCombo->count()) {
        int id = audioOutDevListCombo->findText(gl->AoutDeviceName);
        if (id != -1)
            audioOutDevListCombo->setCurrentIndex(id);
    } else {
        audioOutDevListCombo->addItem(tr("no devices found"));
        audioOutDevListCombo->setDisabled(true);
    }
    reaALay->addStretch(1);
    QLabel *outRangeLab = new QLabel(tr("The only notes from C<sub>1</sub> to e<sup>3</sup> are played !!"), this);
    reaALay->addWidget(outRangeLab, 0, Qt::AlignCenter);
	realAGr->setLayout(reaALay);
	audioOutLay->addWidget(realAGr);
	
	midiRadioButt = new QRadioButton(tr("midi playback"), this);
	audioOutLay->addWidget(midiRadioButt);
	midiGr = new QGroupBox(this);
	QVBoxLayout *midilay = new QVBoxLayout;
	QGridLayout *midiParamLay = new QGridLayout;
	QLabel *midiPortsLab = new QLabel(tr("midi port"), this);
	midiParamLay->addWidget(midiPortsLab, 0, 0);
	midiPortsCombo = new QComboBox(this);
	midiParamLay->addWidget(midiPortsCombo, 1, 0);
	midiPortsCombo->addItems(Tplayer::getMidiPortsList());
	if (gl->AmidiPortName != "") {
		if (midiPortsCombo->count()) {
			int id = midiPortsCombo->findText(gl->AmidiPortName);
			if (id != -1)
				midiPortsCombo->setCurrentIndex(id);		
		} else {
			midiPortsCombo->addItem(tr("no midi ports"));
			midiPortsCombo->setDisabled(true);
		}
	}
	QLabel *midiInstrLab = new QLabel(tr("instrument"), this);
	midiParamLay->addWidget(midiInstrLab, 0, 1);
	midiInstrCombo = new QComboBox(this);
	midiParamLay->addWidget(midiInstrCombo, 1, 1);
	instruments.insert(tr("Grand Piano"), 0);
	instruments.insert(tr("Harpsichord"), 6);
	instruments.insert(tr("Classical Guitar"), 24);
	instruments.insert(tr("Acoustic Guitar"), 25);
	instruments.insert(tr("Electric Guitar"), 27);
	instruments.insert(tr("Electric Guitar (Overdriven)"), 29);
	instruments.insert(tr("Bass Guitar"), 33);
	instruments.insert(tr("Violin"), 40);
	instruments.insert(tr("Flute"), 73);
// 	instruments.insert(tr(""), 0);
	QHashIterator<QString, int> i(instruments);
	int id = 0;
	while(i.hasNext()) {
	  i.next();
	  midiInstrCombo->addItem(i.key());
	  if (i.value() == gl->AmidiInstrNr)
		  midiInstrCombo->setCurrentIndex(id);
	  id++;
	}

	midilay->addLayout(midiParamLay);
	midiGr->setLayout(midilay);
	audioOutLay->addWidget(midiGr);

	audioOutEnableGr->setLayout(audioOutLay);
    lay->addWidget(audioOutEnableGr);
    lay->addStretch(1);
    setLayout(lay);
	
	QButtonGroup *radioGr = new QButtonGroup(this);
	radioGr->addButton(audioRadioButt);
	radioGr->addButton(midiRadioButt);
	audioRadioButt->setChecked(!gl->AmidiEnabled);
	midiRadioButt->setChecked(gl->AmidiEnabled);
	audioOrMidiChanged();
	
	connect(radioGr, SIGNAL(buttonClicked(int)), this, SLOT(audioOrMidiChanged()));
	
}

void SoundSettings::saveSettings() {
    gl->AoutSoundEnabled = audioOutEnableGr->isChecked();
    gl->AoutDeviceName = audioOutDevListCombo->currentText();
	gl->AmidiEnabled = midiRadioButt->isChecked();
	gl->AmidiInstrNr = instruments.value(midiInstrCombo->currentText());
	gl->AmidiPortName = midiPortsCombo->currentText();
}

void SoundSettings::audioOrMidiChanged() {
	if (audioRadioButt->isChecked()) {
		realAGr->setDisabled(false);
		midiGr->setDisabled(true);
	} else {
		realAGr->setDisabled(true);
		midiGr->setDisabled(false);
	}
}

