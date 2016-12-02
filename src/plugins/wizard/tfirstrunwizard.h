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

#ifndef TFIRSTRUNWIZZARD_H
#define TFIRSTRUNWIZZARD_H


#include <QtWidgets/qwizard.h>
#include <music/tnote.h>
#include <QtCore/qlocale.h>


class QVBoxLayout;
class TsimpleScore;
class TroundedLabel;
class TscalePreviewLabel;
class QLabel;
class QTextEdit;
class QStackedLayout;
class QRadioButton;
class QCheckBox;
class Tpage_2;
class Tpage_3;
class TmainHelp;
class Select7note;
class TselectInstrument;


class TfirstRunWizard : public QWizard
{
    Q_OBJECT
public:
  explicit TfirstRunWizard(QWidget *parent = 0);

protected:
  void pageChanged(int pageNr);
  void whenInstrumentChanged(int instr);
  virtual void done(int result);

private:
  Tpage_3 					*m_page3;
  TmainHelp 				*m_page4;
  Tpage_2 					*m_notationWidget;
  TselectInstrument *m_selectInstr;

};

// page 1 is about dialog
// page nr 1.5 is to select an instrument

/**
 * page 2 is a QTextEdit with info about notation
 * or score widget to select clef and instrument scale.
 */
class Tpage_2 : public QWizardPage
{
	Q_OBJECT
	
public:
	explicit Tpage_2(QWidget* parent = 0);

	QTextEdit* notationNote() { return m_notationNote; }
	TsimpleScore *score() { return m_score; }

			/** Changes contents of the widget. 
			 * For guitars it is QTextEdit available through notationNote()
			 * and for non guitar it is score and label with description. */
	void setNoteForInstrument(int instr);

protected slots:
	void scoreHint(QString hint);
	void clefChanged(); /** To keep proper notes in scale */

private:
	QTextEdit			*m_notationNote;
	TsimpleScore  *m_score;
	TroundedLabel *m_scoreHint;
	QVBoxLayout		*m_lay;
  QLabel        *m_headLabel;
};


class Tpage_3 : public QWizardPage
{
  Q_OBJECT
public:
  explicit Tpage_3(QWidget *parent = 0);

  Select7note *select7;
  QCheckBox *dblAccChB, *enharmChB, *useKeyChB;
  TscalePreviewLabel *scaleLab;
  QRadioButton *solfegeRadio, *letterRadio;

  Tnote::EnameStyle nameStyle() { return m_nameStyle; }

  QLocale localization;

protected:
  void seventhNoteChanged(bool is7_B);
  void notationSlot();

private:
  Tnote::EnameStyle         m_nameStyle;

};


#endif // TFIRSTRUNWIZZARD_H
