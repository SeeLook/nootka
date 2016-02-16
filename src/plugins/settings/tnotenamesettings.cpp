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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tnotenamesettings.h"
#include "tcolorbutton.h"
#include "tscalepreviewlabel.h"
#include <tinitcorelib.h>
#include <tscoreparams.h>
#include <tcolor.h>
#include <select7note.h>
#include <tmisctrans.h>
#include <QtWidgets/QtWidgets>



TnoteNameSettings::TnoteNameSettings(QWidget *parent) :
#if defined (Q_OS_ANDROID)
  TtouchArea(parent)
#else
  QWidget(parent)
#endif
{
  QVBoxLayout *mainLay = new QVBoxLayout;
  mainLay->setAlignment(Qt::AlignCenter);
  m_nameStyleGr = new TnotationRadioGroup(Tcore::gl()->S->nameStyleInNoteName, true, this);
  m_select7 = new Select7note(this);
  m_select7->set7th_B(Tcore::gl()->S->seventhIs_B);
  m_select7->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  m_scaleLabel = new TscalePreviewLabel(Tcore::gl()->S->nameStyleInNoteName, true, this);
  m_scaleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  
#if defined (Q_OS_ANDROID)
  QVBoxLayout *seventhLay = new QVBoxLayout;
  seventhLay->setContentsMargins(1, 1, 1, 1);
#else
  QHBoxLayout *seventhLay = new QHBoxLayout;
#endif
    seventhLay->addWidget(m_select7);
    seventhLay->addWidget(m_scaleLabel);
  mainLay->addLayout(seventhLay);
  mainLay->addWidget(m_nameStyleGr);

  m_nameInScoreChB = new QCheckBox(tr("note name on the staff"), this);
    m_nameInScoreChB->setStatusTip(tr("Shows names of all notes on the staff."));
    m_nameInScoreChB->setChecked(Tcore::gl()->S->namesOnScore);
  QLabel *nameColorLab = new QLabel(tr("names highlight color"), this);
  m_nameColorButt = new TcolorButton(Tcore::gl()->S->nameColor, this);
#if defined (Q_OS_ANDROID)
  QVBoxLayout *nameScoreLay = new QVBoxLayout;
  nameScoreLay->addWidget(m_nameInScoreChB, 0, Qt::AlignCenter);
  nameScoreLay->addWidget(getLabelFromStatus(m_nameInScoreChB), 0, Qt::AlignCenter);
  QHBoxLayout *nameColorLay = new QHBoxLayout;
    nameColorLay->addStretch();
    nameColorLay->addWidget(nameColorLab);
    nameColorLay->addStretch();
    nameColorLay->addWidget(m_nameColorButt);
    nameColorLay->addStretch();
  nameScoreLay->addLayout(nameColorLay);
#else
  QHBoxLayout *nameScoreLay = new QHBoxLayout;
    nameScoreLay->addWidget(m_nameInScoreChB);
    nameScoreLay->addStretch(2);
    nameScoreLay->addWidget(nameColorLab);
    nameScoreLay->addStretch();
    nameScoreLay->addWidget(m_nameColorButt);
#endif
  mainLay->addLayout(nameScoreLay);
  mainLay->addStretch(1);
  
  m_octInNameCh = new QCheckBox(tr("show octave in name of note"),this);
    mainLay->addWidget(m_octInNameCh, 0, Qt::AlignCenter);
    m_octInNameCh->setStatusTip(tr("Shows formatted note name. For small octave - the name is small letter,<br>for great octave - the name starts with a capital letter,<br>for one-line, digit <sup>1</sup> is added, and so on." ));
    m_octInNameCh->setChecked(Tcore::gl()->S->octaveInNoteNameFormat);
#if defined (Q_OS_ANDROID)
  mainLay->addWidget(getLabelFromStatus(m_octInNameCh, false), 0, Qt::AlignCenter);
  mainLay->addSpacing(20);
#else
  mainLay->addStretch(1);
#endif
  setLayout(mainLay);

  connect(m_select7, &Select7note::seventhIsBchanged, this, &TnoteNameSettings::seventhNoteWasChanged);
  connect(m_nameStyleGr, &TnotationRadioGroup::noteNameStyleWasChanged, this, &TnoteNameSettings::nameStyleWasChanged);
}


bool TnoteNameSettings::is7th_b() {
		return m_select7->is7th_B();
}


void TnoteNameSettings::saveSettings() {
    Tcore::gl()->S->nameStyleInNoteName = m_nameStyleGr->getNameStyle();
    Tcore::gl()->S->octaveInNoteNameFormat = m_octInNameCh->isChecked();
		Tcore::gl()->S->solfegeStyle = m_nameStyleGr->getSolfegeStyle();
    Tcore::gl()->S->seventhIs_B = is7th_b();
		Tcore::gl()->S->namesOnScore = m_nameInScoreChB->isChecked();
		Tcore::gl()->S->nameColor = m_nameColorButt->getColor();
}


void TnoteNameSettings::restoreDefaults() {
		m_nameStyleGr->setNameStyle(Tcore::gl()->getSolfegeStyle());
		if (TmiscTrans::note7txt().toLower() == "b") {
			m_select7->set7th_B(true);
			m_nameStyleGr->setNameStyle(Tnote::e_nederl_Bis);
		}	else {
			m_select7->set7th_B(false);
			m_nameStyleGr->setNameStyle(Tnote::e_deutsch_His);
		}
		seventhNoteWasChanged(m_select7->is7th_B());
		m_octInNameCh->setChecked(true);
		m_nameColorButt->setColor(Qt::darkCyan);
		m_nameInScoreChB->setChecked(true);
}


void TnoteNameSettings::seventhNoteWasChanged(bool isB) {
    m_nameStyleGr->seventhNoteWasChanged(isB);
    emit seventhIsBChanged(isB);
    m_scaleLabel->changeStyle(m_nameStyleGr->getNameStyle());
}


void TnoteNameSettings::nameStyleWasChanged(Tnote::EnameStyle style) {
		m_scaleLabel->changeStyle(style);
}





