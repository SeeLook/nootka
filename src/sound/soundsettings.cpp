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
	
	QGroupBox *realAGr = new QGroupBox(this);	
    QVBoxLayout *reaALay = new QVBoxLayout;
	audioRadioButt = new QRadioButton(tr("real audio playback"), this);
	reaALay->addWidget(audioRadioButt);
	audioRadioButt->setChecked(!gl->AmidiEnabled);
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
	
	QGroupBox *midiGr = new QGroupBox(this);
	QVBoxLayout *midilay = new QVBoxLayout;
	midiRadioButt = new QRadioButton(tr("midi playback"), this);
	midilay->addWidget(midiRadioButt);
	midiRadioButt->setChecked(gl->AmidiEnabled);
	QGridLayout *midiParamLay = new QGridLayout;
	QLabel *midiPortsLab = new QLabel(tr("midi port"), this);
	midiParamLay->addWidget(midiPortsLab, 0, 0);
	midiPortsCombo = new QComboBox(this);
	midiParamLay->addWidget(midiPortsCombo, 1, 0);
	midiPortsCombo->addItems(Tplayer::getMidiPortsList());
	if (gl->AmidiPortName != "") {
		if (midiPortsCombo->count()) {
			int id = midiPortsCombo->findText(gl->AmidiPortName);
			qDebug() << id;
			if (id != -1)
				midiPortsCombo->setCurrentIndex(id);		
		} else {
			midiPortsCombo->addItem(tr("no midi ports"));
			midiPortsCombo->setDisabled(true);
		}
	}
// 	qDebug() << "works";
	QLabel *midiInstrLab = new QLabel(tr("instrument"), this);
	midiParamLay->addWidget(midiInstrLab, 0, 1);
	midiInstrCombo = new QComboBox(this);
	midiParamLay->addWidget(midiInstrCombo, 1, 1);
	midiInstrCombo->addItem(tr("grand piano"));
	midiInstrCombo->addItem(tr("classical guitar"));
	midilay->addLayout(midiParamLay);
	midiGr->setLayout(midilay);
	audioOutLay->addWidget(midiGr);

	audioOutEnableGr->setLayout(audioOutLay);
    lay->addWidget(audioOutEnableGr);
    lay->addStretch(1);
    setLayout(lay);
}

void SoundSettings::saveSettings() {
    gl->AoutSoundEnabled = audioOutEnableGr->isChecked();
    gl->AoutDeviceName = audioOutDevListCombo->currentText();
}
