/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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


#include "audioinsettings.h"
#include "tpitchview.h"
#include "tpitchfinder.h"
#include <QtGui>
#include "taudioparams.h"
#include "tnotename.h"
#include "trtaudioin.h"
#include <tvolumeslider.h>


AudioInSettings::AudioInSettings(TaudioParams* params, QString path, QWidget* parent) :
  QWidget(parent),
  m_audioIn(0),
  m_glParams(params),
  m_listGenerated(false)
{
  m_tmpParams = new TaudioParams();
  *m_tmpParams = *m_glParams;
  
  QVBoxLayout *lay = new QVBoxLayout();
  
  enableInBox = new QGroupBox(tr("enable pitch detection"), this);
  enableInBox->setCheckable(true);
  
  QVBoxLayout *inLay = new QVBoxLayout();
  
  QHBoxLayout *upLay = new QHBoxLayout();  
  QVBoxLayout *devDetLay = new QVBoxLayout(); // device & detection method layout
  
  QLabel *devLab = new QLabel(tr("input device"), this);
  devDetLay->addWidget(devLab);
  inDeviceCombo = new QComboBox(this);
  devDetLay->addWidget(inDeviceCombo);
  inDeviceCombo->setStatusTip(tr("Be sure Your input device (a mike, a webcam, an instrument pluged to line-in) is properly configured by Your operating system."));
  
  devDetLay->addStretch(1);
  
  modeGr = new QGroupBox(tr("pitch detection mode"), this);
  QGridLayout *modeLay = new QGridLayout();
  QButtonGroup *butGr = new QButtonGroup(this);
  butGr->setExclusive(true);
  instrRadio = new QRadioButton(this);
  modeLay->addWidget(instrRadio, 0, 0, Qt::AlignRight);
  butGr->addButton(instrRadio);
  QLabel *instrLab = new QLabel(tr("for playing") + 
        " <span style=\"font-family: nootka; font-size: 25px;\">g</span>", this);
  modeLay->addWidget(instrLab, 0, 1, Qt::AlignLeft);
  instrLab->setStatusTip(tr("This mode is faster and good enought for guitars and other instruments."));
  voiceRadio = new QRadioButton(this);
  modeLay->addWidget(voiceRadio, 1, 0, Qt::AlignRight);
  butGr->addButton(voiceRadio);
  QLabel *voiceLab = new QLabel(tr("for singing") + 
    " <span style=\"font-family: nootka; font-size: 25px;\">v</span>", this);
  modeLay->addWidget(voiceLab, 1, 1, Qt::AlignLeft);
  voiceLab->setStatusTip(tr("This mode is more accurate but slower. It is recommended for singing and for instruments with \"wobbly\" intonation."));
  modeGr->setLayout(modeLay);
  devDetLay->addWidget(modeGr);
  if (m_glParams->isVoice)
    voiceRadio->setChecked(true);
  else
    instrRadio->setChecked(true);
  tuneFreqlab = new QLabel(this);
  tuneFreqlab->setWordWrap(true);
  tuneFreqlab->setAlignment(Qt::AlignCenter);
  devDetLay->addWidget(tuneFreqlab);
  devDetLay->addStretch(1);
  
  /*QVBoxLayout *tarLay = new QVBoxLayout();
  QGroupBox *tarGr = new QGroupBox(this);
  QLabel *tartiniLab = new QLabel(tr("Pitch detection methods are taken from the <a href=\"http://miracle.otago.ac.nz/tartini/index.html\">Tartini Project</a> developed by<br>Philip McLeod"), this);
  tartiniLab->setWordWrap(true);
  tartiniLab->setAlignment(Qt::AlignCenter);
  tarLay->addWidget(tartiniLab);
  tarGr->setLayout(tarLay);
  devDetLay->addWidget(tarGr);
  devDetLay->addStretch(1);*/
  
  upLay->addLayout(devDetLay);
  QVBoxLayout *tunLay = new QVBoxLayout(); //middle A & threshold layout
  
  midABox = new QGroupBox(this);
//   midABox->setStatusTip(tr("Base frequency of note a<sup>1</sup>"));
  QVBoxLayout *midLay = new QVBoxLayout();
  QLabel *headLab = new QLabel("<table><tr><td valign=\"middle\">" + tr("middle A") + QString("&nbsp;&nbsp;&nbsp;<img src=\"%1\"></td></tr></table>").arg(path + "picts/middleA.png"), this);
  midLay->addWidget(headLab, 0, Qt::AlignCenter);
  QGridLayout *midGrLay = new QGridLayout;
  QLabel *frLab = new QLabel(tr("frequency:"), this);
//   midLay->addWidget(frLab);
  midGrLay->addWidget(frLab, 0, 0);
  freqSpin = new QSpinBox(this);
//   midLay->addWidget(freqSpin);
  midGrLay->addWidget(freqSpin, 0, 1);
  freqSpin->setStatusTip(tr("A pitch of detecting notes is related to this value. It also affects played sounds - for midi exaclty and for real audio it is rounded to semitones."));
  freqSpin->setMinimum(400);
  freqSpin->setMaximum(480);
//   freqSpin->setValue(int(pitch2freq(freq2pitch(440.0) + m_glParams->a440diff)));
  freqSpin->setValue(getFreq(440.0));
  freqSpin->setSuffix(" Hz");
  
  QLabel *intLab = new QLabel(tr("interval:"), this);
//   midLay->addWidget(intLab);
  midGrLay->addWidget(intLab, 1, 0);
  intervalCombo = new QComboBox(this);
//   midLay->addWidget(intervalCombo);
  midGrLay->addWidget(intervalCombo, 1, 1);
  intervalCombo->addItem(tr("semitone up"));
  intervalCombo->addItem(tr("none"));
  intervalCombo->addItem(tr("semitone down"));
  intervalCombo->setStatusTip(tr("Shifts the frequency of base a<sup>1</sup> on semitone."));
  if (freqSpin->value() <= 415)
      intervalCombo->setCurrentIndex(2);
    else if (freqSpin->value() >= 465)
      intervalCombo->setCurrentIndex(0);
    else
      intervalCombo->setCurrentIndex(1);
  midLay->addLayout(midGrLay);
  
  midABox->setLayout(midLay);
  tunLay->addWidget(midABox);
  tunLay->addStretch();
  
  volumeSlider = new TvolumeSlider(this);
  tunLay->addWidget(volumeSlider);
  volumeSlider->setValue(m_glParams->minimalVol);
  tunLay->addStretch();    
  upLay->addLayout(tunLay);
  
  inLay->addLayout(upLay);
  
  testTxt = tr("Test");
  stopTxt = tr("Stop");
  
  QGroupBox *testGr = new QGroupBox(this);
  QHBoxLayout *testLay = new QHBoxLayout();
  testButt = new QPushButton(testTxt, this);
  testButt->setStatusTip(tr("Check, are audio input settings appropirate for You,<br>and does pitch detection work?"));
  testLay->addWidget(testButt);
  testLay->addStretch(1);
  pitchView = new TpitchView(m_audioIn, this, false);
  pitchView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  testLay->addWidget(pitchView);
  pitchView->setPitchColor(palette().highlight().color());
  pitchView->setMinimalVolume(m_glParams->minimalVol);
  testLay->addStretch(1);
  QVBoxLayout *freqLay = new QVBoxLayout();
  freqLay->setAlignment(Qt::AlignCenter);
  pitchLab = new QLabel("--", this);
  pitchLab->setFixedWidth(70);
  pitchLab->setStatusTip(tr("Detected pitch"));
  pitchLab->setAlignment(Qt::AlignCenter);
  freqLay->addWidget(pitchLab);
  
  freqLab = new QLabel("--", this);
  freqLab->setFixedWidth(70);
  freqLab->setAlignment(Qt::AlignCenter);
  getFreqStatusTip();
  freqLay->addWidget(freqLab);
  testLay->addLayout(freqLay);
  testLay->addStretch(1);  
  
  testGr->setLayout(testLay);
  inLay->addWidget(testGr);
  
  enableInBox->setLayout(inLay);  
  lay->addWidget(enableInBox);
  setLayout(lay);
  
  setTestDisabled(true);
  enableInBox->setChecked(m_glParams->INenabled);
  
  connect(testButt, SIGNAL(clicked()), this, SLOT(testSlot()));
  connect(intervalCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(intervalChanged(int)));
  connect(freqSpin, SIGNAL(valueChanged(int)), this, SLOT(baseFreqChanged(int)));
  connect(volumeSlider, SIGNAL(valueChanged(float)), this, SLOT(minimalVolChanged(float)));
  
}

