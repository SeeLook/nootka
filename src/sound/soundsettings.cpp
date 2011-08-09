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

    QVBoxLayout *outLay = new QVBoxLayout;
    QLabel *outDevLab = new QLabel(tr("audio device for output"), this);
    outLay->addWidget(outDevLab);
    audioOutDevListCombo = new QComboBox(this);
    outLay->addWidget(audioOutDevListCombo);
    audioOutDevListCombo->addItems(Tplayer::getAudioDevicesList());
    int id = audioOutDevListCombo->findText(gl->AoutDeviceName);
    if (id != -1)
        audioOutDevListCombo->setCurrentIndex(id);
    audioOutEnableGr->setLayout(outLay);
    outLay->addStretch(1);
    QLabel *outRangeLab = new QLabel(tr("The only notes from C<sub>1</sub> to e<sup>3</sup> are played !!"), this);
    outLay->addWidget(outRangeLab, 0, Qt::AlignCenter);

    lay->addWidget(audioOutEnableGr);
    lay->addStretch(1);
    setLayout(lay);
}

void SoundSettings::saveSettings() {
    gl->AoutSoundEnabled = audioOutEnableGr->isChecked();
    gl->AoutDeviceName = audioOutDevListCombo->currentText();
}
