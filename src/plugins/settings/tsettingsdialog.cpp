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

#include "tsettingsdialog.h"
#include "tglobalsettings.h"
#include "tscoresettings.h"
#include "tguitarsettings.h"
#include "texamsettings.h"
#include "audioinsettings.h"
#include "audiooutsettings.h"
#include "tlaysettings.h"
#if defined (Q_OS_ANDROID)
  #include <tmenuwidget.h>
#else
  #include <trtaudio.h>
  #include <tmidiout.h>
#endif
#include <tmisctrans.h>
#include <tinitcorelib.h>
#include <taudioparams.h>
#include <tscoreparams.h>
#include <tpath.h>
#include <tlayoutparams.h>
#include <qtr.h>
#include <QtWidgets/QtWidgets>



TsettingsDialog::TsettingsDialog(QWidget *parent, EsettingsMode mode) :
	TsettingsDialogBase(parent),
	m_globalSett(0), m_scoreSett(0),
	m_guitarSett(0),
	m_examSett(0),
  m_sndOutSett(0), m_sndInSett(0),
	m_laySett(0),
  m_audioSettingsPage(0),
	m_7thNoteToDefaults(false),
	m_mode(mode),
	m_resetToDefaults(false)
{
#if !defined (Q_OS_ANDROID)
	if (m_mode == e_settings)
    setWindowTitle(QLatin1String("Nootka - ") + tr("application's settings"));
	else if (m_mode == e_exam)
		setWindowTitle(tr("Simple exam settings"));
	else
		setWindowTitle(tr("Simple exercise settings"));
#endif

	setWindowIcon(QIcon(Tpath::img("systemsettings")));
  if (mode == e_settings) {
    addItem(tr("Common"), Tpath::img("global"));
    addItem(tr("Score"), Tpath::img("scoreSettings"));
    addItem(tr("Instrument"), Tpath::img("guitarSettings"));
    addItem(tr("Sound"), Tpath::img("soundSettings"));
    addItem(tr("Exercises") + "\n& " + tr("Exam"), Tpath::img("questionsSettings"));
  // 		navList->addItem(tr("Shortcuts"));
  //     navList->item(6)->setIcon(QIcon(Tpath::img("shortcuts")));
  //     navList->item(6)->setTextAlignment(Qt::AlignCenter);
      addItem(tr("Appearance"), Tpath::img("appearance"));
  }

  defaultBut = buttonBox->addButton(QDialogButtonBox::RestoreDefaults);
    defaultBut->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    defaultBut->setStatusTip(tr("Restore default settings for above parameters."));
  m_helpButt = buttonBox->addButton(QDialogButtonBox::Help);
    m_helpButt->setIcon(QIcon(Tpath::img("help")));
    m_helpButt->setStatusTip(helpButtonTipText());
  okBut = buttonBox->addButton(QDialogButtonBox::Apply);
    okBut->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
  cancelBut = buttonBox->addButton(QDialogButtonBox::Cancel);
    cancelBut->setIcon(style()->standardIcon(QStyle::SP_DialogCancelButton));

  connect(okBut, &QPushButton::clicked, this, &TsettingsDialog::accept);
  if (mode == e_settings)
    connect(navList, &QListWidget::currentRowChanged, this, &TsettingsDialog::changeSettingsWidget);
  connect(this, &TsettingsDialog::accepted, this, &TsettingsDialog::saveSettings);
  connect(defaultBut, &QPushButton::clicked, this, &TsettingsDialog::restoreDefaults);
  connect(m_helpButt, &QPushButton::clicked, this, &TsettingsDialog::helpSlot);
#if !defined (Q_OS_ANDROID) && !defined (Q_OS_MAC)
  connect(this, &TsettingsDialog::rejected, this, &TsettingsDialog::cancelSlot);
#endif

  if (mode == e_settings) {
      navList->setCurrentRow(0);
      changeSettingsWidget(1); // score settings must to exist
#if !defined (Q_OS_ANDROID)
      changeSettingsWidget(3);
      changeSettingsWidget(2); // instrument settings makes window big enough, to avoid re-sizing
      setHighestPage(m_guitarSett);
      QTimer::singleShot(150, [=] { hackSize(); } ); //HACK: adjust dialog to biggest page
#endif
      changeSettingsWidget(0);
  } else if (mode == e_audio) { // NOTE: so far it occurs under Android - maximized, so no need to hacking the size
      changeSettingsWidget(3);
  } else {
#if defined (Q_OS_ANDROID)
      addItem(qTR("QPlatformTheme", "Apply"), Tpath::img("check"));
      addItem(qTR("QPlatformTheme", "Help"), Tpath::img("help"));
      addItem(qTR("QPlatformTheme", "Cancel"), Tpath::img("exit"));
      connect(navList, &QListWidget::currentRowChanged, [=] {
            if (navList->currentRow() == 0)
              accept();
            else if (navList->currentRow() == 1)
              helpSlot();
            else
              reject();
      });
      menuButton->hide();
#else
      navList->hide();
      defaultBut->hide();
#endif
      changeSettingsWidget(4);
  }
}


