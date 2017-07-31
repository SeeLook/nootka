/***************************************************************************
 *   Copyright (C) 2011-2017 by Tomasz Bojczuk                             *
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

#include "mainwindow.h"
#include <score/tmainscore.h>
#include <guitar/tfingerboard.h>
#include <notename/tnotename.h>
#include <gui/tmainview.h>
#include <gui/ttoolbar.h>
#include <gui/tmenu.h>
#include <gui/tmelman.h>
#include <gui/tbgpixmap.h>
#include <exam/tprogresswidget.h>
#include <exam/texamview.h>
#include <tglobals.h>
#include <tscoreparams.h>
#include <music/tchunk.h>
#include <tlayoutparams.h>
#include <exam/texam.h>
#include <widgets/tpitchview.h>
#include <tsound.h>
#if defined (Q_OS_ANDROID)
  #include <ttouchmessage.h>
  #include <ttouchmenu.h>
#else
  #include "gui/tstatuslabel.h"
#endif
#include <exam/tlevel.h>
#include <plugins/tpluginsloader.h>
#include <QtWidgets/QtWidgets>


extern Tglobals *gl;
extern bool resetConfig;

static TbgPixmap *m_bgPix;


/** Converts given note to key signature accidentals or/and preferred accidental */
void noteToKey(Tnote& n, TkeySignature k) {
	bool convToFlat = gl->GpreferFlats;
	if (k.value()) {
		Tnote keyNote = k.inKey(n);
		if (keyNote.isValid()) {
			n = keyNote;
			convToFlat = false;
		}
	}
	if (convToFlat) {
		if (n.note != 3 && n.note != 7) // eliminate Fb from E and Cb from B
				n = n.showWithFlat();
	}
}


// HACK: Workaround to force qtandroiddeploy include QtMultimedia, QtPrintSupport and QtAndroidExtras libs
#if defined (Q_OS_ANDROID)
  #include <QMediaPlayer>
  #include <QtPrintSupport/QPrinter>
  #include <QtAndroidExtras/QtAndroid>
  #include <QtNetwork/qnetworkaccessmanager.h>

    void fakeLibsDemander(QObject* parent) {
      QMediaPlayer dummyPlayer(parent);
      QtAndroid::androidActivity();
      QPrinter printer;
      QNetworkAccessManager nam;
    }

  static TtouchMessage *m_touchMessage;
  static bool m_isAppActive = true; /**< Whether Nootka is active (displayed) or suspended (and/or screen is off)  */
  static bool m_wasPitchEnabled = false; /**< Stores state of pitch detection when Nootka becomes inactive screen is locked (off)  */
#endif



MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_examResults(0),
  m_progress(0),
  m_isPlayerFree(true),
  m_updaterPlugin(0),
  m_updaterStoppedSound(false)
{
  setObjectName(QStringLiteral("MainNootkaWindow"));
#if !defined (Q_OS_ANDROID)
  setWindowIcon(QIcon(gl->path + "picts/nootka.png"));
  int w = qMax(qRound(qApp->primaryScreen()->size().width() * 0.75), 720); // initial width & height adjusted to screen resolution
  int h = qMax(qRound(qApp->primaryScreen()->size().height() * 0.75), 480);
  setMinimumSize(720, 480);
  gl->config->beginGroup("General");
  setGeometry(gl->config->value("geometry",
                  QRect((qApp->primaryScreen()->size().width() - w) / 2, (qApp->primaryScreen()->size().height() - h) / 2, w, h)).toRect());
  gl->config->endGroup();
#endif
  m_bgPix = new TbgPixmap;
  if (gl->isFirstRun) {
      TpluginsLoader *wizardLoader = new TpluginsLoader();
      if (wizardLoader->load(TpluginsLoader::e_wizard)) {
        wizardLoader->init(QString(), this);
      }
      delete wizardLoader;
      gl->isFirstRun = false;
  } else { // show support window once but not with first run wizard
      gl->config->beginGroup("General");
      QString newVersion = gl->config->value("version", QString()).toString();
      int supportDaysPass = gl->config->value("supportDate", QDate(2012, 12, 31)).toDate().daysTo(QDate::currentDate());
      gl->config->endGroup();
      if (supportDaysPass > 5) // display support dialog every five days
        QTimer::singleShot(2000, [=] { showSupportDialog(); });
      else { // check for updates
        m_updaterPlugin = new TpluginsLoader(this);
        if (gl->config->value("Updates/enableUpdates", true).toBool() && m_updaterPlugin->load(TpluginsLoader::e_updater)) {
            connect(m_updaterPlugin->node(), &TpluginObject::value, this, &MainWindow::updaterMessagesSlot);
            m_updaterPlugin->init(QStringLiteral("false"), this); // string argument stops displaying update dialog when no news were send
        } else
            delete m_updaterPlugin;
      }
  }
  gl->config->setValue("General/version", gl->version);

  Tnote::defaultStyle = gl->S->nameStyleInNoteName;
  m_sound = new Tsound(this);
//-------------------------------------------------------------------
// Creating GUI elements
  m_bar = new TtoolBar(gl->version, this);
  m_bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // otherwise it follows global Qt style settings

  m_score = new TmainScore(this);
  m_noteName = m_score->noteName();
  m_pitchView = new TpitchView(m_sound->sniffer, this);
  m_sound->setPitchView(m_pitchView);
  m_pitchView->setVisible(gl->L->soundViewEnabled);
#if !defined (Q_OS_ANDROID)
// Hints - label with clues
  m_statusLabel = new TstatusLabel(this);
  m_statusLabel->setVisible(gl->L->hintsBarEnabled);
#endif

  m_guitar = new TfingerBoard(this);
  m_guitar->setVisible(gl->L->guitarEnabled);

//-------------------------------------------------------------------		
// Setting a layout
  m_melButt = new TmelMan(m_score);
  m_bar->addScoreActions(m_score->scoreActions());
  m_bar->addMelodyButton(m_melButt);
#if defined (Q_OS_ANDROID)
  m_innerWidget = new TmainView(gl->L, m_bar, 0, m_pitchView, m_score, m_guitar, m_noteName, this);
  m_touchMessage = new TtouchMessage();
  m_innerWidget->scene()->addItem(m_touchMessage);
#else
  m_innerWidget = new TmainView(gl->L, m_bar, m_statusLabel, m_pitchView, m_score, m_guitar, m_noteName, this);
#endif
  setCentralWidget(m_innerWidget);
  Tmenu::setMainWidget(m_innerWidget);
//-------------------------------------------------------------------
  m_levelCreatorExist = false;

#if defined (Q_OS_ANDROID)
  connect(m_bar->aboutSimpleAct, &QAction::triggered, this, &MainWindow::aboutSlot);
  connect(m_innerWidget, &TmainView::settingsRequired, this, &MainWindow::createSettingsDialog);
  m_innerWidget->flyActions()->append(m_bar->playMelody()); // default quick actions for mobile
  m_innerWidget->flyActions()->append(m_bar->recordMelody());
  m_innerWidget->flyActions()->append(m_pitchView->pauseAction());
  m_bar->setFlyingActions(m_innerWidget->flyActions());
#else
  connect(m_bar->analyseAct, SIGNAL(triggered()), this, SLOT(analyseSlot()));
  connect(m_bar->aboutAct, &QAction::triggered, this, &MainWindow::aboutSlot);
  connect(m_score, &TmainScore::statusTip, m_statusLabel, &TstatusLabel::messageSlot);
  connect(m_innerWidget, &TmainView::statusTip, m_statusLabel, &TstatusLabel::messageSlot);
#endif
  setSingleNoteMode(gl->S->isSingleNoteMode);

  connect(m_sound, &Tsound::noteStarted, this, &MainWindow::soundWasStarted);
  connect(m_sound, &Tsound::noteFinished, this, &MainWindow::soundWasFinished);
  connect(m_bar->levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));
  connect(m_bar->settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));
  connect(m_bar->startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));
  connect(m_score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
  connect(m_score, &TmainScore::clefChanged, this, &MainWindow::adjustAmbitus);
  connect(m_guitar, &TfingerBoard::guitarClicked, this, &MainWindow::guitarWasClicked);
  connect(m_innerWidget, &TmainView::sizeChanged, this, &MainWindow::updateSize);

#if defined (Q_OS_ANDROID)
  connect(qApp, &QGuiApplication::applicationStateChanged, [=](Qt::ApplicationState state){
      if (state == Qt::ApplicationActive) { // Nootka backs to live
          if (!m_isAppActive) { // restore pitch detection state
            if (m_wasPitchEnabled)
              m_sound->go();
            m_isAppActive = true;
          }
      } else { // Nootka goes to background
          if (m_isAppActive) { // when it was active, store pitch detection state
            m_wasPitchEnabled = !(m_sound->isSniferStopped() || m_sound->isSnifferPaused());
            m_isAppActive = false;
            m_sound->wait();
          }
      }
  });
#endif

  qApp->installEventFilter(this);
}


