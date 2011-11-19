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
#include <QtGui>



AudioInSettings::AudioInSettings(QWidget* parent) :
  QWidget(parent)
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
  
  loudChB = new QCheckBox(tr("low-pass filter"), this);
  devDetLay->addWidget(loudChB);
  voiceChB = new QCheckBox(tr("human voice"), this);
  voiceChB->setStatusTip(tr("Check this for singing."));
  devDetLay->addWidget(voiceChB);
  noiseChB = new QCheckBox(tr("noise floor"), this);
  devDetLay->addWidget(noiseChB);
  
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
  
  QLabel *threLab = new QLabel(tr("noise threshold:"), this);
  tunLay->addWidget(threLab);
  thresholdSlider = new QSlider(Qt::Horizontal, this);
  tunLay->addWidget(thresholdSlider);
  
  upLay->addLayout(tunLay);
  
  inLay->addLayout(upLay);
  
  enableInBox->setLayout(inLay);  
  lay->addWidget(enableInBox);
  setLayout(lay);
  
  inDeviceCombo->addItems(TaudioIN::getAudioDevicesList());
  
}

AudioInSettings::~AudioInSettings()
{
}


