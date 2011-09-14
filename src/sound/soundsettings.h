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


#ifndef SOUNDSETTINGS_H
#define SOUNDSETTINGS_H

#include <QWidget>

class QComboBox;
class QGroupBox;
class QRadioButton;

class SoundSettings : public QWidget
{
    Q_OBJECT
public:
    explicit SoundSettings(QWidget *parent = 0);

    void saveSettings();
		/** The structure describes midi program (instrument).
		 * It has @param name 
		 * and @param progNr which is midi instrument number  */
	struct TmidiInstrListStruct {
		QString name;
		unsigned char progNr;
	};
		/** A list of midi instruments used in Nootka */
	QList<TmidiInstrListStruct> instruments;


private:
	void addInstrument(QString name, unsigned char midiNr);
  
    QGroupBox *audioOutEnableGr, *realAGr, *midiGr;
    QComboBox *audioOutDevListCombo, *midiPortsCombo, *midiInstrCombo;
	QRadioButton *midiRadioButt, *audioRadioButt;
	
private slots:
	void audioOrMidiChanged();

};

#endif // SOUNDSETTINGS_H
