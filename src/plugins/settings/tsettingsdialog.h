/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
 *   seelook@gmail.com                                                     *
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

        /** Usually @p FALSE, goes @p TRUE after user wants reset. */
    bool resetToDefaults() { return m_resetToDefaults; }
    void saveSettings();
    void restoreDefaults();

#if !defined (Q_OS_ANDROID) && (defined (Q_OS_LINUX) || defined (Q_OS_WIN))
    void cancelSlot(); /** When user rejected settings. It is used to restore RtAudio previous state, so not for mobile. */
#endif

private:
    TglobalSettings   	*m_globalSett;
    TscoreSettings    	*m_scoreSett;
    TguitarSettings    	*m_guitarSett;
    TexamSettings      	*m_examSett;
    AudioOutSettings  	*m_sndOutSett;
    AudioInSettings   	*m_sndInSett;
		QTabWidget 					*m_audioTab;
		TlaySettings 				*m_laySett;
    QWidget             *m_audioSettingsPage;
		bool								 m_7thNoteToDefaults;
		EsettingsMode				 m_mode;
    bool                 m_resetToDefaults; /** Usually @p FALSE, goes @p TRUE after user wants reset. */
    QPushButton         *m_helpButt;
    
private:
    void changeSettingsWidget(int index);
    void allDefaultsRequired();
    void helpSlot();
    void createAudioPage();
#if !defined (Q_OS_ANDROID)
    void rtApiSlot();
#endif

};

#endif // SETTINGSDIALOG_H
