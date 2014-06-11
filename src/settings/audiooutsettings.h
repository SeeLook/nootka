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


#ifndef AUDIOOUTSETTINGS_H
#define AUDIOOUTSETTINGS_H

#include <QWidget>

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
      /** A list of midi instruments used in Nootka */
	QList<TmidiInstrListStruct> instruments;
  
      /** Generates devices list for inDeviceCombo QComboBox.*/
  void generateDevicesList();
      /** Grabs devices list from AudioOut and fill audioOutDevListCombo */
  void setDevicesCombo();
	
			/** This static method sets midi instr. or audio depends on given instrument
			 * and previous midi state. */
	static void adjustOutToInstrument(TaudioParams *out, int instr);
  
public slots:
	void whenInstrumentChanged(int instr);

private:
	void addInstrument(QString name, unsigned char midiNr);

  QGroupBox *m_audioOutEnableGr, *m_realAGr, *m_midiGr;
  QComboBox *m_audioOutDevListCombo, *m_audioInstrCombo ,*m_midiPortsCombo, *m_midiInstrCombo;
  QRadioButton *m_midiRadioButt, *m_audioRadioButt;
  TaudioParams *m_params;
  bool m_listGenerated;
	
private slots:
	void audioOrMidiChanged();

};

#endif // AUDIOOUTSETTINGS_H
