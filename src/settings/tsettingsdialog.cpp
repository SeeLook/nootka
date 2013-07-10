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

#include "tsettingsdialog.h"
#include "tglobals.h"
#include "examsettings.h"
#include "tcolorbutton.h"
#include "tupdateprocess.h"
#include "tguitarsettings.h"
#include "tscoresettings.h"
#include "namesettings.h"
#include <audioinsettings.h>
#include <audiooutsettings.h>
#include <taudioparams.h>
#include <trtaudioabstract.h>
#include <tmidiout.h>
#include "tsimplescore.h"
#include <QtGui>


extern Tglobals *gl;


//############# GlobalSettings IMPLEMENTATION ##################

TglobalSettings::TglobalSettings(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout();
    lay->setAlignment(Qt::AlignCenter);
    m_otherEnharmChBox = new QCheckBox(tr("show other enharmonics variants of a note"),this);
    m_otherEnharmChBox->setStatusTip(tr("Shows enharmonical variants of a note.<br>F.e.: E note is also Fb (F flat) and Dx (D with double sharp)."));
    m_otherEnharmChBox->setChecked(gl->showEnharmNotes);
	lay->addWidget(m_otherEnharmChBox);
	QHBoxLayout *colorLay = new QHBoxLayout;
	QLabel *colorLab = new QLabel(tr("color of enharminic notes/names"), this);
	m_enharmColorBut = new TcolorButton(gl->enharmNotesColor, this);
	colorLay->addWidget(colorLab);
	colorLay->addWidget(m_enharmColorBut);
	lay->addLayout(colorLay);
	lay->addStretch(1);
    m_dblAccChBox = new QCheckBox(tr("use double accidentals"),this);
    m_dblAccChBox->setStatusTip(tr("If checked, you can use double sharps and double flats."));
    m_dblAccChBox->setChecked(gl->doubleAccidentalsEnabled);
    lay->addWidget(m_dblAccChBox);
    lay->addStretch(1);
    m_hintsEnabledChBox = new QCheckBox(tr("show hints"), this);
    m_hintsEnabledChBox->setChecked(gl->hintsEnabled);
    m_hintsEnabledChBox->setStatusTip(tr("Show descriptions of interface's elements."));
    lay->addWidget(m_hintsEnabledChBox);
    lay->addStretch(1);
	QHBoxLayout *langLay = new QHBoxLayout;
	langLay->addStretch(1);
	QLabel *langLab = new QLabel(tr("Application's language"), this);
	langLay->addWidget(langLab);
	langLay->addStretch(1);
	m_langCombo = new QComboBox(this);
	langLay->addWidget(m_langCombo);
	langLay->addStretch(1);
	m_langCombo->setStatusTip(tr("Select a language.<br><span style=\"color: red;\">It requires restart of application !!</span>"));
	m_langList[""] = tr("default");
	m_langList["cs"] = QString::fromUtf8("český");
	m_langList["en"] = "english";
  m_langList["fr"] = QString::fromUtf8("français");
	m_langList["pl"] = "polski";
	QMapIterator<QString, QString> i(m_langList);
	int id = 0;
	while (i.hasNext()) {
		i.next();
		m_langCombo->addItem(QIcon(gl->path + "picts/flags-" + i.key() + ".png"), i.value());
		if (i.key() == gl->lang)
			m_langCombo->setCurrentIndex(id);
		id++;
	}
	m_langCombo->insertSeparator(1);
	lay->addLayout(langLay);
	lay->addStretch(1);
  
  QGroupBox *updateBox = new QGroupBox(this);
  QVBoxLayout *upLay = new QVBoxLayout;
  m_updateButton = new QPushButton(tr("Check for updates"), this);
  upLay->addWidget(m_updateButton);
  m_updateLabel = new QLabel(" ", this);
  upLay->addWidget(m_updateLabel);
  updateBox->setLayout(upLay);
  lay->addWidget(updateBox);
  lay->addStretch(1);
  if (TupdateProcess::isPossible())
    connect(m_updateButton, SIGNAL(clicked()), this, SLOT(updateSlot()));
  else 
    updateBox->hide();
  
  setLayout(lay);
}

void TglobalSettings::saveSettings() {
	gl->doubleAccidentalsEnabled = m_dblAccChBox->isChecked();
	gl->showEnharmNotes = m_otherEnharmChBox->isChecked();
	gl->hintsEnabled = m_hintsEnabledChBox->isChecked();
	gl->enharmNotesColor = m_enharmColorBut->getColor();
	QMapIterator<QString, QString> i(m_langList);
	while (i.hasNext()) {
		i.next();
		if (m_langCombo->currentText() == i.value()) {
			gl->lang = i.key();
			break;
		}
	}
}

