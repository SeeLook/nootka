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

class QRadioButton;
class QTabWidget;
class TglobalSettings;
class QPushButton;
class TscoreSettings;
class TnoteNameSettings;
class TguitarSettings;
class AudioOutSettings;
class AudioInSettings;
class QCheckBox;
class TexamSettings;


class TsettingsDialog : public TsettingsDialogBase
{
    Q_OBJECT
public:
    explicit TsettingsDialog(QWidget *parent = 0);
		

public slots:
    void saveSettings();
		void restoreDefaults();

private:
    TglobalSettings   	*m_globalSett;
    TscoreSettings    	*m_scoreSett;
    TnoteNameSettings	  *m_nameSett;
    TguitarSettings    	*m_guitarSett;
    TexamSettings      	*m_examSett;
    AudioOutSettings  	*m_sndOutSett;
    AudioInSettings   	*m_sndInSett;
		QTabWidget 					*m_audioTab;
    QWidget             *m_audioSettingsPage;
    QCheckBox         	*m_jackChBox;
		bool								 m_7thNoteToDefaults;
#if defined(Q_OS_WIN32)
		QRadioButton				*m_DirectSoundRadio, *m_ASIORadio;
#endif
    
private slots:
    void changeSettingsWidget(int index);
				/** Refresh list of audio devices on QComboBox-es of m_audioSettingsPage */
    void changeAudioAPI();
		void allDefaultsRequired();
    
private:
    void createAudioPage();

};

#endif // SETTINGSDIALOG_H

class QRadioButton;