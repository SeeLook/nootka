/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
 *   tomaszbojczuk@gmail.com   						                                 *
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


#include "tscoresettings.h"
#include "tnotationradiogroup.h"
#include "tcolorbutton.h"
#include <tglobals.h>
#include <music/tkeysignature.h>
#include <widgets/tselectclef.h>
#include <tcolor.h>
#include <graphics/tnotepixmap.h>
#include <tfirstrunwizzard.h>
#include <music/tnamestylefilter.h>
#include <QtWidgets>

extern Tglobals *gl;


TscoreSettings::TscoreSettings(QWidget *parent) :
    QWidget(parent)
{
	QToolBox *m_toolBox = new QToolBox(this);
	QWidget *m_3_misc, *m_1_keys, *m_2_clefs;
	
// 1. Key signatures settings
	m_1_keys = new QWidget();
	m_toolBox->addItem(m_1_keys, "1. " + tr("Key signatures"));
		
    m_workStyle = gl->SnameStyleInKeySign;
    QVBoxLayout *keyLay = new QVBoxLayout();
    m_enablKeySignCh = new QCheckBox(tr("enable key signature"), m_1_keys);
			m_enablKeySignCh->setChecked(gl->SkeySignatureEnabled);
    keyLay->addWidget(m_enablKeySignCh);
    QHBoxLayout *nameLay = new QHBoxLayout();
    m_enablKeyNameGr = new QGroupBox(showKeySigName(), m_1_keys);
			m_enablKeyNameGr->setCheckable(true);
			m_enablKeyNameGr->setChecked(gl->SshowKeySignName);
			m_enablKeyNameGr->setDisabled(!gl->SkeySignatureEnabled);

    m_nameStyleGr = new TnotationRadioGroup(gl->SnameStyleInKeySign, false, m_1_keys);
    nameLay->addWidget(m_nameStyleGr);

    m_nameExtGr = new QGroupBox(tr("Naming extension"));

    QVBoxLayout *majLay = new QVBoxLayout();
    m_majExtLab = new QLabel(tr("in the major keys:"), m_1_keys);
    majLay->addWidget(m_majExtLab,0,Qt::AlignCenter);
    m_majEdit = new QLineEdit(gl->SmajKeyNameSufix, m_1_keys);
    m_majEdit->setMaxLength(10);
    m_majEdit->setAlignment(Qt::AlignCenter);
    majLay->addWidget(m_majEdit, 0, Qt::AlignCenter);
    m_majExampl = new QLabel(m_1_keys);
    m_majExampl->setAlignment(Qt::AlignCenter);
    majLay->addWidget(m_majExampl, 0, Qt::AlignCenter);
    majLay->addStretch(1);

    QVBoxLayout *minLay = new QVBoxLayout();
    m_minExtLab = new QLabel(tr("in the minor keys:"));
    minLay->addWidget(m_minExtLab, 0, Qt::AlignCenter);
    m_minEdit = new QLineEdit(gl->SminKeyNameSufix, m_1_keys);
    m_minEdit->setMaxLength(10);
    m_minEdit->setAlignment(Qt::AlignCenter);
    minLay->addWidget(m_minEdit, 0, Qt::AlignCenter);
    m_minExampl = new QLabel(m_1_keys);
    m_minExampl->setAlignment(Qt::AlignCenter);
    minLay->addWidget(m_minExampl, 0, Qt::AlignCenter);
    minLay->addStretch(1);

    QHBoxLayout *nameExtLay = new QHBoxLayout();
    nameExtLay->addLayout(majLay);
    nameExtLay->addLayout(minLay);
    m_nameExtGr->setLayout(nameExtLay);

    nameLay->addWidget(m_nameExtGr);

    m_enablKeyNameGr->setLayout(nameLay);
    keyLay->addWidget(m_enablKeyNameGr);
	   m_1_keys->setLayout(keyLay);
    
// 2. Clefs settings
	m_2_clefs = new QWidget();
	m_toolBox->addItem(m_2_clefs, "2. " + tr("Clefs"));
	
		m_clefSelector = new TselectClef(m_2_clefs);
		QHBoxLayout* clefLay = new QHBoxLayout;
		QLabel *clefUsageLab = new QLabel(tr("Default clef").replace(" ", "<br>"), m_2_clefs);
		clefUsageLab->setAlignment(Qt::AlignCenter);
		  clefLay->addWidget(clefUsageLab);
		  m_2_clefs->setStatusTip(tr("Select default clef for the application.") + "<br><b>" + tr("Remember! Not all clefs are suitable for some possible tunings or instrument types!") + "<b>");
		  clefLay->addWidget(m_clefSelector, 0, Qt::AlignCenter);
		m_clefSelector->selectClef(gl->Sclef);
	   m_2_clefs->setLayout(clefLay);
		 
// 3. Miscellaneous score settings
	m_3_misc = new QWidget();
	m_toolBox->addItem(m_3_misc, "3. " + tr("Miscellaneous score settings"));
	
		QLabel *tempoLab = new QLabel(tr("tempo of played notes"), m_3_misc);
		m_tempoSpin = new QSpinBox(m_3_misc);
			m_tempoSpin->setMinimum(50);
			m_tempoSpin->setMaximum(240);
	
    QLabel *colLab = new QLabel(tr("note-cursor color"), this);
    m_notePointColorBut = new TcolorButton(gl->SpointerColor, this);
		
		QVBoxLayout *miscLay = new QVBoxLayout;
		QHBoxLayout *tempoLay = new QHBoxLayout;
			tempoLay->addStretch(2);
			tempoLay->addWidget(tempoLab);
			tempoLay->addStretch(1);
			tempoLay->addWidget(m_tempoSpin);
			tempoLay->addStretch(2);
		miscLay->addLayout(tempoLay);
		miscLay->addStretch();
		QHBoxLayout *colLay = new QHBoxLayout;
			colLay->addStretch(1);
			colLay->addWidget(colLab, 0, Qt::AlignRight);
			colLay->addStretch();
			colLay->addWidget(m_notePointColorBut);
			colLay->addStretch(1);
		miscLay->addLayout(colLay);
	   m_3_misc->setLayout(miscLay);


	QVBoxLayout *mainLay = new QVBoxLayout;
	mainLay->addWidget(m_toolBox);
	
	setLayout(mainLay);

    connect(m_enablKeySignCh, SIGNAL(toggled(bool)), this, SLOT(enableKeySignGroup(bool)));
    connect(m_nameStyleGr, SIGNAL(noteNameStyleWasChanged(Tnote::EnameStyle)), this, SLOT(nameStyleWasChanged(Tnote::EnameStyle)));
    connect(m_majEdit ,SIGNAL(textChanged(QString)), this, SLOT(majorExtensionChanged()));
    connect(m_minEdit ,SIGNAL(textChanged(QString)), this, SLOT(minorExtensionChanged()));
    m_majExampl->setText(getMajorExample(m_workStyle));
    m_minExampl->setText(getMinorExample(m_workStyle));
}