void TglobalSettings::updateSlot() {
  TupdateProcess *process = new TupdateProcess(false, this);
  if (process->isPossible()) {
    m_updateButton->setDisabled(true);
    connect(process, SIGNAL(updateOutput(QString)), this, SLOT(processOutputSlot(QString)));
    process->start();
  }
}


void TglobalSettings::processOutputSlot(QString output) {
    m_updateLabel->setText(output);
}



//############# SettingsDialog IMPLEMENTATION ##################
TsettingsDialog::TsettingsDialog(QWidget *parent) :
        TsettingsDialogBase(parent),
        m_globalSett(0),
        m_scoreSett(0),
        m_nameSett(0),
        m_guitarSett(0),
        m_examSett(0),
        m_sndOutSett(0),
        m_sndInSett(0),
        m_audioSettingsPage(0),
        m_jackChBox(0)
{
    setWindowTitle("Nootka - "+tr("application's settings"));

    navList->setFixedWidth(110);
    navList->addItem(tr("Common"));
    navList->item(0)->setIcon(QIcon(gl->path+"picts/global.png"));
    navList->item(0)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Score"));
    navList->item(1)->setIcon(QIcon(gl->path+"picts/scoreSettings.png"));
    navList->item(1)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Names","name-calling"));
    navList->item(2)->setIcon(QIcon(gl->path+"picts/nameSettings.png"));
    navList->item(2)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Guitar"));
    navList->item(3)->setIcon(QIcon(gl->path+"picts/guitarSettings.png"));
    navList->item(3)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Exam"));
    navList->item(4)->setIcon(QIcon(gl->path+"picts/questionsSettings.png"));
    navList->item(4)->setTextAlignment(Qt::AlignCenter);
    navList->addItem(tr("Sound"));
    navList->item(5)->setIcon(QIcon(gl->path+"picts/soundSettings.png"));
    navList->item(5)->setTextAlignment(Qt::AlignCenter);
    
    defaultBut->show();
    defaultBut->setStatusTip(tr("Restore default settings for above parameters."));
    
    m_globalSett = new TglobalSettings();
//     m_scoreSett = new TscoreSettings();
//     m_nameSett = new NameSettings();
//     m_guitarSett = new TguitarSettings();
//     m_examSett = new ExamSettings(gl->E, &gl->EquestionColor, &gl->EanswerColor, &gl->EnotBadColor);
// #if defined (Q_OS_LINUX)
    m_sndInSett = new AudioInSettings(gl->A, gl->path);
    m_sndOutSett = new AudioOutSettings(gl->A, m_sndInSett); // m_sndInSett is bool - true when exist
    QWidget *audioSettingsPage = new QWidget();
    QTabWidget *sndTTab = new QTabWidget(audioSettingsPage);
    QVBoxLayout *audioLay = new QVBoxLayout;
    audioLay->addWidget(sndTTab);
#if defined(__UNIX_JACK__)
    m_jackChBox = new QCheckBox(tr("use JACK (Jack Audio Connection Kit"), audioSettingsPage);
    m_jackChBox->setChecked(gl->A->useJACK);
    audioLay->addWidget(m_jackChBox, 0, Qt::AlignCenter);
    m_jackChBox->setStatusTip("Uses JACK if it is run or other sound backend if not.<br>EXPERIMENTAL and not tested.<br>Let me know when you will get this working.");
    connect(m_jackChBox, SIGNAL(toggled(bool)), this, SLOT(changeUseJack()));
#endif
    if (m_sndInSett)
        sndTTab->addTab(m_sndInSett, tr("listening"));
    sndTTab->addTab(m_sndOutSett, tr("playing"));
    audioSettingsPage->setLayout(audioLay);

    stackLayout->addWidget(m_globalSett);
//     stackLayout->addWidget(m_scoreSett);
//     stackLayout->addWidget(m_nameSett);
//     stackLayout->addWidget(m_guitarSett);
//     stackLayout->addWidget(m_examSett);
//     stackLayout->addWidget(audioSettingsPage);

    connect(navList, SIGNAL(currentRowChanged(int)), this, SLOT(changeSettingsWidget(int)));
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
//     connect(m_nameSett, SIGNAL(seventhIsBChanged(bool)), m_scoreSett, SLOT(seventhIsBChanged(bool)));

    navList->setCurrentRow(0);

}


