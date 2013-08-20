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


#include "audiooutsettings.h"
#include <QtGui>
#include "taudioparams.h"
#include "tmidiout.h"
#include "trtaudioout.h"
#include <tinstrument.h>


AudioOutSettings::AudioOutSettings(TaudioParams* aParams, QWidget* parent) :
    QWidget(parent),
    m_params(aParams),
    m_listGenerated(false)
{
    QVBoxLayout *lay = new QVBoxLayout;

    m_audioOutEnableGr = new QGroupBox(tr("play sound"), this);
    m_audioOutEnableGr->setStatusTip(tr("Selected notes and guitar positions will be played."));
    m_audioOutEnableGr->setCheckable(true);
    m_audioOutEnableGr->setChecked(m_params->OUTenabled ); 

    QVBoxLayout *audioOutLay = new QVBoxLayout;

    m_audioRadioButt = new QRadioButton(tr("real audio playback"), this);
//     m_audioRadioButt->setStatusTip(tr("Natural, high quality sound of classical guitar<br>but limited to scale <b>from C<sub>1</sub> to e<sup>3</sup></b>"));
    audioOutLay->addWidget(m_audioRadioButt);
    m_realAGr = new QGroupBox(this);
    m_realAGr->setStatusTip(m_audioRadioButt->statusTip());
    QVBoxLayout *realLay = new QVBoxLayout;
    QLabel *outDevLab = new QLabel(tr("audio device for output"), this);
    realLay->addWidget(outDevLab);
    m_audioOutDevListCombo = new QComboBox(this);
    realLay->addWidget(m_audioOutDevListCombo);
		m_audioInstrCombo = new QComboBox(this);
		realLay->addWidget(m_audioInstrCombo);

    realLay->addStretch(1);
    m_realAGr->setLayout(realLay);
    audioOutLay->addWidget(m_realAGr);
    
    m_midiRadioButt = new QRadioButton(tr("midi playback"), this);
//     m_midiRadioButt->setStatusTip(tr("Artificial, low quality sound (depends on hardware)<br>but there is possibility to select an instrument<br>and whole scale can be played."));
    audioOutLay->addWidget(m_midiRadioButt);
    m_midiGr = new QGroupBox(this);
    m_midiGr->setStatusTip(m_midiRadioButt->statusTip());
    QVBoxLayout *midilay = new QVBoxLayout;
    QGridLayout *midiParamLay = new QGridLayout;
    QLabel *midiPortsLab = new QLabel(tr("midi port"), this);
    midiParamLay->addWidget(midiPortsLab, 0, 0);
    m_midiPortsCombo = new QComboBox(this);
    midiParamLay->addWidget(m_midiPortsCombo, 1, 0);
    m_midiPortsCombo->addItems(TmidiOut::getMidiPortsList());
//     if (m_params->midiPortName != "") {
//       if (midiPortsCombo->count()) {
//         int id = midiPortsCombo->findText(m_params->midiPortName);
//         if (id != -1)
//           midiPortsCombo->setCurrentIndex(id);		
//       } else {
//         midiPortsCombo->addItem(tr("no midi ports"));
//         midiPortsCombo->setDisabled(true);
//       }
//     }
    QLabel *midiInstrLab = new QLabel(tr("instrument"), this);
    midiParamLay->addWidget(midiInstrLab, 2, 0);
    m_midiInstrCombo = new QComboBox(this);
    midiParamLay->addWidget(m_midiInstrCombo, 3, 0);
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
      m_midiInstrCombo->addItem(instruments[i].name);
      if (instruments[i].progNr == m_params->midiInstrNr)
        m_midiInstrCombo->setCurrentIndex(i);
    }

    midilay->addLayout(midiParamLay);
    
    m_midiGr->setLayout(midilay);
    audioOutLay->addWidget(m_midiGr);

    m_audioOutEnableGr->setLayout(audioOutLay);
      lay->addWidget(m_audioOutEnableGr);
      lay->addStretch(1);
      setLayout(lay);
    
    QButtonGroup *radioGr = new QButtonGroup(this);
    radioGr->addButton(m_audioRadioButt);
    radioGr->addButton(m_midiRadioButt);
    m_audioRadioButt->setChecked(!m_params->midiEnabled);
    m_midiRadioButt->setChecked(m_params->midiEnabled);
    
    audioOrMidiChanged();
		m_audioInstrCombo->addItem(instruments[2].name); // Classical guitar
		m_audioInstrCombo->addItem(instruments[4].name); // Electric guitar
		m_audioInstrCombo->addItem(instruments[6].name); // Bass guitar
		m_audioInstrCombo->setCurrentIndex(m_params->audioInstrNr - 1);
    
    connect(radioGr, SIGNAL(buttonClicked(int)), this, SLOT(audioOrMidiChanged()));
    
    setFocusPolicy(Qt::StrongFocus);
    
}

