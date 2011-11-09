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

#include "mainwindow.h"
#include <QtGui>
#include "taudioin.h"

// #include "tnote.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{

    QWidget *widget = new QWidget(this);

    QVBoxLayout *mLay = new QVBoxLayout;

    devListCombo = new QComboBox(widget);
    mLay->addWidget(devListCombo);
    devNameLab = new QLabel(widget);
    mLay->addWidget(devNameLab);
	startSniffBut = new QPushButton("start sniffinfg", widget);
	mLay->addWidget(startSniffBut);
	QHBoxLayout *peakLay = new QHBoxLayout;
	QLabel *peakDescLab = new QLabel("max peak: ", widget);
	peakLay->addWidget(peakDescLab);
	peakLab = new QLabel(widget);
	peakLay->addWidget(peakLab);
	mLay->addLayout(peakLay);
	mLay->addStretch(1);
	QHBoxLayout *pitchLay = new QHBoxLayout;
	QLabel *pitchDescLab = new QLabel("pitch: ", widget);
	pitchLay->addWidget(pitchDescLab);
	pitchLab = new QLabel(widget);
	pitchLay->addWidget(pitchLab);
	mLay->addLayout(pitchLay);

    widget->setLayout(mLay);
    setCentralWidget(widget);
	
	m_levelTimer = new QTimer();

	m_pitch = new TaudioIN();
    devListCombo->addItems(TaudioIN::getAudioDevicesList());

    connect(devListCombo, SIGNAL(currentIndexChanged(QString)), this, SLOT(setDeviceName(QString)));
// 	connect(startSniffBut, SIGNAL(clicked(bool)), this, SLOT(startSniffSlot()));
// 	connect(m_levelTimer, SIGNAL(timeout()), this, SLOT(levelSlot()));

}

void MainWindow::setDeviceName(QString devN) {
    devNameLab->setText(devN);
	m_pitch->setAudioDevice(devN);
}
