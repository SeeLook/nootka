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
#include "saudioinparams.h"
#include "tnote.h"

class TpitchView;
class QDoubleSpinBox;
class TaudioIN;
class QLabel;
class QPushButton;
class QSpinBox;
class QCheckBox;
class QGroupBox;
class QComboBox;


class AudioInSettings: public QWidget
{
  Q_OBJECT
public:
  
  explicit AudioInSettings(QWidget *parent = 0);
  virtual ~AudioInSettings();
  
  QString testTxt, stopTxt;
  
protected:
  void setTestDisabled(bool disabled);
	/** Writes state of widgets to m_aInParams struct. */
  void grabParams();
  
protected slots:
  void testSlot();
  void calcSlot();
  void noiseDetected(qint16 noise);
  void noteSlot(Tnote note);
  void freqSlot(float freq);
  
private:
  QComboBox *inDeviceCombo, *detectMethodCombo, *intervalCombo;
  QGroupBox *enableInBox, *midABox, *noisGr;
  QCheckBox /* *loudChB,*/ *voiceChB/*, *noiseChB*/;
  QSpinBox *freqSpin;
  QDoubleSpinBox *noiseSpin;
  QPushButton *calcButt, *testButt;
  QLabel *pitchLab, *freqLab;
  bool m_testDisabled;
  TpitchView *volMeter;
  TaudioIN *m_audioIn;
  qint16 m_noiseLevel;
  SaudioInParams m_aInParams; 
  
};

#endif // AUDIOINSETTINGS_H
