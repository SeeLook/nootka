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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../../settings/tsettingsdialogbase.h"

// class TaudioIN;
// class QComboBox;
// class QLabel;
// class QPushButton;
// class AudioInSettings;
QT_FORWARD_DECLARE_CLASS(AudioInSettings);


class MainWindow : public TsettingsDialogBase
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

//     QString deviceName() { return m_devName; }


signals:

public slots:
//     void setDeviceName(QString devN);
// 	void startListening();

private:
  AudioInSettings *aInSett;
//     QComboBox *devListCombo;
//     QLabel *devNameLab, *peakLab, *pitchLab;
// 	QPushButton *startSniffBut;
// 	TaudioIN *m_pitch;
	

//     QString m_devName;
// 	QTimer *m_levelTimer;


};

#endif // MAINWINDOW_H

