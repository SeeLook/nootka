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


#ifndef EXAMSETTINGS_H
#define EXAMSETTINGS_H


#include <touch/ttoucharea.h>
#include "tsettingsmode.h"


class QLabel;
class QSpinBox;
class QRadioButton;
class TroundedLabel;
class QSlider;
class TexamParams;
class QLineEdit;
class QCheckBox;
class TcolorButton;

class TexamSettings : public TtouchArea
{
    Q_OBJECT
public:
      
  explicit TexamSettings(QWidget* parent = 0, EsettingsMode mode = e_settings);

  void saveSettings();
	void restoreDefaults();

public slots:
  void expertAnswersChanged(bool enabled);
	
protected slots:
	void autoQuestionSlot(bool state);
  
private:
// exercises
  QCheckBox				*m_correctChB, *m_suggestExamChB, *m_showDetectedChB;
  QSpinBox				*m_correctPreviewSpin;
// exams
  QCheckBox 			*m_repeatIncorChB, *m_closeConfirmChB;

// common
  QSpinBox				*m_mistakePreviewSpin, *m_questionDelaySpin;
  QLabel          *m_afterLab;
  QCheckBox				*m_autoNextChB, *m_expertAnswChB, *m_showNameChB;
  TcolorButton 		*m_questColorBut, *m_answColorBut, *m_notBadButt;
  QRadioButton		*m_contRadio, *m_waitRadio, *m_stopRadio;
  QLineEdit 			*m_nameEdit;
  TexamParams 		*m_params;
  QColor 					*m_qColor, *m_aColor, *m_nbColor;
  EsettingsMode		 m_mode;

};

#endif // EXAMSETTINGS_H
