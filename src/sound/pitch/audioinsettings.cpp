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
#include "taudioin.h"
#include "../tvolumemeter.h"
#include <QtGui>



AudioInSettings::AudioInSettings(QWidget* parent) :
  QWidget(parent),
  m_audioIn(0)
{
  QVBoxLayout *lay = new QVBoxLayout();
  
  enableInBox = new QGroupBox(tr("enable audio input (pitch detection)"), this);
  enableInBox->setCheckable(true);
  enableInBox->setChecked(true);
  
  QVBoxLayout *inLay = new QVBoxLayout();
  
  QHBoxLayout *upLay = new QHBoxLayout();  
  QVBoxLayout *devDetLay = new QVBoxLayout(); // device & detection method layout
  
  QLabel *devLab = new QLabel(tr("input device"), this);
  devDetLay->addWidget(devLab);
  inDeviceCombo = new QComboBox(this);
  devDetLay->addWidget(inDeviceCombo);
  QLabel *detectLab = new QLabel(tr("detection method"), this);
  devDetLay->addWidget(detectLab);
  detectMethodCombo = new QComboBox(this);
  devDetLay->addWidget(detectMethodCombo);
  detectMethodCombo->addItem("MPM");
  detectMethodCombo->addItem(tr("Autocorrelation"));
  detectMethodCombo->addItem(tr("MPM & modified cepstrum"));
  
  devDetLay->addStretch(1);
  loudChB = new QCheckBox(tr("low-pass filter"), this);
  devDetLay->addWidget(loudChB);
  voiceChB = new QCheckBox(tr("human voice"), this);
  voiceChB->setStatusTip(tr("Check this for singing."));
  devDetLay->addWidget(voiceChB);
  noiseChB = new QCheckBox(tr("noise floor"), this);
  devDetLay->addWidget(noiseChB);
  devDetLay->addStretch(1);
  
  upLay->addLayout(devDetLay);
  QVBoxLayout *tunLay = new QVBoxLayout(); //middle A & threshold layout
  
  QGroupBox *midABox = new QGroupBox(tr("middle A")+" (a1)", this);
  QVBoxLayout *midLay = new QVBoxLayout();
  QLabel *freqLab = new QLabel(tr("frequency:"), this);
  midLay->addWidget(freqLab);
  freqSpin = new QSpinBox(this);
  midLay->addWidget(freqSpin);
  freqSpin->setMinimum(420);
  freqSpin->setMaximum(460);
  freqSpin->setValue(440);
  freqSpin->setSuffix(" Hz");
  
  QLabel *intLab = new QLabel(tr("interval:"), this);
  midLay->addWidget(intLab);
  intervalCombo = new QComboBox(this);
  midLay->addWidget(intervalCombo);
  intervalCombo->addItem(tr("semitone up"));
  intervalCombo->addItem(tr("none"));
  intervalCombo->addItem(tr("semitone down"));
  intervalCombo->setCurrentIndex(1);
  midABox->setLayout(midLay);
  tunLay->addWidget(midABox);
  
  QGroupBox *noisGr = new QGroupBox(this);
  QVBoxLayout *noisLay = new QVBoxLayout();
  QLabel *threLab = new QLabel(tr("noise threshold:"), this);
  noisLay->addWidget(threLab, 1, Qt::AlignCenter);
  noiseLab = new QLabel(this);
  noisLay->addWidget(noiseLab, 1, Qt::AlignCenter);
  thresholdSlider = new QSlider(Qt::Horizontal, this);
  thresholdSlider->setMinimum(0);
  thresholdSlider->setMaximum(100);
  noisLay->addWidget(thresholdSlider);
  calcButt = new QPushButton(tr("Calculate"), this);
  noisLay->addWidget(calcButt, 1, Qt::AlignCenter);
  calcButt->setStatusTip(tr("Click to automatically detect noise level."));
  noisGr->setLayout(noisLay);
  tunLay->addWidget(noisGr);
  
  upLay->addLayout(tunLay);
  
  inLay->addLayout(upLay);
  
  QGroupBox *testGr = new QGroupBox(this);
  QHBoxLayout *testLay = new QHBoxLayout();
  testButt = new QPushButton(tr("Test"), this);
  testButt->setStatusTip(tr("Check, Are audio input settings appropirate for You,<br>and pitch detection works."));
  testLay->addWidget(testButt);
  testLay->addStretch(1);
  volMeter = new TvolumeMeter(this);
  testLay->addWidget(volMeter);
  volMeter->setStatusTip(tr("Level of a volume"));
  testLay->addStretch(1);
  pitchLab = new QLabel("--", this);
  testLay->addWidget(pitchLab);
  testLay->addStretch(1);
  pitchLab->setStatusTip(tr("Detected pitch"));
  
  
  testGr->setLayout(testLay);
  inLay->addWidget(testGr);
  
  enableInBox->setLayout(inLay);  
  lay->addWidget(enableInBox);
  setLayout(lay);
  
  inDeviceCombo->addItems(TaudioIN::getAudioDevicesList());
  volMeter->setVolume(0.5);
  setTestDisabled(true);
  
  connect(testButt, SIGNAL(clicked()), this, SLOT(testSlot()));
  connect(calcButt, SIGNAL(clicked()), this, SLOT(calcSlot()));
  
}

AudioInSettings::~AudioInSettings()
{
}

//------------------------------------------------------------------------------------
//------------          methods       --------------------------------------------------
//------------------------------------------------------------------------------------
void AudioInSettings::setTestDisabled(bool disabled) {
  m_testDisabled = disabled;
  if (disabled) {
	volMeter->setDisabled(true);
	pitchLab->setDisabled(true);
  } else {
	volMeter->setDisabled(false);
	pitchLab->setDisabled(false);
  }
}



//------------------------------------------------------------------------------------
//------------          slots       --------------------------------------------------
//------------------------------------------------------------------------------------

void AudioInSettings::calcSlot() {
  if (!m_audioIn)
	m_audioIn = new TaudioIN(this);
  m_audioIn->setAudioDevice(inDeviceCombo->currentText());
  connect(m_audioIn, SIGNAL(noiseLevel(qint16)), this, SLOT(noiseDetected(qint16)));
  m_audioIn->calculateNoiseLevel();
}

void AudioInSettings::testSlot() {
  setTestDisabled(!m_testDisabled);
}

void AudioInSettings::noiseDetected(qint16 noise) {
  int nVal = qRound(float(noise/32768.0f)*100);
  disconnect(m_audioIn, SIGNAL(noiseLevel(qint16)), this, SLOT(noiseDetected(qint16)));
  thresholdSlider->setValue(nVal);
  noiseLab->setText(QString("<b>%1 %</b>").arg(nVal));
}