#if !defined (Q_OS_ANDROID) && !defined(Q_OS_MAC) && (defined (Q_OS_LINUX) || defined (Q_OS_WIN))
void TsettingsDialog::cancelSlot() {
  if (m_sndInSett && Tcore::gl()->A->JACKorASIO != m_sndInSett->rtApiCheckBox()->isChecked())
    TrtAudio::setJACKorASIO(Tcore::gl()->A->JACKorASIO);
}
#endif

void TsettingsDialog::saveSettings() {
  if (m_scoreSett)
			m_scoreSett->saveSettings();
  if (m_globalSett)
			m_globalSett->saveSettings();
  if (m_guitarSett) {
			m_guitarSett->saveSettings();
			if (!m_audioSettingsPage) // when no audio settings set appropriate audio-out instrument
				Tcore::gl()->A->audioInstrNr = qBound(1, m_guitarSett->currentInstrument(), 3);
			if (!m_laySett)
				Tcore::gl()->L->guitarEnabled = (Tcore::gl()->instrument != e_noInstrument);
	}
  if (m_examSett)
			m_examSett->saveSettings();
  if (m_sndOutSett) // if audio out settings was created
			m_sndOutSett->saveSettings(); // respect user settings
  if (m_sndInSett)
				m_sndInSett->saveSettings();
	if (m_7thNoteToDefaults) {
		if ((TmiscTrans::note7txt().toLower() == QLatin1String("b")) != (Tcore::gl()->S->seventhIs_B)) {
			/** NOTE As long as TscoreSettings is created at first and always exist 
			 * only adjustment of global note names is required. 
			 * How: When user opens Name settings and changes 7-th note TscoreSettings changes automatically 
			 * This condition is called in opposite situation: 
			 * TscoreSettings wants defaults and already has been adjusted. 
			 * Theoretically - if TscoreSettings would not exist it is more difficult to restore its defaults here. */
			if (TmiscTrans::note7txt().toLower() == QLatin1String("b"))
					Tcore::gl()->S->seventhIs_B = true;
			else
					Tcore::gl()->S->seventhIs_B = false;
		}
	}
	if (m_laySett)
			m_laySett->saveSettings();
}


void TsettingsDialog::restoreDefaults() {
	if (stackLayout->currentWidget() == m_globalSett)
		m_globalSett->restoreDefaults();
	if (stackLayout->currentWidget() == m_scoreSett) {
		m_scoreSett->restoreDefaults();
		m_7thNoteToDefaults = true;
	}
	if (stackLayout->currentWidget() == m_guitarSett)
		m_guitarSett->restoreDefaults();
	if (stackLayout->currentWidget() == m_examSett)
		m_examSett->restoreDefaults();
	if (m_audioSettingsPage) {
		if (m_audioTab->currentWidget() == m_sndInSett)
			m_sndInSett->restoreDefaults();
		else if (m_audioTab->currentWidget() == m_sndOutSett)
			m_sndOutSett->restoreDefaults();
	}
	if (stackLayout->currentWidget() == m_laySett)
		m_laySett->restoreDefaults();
}


