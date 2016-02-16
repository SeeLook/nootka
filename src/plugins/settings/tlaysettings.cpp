/***************************************************************************
 *   Copyright (C) 2014-2015 by Tomasz Bojczuk                             *
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

#include "tlaysettings.h"
#include <tlayoutparams.h>
#if !defined (Q_OS_ANDROID)
  #include <widgets/troundedlabel.h>
#endif
#include <music/tmelody.h>
#include <widgets/tvolumeview.h>
#include <graphics/tnotepixmap.h>
#include <music/tinstrument.h>
#include <QtWidgets/QtWidgets>



TlaySettings::TlaySettings(TlayoutParams* layParams, QWidget* parent) :
	TtouchArea(parent),
	m_layParams(layParams)
{
// tool bar
#if !defined (Q_OS_ANDROID)
  setStatusTip(tr("Show or hide main window elements."));
	m_toolBox = new QGroupBox(tr("tool bar"), this);
	m_barAutoHideChB = new QCheckBox(tr("auto hide"), this);
	m_barAutoHideChB->setChecked(m_layParams->toolBarAutoHide);
	m_textUnderRadio = new QRadioButton(tr("text under icons"), this);
	m_iconsOnlyRadio = new QRadioButton(tr("icons only"), this);
	m_textOnlyRadio = new QRadioButton(tr("text only"), this);
	if (m_layParams->iconTextOnToolBar == Qt::ToolButtonTextUnderIcon)
		m_textUnderRadio->setChecked(true);
	else if (m_layParams->iconTextOnToolBar == Qt::ToolButtonIconOnly)
		m_iconsOnlyRadio->setChecked(true);
// 	else // TODO uncomment when text only will be implemented 
// 		m_textOnlyRadio->setChecked(true);
	m_textOnlyRadio->hide();
	m_toolBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
// hints
	m_hintBox = new QGroupBox(tr("hints"), this);
	m_hintBox->setCheckable(true);
	m_hintBox->setChecked(m_layParams->hintsBarEnabled);
// 	m_hintBox->setContentsMargins(1, 1, 1, 1);
	TroundedLabel *hintsLab = new TroundedLabel("Bla, bla, bla...", this);
	hintsLab->setAlignment(Qt::AlignCenter);
	m_hintBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
#endif
// sound view
	m_soundBox = new QGroupBox(tr("sound bar"), this);
	m_soundBox->setCheckable(true);
	TvolumeView *soundView = new TvolumeView(this);
	soundView->setMinimalVolume(0.6);
#if defined (Q_OS_ANDROID)
	soundView->setFixedHeight(qApp->desktop()->availableGeometry().height() / 20);
#else
  soundView->setFixedHeight(soundView->minimumHeight());
#endif
	soundView->setVolume(0.4);
	soundView->setPaused(false);
	m_soundBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	connect(m_soundBox, &QGroupBox::toggled, soundView, &TvolumeView::setEnabled);
	m_soundBox->setChecked(m_layParams->soundViewEnabled);
// score
	m_scoreBox = new QGroupBox(this);
	Tmelody *mel = new Tmelody();
  mel->setClef(Tclef::e_treble_G_8down);
	mel->addNote(Tchunk(Tnote(1, 0), Trhythm()));
	mel->addNote(Tchunk(Tnote(2, 0), Trhythm()));
	mel->addNote(Tchunk(Tnote(3, 0), Trhythm()));
	mel->addNote(Tchunk(Tnote(4, 0), Trhythm()));
	mel->addNote(Tchunk(Tnote(5, 0), Trhythm()));
	QLabel *scoreLab = new QLabel(pixToHtml(getMelodyPixmap(mel)), this);
  delete mel;
	scoreLab->setAlignment(Qt::AlignCenter);
	scoreLab->setStyleSheet("border: 1px solid palette(Text); border-radius: 10px; background-color: palette(Base);");
	auto *descLab = new QLabel("<br><big><big><b>" + tr("There is no Nootka without a score!") + "</b></big></big><br>" +
			tr("But you can hide even all the rest of widgets to see a score only."), this);
	descLab->setAlignment(Qt::AlignCenter);
#if defined (Q_OS_ANDROID)
  descLab->setWordWrap(true);
#endif
	m_scoreBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
// guitar
	m_guitarBox = new QGroupBox(tr("guitar"), this);
	m_guitarBox->setCheckable(true);
	m_guitarBox->setChecked(m_layParams->guitarEnabled);
// 	m_guitarBox->setStyleSheet(QString("background-image: url(%1);}").arg(Tpath::img("fingbg")));
	
// layout
	QVBoxLayout *mainLay = new QVBoxLayout;
#if !defined (Q_OS_ANDROID)
		QHBoxLayout *toolLay = new QHBoxLayout;
			toolLay->addWidget(m_barAutoHideChB);
			toolLay->addStretch();
			toolLay->addWidget(m_textUnderRadio);
			toolLay->addWidget(m_iconsOnlyRadio);
			toolLay->addWidget(m_textOnlyRadio);
			toolLay->addStretch();
			m_toolBox->setLayout(toolLay);
	mainLay->addWidget(m_toolBox);
		QHBoxLayout *hintLay = new QHBoxLayout;
			hintLay->addWidget(hintsLab);
			m_hintBox->setLayout(hintLay);
#endif
		QHBoxLayout *soundLay = new QHBoxLayout;
			soundLay->addWidget(soundView);
			m_soundBox->setLayout(soundLay);
#if defined (Q_OS_ANDROID)
    mainLay->addWidget(m_soundBox);
#else
		QHBoxLayout *sndAndHintLay = new QHBoxLayout;
			sndAndHintLay->addWidget(m_hintBox);
			sndAndHintLay->addWidget(m_soundBox);
	mainLay->addLayout(sndAndHintLay);
#endif
		QVBoxLayout *scoreLay = new QVBoxLayout;
			scoreLay->addWidget(scoreLab);
			scoreLay->addWidget(descLab);
			m_scoreBox->setLayout(scoreLay);
	mainLay->addWidget(m_scoreBox);
		QHBoxLayout *guitaLay = new QHBoxLayout;
		m_guitarBox->setLayout(guitaLay);
	mainLay->addWidget(m_guitarBox);
	setLayout(mainLay);
	
}


void TlaySettings::saveSettings() {
#if !defined (Q_OS_ANDROID)
	m_layParams->toolBarAutoHide = m_barAutoHideChB->isChecked();
	if (m_textUnderRadio->isChecked())
		m_layParams->iconTextOnToolBar = Qt::ToolButtonTextUnderIcon;
	else if (m_iconsOnlyRadio->isChecked())
		m_layParams->iconTextOnToolBar = Qt::ToolButtonIconOnly;
	else
		m_layParams->iconTextOnToolBar = Qt::ToolButtonTextOnly;
	m_layParams->hintsBarEnabled = m_hintBox->isChecked();
#endif
	m_layParams->soundViewEnabled = m_soundBox->isChecked();
	m_layParams->guitarEnabled = m_guitarBox->isChecked();
}


void TlaySettings::restoreDefaults() {
#if !defined (Q_OS_ANDROID)
	m_barAutoHideChB->setChecked(false);
	m_textUnderRadio->setChecked(true);
	m_hintBox->setChecked(true);
#endif
	m_soundBox->setChecked(true);
	m_guitarBox->setChecked(true);
}


void TlaySettings::instrumentChanged(int instr) {
  if ((Einstrument)instr == e_noInstrument) {
    m_guitarBox->setChecked(false);
    m_guitarBox->setDisabled(true);
  } else {
    m_guitarBox->setDisabled(false);
		m_guitarBox->setChecked(m_layParams->guitarEnabled); // when unlocking, respect previous guitar visibility			
	}
}


void TlaySettings::resizeEvent(QResizeEvent*) {
	m_guitarBox->setFixedHeight(height() * 0.18);
}




