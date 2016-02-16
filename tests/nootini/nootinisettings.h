/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef NOOTINISETTINGS_H
#define NOOTINISETTINGS_H


#include <QtWidgets/QDialog>
#include <QSpinBox>

class QDoubleSpinBox;
class QGroupBox;
class QCheckBox;
class TartiniParams;
class QComboBox;
class TvolumeSlider;
class QRadioButton;


/**
 * Nootini tool settings dialog window
 */
class NootiniSettings : public QDialog
{

public:
  explicit NootiniSettings(TartiniParams* tp, QWidget* parent = 0);

  bool drawVolumeChart();
  void setDrawVolumeChart(bool draw);

  void setRange(int r);
  int range();

protected:
  virtual void accept();
  void splitByVolChanged(bool enab) { m_splitVolSpin->setDisabled(!enab); }
  void skipStillerChanged(bool enab) { m_skipStillerSpin->setDisabled(!enab); }

private:
  QRadioButton        *m_highRadio, *m_middleRadio, *m_lowRadio;
  QSpinBox            *m_freqSpin, *m_durationSpin, *m_thresholdSpin, *m_splitVolSpin, *m_skipStillerSpin;
  QComboBox           *m_methodCombo;
  QDoubleSpinBox      *m_dbFlorSpin;
  TvolumeSlider       *m_volumeSlider;
  QCheckBox           *m_noiseFilterChB, *m_calcNoiseChB, *m_drawVolChB;
  QCheckBox           *m_splitVolChB, *m_skipStillerChB;
  TartiniParams       *m_tartiniParams;
};

#endif // NOOTINISETTINGS_H