void TsettingsDialog::allDefaultsRequired() {
  m_resetToDefaults = true;
  close();
}


    /**
     * Settings pages are created on demand, also
     * to avoid generating audio devices list every opening Nootka preferences
     * witch is slow for pulseaudio, the list is generated on demand,
     * when user first time opens Sound settings widget.
     */
void TsettingsDialog::changeSettingsWidget(int index) {
  QWidget* currentWidget = 0;
  if (m_audioSettingsPage)
    m_sndInSett->stopSoundTest();
  switch (index) {
    case 0: {
      if (!m_globalSett) {
        addPage(m_globalSett = new TglobalSettings());
#if defined (Q_OS_ANDROID)
        markChanges(m_globalSett);
#endif
        connect(m_globalSett, &TglobalSettings::restoreAllDefaults, this, &TsettingsDialog::allDefaultsRequired);
      }
      currentWidget = m_globalSett;
      break;
    }
    case 1: {
      if (!m_scoreSett) {
        addPage(m_scoreSett = new TscoreSettings());
#if defined (Q_OS_ANDROID)
        markChanges(m_scoreSett);
#endif
				if (m_guitarSett) {
						m_scoreSett->setDefaultClef(m_guitarSett->currentClef());
						connect(m_guitarSett, &TguitarSettings::clefChanged, m_scoreSett, &TscoreSettings::defaultClefChanged);
				}
			}
      currentWidget = m_scoreSett;
      break;
    }
    case 2: {
      if (!m_guitarSett) {
        addPage(m_guitarSett = new TguitarSettings());
#if defined (Q_OS_ANDROID)
        markChanges(m_guitarSett);
#endif
        if (m_scoreSett)
          connect(m_guitarSett, &TguitarSettings::clefChanged, m_scoreSett, &TscoreSettings::defaultClefChanged);
        if (m_sndOutSett)
          connect(m_guitarSett, &TguitarSettings::instrumentChanged, m_sndOutSett, &AudioOutSettings::whenInstrumentChanged);
        if (m_sndInSett) {
          connect(m_guitarSett, SIGNAL(tuneChanged(Ttune*)), m_sndInSett, SLOT(tuneWasChanged(Ttune*)));
          connect(m_guitarSett, &TguitarSettings::instrumentChanged, m_sndInSett, &AudioInSettings::whenInstrumentChanged);
        }
        if (m_laySett)
          connect(m_guitarSett, &TguitarSettings::instrumentChanged, m_laySett, &TlaySettings::instrumentChanged);
      }
      currentWidget = m_guitarSett;
      break;
    }
    case 4: {
      if (!m_examSett) {
        stackLayout->addWidget(m_examSett = new TexamSettings(0, m_mode));
#if defined (Q_OS_ANDROID)
        markChanges(m_examSett);
#endif
      }
      currentWidget = m_examSett;
      break;
    }
    case 3: {
      if (!m_audioSettingsPage) {
        createAudioPage();
#if defined (Q_OS_ANDROID)
        markChanges(m_audioSettingsPage);
#endif
        stackLayout->addWidget(m_audioSettingsPage);
        m_sndInSett->generateDevicesList();
        m_sndOutSett->generateDevicesList();
				if (m_guitarSett) { // update pitches range according to guitar settings state
					m_sndOutSett->whenInstrumentChanged(m_guitarSett->currentInstrument());
          m_sndInSett->whenInstrumentChanged(m_guitarSett->currentInstrument());
					m_sndInSett->tuneWasChanged(m_guitarSett->currentTune());
					connect(m_guitarSett, &TguitarSettings::instrumentChanged, m_sndOutSett, &AudioOutSettings::whenInstrumentChanged);
          connect(m_guitarSett, &TguitarSettings::instrumentChanged, m_sndInSett, &AudioInSettings::whenInstrumentChanged);
					connect(m_guitarSett, SIGNAL(tuneChanged(Ttune*)), m_sndInSett, SLOT(tuneWasChanged(Ttune*)));
				}
      }
      currentWidget = m_audioSettingsPage;
      break;
    }
		case 5: {
			if (!m_laySett) {
				m_laySett = new TlaySettings(Tcore::gl()->L);
#if defined (Q_OS_ANDROID)
        markChanges(m_laySett);
#endif
				stackLayout->addWidget(m_laySett);
				if (m_guitarSett) {
          connect(m_guitarSett, &TguitarSettings::instrumentChanged, m_laySett, &TlaySettings::instrumentChanged);
					m_laySett->instrumentChanged(m_guitarSett->currentInstrument());
				}
			}
			currentWidget = m_laySett;
			break;
		}
  }
  stackLayout->setCurrentWidget(currentWidget);
}