MainWindow::~MainWindow()
{
#if !defined (Q_OS_ANDROID)
	gl->config->beginGroup("General");
		gl->config->setValue("geometry", geometry());
	gl->config->endGroup();
#endif
  Tmenu::deleteMenuHandler();
  Tmenu::setMainWidget(0);
  delete m_bgPix;
}

//##########################################################################################
//#######################     METHODS       ################################################
//##########################################################################################

void MainWindow::examMessageSlot(int demand) {
// First, handle executor demands during processing
  if (demand == Torders::e_examSingle) {
      setSingleNoteMode(true);
      return;
  } else if (demand == Torders::e_examMultiple) {
      setSingleNoteMode(false);
      return;
  } else if (demand == Torders::e_examResize) {
      updateSize(m_innerWidget->size());
      return;
  } else if (demand == Torders::e_examDisconnect) {
      disconnect(m_score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
      disconnect(m_guitar, &TfingerBoard::guitarClicked, this, &MainWindow::guitarWasClicked);
      disconnect(m_sound, &Tsound::noteStarted, this, &MainWindow::soundWasStarted);
      disconnect(m_sound, &Tsound::noteFinished, this, &MainWindow::soundWasFinished);
      disconnect(m_bar->levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));
      disconnect(m_bar->startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));
      return;
  } else if (demand == Torders::e_examConnect) {
      connect(m_score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
      connect(m_guitar, &TfingerBoard::guitarClicked, this, &MainWindow::guitarWasClicked);
      connect(m_sound, &Tsound::noteStarted, this, &MainWindow::soundWasStarted);
      connect(m_sound, &Tsound::noteFinished, this, &MainWindow::soundWasFinished);
      connect(m_bar->levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));
      connect(m_bar->startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));
      return;
  } else if (demand == Torders::e_examAllowClose) {
      m_executorAllowsClose = true;
      return;
  }

// Other messages are about finishing/closing
	m_bar->actionsAfterExam();
#if !defined (Q_OS_ANDROID)
  m_statusLabel->setBackground(-1);
#endif
	m_innerWidget->takeExamViews();
	m_progress = 0;
	m_examResults = 0;
	if (m_score->insertMode() != TmultiScore::e_single)
      m_bar->setMelodyButtonVisible(true);
	updateSize(m_innerWidget->size());
  m_examPlugin->deleteLater();
  if (demand == Torders::e_examAskCreator)
      QTimer::singleShot(500, [=]{ openLevelCreator(); }); // open creator with delay to give executor time to finish his routines
  else
      m_sound->startListen();
}

//##########################################################################################
//#######################     PUBLIC SLOTS       ###########################################
//##########################################################################################
void MainWindow::openFile(QString runArg) {
  if (m_examPlugin || m_levelCreatorExist)
    return;
  if (QFile::exists(runArg)) {
    QFile file(runArg);
    quint32 hdr = 0;
    if (file.open(QIODevice::ReadOnly)) {
      QDataStream in(&file);
      in >> hdr; // check what file type
    }
    runArg = QDir(file.fileName()).absolutePath();
    file.close();
    if (Texam::couldBeExam(hdr)) {
      if (Texam::isExamVersion(hdr)) {
        runArg.prepend(QLatin1String("file:")); // exam file path starts with 'file:' text
        startExamPlugin(runArg);
      }
    } else {
      if (Tlevel::couldBeLevel(hdr)) {
        if (Tlevel::isLevelVersion(hdr))
          openLevelCreator(runArg);
      }
    }
  }
}


