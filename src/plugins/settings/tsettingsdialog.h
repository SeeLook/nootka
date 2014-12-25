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
/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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

#include <widgets/tsettingsdialogbase.h>
#include "tsettingsmode.h"

class TlaySettings;
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

/** 
 * Dialog window - a container - for Nootka settings
 * Each page of it is represented and implemented by separate class.
 * @p mode set in constructor (and only there) determines which pages are available.
 */
class TsettingsDialog : public TsettingsDialogBase
{
    Q_OBJECT
public:		
	
    explicit TsettingsDialog(QWidget *parent = 0, EsettingsMode mode = e_settings);
		

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
		TlaySettings 				*m_laySett;
    QWidget             *m_audioSettingsPage;
		bool								 m_7thNoteToDefaults;
		EsettingsMode				 m_mode;
    
private slots:
    void changeSettingsWidget(int index);
		void allDefaultsRequired();
		void rtApiSlot();
    
private:
    void createAudioPage();

};

#endif // SETTINGSDIALOG_H
