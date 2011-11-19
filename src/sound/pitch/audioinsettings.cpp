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
  QVBoxLayout *devDetLay = new QVBoxLayout(); // device & detection method layout
  
  QLabel *devLab = new QLabel(tr("input device"), this);
  devDetLay->addWidget(devLab);
  inDeviceCombo = new QComboBox(this);
  devDetLay->addWidget(inDeviceCombo);
  QLabel *detectLab = new QLabel(tr("detection method"), this);
  devDetLay->addWidget(detectLab);
  detectMethodCombo = new QComboBox(this);
  devDetLay->addWidget(detectMethodCombo);
  detectMethodCombo->addItem("Mpm");
  detectMethodCombo->addItem(tr("Auto correlation"));
  detectMethodCombo->addItem(tr("Mpm & modified cepstrum"));
  
  inLay->addLayout(devDetLay);  
  
  enableInBox->setLayout(inLay);  
  lay->addWidget(enableInBox);
  setLayout(lay);
  
  inDeviceCombo->addItems(TaudioIN::getAudioDevicesList());
  
}

AudioInSettings::~AudioInSettings()
{
}


