/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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


#ifndef AUDIOOUTSETTINGS_H
#define AUDIOOUTSETTINGS_H

#include <QWidget>

class QCheckBox;
class TaudioParams;
class QComboBox;
class QGroupBox;
class QRadioButton;


class AudioOutSettings : public QWidget
{
    Q_OBJECT
    
public:
	explicit AudioOutSettings(TaudioParams *aParams, QWidget *parent = 0);

	void saveSettings();
	void restoreDefaults();
		
		/** The structure describes midi program (instrument).
		 * It has @param name 
		 * and @param progNr which is midi instrument number  */
	struct TmidiInstrListStruct {
		QString name;
		unsigned char progNr;
	};

	QList<TmidiInstrListStruct> instruments; /** A list of midi instruments used in Nootka */
  
  void generateDevicesList(); /** Generates devices list for inDeviceCombo QComboBox.*/
  void setDevicesCombo(); /** Grabs devices list from TrtAudioOut and fill audioOutDevListCombo */
	void updateAudioDevList(); /** Grabs audio devices from TrtAudio and fill combo box */
	
	QCheckBox* rtApiCheckBox() { return m_JACK_ASIO_ChB; }
	
			/** This static method sets midi instr. or audio depends on given instrument
			 * and previous midi state. */
	static void adjustOutToInstrument(TaudioParams *out, int instr);
	
signals:
	void rtApiChanged(); /** Emitted when user wants ASIO/JACK sound */
#if defined(Q_OS_WIN)
      /** When ASIO API is set and there are two or more devices (drivers) on the list
       * signal is send when user changes it. */
  void asioDriverChanged(int);
#endif
  
public slots:
	void whenInstrumentChanged(int instr);
#if defined(Q_OS_WIN)
  void asioDeviceSlot(int id);
#endif

private:
	void addInstrument(QString name, unsigned char midiNr);

  QGroupBox 				*m_audioOutEnableGr, *m_realAGr, *m_midiGr;
  QComboBox 				*m_audioOutDevListCombo, *m_audioInstrCombo ,*m_midiPortsCombo, *m_midiInstrCombo;
  QRadioButton 			*m_midiRadioButt, *m_audioRadioButt;
	QCheckBox					*m_playInputChB, *m_playDetectedChB;
  TaudioParams 			*m_params;
  bool 							 m_listGenerated;
	QCheckBox					*m_JACK_ASIO_ChB;
	
private slots:
	void audioOrMidiChanged();
	void JACKASIOSlot();

};

#endif // AUDIOOUTSETTINGS_H