AudioInSettings::~AudioInSettings()
{
  pitchView->stopVolume();
  if (m_audioIn) {
    m_audioIn->stopListening();
    delete m_audioIn;
  }
  delete m_tmpParams;
}

//------------------------------------------------------------------------------------
//------------          methods       ------------------------------------------------
//------------------------------------------------------------------------------------
void AudioInSettings::setTestDisabled(bool disabled) {
  m_testDisabled = disabled;
  if (disabled) {
    pitchLab->setText("--");
    freqLab->setText("--");
    pitchLab->setDisabled(true);
    freqLab->setDisabled(true);
    // enable the rest of widget
    inDeviceCombo->setDisabled(false);
    modeGr->setDisabled(false);
    midABox->setDisabled(false);
    volumeSlider->setDisabled(false);
  } else {
    pitchLab->setDisabled(false);
    freqLab->setDisabled(false);
    // disable the rest of widget
    inDeviceCombo->setDisabled(true);
    midABox->setDisabled(true);
    modeGr->setDisabled(true);
    volumeSlider->setDisabled(true);
  }
}

void AudioInSettings::grabParams(TaudioParams *params) {
  if (freqSpin->value() == 440 )
      params->a440diff = 0.0;
  else
      params->a440diff = m_tmpParams->a440diff = getDiff(freqSpin->value());
  params->INdevName = inDeviceCombo->currentText();
  if (voiceRadio->isChecked())
      params->isVoice = true;
  else
      params->isVoice = false;
  params->INenabled = enableInBox->isChecked();
  params->minimalVol = volumeSlider->value();
}


