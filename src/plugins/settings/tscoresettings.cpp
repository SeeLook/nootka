/***************************************************************************
 *   Copyright (C) 2011-2015 by Tomasz Bojczuk                             *
 *   seelook@gmail.com   		      				                                 *
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
#include "tnotenamesettings.h"
#include <tinitcorelib.h>
#include <tscoreparams.h>
#include <music/tkeysignature.h>
#include <widgets/tselectclef.h>
#include <tcolor.h>
#include <graphics/tnotepixmap.h>
#include <music/tnamestylefilter.h>
#include <tmisctrans.h>
#include <QtWidgets/QtWidgets>
#if defined (Q_OS_ANDROID)
  #include "tlistmenu.h"
  #include <touch/ttoucharea.h>
  #include <tmtr.h>
#endif



TscoreSettings::TscoreSettings(QWidget *parent) :
  TtouchArea(parent)
{
#if defined (Q_OS_ANDROID)
  m_topList = new TlistMenu(QListWidget::LeftToRight, this);
  QStackedLayout *m_stackedLay = new QStackedLayout;
  m_stackedLay->setContentsMargins(0, 0, 0, 0);
  TtouchArea *m_1_misc, *m_2_keys, *m_3_clefs;
#else
  QToolBox *m_toolBox = new QToolBox(this);
  QWidget *m_1_misc, *m_2_keys, *m_3_clefs;
#endif

// 1. Miscellaneous score settings
#if defined (Q_OS_ANDROID)
  m_1_misc = new TtouchArea(this);
  m_topList->addItem("1. " + tr("Score settings"));
  m_stackedLay->addWidget(m_1_misc);
#else
  m_1_misc = new QWidget();
  m_toolBox->addItem(m_1_misc, "1. " + tr("Score settings"));
#endif
		m_singleNoteGr = new QGroupBox(tr("use single note only"), m_1_misc);
			m_singleNoteGr->setStatusTip(tr("When enabled, a score displays only a single note."));
			m_singleNoteGr->setCheckable(true);
			m_singleNoteGr->setChecked(Tcore::gl()->S->isSingleNoteMode);
		m_otherEnharmChBox = new QCheckBox(tr("show enharmonic variants of notes"), m_1_misc);
			m_otherEnharmChBox->setStatusTip(tr("Shows enharmonic variants of notes.<br>i.e.: the note E is also Fb (F flat) <i>and</i> Dx (D with double sharp)."));
		QLabel *colorLab = new QLabel(tr("color of enharmonic notes"), m_1_misc);
		m_enharmColorBut = new TcolorButton(Tcore::gl()->S->enharmNotesColor, m_1_misc);
		m_otherEnharmChBox->setChecked(Tcore::gl()->S->showEnharmNotes); // it has to be disabled after color button is set.
		m_dblAccChBox = new QCheckBox(tr("use double accidentals"),m_1_misc);
			m_dblAccChBox->setStatusTip(tr("If checked, you can use double sharps and double flats."));
			m_dblAccChBox->setChecked(Tcore::gl()->S->doubleAccidentalsEnabled);
		QLabel *tempoLab = new QLabel(tr("tempo of played notes"), m_1_misc);
		m_tempoSpin = new QSpinBox(m_1_misc);
			m_tempoSpin->setMinimum(50);
			m_tempoSpin->setMaximum(240);
			m_tempoSpin->setValue(Tcore::gl()->S->tempo);

    QLabel *colLab = new QLabel(tr("note-cursor color"), m_1_misc);
    m_notePointColorBut = new TcolorButton(Tcore::gl()->S->pointerColor, m_1_misc);

		QVBoxLayout *miscLay = new QVBoxLayout;
    QVBoxLayout *enColorLay = new QVBoxLayout;
#if defined (Q_OS_ANDROID)
    enColorLay->addWidget(getLabelFromStatus(m_singleNoteGr, false), 0, Qt::AlignCenter);
#endif
    enColorLay->addWidget(m_otherEnharmChBox);
#if defined (Q_OS_ANDROID)
    enColorLay->addWidget(getLabelFromStatus(m_otherEnharmChBox));
#endif
    QHBoxLayout *enharmColorLay = new QHBoxLayout;
      enharmColorLay->addStretch();
      enharmColorLay->addWidget(colorLab);
      enharmColorLay->addStretch();
      enharmColorLay->addWidget(m_enharmColorBut);
      enharmColorLay->addStretch();
    enColorLay->addLayout(enharmColorLay);
		m_singleNoteGr->setLayout(enColorLay);
		miscLay->addWidget(m_singleNoteGr);
		miscLay->addStretch();
		miscLay->addWidget(m_dblAccChBox);
#if defined (Q_OS_ANDROID)
    miscLay->addWidget(getLabelFromStatus(m_dblAccChBox));
#endif
		miscLay->addStretch();
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
    miscLay->addStretch(1);
		miscLay->addLayout(colLay);
#if defined (Q_OS_ANDROID)
    miscLay->addSpacing(Tmtr::fingerPixels());
#endif
    m_1_misc->setLayout(miscLay);
	
// 2. Key signatures settings
#if defined (Q_OS_ANDROID)
  m_2_keys = new TtouchArea(this);
  m_topList->addItem("2. " + tr("Key signatures"));
  m_stackedLay->addWidget(m_2_keys);
#else
  m_2_keys = new QWidget();
  m_toolBox->addItem(m_2_keys, "2. " + tr("Key signatures"));
#endif
    m_workStyle = Tcore::gl()->S->nameStyleInKeySign;
    QVBoxLayout *keyLay = new QVBoxLayout();
    m_enablKeySignCh = new QCheckBox(tr("enable key signature"), m_2_keys);
			m_enablKeySignCh->setChecked(Tcore::gl()->S->keySignatureEnabled);
    keyLay->addWidget(m_enablKeySignCh, 0, Qt::AlignCenter);
#if defined (Q_OS_ANDROID)
    QVBoxLayout *nameLay = new QVBoxLayout();
#else
    QHBoxLayout *nameLay = new QHBoxLayout();
#endif
    m_enablKeyNameGr = new QGroupBox(showKeySigName(), m_2_keys);
			m_enablKeyNameGr->setCheckable(true);
			m_enablKeyNameGr->setChecked(Tcore::gl()->S->showKeySignName);
			m_enablKeyNameGr->setDisabled(!Tcore::gl()->S->keySignatureEnabled);

    m_nameStyleGr = new TnotationRadioGroup(Tcore::gl()->S->nameStyleInKeySign, false, m_2_keys);
    nameLay->addWidget(m_nameStyleGr);

    m_nameExtGr = new QGroupBox(tr("Naming extension"));

    QVBoxLayout *majLay = new QVBoxLayout();
    m_majExtLab = new QLabel(tr("in the major keys:"), m_2_keys);
    majLay->addWidget(m_majExtLab,0,Qt::AlignCenter);
    m_majEdit = new QLineEdit(Tcore::gl()->S->majKeyNameSufix, m_2_keys);
    m_majEdit->setMaxLength(10);
    m_majEdit->setAlignment(Qt::AlignCenter);
    majLay->addWidget(m_majEdit, 0, Qt::AlignCenter);
    m_majExampl = new QLabel(m_2_keys);
    m_majExampl->setAlignment(Qt::AlignCenter);
    majLay->addWidget(m_majExampl, 0, Qt::AlignCenter);
    majLay->addStretch(1);

    QVBoxLayout *minLay = new QVBoxLayout();
    m_minExtLab = new QLabel(tr("in the minor keys:"));
    minLay->addWidget(m_minExtLab, 0, Qt::AlignCenter);
    m_minEdit = new QLineEdit(Tcore::gl()->S->minKeyNameSufix, m_2_keys);
    m_minEdit->setMaxLength(10);
    m_minEdit->setAlignment(Qt::AlignCenter);
    minLay->addWidget(m_minEdit, 0, Qt::AlignCenter);
    m_minExampl = new QLabel(m_2_keys);
    m_minExampl->setAlignment(Qt::AlignCenter);
    minLay->addWidget(m_minExampl, 0, Qt::AlignCenter);
    minLay->addStretch(1);

    QHBoxLayout *nameExtLay = new QHBoxLayout;
    nameExtLay->addLayout(majLay);
    nameExtLay->addLayout(minLay);
    m_nameExtGr->setLayout(nameExtLay);

    nameLay->addWidget(m_nameExtGr);

    m_enablKeyNameGr->setLayout(nameLay);
    keyLay->addWidget(m_enablKeyNameGr);
    m_2_keys->setLayout(keyLay);
    
// 2. Clefs settings
#if defined (Q_OS_ANDROID)
  m_3_clefs = new TtouchArea(this);
  m_topList->addItem("3. " + tr("Clefs"));
  m_stackedLay->addWidget(m_3_clefs);
#else
  m_3_clefs = new QWidget();
  m_toolBox->addItem(m_3_clefs, "3. " + tr("Clefs"));
#endif
		m_clefSelector = new TselectClef(m_3_clefs);
#if defined (Q_OS_ANDROID)
    QVBoxLayout* clefLay = new QVBoxLayout;
    QLabel *clefUsageLab = new QLabel(tr("Default clef"), m_3_clefs);
#else
		QHBoxLayout* clefLay = new QHBoxLayout;
    QLabel *clefUsageLab = new QLabel(tr("Default clef").replace(" ", "<br>"), m_3_clefs);
#endif
		clefUsageLab->setAlignment(Qt::AlignCenter);
      clefLay->addStretch();
		  clefLay->addWidget(clefUsageLab);
		  m_3_clefs->setStatusTip(tr("Select default clef for the application.") + "<br><b>" + tr("Remember! Not all clefs are suitable for some possible tunings or instrument types!") + "<b>");
#if defined (Q_OS_ANDROID)
      clefLay->addWidget(getLabelFromStatus(m_3_clefs, false), 0, Qt::AlignCenter);
#endif
		  clefLay->addWidget(m_clefSelector, 0, Qt::AlignCenter);
      clefLay->addStretch();
		m_clefSelector->selectClef(Tcore::gl()->S->clef);
    m_3_clefs->setLayout(clefLay);

  m_nameTab = new TnoteNameSettings(this); // QWidget under desktops and TtouchArea under mobile
  QVBoxLayout *mainLay = new QVBoxLayout;
#if defined (Q_OS_ANDROID)
  m_topList->addItem("4. " + tr("Notes naming"));
  m_stackedLay->addWidget(m_nameTab);
  mainLay->addWidget(m_topList);
  mainLay->addLayout(m_stackedLay);
  mainLay->setContentsMargins(0, 0, 0, 0);
  m_stackedLay->setCurrentIndex(0);
  m_topList->setCurrentRow(0);
  connect(m_topList, &TlistMenu::currentRowChanged, m_stackedLay, &QStackedLayout::setCurrentIndex);
  m_topList->adjustItemsLayout(100);
//   QTimer::singleShot(100, this, [this] { m_topList->setFixedHeight(m_topList->sizeHintForRow(0) + 2 * m_topList->frameWidth()); } );
#else
  m_toolBox->addItem(m_nameTab, "4. " + tr("Notes naming"));
  mainLay->addWidget(m_toolBox);
#endif
  setLayout(mainLay);

  connect(m_enablKeySignCh, &QCheckBox::toggled, this, &TscoreSettings::enableKeySignGroup);
  connect(m_nameStyleGr, &TnotationRadioGroup::noteNameStyleWasChanged, this, &TscoreSettings::nameStyleWasChanged);
  connect(m_majEdit, &QLineEdit::textChanged, this, &TscoreSettings::majorExtensionChanged);
  connect(m_minEdit, &QLineEdit::textChanged, this, &TscoreSettings::minorExtensionChanged);
  connect(m_nameTab, &TnoteNameSettings::seventhIsBChanged, this, &TscoreSettings::seventhIsBChanged);
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
  Tcore::gl()->S->keySignatureEnabled = m_enablKeySignCh->isChecked();
  if (Tcore::gl()->S->keySignatureEnabled) { //changed only if key signature is enabled
  if (m_majEdit->text() == "") m_majEdit->setText(" "); // because "" means default suffix for language
      Tcore::gl()->S->majKeyNameSufix = m_majEdit->text();
  if (m_minEdit->text() == "") m_minEdit->setText(" ");
      Tcore::gl()->S->minKeyNameSufix = m_minEdit->text();
      Tcore::gl()->S->nameStyleInKeySign = m_nameStyleGr->getNameStyle();
      Tcore::gl()->S->showKeySignName = m_enablKeyNameGr->isChecked();
//         TkeySignature::setNameStyle(Tcore::gl()->S->nameStyleInKeySign, Tcore::gl()->S->majKeyNameSufix, Tcore::gl()->S->minKeyNameSufix);
  }
  Tcore::gl()->S->pointerColor = m_notePointColorBut->getColor();
  Tcore::gl()->S->pointerColor.setAlpha(200);
  Tcore::gl()->S->clef = m_clefSelector->selectedClef().type();
  Tcore::gl()->S->doubleAccidentalsEnabled = m_dblAccChBox->isChecked();
  Tcore::gl()->S->showEnharmNotes = m_otherEnharmChBox->isChecked();
  Tcore::gl()->S->enharmNotesColor = m_enharmColorBut->getColor();
  Tcore::gl()->S->tempo = m_tempoSpin->value();
  Tcore::gl()->S->isSingleNoteMode = m_singleNoteGr->isChecked();
  m_nameTab->saveSettings();
}


void TscoreSettings::restoreDefaults() {
  m_enablKeySignCh->setChecked(false);
  m_majEdit->setText(TkeySignature::majorSufixTxt());
  m_minEdit->setText(TkeySignature::minorSufixTxt());
  m_enablKeyNameGr->setChecked(true);
  /** This method is called by TsettingsDialog witch manages all of setting.
    * IT HAS TO TAKE A CARE 
    * that seventh note is also restored to default (from translation). */
  if (TmiscTrans::keyNameStyle() == "solfege")
      m_nameStyleGr->setNameStyle(Tcore::gl()->getSolfegeStyle());
  else
    if (TmiscTrans::note7txt().toLower() == "b")
        m_nameStyleGr->setNameStyle(Tnote::e_nederl_Bis);
    else
        m_nameStyleGr->setNameStyle(Tnote::e_deutsch_His);
  m_notePointColorBut->setColor(Tcolor::invert(palette().highlight().color()));
  m_clefSelector->selectClef(Tclef(Tclef::e_treble_G_8down));
  nameStyleWasChanged(m_nameStyleGr->getNameStyle());
  m_dblAccChBox->setChecked(false);
  m_otherEnharmChBox->setChecked(false);
  m_enharmColorBut->setColor(palette().highlight().color());
  m_tempoSpin->setValue(120);
  m_singleNoteGr->setChecked(false);
  m_nameTab->restoreDefaults();
}


void TscoreSettings::seventhIsBChanged(bool isB) {
  m_nameStyleGr->seventhNoteWasChanged(isB);
  nameStyleWasChanged(m_nameStyleGr->getNameStyle());
}


void TscoreSettings::defaultClefChanged(Tclef clef) {
		m_clefSelector->selectClef(clef);
}