void MainWindow::createSettingsDialog() {
	if (m_score->isScorePlayed())
		m_melButt->playMelodySlot(); // stop playing
  if (m_examPlugin) {
      m_examPlugin->node()->emitBackValue(Torders::e_examSettings);
      return; // exam plugin will invoke settings plugin itself
  } else {
			if (m_score->insertMode() == TmultiScore::e_record)
				m_melButt->recordMelodySlot(); // switch to multi mode
			m_sound->prepareToConf();
	}
  QString args;
  if (sender() == m_innerWidget) // NOTE: so far TmainView emits signal related to settings only from quick audio dialog (Android)
    args = QStringLiteral("audio");
  auto loader = new TpluginsLoader();
  if (loader->load(TpluginsLoader::e_settings))
      loader->init(args, this);
	Torders::Esettings lastValue = (Torders::Esettings)loader->lastValue();
	loader->deleteLater();
  if (lastValue == Torders::e_settingsAccept) {
			m_isPlayerFree = false;
			m_sound->acceptSettings();
			setSingleNoteMode(gl->S->isSingleNoteMode);
			m_score->acceptSettings();
      updateSize(m_innerWidget->size()); // layout settings of tool bar is approved here
			if (gl->L->guitarEnabled && gl->instrument != e_noInstrument)
					m_guitar->acceptSettings(); //refresh guitar
			m_innerWidget->setBarAutoHide(gl->L->toolBarAutoHide);
#if !defined (Q_OS_ANDROID)
			m_statusLabel->setVisible(gl->L->hintsBarEnabled);
#endif
			m_pitchView->setVisible(gl->L->soundViewEnabled);
			m_guitar->setVisible(gl->L->guitarEnabled);
      if (gl->S->isSingleNoteMode) // refresh note name
        m_noteName->setNoteName(m_noteName->getNoteName());
			m_isPlayerFree = true;
	} else if (lastValue == Torders::e_settingsReset) {
      resetConfig = true;
      close();
  } else { // settings not accepted
      if (!m_examPlugin) // skip this when settings were called during exam
        m_sound->restoreAfterConf();
	}
}


void MainWindow::openLevelCreator(QString levelFile) {
  if (m_score->isScorePlayed())
    m_melButt->playMelodySlot(); // stop playing
  m_sound->stopListen(); // stops pitch detection
  m_sound->stopPlaying();
  m_levelCreatorExist = true;
  TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_level)) {
    loader.init(levelFile, this);
  }
  QString levelText = loader.lastWord();
  gl->config->sync(); // it is necessary to save recent levels list
  bool startExercise = false;
  if (levelText.contains(QLatin1String("exam:")))
    levelText.remove(QLatin1String("exam:"));
  else {
    levelText.remove(QLatin1String("exercise:"));
    startExercise = true;
  }
  m_levelCreatorExist = false;
  bool ok;
  int levelNr = levelText.toInt(&ok);
  if (ok) {
    QString args = QString("level:%1").arg(levelNr); // plugin run argument is string  'level:' with level number
    if (startExercise)
        args.append(QLatin1String(":exercise")); // and ';exercise' string if exercise has to be started
    startExamPlugin(args);
  }
  else
    m_sound->startListen(); // restore pitch detection
}


void MainWindow::startExamSlot() {
  startExamPlugin(QString());
}


void MainWindow::aboutSlot() {
  if (m_examPlugin) // ignore About dialog during exams/exercises
    return;

	if (m_score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
	m_sound->stopListen();
	m_sound->stopPlaying();
  TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_about))
    loader.init(QString(), this);
	m_sound->startListen();
}


void MainWindow::analyseSlot() {
#if !defined (Q_OS_ANDROID)
	if (m_score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
	m_sound->stopListen();
	m_sound->stopPlaying();
	TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_analyzer)) {
    loader.init(QString(), this);
  }
	m_sound->startListen();
#endif
}

void MainWindow::noteWasClicked(int index, Tnote note) {
	Q_UNUSED(index)
	if (m_isPlayerFree)
			m_sound->play(note);
		if (m_guitar->isVisible())
	m_guitar->setFinger(note);
}


void MainWindow::guitarWasClicked(const Tnote& note) {
	Tnote n = note;
	noteToKey(n, m_score->keySignature());
	m_sound->play(n);
	m_score->setNote(n);
}


void MainWindow::soundWasStarted(const Tnote& note) {
//   if (gl->instrument != e_noInstrument) { // quick reaction for guitar
    Tnote n = note;
    noteToKey(n, m_score->keySignature());
    m_score->setNote(n);
    m_startedSoundId = qMax<int>(m_score->currentIndex(), 0);
    if (m_guitar->isVisible())
      m_guitar->setFinger(note);
//   }
}


