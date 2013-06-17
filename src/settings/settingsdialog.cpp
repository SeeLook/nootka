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

#include "settingsdialog.h"
#include "tglobals.h"
#include "examsettings.h"
#include "tcolorbutton.h"
#include "tupdateprocess.h"
#include "tguitarsettings.h"
#include <audioinsettings.h>
#include <audiooutsettings.h>
#include <taudioparams.h>
#include <trtaudioabstract.h>
#include <tmidiout.h>
#include "tsimplescore.h"
#include <QtGui>


extern Tglobals *gl;


//############# GlobalSettings IMPLEMENTATION ##################

GlobalSettings::GlobalSettings(QWidget *parent) :
        QWidget(parent)
{
    QVBoxLayout *lay = new QVBoxLayout();
    lay->setAlignment(Qt::AlignCenter);
    otherEnharmChBox = new QCheckBox(tr("show other enharmonics variants of a note"),this);
    otherEnharmChBox->setStatusTip(tr("Shows enharmonical variants of a note.<br>F.e.: E note is also Fb (F flat) and Dx (D with double sharp)."));
    otherEnharmChBox->setChecked(gl->showEnharmNotes);
	lay->addWidget(otherEnharmChBox);
	QHBoxLayout *colorLay = new QHBoxLayout;
	QLabel *colorLab = new QLabel(tr("color of enharminic notes/names"), this);
	enharmColorBut = new TcolorButton(gl->enharmNotesColor, this);
	colorLay->addWidget(colorLab);
	colorLay->addWidget(enharmColorBut);
	lay->addLayout(colorLay);
	lay->addStretch(1);
    dblAccChBox = new QCheckBox(tr("use double accidentals"),this);
    dblAccChBox->setStatusTip(tr("If checked, you can use double sharps and double flats."));
    dblAccChBox->setChecked(gl->doubleAccidentalsEnabled);
    lay->addWidget(dblAccChBox);
    lay->addStretch(1);
    hintsEnabledChBox = new QCheckBox(tr("show hints"), this);
    hintsEnabledChBox->setChecked(gl->hintsEnabled);
    hintsEnabledChBox->setStatusTip(tr("Show descriptions of interface's elements."));
    lay->addWidget(hintsEnabledChBox);
    lay->addStretch(1);
	QHBoxLayout *langLay = new QHBoxLayout;
	langLay->addStretch(1);
	QLabel *langLab = new QLabel(tr("Application's language"), this);
	langLay->addWidget(langLab);
	langLay->addStretch(1);
	langCombo = new QComboBox(this);
	langLay->addWidget(langCombo);
	langLay->addStretch(1);
	langCombo->setStatusTip(tr("Select a language.<br><span style=\"color: red;\">It requires restart of application !!</span>"));
	langList[""] = tr("default");
	langList["cs"] = QString::fromUtf8("český");
	langList["en"] = "english";
  langList["fr"] = QString::fromUtf8("français");
	langList["pl"] = "polski";
	QMapIterator<QString, QString> i(langList);
	int id = 0;
	while (i.hasNext()) {
		i.next();
		langCombo->addItem(QIcon(gl->path + "picts/flags-" + i.key() + ".png"), i.value());
		if (i.key() == gl->lang)
			langCombo->setCurrentIndex(id);
		id++;
	}
	langCombo->insertSeparator(1);
	lay->addLayout(langLay);
	lay->addStretch(1);
  
  QGroupBox *updateBox = new QGroupBox(this);
  QVBoxLayout *upLay = new QVBoxLayout;
  updateButton = new QPushButton(tr("Check for updates"), this);
  upLay->addWidget(updateButton);
  updateLabel = new QLabel(" ", this);
  upLay->addWidget(updateLabel);
  updateBox->setLayout(upLay);
  lay->addWidget(updateBox);
  lay->addStretch(1);
  if (TupdateProcess::isPossible())
    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateSlot()));
  else 
    updateBox->hide();
  
  setLayout(lay);
}

void GlobalSettings::saveSettings() {
	gl->doubleAccidentalsEnabled = dblAccChBox->isChecked();
	gl->showEnharmNotes = otherEnharmChBox->isChecked();
	gl->hintsEnabled = hintsEnabledChBox->isChecked();
	gl->enharmNotesColor = enharmColorBut->getColor();
	QMapIterator<QString, QString> i(langList);
	while (i.hasNext()) {
		i.next();
		if (langCombo->currentText() == i.value()) {
			gl->lang = i.key();
			break;
		}
	}
}

void GlobalSettings::updateSlot() {
  TupdateProcess *process = new TupdateProcess(false, this);
  if (process->isPossible()) {
    updateButton->setDisabled(true);
    connect(process, SIGNAL(updateOutput(QString)), this, SLOT(processOutputSlot(QString)));
    process->start();
  }
}


void GlobalSettings::processOutputSlot(QString output) {
    updateLabel->setText(output);
}



//############# SettingsDialog IMPLEMENTATION ##################
SettingsDialog::SettingsDialog(QWidget *parent) :
        TsettingsDialogBase(parent)
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
    
    
    m_globalSett = new GlobalSettings();
    m_scoreSett = new ScoreSettings();
    m_nameSett = new NameSettings();
    m_guitarSett = new TguitarSettings();
    m_examSett = new ExamSettings(gl->E, &gl->EquestionColor, &gl->EanswerColor, &gl->EnotBadColor);
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
    stackLayout->addWidget(m_scoreSett);
    stackLayout->addWidget(m_nameSett);
    stackLayout->addWidget(m_guitarSett);
    stackLayout->addWidget(m_examSett);
    stackLayout->addWidget(audioSettingsPage);

    connect(navList, SIGNAL(currentRowChanged(int)), this, SLOT(changeSettingsWidget(int)));
    connect(this, SIGNAL(accepted()), this, SLOT(saveSettings()));
    connect(m_nameSett, SIGNAL(seventhIsBChanged(bool)), m_scoreSett, SLOT(seventhIsBChanged(bool)));

    navList->setCurrentRow(0);

}

/** To avoid generating audio devices list every opening Nootka prefereces
     * witch is slow for pulseaudio, the list is generated on demand.
     * When user first time opens Sound settings widget.*/
void SettingsDialog::changeSettingsWidget(int index) {
  stackLayout->setCurrentIndex(index);
  if (index == 5 && m_sndInSett) { // generate devices list for sound settings if sound is available
      m_sndInSett->generateDevicesList();
      m_sndOutSett->generateDevicesList();
  }
}


void SettingsDialog::changeUseJack() {
#if defined(__UNIX_JACK__)
  TrtAudioAbstract::setUseJACK(m_jackChBox->isChecked());
  TmidiOut::setUseJack(m_jackChBox->isChecked());
  m_sndInSett->setDevicesCombo();
  m_sndOutSett->setDevicesCombo();
#endif
}


void SettingsDialog::saveSettings() {
    m_scoreSett->saveSettings();
    m_globalSett->saveSettings();
    m_nameSett->saveSettings();
    m_guitarSett->saveSettings();
    m_examSett->saveSettings();
    m_sndOutSett->saveSettings();
    if (m_sndInSett)
      m_sndInSett->saveSettings();
#if defined(__UNIX_JACK__)
    gl->A->useJACK = m_jackChBox->isChecked();
#endif
}


