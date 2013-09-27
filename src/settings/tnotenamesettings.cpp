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
 *  You should have received a copy of the GNU General Public License	     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/


#include "tnotenamesettings.h"
#include "tscalepreviewlabel.h"
#include "tglobals.h"
#include "select7note.h"
#include <tfirstrunwizzard.h>
#include <QtGui>


extern Tglobals *gl;



TnoteNameSettings::TnoteNameSettings(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *mainLay = new QVBoxLayout;
		mainLay->addStretch(1);
    mainLay->setAlignment(Qt::AlignCenter);
    m_nameStyleGr = new TnotationRadioGroup(gl->NnameStyleInNoteName, this);
    m_select7 = new Select7note(this);
    m_select7->set7th_B(gl->seventhIs_B);
		m_scaleLabel = new TscalePreviewLabel(gl->NnameStyleInNoteName, this);
		
		mainLay->addStretch(1);
		mainLay->addWidget(m_select7, 0, Qt::AlignCenter);
		mainLay->addWidget(m_scaleLabel, 0, Qt::AlignCenter);
		mainLay->addWidget(m_nameStyleGr);
    mainLay->addStretch(1);

    m_octInNameCh = new QCheckBox(tr("show octave in name of note"),this);
    mainLay->addWidget(m_octInNameCh, 0, Qt::AlignCenter);
    m_octInNameCh->setStatusTip(tr("Shows formatted note name. For small octave - the name is small letter,<br>for great octave - the name starts with a capital letter,<br>for one-line, digit <sup>1</sup> is added, and so on." ));
    m_octInNameCh->setChecked(gl->NoctaveInNoteNameFormat);
    mainLay->addStretch(1);
    setLayout(mainLay);

    connect(m_select7, SIGNAL(seventhIsBchanged(bool)), this, SLOT(seventhNoteWasChanged(bool)));
		connect(m_nameStyleGr, SIGNAL(noteNameStyleWasChanged(Tnote::EnameStyle)), this, SLOT(nameStyleWasChanged(Tnote::EnameStyle)));
}


bool TnoteNameSettings::is7th_b() {
		return m_select7->is7th_B();
}


void TnoteNameSettings::saveSettings() {
    gl->NnameStyleInNoteName = m_nameStyleGr->getNameStyle();
    gl->NoctaveInNoteNameFormat = m_octInNameCh->isChecked();
		gl->NsolfegeStyle = m_nameStyleGr->getSolfegeStyle();
    gl->seventhIs_B = is7th_b();
}


void TnoteNameSettings::restoreDefaults() {
		m_nameStyleGr->setNameStyle(gl->getSolfegeStyle());
		if (Tpage_3::note7txt().toLower() == "b") {
			m_select7->set7th_B(true);
			m_nameStyleGr->setNameStyle(Tnote::e_nederl_Bis);
		}	else {
			m_select7->set7th_B(false);
			m_nameStyleGr->setNameStyle(Tnote::e_deutsch_His);
		}
		seventhNoteWasChanged(m_select7->is7th_B());
		m_octInNameCh->setChecked(true);
}


void TnoteNameSettings::seventhNoteWasChanged(bool isB) {
        m_nameStyleGr->seventhNoteWasChanged(isB);
        emit seventhIsBChanged(isB);
				m_scaleLabel->changeStyle(m_nameStyleGr->getNameStyle());
}


void TnoteNameSettings::nameStyleWasChanged(Tnote::EnameStyle style) {
		m_scaleLabel->changeStyle(style);
}





