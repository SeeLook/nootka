/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk                                  *
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

class QRadioButton;
class TaudioParams;
class TpitchView;
class QDoubleSpinBox;
class TaudioIN;
class QLabel;
class QPushButton;
class QSpinBox;
class QGroupBox;
class QComboBox;


class AudioInSettings: public QWidget
{
  Q_OBJECT
public:
  
  explicit AudioInSettings(TaudioParams *params, QString path, QWidget *parent = 0);
  virtual ~AudioInSettings();
  
  QString testTxt, stopTxt;
  void saveSettings();
    /** Generates devices list for inDeviceCombo QComboBox.*/
  void generateDevicesList();
  
protected:
  void setTestDisabled(bool disabled);
    /** Writes state of widgets to TaudioParams object. */
  void grabParams(TaudioParams *params);
  
protected slots:
  void testSlot();
  void calcSlot();
  void noiseDetected(qint16 noise);
  void noteSlot(Tnote note);
  void freqSlot(float freq);
  void intervalChanged(int index);
  void baseFreqChanged(int bFreq);
  
private:
    /** Calculates frequencies of strings related to a440diff 
     * and sets status tip.*/
  void getFreqStatusTip();
      /** returns frequency of @param freq shifted by a440diff 
       * rounded to int */
  int getFreq(double freq);
      /** returns difference of @param freq related to 440 Hz in semitones */
  float getDiff(int freq); 
  
  QComboBox *inDeviceCombo, *intervalCombo;
  QGroupBox *enableInBox, *modeGr, *midABox, *noisGr;
  QRadioButton *voiceRadio, *instrRadio;
  QSpinBox *freqSpin;
  QDoubleSpinBox *noiseSpin;
  QPushButton *calcButt, *testButt;
  QLabel *pitchLab, *freqLab;
  bool m_testDisabled, m_listGenerated;
  TpitchView *volMeter;
  TaudioIN *m_audioIn;
  qint16 m_noiseLevel;
  TaudioParams *m_glParams, *m_tmpParams; 
  
};

#endif // AUDIOINSETTINGS_H
