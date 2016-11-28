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

#ifndef AUDIOINSETTINGS_H
#define AUDIOINSETTINGS_H


#include <touch/ttoucharea.h>
#include <music/tnote.h>


class TselectInstrument;
class QDoubleSpinBox;
class TnoteStruct;
class QCheckBox;
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
class TlistMenu;



class AudioInSettings: public TtouchArea
{
  Q_OBJECT
  
public:
  
  explicit AudioInSettings(TaudioParams *params, Ttune *tune, QWidget *parent = 0);
  virtual ~AudioInSettings();
  
  QString testTxt, stopTxt;
  void saveSettings();
	void restoreDefaults();
  void generateDevicesList(); /** Generates devices list for inDeviceCombo QComboBox.*/
	
      /** Grabs (refresh) devices list from AudioIn and fill audioOutDevListCombo */
  void updateAudioDevList();
	
			/** Calculates interval from given frequency and sets interval Spin Box. */
	void intervalFromFreq(int bFreq);
	
			/** Calculates frequency from given interval [-12 to 12 semitones] and sets frequency Spin Box */
	void freqFromInterval(int interval);
	
			/** Changes value of interval and adjust its suffix. Also Adjust up/down radio buttons */
	void setTransposeInterval(int interval);
	
#if !defined (Q_OS_ANDROID) && (defined (Q_OS_LINUX) || defined (Q_OS_WIN))
  QCheckBox* rtApiCheckBox() { return m_JACK_ASIO_ChB; }
#endif

signals:
	void rtApiChanged(); /** Emitted when user wants ASIO/JACK sound */

  #if defined(Q_OS_WIN)
      /** When ASIO API is set and there are two or more devices (drivers) on the list
      * signal is send when user changes it. */
    void asioDriverChanged(int);
  #endif

public	slots:
	void tuneWasChanged(Ttune *tune);
	void stopSoundTest(); /** Public method (slot) to stop sound test */
  void whenInstrumentChanged(int instr);
#if defined(Q_OS_WIN)
  void asioDeviceSlot(int id);
#endif
  
protected:
  void setTestDisabled(bool disabled);
	
    /** Writes state of widgets to TaudioParams object. */
  void grabParams(TaudioParams *params);
  
#if !defined (Q_OS_ANDROID) && (defined (Q_OS_LINUX) || defined (Q_OS_WIN))
  void JACKASIOSlot();
#endif

  
protected slots:
  void testSlot();
  void noteSlot(const TnoteStruct& ns);
  void intervalChanged();
  void baseFreqChanged(int bFreq);
  void minimalVolChanged(float vol);
  void upDownIntervalSlot();
  void splitByVolChanged(bool enab);
  void skipStillerChanged(bool enab);
  void adjustInstrSlot(int instr);
  
private:
    /** Calculates frequencies of strings related to a440diff and sets status tip.*/
  void getFreqStatusTip();
	
      /** returns frequency of @param freq shifted by a440diff 
       * rounded to int */
  int getFreq(double freq);

      /** returns difference of @param freq related to 440 Hz in semitones */
  float getDiff(int freq);
	
  
  QComboBox 					*m_inDeviceCombo, *m_intonationCombo, *m_methodCombo;
  QGroupBox 					*enableInBox, *midABox;
	QRadioButton				*m_upSemiToneRadio, *m_downsSemitoneRadio;
  QSpinBox 						*freqSpin, *durationSpin, *m_intervalSpin, *m_splitVolSpin, *m_skipStillerSpin;
  TvolumeSlider 			*volumeSlider;
  QPushButton 				*testButt;
  TselectInstrument   *m_adjustToInstrButt;
  QLabel 							*durHeadLab;
	TroundedLabel				*pitchLab, *freqLab, *tuneFreqlab;
  bool 								m_testDisabled, m_listGenerated;
  TpitchView 					*pitchView;
  TaudioIN 						*m_audioIn;
  TaudioParams 				*m_glParams, *m_tmpParams;
	Ttune								*m_tune;
#if defined (Q_OS_ANDROID)
  TtouchArea          *m_4_test;
  TlistMenu           *m_topList;
#else
  QWidget             *m_4_test;
  QToolBox            *m_toolBox;
  #if defined (Q_OS_LINUX) || defined (Q_OS_WIN)
    QCheckBox         *m_JACK_ASIO_ChB;
  #endif
#endif
	QCheckBox						*m_splitVolChB, *m_skipStillerChB, *m_noiseFilterChB;
};

#endif // AUDIOINSETTINGS_H
