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

#include "mainwindow.h"
#include <score/tmainscore.h>
#include <guitar/tfingerboard.h>
#include <notename/tnotename.h>
#include <gui/tmainview.h>
#include <gui/ttoolbar.h>
#include <gui/tmenu.h>
#include <gui/tmelman.h>
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

  TtouchMessage *m_touchMessage;
#endif


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_examResults(0),
  m_progress(0),
  m_updaterPlugin(0),
  m_updaterStoppedSound(false),
  m_isPlayerFree(true)
{
  setObjectName(QStringLiteral("MainNootkaWindow"));
#if !defined (Q_OS_ANDROID)
  setWindowIcon(QIcon(gl->path + "picts/nootka.png"));
  setMinimumSize(720, 480);
  gl->config->beginGroup("General");
  setGeometry(gl->config->value("geometry", QRect(50, 50, 750, 480)).toRect());
  gl->config->endGroup();
#endif
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
      gl->config->endGroup();
      if (newVersion != gl->version) // display support dialog when new version
        QTimer::singleShot(2000, [=] { showSupportDialog(); });
      else { // check for updates
        gl->config->beginGroup("Updates");
        m_updaterPlugin = new TpluginsLoader();
        if (gl->config->value("enableUpdates", true).toBool() && m_updaterPlugin->load(TpluginsLoader::e_updater)) {
            connect(m_updaterPlugin->node(), &TpluginObject::value, this, &MainWindow::updaterMessagesSlot);
            gl->config->endGroup(); // close settings note because updater need to open it again
            m_updaterPlugin->init(QStringLiteral("false"), this); // string argument stops displaying update dialog when no news were send
        } else
            delete m_updaterPlugin;
      }
  }
  if (!gl->config->group().isEmpty()) // close settings group when was open
    gl->config->endGroup();

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
      m_sound->go();
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
//         prepareToExam();
        runArg.prepend(QLatin1String("file:")); // exam file path starts with 'file:' text
        startExamPlugin(runArg);
//         m_examPlugin = new TpluginsLoader(this);
//         if (m_examPlugin->load(TpluginsLoader::e_exam)) {
//             connect(m_examPlugin->node(), &TpluginObject::value, this, &MainWindow::examMessageSlot);
//             m_examPlugin->init(runArg, this);
//         }
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
	QString args;
  if (m_examPlugin) {
      m_examPlugin->node()->emitBackValue(Torders::e_examSettings);
      return; // exam plugin will invoke settings plugin itself
  } else {
			if (m_score->insertMode() == TmultiScore::e_record)
				m_melButt->recordMelodySlot(); // switch to multi mode
			m_sound->prepareToConf();
	}
  TpluginsLoader *loader = new TpluginsLoader();
  if (loader->load(TpluginsLoader::e_settings))
      loader->init(args, this);
	Torders::Esettings lastValue = (Torders::Esettings)loader->lastValue();
	loader->deleteLater();
		if (lastValue == Torders::e_settingsAccept) {
			m_isPlayerFree = false;
			m_sound->acceptSettings();
			setSingleNoteMode(gl->S->isSingleNoteMode);
			m_score->acceptSettings();
			updateSize(m_innerWidget->size());
			if (gl->L->guitarEnabled && gl->instrument != e_noInstrument)
					m_guitar->acceptSettings(); //refresh guitar
			m_bar->setBarIconStyle(gl->L->iconTextOnToolBar, m_bar->iconSize().width());
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
  m_sound->wait(); // stops pitch detection
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
//     prepareToExam();
    QString args = QString("level:%1").arg(levelNr); // plugin run argument is string  'level:' with level number
    if (startExercise)
        args.append(QLatin1String(":exercise")); // and ';exercise' string if exercise has to be started
    startExamPlugin(args);
//     m_examPlugin = new TpluginsLoader(this);
//     if (m_examPlugin->load(TpluginsLoader::e_exam)) {
//         connect(m_examPlugin->node(), &TpluginObject::value, this, &MainWindow::examMessageSlot);
//         m_examPlugin->init(args, this);
//     }
  }
  else
    m_sound->go(); // restore pitch detection
}


void MainWindow::startExamSlot() {
// 	prepareToExam();
  startExamPlugin(QString());
//   m_examPlugin = new TpluginsLoader(this);
//   if (m_examPlugin->load(TpluginsLoader::e_exam)) {
//     connect(m_examPlugin->node(), &TpluginObject::value, this, &MainWindow::examMessageSlot);
//     m_examPlugin->init(QString(), this);
//   }
}


void MainWindow::aboutSlot() {
	if (m_score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
	m_sound->wait();
	m_sound->stopPlaying();
  TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_about))
    loader.init(QString(), this);
	m_sound->go();
}


void MainWindow::analyseSlot() {
#if !defined (Q_OS_ANDROID)
	if (m_score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
	m_sound->wait();
	m_sound->stopPlaying();
	TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_analyzer)) {
    loader.init(QString(), this);
  }
	m_sound->go();
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
  m_sound->wait();
  m_sound->stopPlaying();
  TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_about))
    loader.init(QStringLiteral("support"), this);
  gl->config->beginGroup("General");
    gl->config->setValue("version", gl->version);
    gl->config->setValue("supportDate", QDate::currentDate());
  gl->config->endGroup();
  m_sound->go();
}