void MainWindow::soundWasFinished(Tchunk* chunk) {
//   if (gl->instrument == e_noInstrument) { // whole played note and average pitch for other instruments
    Tnote n = chunk->p();
    noteToKey(n, m_score->keySignature());
    m_score->setNote(m_startedSoundId, n);
    if (m_guitar->isVisible())
      m_guitar->setFinger(chunk->p());
//   }
}


void MainWindow::setSingleNoteMode(bool isSingle) {
	if (isSingle && m_score->insertMode() != TmultiScore::e_single) {
      if (!m_examPlugin)
          m_bar->setMelodyButtonVisible(false);
//           m_melButt->melodyAction()->setVisible(false);
      m_innerWidget->addNoteName();
      m_score->setInsertMode(TmultiScore::e_single);
	} else if	(!isSingle && m_score->insertMode() == TmultiScore::e_single) {
      if (!m_examPlugin)
          m_bar->setMelodyButtonVisible(true);
//           m_melButt->melodyAction()->setVisible(true);
      m_innerWidget->takeNoteName();
      m_noteName->setNoteName(Tnote(1, 0)); // unset buttons
      m_score->setInsertMode(TmultiScore::e_multi);
	}
}

//##########################################################################################
//#######################     PROTECTED SLOTS       ########################################
//##########################################################################################
void MainWindow::showSupportDialog() {
  m_sound->stopListen();
  m_sound->stopPlaying();
  TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_about))
    loader.init(QStringLiteral("support"), this);
  gl->config->setValue("General/supportDate", QDate::currentDate());
  m_sound->startListen();
}


void MainWindow::updaterMessagesSlot(int m) {
  if (m == Torders::e_updaterOffline || m == Torders::e_updaterNoNeed
      || m == Torders::e_updaterFinished || m >= Torders::e_updaterError) {
      m_updaterPlugin->deleteLater();
      if (m_updaterStoppedSound)
        m_sound->startListen();
  } else if (m == Torders::e_updaterSuccess && !m_sound->isSnifferPaused()) {
      m_sound->stopListen();
      m_updaterStoppedSound = true;
  }
  // It sends Torders::e_updaterSuccess as well but it means that updater window is displayed,
  // when user will close it - Torders::e_updaterFinished is send
}


void MainWindow::adjustAmbitus() {
	if (!m_sound->sniffer)
		return;
	Tnote hiNote, loNote;
	const int noteOffset = 2; // major 2nd up and down 
	if (m_score->clef().type() != gl->S->clef) {
		if (m_score->highestNote().chromatic() < gl->hiNote().chromatic())
			hiNote = Tnote(gl->hiNote().chromatic() + noteOffset);
		else
			hiNote = Tnote(m_score->highestNote().chromatic() + noteOffset);
		if (m_score->lowestNote().chromatic() > gl->loNote().chromatic())
			loNote = Tnote(gl->loNote().chromatic() - noteOffset);
		else
			loNote = Tnote(m_score->lowestNote().chromatic() - noteOffset);
		m_sound->sniffer->setAmbitus(loNote, hiNote);
	} else
		m_sound->setDefaultAmbitus();
}

//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void MainWindow::startExamPlugin(const QString& pluginArgs) {
  if (m_examPlugin) {
    qDebug() << "Exam plugin is already loaded/started!";
    return;
  }

  if (m_score->insertMode() != TmultiScore::e_single) {
    if (m_score->isScorePlayed())
      m_melButt->playMelodySlot(); // stop playing when played
    if (m_melButt->recordAction()->isChecked()) // exit record mode
          m_melButt->recordAction()->trigger();
  }
  m_sound->stopPlaying();
  m_examResults = new TexamView();
  m_examResults->setStyleBg(Tcolor::bgTag(gl->EanswerColor), Tcolor::bgTag(gl->EquestionColor), Tcolor::bgTag(gl->EnotBadColor));
  m_progress = new TprogressWidget();
  m_innerWidget->addExamViews(m_examResults, m_progress);
  m_progress->resize(m_examResults->font().pointSize());

  m_examPlugin = new TpluginsLoader(this);
  if (m_examPlugin->load(TpluginsLoader::e_exam)) {
    connect(m_examPlugin->node(), &TpluginObject::value, this, &MainWindow::examMessageSlot);
    m_examPlugin->init(pluginArgs, this);
  }
}



//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

