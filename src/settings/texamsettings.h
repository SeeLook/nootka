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


#ifndef EXAMSETTINGS_H
#define EXAMSETTINGS_H

#include <QWidget>

class TroundedLabel;
class QSlider;
class TexamParams;
class QLineEdit;
class QCheckBox;
class TcolorButton;

class TexamSettings : public QWidget
{
    Q_OBJECT
public:
      /** Params are:
       * TexamParams @param params - is obvious
       * QColor @param qColor - is pointer to gl->EquestionColor
       * QColor @param aColor - is pointer to gl->EanswerColor
       * QColor @param nbColor - is pointer to gl->EnotBadColor
       */
  explicit TexamSettings(TexamParams *params, QColor *qColor, QColor *aColor, QColor *nbColor, QWidget *parent = 0);

			/** ask next question automatically */
  static QString autoNextQuestTxt() { return tr("ask next question automatically"); }
			/** check answers without requiring confirmation */
  static QString expertsAnswerTxt() { return tr("check answers without requiring confirmation"); }
  static QString correctMistakesTxt() { return tr("correct mistakes (exercises only)"); } // correct mistakes (exercises only)

  void saveSettings();
	void restoreDefaults();

public slots:
  void expertAnswersChanged(bool enabled);
	void timePreviewChanged(int val);
  
private:
	// exercises
		QCheckBox				*m_correctChB, *m_suggestExamChB;
		QSlider					*m_viewTimeSlider; // preview time duration slider
		TroundedLabel		*m_timeLabel;
	// exams
    QCheckBox 			*m_repeatIncorChB, *m_closeConfirmChB;
		
	// common
		QCheckBox				*m_autoNextChB, *m_expertAnswChB;
    TcolorButton 		*m_questColorBut, *m_answColorBut, *m_notBadButt;
    QLineEdit 			*m_nameEdit;
    TexamParams 		*m_params;
    QColor 					*m_qColor, *m_aColor, *m_nbColor;

};

#endif // EXAMSETTINGS_H
