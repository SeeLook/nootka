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

#ifndef TGUITARSETTINGS_H
#define TGUITARSETTINGS_H

#include <QWidget>
#include "tnote.h"
#include "tclef.h"

class Ttune;
class TcolorButton;
class QComboBox;
class QSpinBox;
class QRadioButton;
class QCheckBox;
class TsimpleScore;

/** 
 * Those are guitar settings in settings window
 */
class TguitarSettings : public QWidget
{
    Q_OBJECT
public:
    explicit TguitarSettings(QWidget *parent = 0);
		
    void saveSettings();

private:
    void setTune (Ttune *tune);
		void updateAmbitus();

    TsimpleScore    *m_tuneView;
    QCheckBox       *m_righthandCh, *m_morePosCh;
    QRadioButton    *m_prefFlatBut, *m_prefSharpBut;
    QSpinBox        *m_fretsNrSpin, *m_stringNrSpin;
    QComboBox       *m_tuneCombo, *m_instrumentTypeCombo;
    TcolorButton    *m_pointColorBut, *m_selColorBut;

private slots:
    void tuneSelected(int tuneId);
    void userTune(int, Tnote);
		void switchedToPianoStaff();
		void onClefChanged(Tclef clef);
		void stringNrChanged(int strNr);
		void instrumentTypeChanged(int index);
};

#endif // TGUITARSETTINGS_H