void TscoreSettings::enableKeySignGroup(bool enable) {
    m_enablKeyNameGr->setDisabled(!enable);
}


void TscoreSettings::majorExtensionChanged() {
    m_majExampl->setText(getMajorExample(m_workStyle));
}


void TscoreSettings::minorExtensionChanged() {
    m_minExampl->setText(getMinorExample(m_workStyle));
}


QString TscoreSettings::getMajorExample(Tnote::EnameStyle nameStyle) {
    Tnote noteE = Tnote(3,0,0);
    Tnote noteBflat = Tnote(7,0,-1);
    QString S;
    if (m_majEdit->text().isEmpty() || m_majEdit->text() == " ") 
		S = "";
	else S = "-"+m_majEdit->text();
    return forExample() + "<br><b>" + noteE.toText(nameStyle,false) + S +
              "<br>" + noteBflat.toText(nameStyle,false) + S + "</b>";
}


QString TscoreSettings::getMinorExample(Tnote::EnameStyle nameStyle) {
    Tnote noteCsharp = Tnote(1,0,1);
    Tnote noteG = Tnote(5,0,0);
    QString S;
    if (m_minEdit->text().isEmpty() || m_minEdit->text() == " ")
		S = "";
	else S = "-"+m_minEdit->text();
	return forExample() + "<br><b>" +
              noteCsharp.toText(nameStyle,false).toLower()+ S + "<br>" + noteG.toText(nameStyle,false).toLower() + S + "</b>";
}