void TsettingsDialog::createAudioPage() {
#if !defined (Q_OS_ANDROID) && (defined (Q_OS_LINUX) || defined (Q_OS_WIN))
	TrtAudio::initJACKorASIO(Tcore::gl()->A->JACKorASIO);
#endif
	m_sndInSett = new AudioInSettings(Tcore::gl()->A, Tcore::gl()->Gtune());
	m_sndOutSett = new AudioOutSettings(Tcore::gl()->A, m_sndInSett); // m_sndInSett is bool - true when exist
	m_audioSettingsPage = new QWidget();
	m_audioTab = new QTabWidget(m_audioSettingsPage);
	QVBoxLayout *audioLay = new QVBoxLayout;
#if defined (Q_OS_ANDROID)
  audioLay->setContentsMargins(0, 0, 0, 0);
#endif
	audioLay->addWidget(m_audioTab);
	m_audioTab->addTab(m_sndInSett, tr("listening"));
	m_audioTab->addTab(m_sndOutSett, tr("playing"));
	m_audioSettingsPage->setLayout(audioLay);
	connect(m_audioTab, SIGNAL(currentChanged(int)), m_sndInSett, SLOT(stopSoundTest()));
#if !defined (Q_OS_ANDROID) && !defined (Q_OS_MAC)
	connect(m_sndInSett, &AudioInSettings::rtApiChanged, this, &TsettingsDialog::rtApiSlot);
	connect(m_sndOutSett, &AudioOutSettings::rtApiChanged, this, &TsettingsDialog::rtApiSlot);
#endif
#if defined(Q_OS_WIN)
  connect(m_sndInSett, &AudioInSettings::asioDriverChanged, m_sndOutSett, &AudioOutSettings::asioDeviceSlot);
  connect(m_sndOutSett, &AudioOutSettings::asioDriverChanged, m_sndInSett, &AudioInSettings::asioDeviceSlot);
#endif
}


void TsettingsDialog::helpSlot() {
  QString docHash = QStringLiteral("settings");
  if (stackLayout->currentWidget() == m_audioSettingsPage)
    docHash = QStringLiteral("input-settings");
  else if (stackLayout->currentWidget() == m_scoreSett)
    docHash = QStringLiteral("score-settings");
  else if (stackLayout->currentWidget() == m_guitarSett)
    docHash = QStringLiteral("instrument-settings");
  else if (stackLayout->currentWidget() == m_laySett)
    docHash = QStringLiteral("appearance-settings");
  else if (stackLayout->currentWidget() == m_examSett)
    docHash = ("exam-settings");
  openHelpLink(docHash);
}


#if !defined (Q_OS_ANDROID) && !defined (Q_OS_MAC)
void TsettingsDialog::rtApiSlot() {
  if (sender() == m_sndInSett) {
    m_sndOutSett->rtApiCheckBox()->setChecked(m_sndInSett->rtApiCheckBox()->isChecked());
    m_sndOutSett->updateAudioDevList();
  } else {
    m_sndInSett->rtApiCheckBox()->setChecked(m_sndOutSett->rtApiCheckBox()->isChecked());
    m_sndInSett->updateAudioDevList();
  }
}
#endif


