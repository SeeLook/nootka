/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
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

#ifndef TGUITARSETTINGS_H
#define TGUITARSETTINGS_H

#include <touch/ttoucharea.h>
#include <music/tnote.h>
#include <music/tclef.h>

class QLineEdit;
class TselectInstrument;
class QLabel;
class QGroupBox;
class Ttune;
class TcolorButton;
class QComboBox;
class QSpinBox;
class QRadioButton;
class QCheckBox;
class TsimpleScore;

/** 
 * Guitar settings page in settings window
 */

class TguitarSettings : public TtouchArea
{
    Q_OBJECT
public:
    explicit TguitarSettings(QWidget *parent = 0);
		virtual ~TguitarSettings();
		
    void saveSettings();
		void restoreDefaults();
		
				/** Returns instrument currently selected. */
		int currentInstrument() { return m_currentInstr; }
		Ttune* currentTune() { return m_curentTune; }
		
		
		Tclef currentClef();
		
				/** Returns lowest note  in current tune. */
		Tnote lowestNote();

signals:
		void clefChanged(Tclef);
		void lowestNoteChanged(Tnote loNote);
		
				/** Is emitted when user changes m_instrumentTypeCombo. Sending value is cast of Einstrument type to int. */
		void instrumentChanged(int);
		void tuneChanged(Ttune*);
		
private:
    void setTune (Ttune *tune);
		
				/** Sets highest and lowest notes in a segment when selected tune is not appropriate to a clef. */
		void updateAmbitus();
		
				/** Disables or enables all widgets on the page. */
		void guitarDisabled(bool disabled);
		
				/** It sets notes state (disables or enables it or sets highest or lowest) according to current tune. */
		void updateNotesState();
		
				/** Takes note by note from m_tuneView and fills given tune. Name is taken from m_tuneCombo */
		void grabTuneFromScore(Ttune *tune);
		
				/** Takes note from given segment number 
				 * and returns lowest score note + @p noteSegm when it is not set.
				 * It is used to put proper note values to tuning. */
		Tnote fixEmptyNote(int noteSegm);
		
				/** Grabs m_fretMarksEdit text and converts it to QVariant list with checking are frets valid.
				 * It overrides content of given list. */
		void checkFretsAndStore(QList<QVariant>& fretList);
		
				/** Converts  QList<QVariant> to QString  */
		QString grabFretsFromList(const QList<QVariant>& fretList);
		

    TsimpleScore    			*m_tuneView;
    QCheckBox       			*m_righthandCh, *m_morePosCh;
    QRadioButton    			*m_prefFlatBut, *m_prefSharpBut;
    QSpinBox        			*m_fretsNrSpin, *m_stringNrSpin;
    QComboBox       			*m_tuneCombo;
		TselectInstrument			*m_selectInstr;
    TcolorButton    			*m_pointColorBut, *m_selColorBut;
		QGroupBox 						*m_tuneGroup, *m_guitarGroup, *m_accidGroup;
		QLabel								*m_fretNrLab, *m_stringNrLab, *m_pointerColorLab, *m_selectColorLab;
		int							 			m_currentInstr;
		Ttune									*m_curentTune; // current guitar tune 
		Ttune 								*m_customTune;
		QLineEdit							*m_fretMarksEdit;
    QList<QWidget*>        m_disabledWidgets; // List of widgets that are disabled when instrument is different than guitar

private slots:
    void tuneSelected(int tuneId);
    void userTune(int, Tnote);
		void onClefChanged(Tclef clef);
		void stringNrChanged(int strNr);
		void instrumentTypeChanged(int index);
#if defined(Q_OS_WIN)
    void delayedBgGlyph();
#endif
};

#endif // TGUITARSETTINGS_H
