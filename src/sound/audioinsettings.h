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

#ifndef AUDIOINSETTINGS_H
#define AUDIOINSETTINGS_H
#include <QWidget>
#include "tnote.h"

class QToolBox;
class TroundedLabel;
class TvolumeSlider;
class QRadioButton;
class TaudioParams;
class TpitchView;
class TaudioIN;
class QLabel;
class QPushButton;
class QSpinBox;
class QGroupBox;
class QComboBox;
class Ttune;


class AudioInSettings: public QWidget
{
  Q_OBJECT
  
public:
  
  explicit AudioInSettings(TaudioParams *params, QString path, Ttune *tune, QWidget *parent = 0);
  virtual ~AudioInSettings();
  
  QString testTxt, stopTxt;
  void saveSettings();
	void restoreDefaults();
    /** Generates devices list for inDeviceCombo QComboBox.*/
  void generateDevicesList();
      /** Grabs (refresh) devices list from AudioIn and fill audioOutDevListCombo */
  void setDevicesCombo();
			/** Calculates interval from given frequency and sets interval Spin Box. */
	void intervalFromFreq(int bFreq);
			/** Calculates frequency from given interval [-12 to 12 semitones] and sets frequency Spin Box */
	void freqFromInterval(int interval);
			/** Changes value of interval and adjust its suffix. Also Adjust up/down radio buttons */
	void setTransposeInterval(int interval);
	
public	slots:
			/** Occurs when tune of a guitar is changed and range of detecting pitches has to be adjusted. */
	void whenLowestNoteChanges(Tnote loNote);
	void tuneWasChanged(Ttune *tune);
  
protected:
  void setTestDisabled(bool disabled);
    /** Writes state of widgets to TaudioParams object. */
  void grabParams(TaudioParams *params);
  
protected slots:
  void testSlot();
  void noteSlot(Tnote note);
  void freqSlot(float freq);
  void intervalChanged();
  void baseFreqChanged(int bFreq);
  void minimalVolChanged(float vol);
	void voiceOrInstrumentChanged();
	void upDownIntervalSlot();
  
private:
    /** Calculates frequencies of strings related to a440diff 
     * and sets status tip.*/
  void getFreqStatusTip();
      /** returns frequency of @param freq shifted by a440diff 
       * rounded to int */
  int getFreq(double freq);
  
  float offPitch(float pitch);
      /** returns difference of @param freq related to 440 Hz in semitones */
  float getDiff(int freq);
	
  
  QComboBox 					*m_inDeviceCombo, *m_intonationCombo;
  QGroupBox 					*enableInBox, *modeGr, *midABox;
  QRadioButton 				*voiceRadio, *instrRadio;
	QRadioButton				*highRadio, *middleRadio, *lowRadio;
	QRadioButton				*m_upSemiToneRadio, *m_downsSemitoneRadio; 
  QSpinBox 						*freqSpin, *durationSpin, *m_intervalSpin;
  TvolumeSlider 			*volumeSlider;
  QPushButton 				*testButt;
  QLabel 							*durHeadLab;
	TroundedLabel				*pitchLab, *freqLab, *tuneFreqlab;
  bool 								m_testDisabled, m_listGenerated;
  TpitchView 					*pitchView;
  TaudioIN 						*m_audioIn;
  TaudioParams 				*m_glParams, *m_tmpParams;
	Ttune								*m_tune;
	QWidget							*m_4_test;
  QToolBox 						*m_toolBox;
};

#endif // AUDIOINSETTINGS_H