void MainWindow::updaterMessagesSlot(int m) {
  if (m == Torders::e_updaterOffline || m == Torders::e_updaterNoNeed
      || m == Torders::e_updaterFinished || m >= Torders::e_updaterError) {
      m_updaterPlugin->deleteLater();
      if (m_updaterStoppedSound)
        m_sound->go();
  } else if (m == Torders::e_updaterSuccess && !m_sound->isSnifferPaused()) {
      m_sound->wait();
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
    m_bar->setMelodyButtonVisible(false);
  }
  m_sound->stopPlaying();
  m_examResults = new TexamView();
  m_examResults->setStyleBg(Tcolor::bgTag(gl->EanswerColor), Tcolor::bgTag(gl->EquestionColor), Tcolor::bgTag(gl->EnotBadColor));
  m_progress = new TprogressWidget();
  m_innerWidget->addExamViews(m_examResults, m_progress);

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
	m_statFontSize = (newS.height() / 10) / 4 - 2;
#if defined (Q_OS_ANDROID) // keep Mobile font not too big
  m_statFontSize = qMin(m_statFontSize, fontMetrics().height());
#endif
	if (m_statFontSize < 0)
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
	f.setPointSize(m_statFontSize * 0.95);
	QFontMetrics fMetr(f);
	qreal fact = (qreal)(m_statFontSize * 1.4) / (qreal)fMetr.boundingRect("A").height();
	f.setPointSize(f.pointSize() * fact);
	m_statusLabel->setFont(f);
#endif
  if (m_progress) {
    m_progress->resize(m_statFontSize);
    m_examResults->setFontSize(m_statFontSize);
  }
	int newGuitH = (newS.height() - m_bar->height()) * 0.25;
	if (gl->instrument == e_electricGuitar || gl->instrument == e_bassGuitar) {
		QPixmap rosePix(gl->path + "picts/pickup.png");
		qreal pickCoef = ((newGuitH * 2.9) / 614.0) * 0.6;
		m_rosettePixmap = rosePix.scaled(rosePix.width() * pickCoef, rosePix.height() * pickCoef, Qt::KeepAspectRatio);
		pickCoef = (newGuitH * 3.3) / 535;
		int xPic = (newS.width()) * 0.8571428571 + 20 * pickCoef;;
    int yPic = (newS.height() - newGuitH) - 30 * pickCoef;
		if (!gl->GisRightHanded)
				xPic = newS.width() - xPic - m_rosettePixmap.width(); // reversed
		m_guitar->setPickUpRect(QRect(QPoint(xPic, yPic), m_rosettePixmap.size()));
	}
#if defined (Q_OS_ANDROID)
  m_guitar->setFixedHeight(newS.height() * 0.25);
#else
  m_guitar->setFixedHeight(newGuitH);
#endif
	if (gl->instrument != e_noInstrument && gl->L->guitarEnabled) {
		QPixmap bgPix;
		qreal guitH;
		qreal ratio;
		if (gl->instrument == e_classicalGuitar) {
			m_guitar->setPickUpRect(QRect());
			bgPix = QPixmap(gl->path + "picts/body.png"); // size 800x535
			guitH = qRound(((double)m_guitar->height() / 350.0) * 856.0);
			int guitW = centralWidget()->width() / 2;
			m_bgPixmap = bgPix.scaled(guitW, guitH, Qt::IgnoreAspectRatio);
		} else {
			if (gl->instrument == e_bassGuitar)
					bgPix = QPixmap(gl->path + "picts/body-bass.png"); // size 
			else
					bgPix = QPixmap(gl->path + "picts/body-electro.png");
			guitH = m_guitar->height() * 2.9;
			ratio = guitH / bgPix.height();
			m_bgPixmap = bgPix.scaled(qRound(bgPix.width() * ratio), guitH, Qt::KeepAspectRatio);
		}
	}
	setUpdatesEnabled(true);
}


void MainWindow::closeEvent(QCloseEvent *event) {
  m_sound->stopPlaying();
  m_sound->wait();
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
        event->accept();
    else
        event->ignore();
  }
}


void MainWindow::paintEvent(QPaintEvent* ) {
  if (gl->instrument != e_noInstrument && gl->L->guitarEnabled) {
    QPainter painter(this);
    if (!gl->GisRightHanded) {
      painter.translate(width(), 0);
      painter.scale(-1, 1);
    }
    if (gl->instrument == e_classicalGuitar || gl->instrument == e_noInstrument) {
      painter.drawPixmap(m_guitar->posX12fret() + 7, m_guitar->geometry().bottom() - m_bgPixmap.height() + 1, m_bgPixmap);
    } else {
      qreal ratio = (m_guitar->height() * 3.3) / 535;
      painter.drawPixmap(m_guitar->fbRect().right() - 235 * ratio, height() - m_bgPixmap.height() , m_bgPixmap);
      if (!gl->GisRightHanded)
        painter.resetTransform();
      painter.drawPixmap(m_guitar->pickRect()->x(), m_guitar->pickRect()->y(), m_rosettePixmap);
    }
  }
}


#if defined (Q_OS_MAC)
bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  if (event->type() == QEvent::FileOpen) {
    QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(event);
    openFile(fileEvent->file());
    return true;
  }
  // standard event processing
  return QObject::eventFilter(obj, event);
}
#endif

