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


#include "audiooutsettings.h"
#include "taudioout.h"
#include <QtGui>
#include "taudioparams.h"
#include <QDebug>


AudioOutSettings::AudioOutSettings(TaudioParams* aParams, QWidget* parent) :
    QWidget(parent),
    m_params(aParams),
    m_firstTime(true)
{
    QVBoxLayout *lay = new QVBoxLayout;

    audioOutEnableGr = new QGroupBox(tr("play sound"), this);
    audioOutEnableGr->setStatusTip(tr("Selected notes and guitar positions will be played."));
    audioOutEnableGr->setCheckable(true);
    audioOutEnableGr->setChecked(m_params->OUTenabled ); 

    QVBoxLayout *audioOutLay = new QVBoxLayout;

    audioRadioButt = new QRadioButton(tr("real audio playback"), this);
    audioRadioButt->setStatusTip(tr("Natural, high quality sound of classical guitar<br>but limited to scale <b>from C<sub>1</sub> to e<sup>3</sup></b>"));
    audioOutLay->addWidget(audioRadioButt);
    realAGr = new QGroupBox(this);
    realAGr->setStatusTip(audioRadioButt->statusTip());
    QVBoxLayout *reaALay = new QVBoxLayout;
    QLabel *outDevLab = new QLabel(tr("audio device for output"), this);
    reaALay->addWidget(outDevLab);
    audioOutDevListCombo = new QComboBox(this);
    reaALay->addWidget(audioOutDevListCombo);
    audioOutDevListCombo->addItems(TaudioOUT::getAudioDevicesList());
    if (audioOutDevListCombo->count()) {
        int id = audioOutDevListCombo->findText(m_params->OUTdevName);
        if (id != -1)
            audioOutDevListCombo->setCurrentIndex(id);
    } else {
        audioOutDevListCombo->addItem(tr("no devices found"));
        audioOutDevListCombo->setDisabled(true);
    }
    reaALay->addStretch(1);
    realAGr->setLayout(reaALay);
    audioOutLay->addWidget(realAGr);
    
    midiRadioButt = new QRadioButton(tr("midi playback"), this);
    midiRadioButt->setStatusTip(tr("Artificial, low quality sound (depends on hardware)<br>but there is possibility to select an instrument<br>and whole scale can be played."));
    audioOutLay->addWidget(midiRadioButt);
    midiGr = new QGroupBox(this);
    midiGr->setStatusTip(midiRadioButt->statusTip());
    QVBoxLayout *midilay = new QVBoxLayout;
    QGridLayout *midiParamLay = new QGridLayout;
    QLabel *midiPortsLab = new QLabel(tr("midi port"), this);
    midiParamLay->addWidget(midiPortsLab, 0, 0);
    midiPortsCombo = new QComboBox(this);
    midiParamLay->addWidget(midiPortsCombo, 1, 0);
    midiPortsCombo->addItems(TaudioOUT::getMidiPortsList());
    if (m_params->midiPortName != "") {
      if (midiPortsCombo->count()) {
        int id = midiPortsCombo->findText(m_params->midiPortName);
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
    addInstrument(tr("Grand Piano"), 0);
    addInstrument(tr("Harpsichord"), 6);
    addInstrument(tr("Classical Guitar"), 24);
    addInstrument(tr("Acoustic Guitar"), 25);
    addInstrument(tr("Electric Guitar"), 27);
    addInstrument(tr("Electric Guitar (Overdriven)"), 29);
    addInstrument(tr("Bass Guitar"), 33);
    addInstrument(tr("Violin"), 40);
    addInstrument(tr("Sax"), 66);
    addInstrument(tr("Flute"), 73);
    for(int i = 0; i < instruments.size(); i++) {
      midiInstrCombo->addItem(instruments[i].name);
      if (instruments[i].progNr == m_params->midiInstrNr)
        midiInstrCombo->setCurrentIndex(i);
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
    audioRadioButt->setChecked(!m_params->midiEnabled);
    midiRadioButt->setChecked(m_params->midiEnabled);
    audioOrMidiChanged();
    
    connect(radioGr, SIGNAL(buttonClicked(int)), this, SLOT(audioOrMidiChanged()));
    
    setFocusPolicy(Qt::StrongFocus);
    
}

void AudioOutSettings::saveSettings() {
    m_params->OUTenabled = audioOutEnableGr->isChecked();
    m_params->OUTdevName = audioOutDevListCombo->currentText();
    m_params->midiEnabled = midiRadioButt->isChecked();
    m_params->midiInstrNr = instruments[midiInstrCombo->currentIndex()].progNr;
    m_params->midiPortName = midiPortsCombo->currentText();
}

void AudioOutSettings::audioOrMidiChanged() {
	if (audioRadioButt->isChecked()) {
		realAGr->setDisabled(false);
		midiGr->setDisabled(true);
	} else {
		realAGr->setDisabled(true);
		midiGr->setDisabled(false);
	}
}

void AudioOutSettings::addInstrument(QString name, unsigned char midiNr) {
	TmidiInstrListStruct mi;
	mi.name = name;
	mi.progNr = midiNr;
	instruments << mi;
}

void AudioOutSettings::focusInEvent(QFocusEvent* ) {
    qDebug("out: focus");
}