void TsettingsDialog::saveSettings() {
  if (m_scoreSett)
    m_scoreSett->saveSettings();
  if (m_globalSett)
    m_globalSett->saveSettings();
  if (m_nameSett)
    m_nameSett->saveSettings();
  if (m_guitarSett)
    m_guitarSett->saveSettings();
  if (m_examSett)
    m_examSett->saveSettings();
  if (m_sndOutSett)
    m_sndOutSett->saveSettings();
  if (m_sndInSett)
      m_sndInSett->saveSettings();
#if defined(__UNIX_JACK__)
    gl->A->useJACK = m_jackChBox->isChecked();
#endif
}

/** Settings pages are created on demand, also 
     * to avoid generating audio devices list every opening Nootka prefereces
     * witch is slow for pulseaudio, the list is generated on demand.
     * When user first time opens Sound settings widget.*/
void TsettingsDialog::changeSettingsWidget(int index) {
  QWidget* currentWidget = 0;
  switch (index) {
    case 0: {
      if (!m_globalSett) {
        m_globalSett = new TglobalSettings();
        stackLayout->addWidget(m_globalSett);
      }
      currentWidget = m_globalSett;
      break;
    }
    case 1: {
      if (!m_scoreSett) {
        m_scoreSett = new TscoreSettings();
        stackLayout->addWidget(m_scoreSett);
      }
      if (m_nameSett)
          connect(m_nameSett, SIGNAL(seventhIsBChanged(bool)), m_scoreSett, SLOT(seventhIsBChanged(bool)));
      currentWidget = m_scoreSett;
      break;
    }
    case 2: {
      if (!m_nameSett) {
        m_nameSett = new NameSettings();
        stackLayout->addWidget(m_nameSett);        
      }
      if (m_scoreSett)
          connect(m_nameSett, SIGNAL(seventhIsBChanged(bool)), m_scoreSett, SLOT(seventhIsBChanged(bool)));
      currentWidget = m_nameSett;
      break;
    }
    case 3: {
      if (!m_guitarSett) {
        m_guitarSett = new TguitarSettings();
        stackLayout->addWidget(m_guitarSett);
      }
      currentWidget = m_guitarSett;
      break;
    }
    case 4: {
      if (!m_examSett) {
        m_examSett = new ExamSettings(gl->E, &gl->EquestionColor, &gl->EanswerColor, &gl->EnotBadColor);
        stackLayout->addWidget(m_examSett);
      }
      currentWidget = m_examSett;
      break;
    }
    case 5: {
      if (!m_audioSettingsPage) {
        createAudioPage();
        stackLayout->addWidget(m_audioSettingsPage);
        m_sndInSett->generateDevicesList();
        m_sndOutSett->generateDevicesList();
      }
      currentWidget = m_audioSettingsPage;
      break;
    }
  }
  stackLayout->setCurrentWidget(currentWidget);
//   stackLayout->setCurrentIndex(index);
//   if (index == 5 && m_sndInSett) { // generate devices list for sound settings if sound is available
//       m_sndInSett->generateDevicesList();
//       m_sndOutSett->generateDevicesList();
//   }
}


void TsettingsDialog::createAudioPage() {
    m_sndInSett = new AudioInSettings(gl->A, gl->path);
    m_sndOutSett = new AudioOutSettings(gl->A, m_sndInSett); // m_sndInSett is bool - true when exist
    m_audioSettingsPage = new QWidget();
    QTabWidget *sndTTab = new QTabWidget(m_audioSettingsPage);
    QVBoxLayout *audioLay = new QVBoxLayout;
    audioLay->addWidget(sndTTab);
#if defined(__UNIX_JACK__)
    m_jackChBox = new QCheckBox(tr("use JACK (Jack Audio Connection Kit"), m_audioSettingsPage);
    m_jackChBox->setChecked(gl->A->useJACK);
    audioLay->addWidget(m_jackChBox, 0, Qt::AlignCenter);
    m_jackChBox->setStatusTip("Uses JACK if it is run or other sound backend if not.<br>EXPERIMENTAL and not tested.<br>Let me know when you will get this working.");
    connect(m_jackChBox, SIGNAL(toggled(bool)), this, SLOT(changeUseJack()));
#endif
    if (m_sndInSett)
        sndTTab->addTab(m_sndInSett, tr("listening"));
    sndTTab->addTab(m_sndOutSett, tr("playing"));
    m_audioSettingsPage->setLayout(audioLay);
}



void TsettingsDialog::changeUseJack() {
#if defined(__UNIX_JACK__)
  TrtAudioAbstract::setUseJACK(m_jackChBox->isChecked());
  TmidiOut::setUseJack(m_jackChBox->isChecked());
  m_sndInSett->setDevicesCombo();
  m_sndOutSett->setDevicesCombo();
#endif
}



