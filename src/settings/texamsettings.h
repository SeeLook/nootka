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

  static QString autoNextQuestTxt() { return tr("ask next question automatically"); }
  static QString expertsAnswerTxt() { return tr("check answers without confirm"); }
  static QString showHelpWindowTxt() { return tr("show help when exam starts"); }

  void saveSettings();
	void restoreDefaults();

public slots:
  void expertAnswersChanged(bool enabled);
  
private:
    QCheckBox 			*m_autoNextChB, *m_repeatIncorChB, *m_expertAnswChB, *m_showHelpChB;
    TcolorButton 		*m_questColorBut, *m_answColorBut, *m_notBadButt;
    QLineEdit 			*m_nameEdit;
    TexamParams 		*m_params;
    QColor 					*m_qColor, *m_aColor, *m_nbColor;

};

#endif // EXAMSETTINGS_H