void MainWindow::updateSize(QSize newS) {
  setUpdatesEnabled(false);
  m_statFontSize = newS.height() / 40 - 2;
#if defined (Q_OS_ANDROID) // keep Mobile font not too big
  m_statFontSize = qMin(m_statFontSize, fontMetrics().height());
#endif
	if (m_statFontSize < 1)
		return;

	if (gl->L->soundViewEnabled) {
		if (gl->L->hintsBarEnabled) {
			m_pitchView->setDirection(QBoxLayout::TopToBottom);
			m_pitchView->setMaximumHeight(QWIDGETSIZE_MAX);
      m_pitchView->setMinimumHeight(0);
			m_pitchView->setFixedWidth(newS.width() * 0.4);
		} else {
			m_pitchView->setDirection(QBoxLayout::LeftToRight);
      m_pitchView->setMaximumWidth(QWIDGETSIZE_MAX);
      m_pitchView->setMinimumWidth(0);
			m_pitchView->setFixedHeight(newS.height() * 0.04);
		}
	}
#if !defined (Q_OS_ANDROID)
	int barIconSize = qMin(newS.width(), newS.height()) / 20;
  m_bar->setBarIconStyle(gl->L->iconTextOnToolBar, barIconSize);
#endif
	int baseH = qMin(newS.height(), newS.width());
	if (m_score->insertMode() == TmultiScore::e_single)
		m_noteName->setMaximumWidth(newS.width() / 2);
	else
    m_noteName->setMaximumWidth(QWIDGETSIZE_MAX);
#if defined (Q_OS_ANDROID)
  m_noteName->resize(qMin(baseH / 20, fontMetrics().height()));
#else
  m_noteName->resize(baseH / 40);
	m_statusLabel->setFixedHeight(newS.height() / 10);
	QFont f = m_statusLabel->font();
	f.setPointSize(qRound(m_statFontSize * 0.95));
	QFontMetrics fMetr(f);
  qreal fact = (m_statFontSize * 1.4) / static_cast<qreal>(fMetr.boundingRect("A").height());
  f.setPointSize(qRound(f.pointSize() * fact));
	m_statusLabel->setFont(f);
#endif
  if (m_progress) {
    m_examResults->setFontSize(m_statFontSize);
    m_progress->resize(m_examResults->font().pointSize());
  }
#if defined (Q_OS_ANDROID)
  int newGuitH = newS.height() / 4;
#else
	int newGuitH = (newS.height() - m_bar->height()) / 4;
#endif

  m_guitar->updateSize(QSize(m_innerWidget->width(), newGuitH));
  BG_PIX->update(gl->L->guitarEnabled ? newS : QSize(), static_cast<int>(gl->instrument), newGuitH,
                 m_guitar->posX12fret(), m_guitar->fbRect().right(), gl->GisRightHanded); // empty size when guitar is hidden by user
  m_guitar->setFixedHeight(newGuitH);
	setUpdatesEnabled(true);
}


void MainWindow::closeEvent(QCloseEvent *event) {
  m_sound->stopPlaying();
  m_sound->stopListen();
#if !defined (Q_OS_ANDROID)
  disconnect(m_score, &TmainScore::statusTip, m_statusLabel, &TstatusLabel::messageSlot);
  disconnect(m_innerWidget, &TmainView::statusTip, m_statusLabel, &TstatusLabel::messageSlot);
#endif
#if defined (Q_OS_ANDROID)
  if (TtouchMenu::instance()) {
    TtouchMenu::instance()->close();
    event->ignore();
  } else
#endif
  if (m_examPlugin) {
    m_executorAllowsClose = false;
    m_examPlugin->node()->emitBackValue(Torders::e_examClose);
    // After sending above demand TexamExecutor displays question and emits demand to plugin
    // examMessageSlot() will set m_executorAllowsClose to TRUE or FALSE apparently
    if (m_executorAllowsClose)
      qApp->quit(); // FIXME: force quick of the app during exercise, otherwise it hangs (it is dirty solution)
//      event->accept();
    else
        event->ignore();
  }
}


bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
#if defined (Q_OS_MAC)
  if (event->type() == QEvent::FileOpen) {
    QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(event);
    openFile(fileEvent->file());
    return true;
  }
#endif
  if (obj == m_innerWidget->viewport()) {
    if (event->type() == QEvent::TouchBegin || event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchUpdate) {
        return m_innerWidget->handleTouchEvent(event);
    }
  }
  return QObject::eventFilter(obj, event);
}