void AudioInSettings::saveSettings() {
//   qDebug() << "AudioInSettings::saveSettings";
//   if (m_audioIn) {
//     volMeter->stopVolume();
//     m_audioIn->stopListening();
//     delete m_audioIn;
//     m_audioIn = 0;
//   }
  if (m_listGenerated)
      grabParams(m_glParams);
}

void AudioInSettings::generateDevicesList() {
  if (m_listGenerated)
    return;
  inDeviceCombo->addItems(TaudioIN::getAudioDevicesList());
  if (inDeviceCombo->count()) {
        int id = inDeviceCombo->findText(m_glParams->INdevName);
        if (id != -1)
            inDeviceCombo->setCurrentIndex(id);
    } else {
        inDeviceCombo->addItem(tr("no devices found"));
        inDeviceCombo->setDisabled(true);
  }
  m_listGenerated = true;
}

void AudioInSettings::getFreqStatusTip() {
    QString freqTxt = QString("<br><span style=\"font-family: nootka;\">6</span>E = %1Hz, ").arg(getFreq(82.5)) +
            QString("<span style=\"font-family: nootka;\">5</span>A = %1Hz, ").arg(getFreq(110.0)) + "<br>" +
            QString("<span style=\"font-family: nootka;\">4</span>d = %1Hz, ").arg(getFreq(146.0)) + 
            QString("<span style=\"font-family: nootka;\">3</span>g = %1Hz, ").arg(getFreq(195.0)) + "<br>" +
            QString("<span style=\"font-family: nootka;\">2</span>h = %1Hz, ").arg(getFreq(245.0)) +
            QString("<span style=\"font-family: nootka;\">1</span>e<sup>1</sup> = %1Hz").arg(getFreq(330.0));
    freqLab->setStatusTip(tr("Frequency of detected note. You can use it for tune") + freqTxt);
    tuneFreqlab->setText(freqTxt);
}

int AudioInSettings::getFreq(double freq) {
    return qRound((pitch2freq(freq2pitch(freq) + m_tmpParams->a440diff)));
}

float AudioInSettings::getDiff(int freq) {
   return float(freq2pitch((double)freq) - freq2pitch(440.0)); // in semitones
}


//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------


void AudioInSettings::minimalVolChanged(float vol) {
  pitchView->setMinimalVolume(vol);
}



void AudioInSettings::testSlot() {
  setTestDisabled(!m_testDisabled);
  if (!m_testDisabled) { // start a test
    grabParams(m_tmpParams);
    if (!m_audioIn)
      m_audioIn = new TaudioIN(m_tmpParams, this);
    else 
      m_audioIn->setParameters(m_tmpParams);
    testButt->setText(stopTxt);
    pitchView->setAudioInput(m_audioIn);
    m_audioIn->startListening();
    pitchView->startVolume();
    connect(m_audioIn, SIGNAL(noteDetected(Tnote)), this, SLOT(noteSlot(Tnote)));
    connect(m_audioIn, SIGNAL(fundamentalFreq(float)), this, SLOT(freqSlot(float)));
  } 
  else { // stop a test
    pitchView->stopVolume();
    m_audioIn->stopListening();
    testButt->setText(testTxt);
    setTestDisabled(true);
  }
}


void AudioInSettings::noteSlot(Tnote note) {
  pitchLab->setText("<b>" + TnoteName::noteToRichText(note) + "</b>");
}

void AudioInSettings::freqSlot(float freq) {
	freqLab->setText(QString("%1 Hz").arg(freq, 0, 'f', 1, '0'));
}

void AudioInSettings::intervalChanged(int index) {
  if (intervalCombo->hasFocus()) {
		switch (index) {
      case 0 : freqSpin->setValue(pitch2freq(70)); break;
      case 1 : freqSpin->setValue(pitch2freq(69)); break;
      case 2 : freqSpin->setValue(pitch2freq(68)); break;
		}
    m_tmpParams->a440diff = getDiff(freqSpin->value());
		getFreqStatusTip();
  }
}

void AudioInSettings::baseFreqChanged(int bFreq) {
  if (freqSpin->hasFocus()) {
    if (freqSpin->value() <= 415)
      intervalCombo->setCurrentIndex(2);
    else if (freqSpin->value() >= 465)
      intervalCombo->setCurrentIndex(0);
    else
      intervalCombo->setCurrentIndex(1);
    m_tmpParams->a440diff = getDiff(freqSpin->value());
    getFreqStatusTip();
  }
}


