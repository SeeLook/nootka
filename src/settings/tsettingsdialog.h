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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "tsettingsdialogbase.h"
#include <QMap>

class QPushButton;
class TscoreSettings;
class NameSettings;
class TguitarSettings;
class AudioOutSettings;
class AudioInSettings;
class QCheckBox;
class QSpinBox;
class QComboBox;
class ExamSettings;
class TcolorButton;

class TglobalSettings : public QWidget
{
    Q_OBJECT
public:
    explicit TglobalSettings(QWidget *parent = 0);

    void saveSettings();
		
private:
  QCheckBox *m_otherEnharmChBox, *m_dblAccChBox, *m_hintsEnabledChBox;
	TcolorButton *m_enharmColorBut;
	QComboBox *m_langCombo;
	QMap<QString, QString> m_langList;
  QPushButton *m_updateButton;
  QLabel* m_updateLabel;
  
private slots:
  void updateSlot();
  void processOutputSlot(QString output);
};



//################ TsettingsDialog ##############################################
class TsettingsDialog : public TsettingsDialogBase
{
    Q_OBJECT
public:
    explicit TsettingsDialog(QWidget *parent = 0);

public slots:
    void saveSettings();

private:
    TglobalSettings    	*m_globalSett;
    TscoreSettings    	*m_scoreSett;
    NameSettings	     	*m_nameSett;
    TguitarSettings    	*m_guitarSett;
    ExamSettings      	*m_examSett;
    AudioOutSettings  	*m_sndOutSett;
    AudioInSettings   	*m_sndInSett;
    QWidget             *m_audioSettingsPage;
    QCheckBox         	*m_jackChBox;
    
private slots:
    void changeSettingsWidget(int index);
    void changeUseJack();
    
private:
    void createAudioPage();

};

#endif // SETTINGSDIALOG_H