void AudioOutSettings::generateDevicesList() {
  if (m_listGenerated)
    return;
  setDevicesCombo();
  m_listGenerated = true;
}


void AudioOutSettings::setDevicesCombo() {
  m_audioOutDevListCombo->clear();
  m_audioOutDevListCombo->addItems(TaudioOUT::getAudioDevicesList());
    if (m_audioOutDevListCombo->count()) {
        int id = m_audioOutDevListCombo->findText(m_params->OUTdevName);
        if (id != -1)
            m_audioOutDevListCombo->setCurrentIndex(id);
    } else {
        m_audioOutDevListCombo->addItem(tr("no devices found"));
        m_audioOutDevListCombo->setDisabled(true);
  }
  if (m_params->midiPortName != "") {
      if (m_midiPortsCombo->count()) {
        int id = m_midiPortsCombo->findText(m_params->midiPortName);
        if (id != -1)
          m_midiPortsCombo->setCurrentIndex(id);    
      } else {
        m_midiPortsCombo->addItem(tr("no midi ports"));
        m_midiPortsCombo->setDisabled(true);
      }
    }
}


void AudioOutSettings::saveSettings() {
  if (m_listGenerated) { // save only when user opened a tab
    m_params->OUTenabled = m_audioOutEnableGr->isChecked();
    m_params->OUTdevName = m_audioOutDevListCombo->currentText();
    m_params->midiEnabled = m_midiRadioButt->isChecked();
    m_params->midiInstrNr = instruments[m_midiInstrCombo->currentIndex()].progNr;
    m_params->midiPortName = m_midiPortsCombo->currentText();
		m_params->audioInstrNr = m_audioInstrCombo->currentIndex() + 1;
  }
}


void AudioOutSettings::restoreDefaults() {
		m_audioRadioButt->setChecked(true);
		m_audioOutEnableGr->setChecked(true);
		m_audioOutDevListCombo->setCurrentIndex(0);
		m_midiInstrCombo->setCurrentIndex(2); // classical guitar
		m_audioInstrCombo->setCurrentIndex(0);
}




void AudioOutSettings::audioOrMidiChanged() {
	if (m_audioRadioButt->isChecked()) {
		m_realAGr->setDisabled(false);
		m_midiGr->setDisabled(true);
	} else {
		m_realAGr->setDisabled(true);
		m_midiGr->setDisabled(false);
	}
}


void AudioOutSettings::addInstrument(QString name, unsigned char midiNr) {
	TmidiInstrListStruct mi;
	mi.name = name;
	mi.progNr = midiNr;
	instruments << mi;
}


void AudioOutSettings::adjustOutToInstrument(TaudioParams* out, int instr) {
	if (out->midiEnabled) {
			switch((Einstrument)instr) {
				case e_classicalGuitar:
						out->midiInstrNr = 24; break;
				case e_electricGuitar:
						out->midiInstrNr = 29; break;
				case e_bassGuitar:
						out->midiInstrNr = 33; break;
			}
	} else
		out->audioInstrNr = instr;
}



void AudioOutSettings::whenInstrumentChanged(int instr) {
	adjustOutToInstrument(m_params, instr);
	if (m_params->midiEnabled)
		m_midiRadioButt->setChecked(true);
	else
		m_audioRadioButt->setChecked(true);
	m_audioInstrCombo->setCurrentIndex(instr - 1);
	audioOrMidiChanged();
}






