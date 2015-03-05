/***************************************************************************
 *   Copyright (C) 2015 by Tomasz Bojczuk                                  *
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

#ifndef NOOTINISETTINGS_H
#define NOOTINISETTINGS_H


#include <QtWidgets/QDialog>

class QCheckBox;
class TartiniParams;
class QComboBox;
class TvolumeSlider;
class QSpinBox;
class QRadioButton;


/**
 * Nootini tool settings dialog window
 */
class NootiniSettings : public QDialog
{

public:
  explicit NootiniSettings(TartiniParams* tp, QWidget* parent = 0);

protected:
  virtual void accept();

private:
  QRadioButton        *m_mpmRadio, *m_correlRadio, *m_cepstrumRadio;
  QSpinBox            *m_freqSpin, *m_durationSpin, *m_thresholdSpin;
  TvolumeSlider       *m_volumeSlider;
  QCheckBox           *m_noiseFilterChB;
  TartiniParams       *m_tartiniParams;
};

#endif // NOOTINISETTINGS_H