void TscoreSettings::setDefaultClef(Tclef clef) {
		m_clefSelector->selectClef(clef);
}



void TscoreSettings::nameStyleWasChanged(Tnote::EnameStyle nameStyle) {
		// Tnote::toRichText() method returns only names in user preferred according to settings
	// To cheat it and force note name in any given style we are resetting pointer of is7th_B 
	// then Tnote skips filtering of a style during name generation.
		bool* tmpPtr = TnameStyleFilter::is7th_B();
		TnameStyleFilter::setStyleFilter(0, TnameStyleFilter::solfegeStyle());
    m_workStyle = nameStyle;
    m_majExampl->setText(getMajorExample(m_workStyle));
    m_minExampl->setText(getMinorExample(m_workStyle));
		TnameStyleFilter::setStyleFilter(tmpPtr, TnameStyleFilter::solfegeStyle()); // restore is7th_B settings
}


void TscoreSettings::saveSettings() {
    gl->SkeySignatureEnabled = m_enablKeySignCh->isChecked();
    if (gl->SkeySignatureEnabled) { //changed only if key signature is enabled
		if (m_majEdit->text() == "") m_majEdit->setText(" "); // because "" means default suffix for language
        gl->SmajKeyNameSufix = m_majEdit->text();
		if (m_minEdit->text() == "") m_minEdit->setText(" ");
        gl->SminKeyNameSufix = m_minEdit->text();
        gl->SnameStyleInKeySign = m_nameStyleGr->getNameStyle();
        gl->SshowKeySignName = m_enablKeyNameGr->isChecked();
        TkeySignature::setNameStyle(gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);
    }
    gl->SpointerColor = m_notePointColorBut->getColor();
    gl->SpointerColor.setAlpha(200);
		gl->Sclef = m_clefSelector->selectedClef().type();
}


void TscoreSettings::restoreDefaults() {
		m_enablKeySignCh->setChecked(false);
		m_majEdit->setText(TkeySignature::majorSufixTxt());
		m_minEdit->setText(TkeySignature::minorSufixTxt());
		m_enablKeyNameGr->setChecked(true);
		/** This method is called by TsettingsDialog witch manages all of setting.
		 * IT HAS TO TAKE A CARE 
		 * that seventh note is also restored to default (from translation). */
		if (Tpage_3::keyNameStyle() == "solfege")
				m_nameStyleGr->setNameStyle(gl->getSolfegeStyle());
		else
			if (Tpage_3::note7txt().toLower() == "b")
					m_nameStyleGr->setNameStyle(Tnote::e_nederl_Bis);
			else
					m_nameStyleGr->setNameStyle(Tnote::e_deutsch_His);
		m_notePointColorBut->setColor(Tcolor::invert(palette().highlight().color()));
		m_clefSelector->selectClef(Tclef(Tclef::e_treble_G_8down));
		nameStyleWasChanged(m_nameStyleGr->getNameStyle());
}


void TscoreSettings::seventhIsBChanged(bool isB) {
    m_nameStyleGr->seventhNoteWasChanged(isB);
    nameStyleWasChanged(m_nameStyleGr->getNameStyle());
}


void TscoreSettings::defaultClefChanged(Tclef clef) {
		m_clefSelector->selectClef(clef);
}




