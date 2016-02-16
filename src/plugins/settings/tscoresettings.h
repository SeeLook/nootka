/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk  				                   *
 *   seelook@gmail.com   	      					                                 *
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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#ifndef SCORESETTINGS_H
#define SCORESETTINGS_H

#include <music/tnote.h>
#include <music/tclef.h>
  #include <touch/ttoucharea.h>


class TnoteNameSettings;
class QSpinBox;
class QGroupBox;
class TnotationRadioGroup;
class Tclef;
class TselectClef;
class QCheckBox;
class QLabel;
class QLineEdit;
class TcolorButton;
#if defined (Q_OS_ANDROID)
  class TlistMenu;
#endif


class TscoreSettings : public TtouchArea
{
  Q_OBJECT

public:
  explicit TscoreSettings(QWidget *parent = 0);

  static QString forExample() { return tr("for example"); }
  static QString showKeySigName() { return tr("show names of key signature"); }

  QString getMajorExample(Tnote::EnameStyle nameStyle);
  QString getMinorExample(Tnote::EnameStyle nameStyle);
  
  void setDefaultClef(Tclef clef);
  void saveSettings();
  void restoreDefaults();

  void enableKeySignGroup(bool enable);
  void nameStyleWasChanged(Tnote::EnameStyle nameStyle);
  void majorExtensionChanged();
  void minorExtensionChanged();
  void seventhIsBChanged(bool isB);
  void defaultClefChanged(Tclef clef);

private:
  QCheckBox 						*m_enablKeySignCh;
  QCheckBox 						*m_otherEnharmChBox, *m_dblAccChBox;
  QGroupBox 						*m_enablKeyNameGr, *m_nameExtGr, *m_singleNoteGr;
  QLabel 								*m_majExtLab, *m_minExtLab, *m_majExampl, *m_minExampl;
  QLineEdit 						*m_majEdit, *m_minEdit;
  TnotationRadioGroup 	*m_nameStyleGr;
  Tnote::EnameStyle 		 m_workStyle;
  TcolorButton 					*m_notePointColorBut, *m_enharmColorBut;;
  TselectClef						*m_clefSelector;
  QSpinBox							*m_tempoSpin;
  TnoteNameSettings     *m_nameTab;
#if defined (Q_OS_ANDROID)
  TlistMenu             *m_topList;
#endif
};


#endif // SCORESETTINGS_H
