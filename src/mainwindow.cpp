/***************************************************************************
 *   Copyright (C) 2011-2016 by Tomasz Bojczuk                             *
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

#include "mainwindow.h"
#include "score/tmainscore.h"
#include "guitar/tfingerboard.h"
#include "notename/tnotename.h"
#include "gui/tmainview.h"
#include "gui/ttoolbar.h"
#include "gui/tmenu.h"
#include "gui/tmelman.h"
#include "exam/tprogresswidget.h"
#include "exam/texamview.h"
#include "exam/texamexecutor.h"
#include <tglobals.h>
#include <widgets/troundedlabel.h>
#include <tscoreparams.h>
#include <music/tchunk.h>
#include <tlayoutparams.h>
#include <exam/texam.h>
#include <widgets/tpitchview.h>
#include <tsound.h>
#if defined (Q_OS_ANDROID)
  #include <touch/ttouchmessage.h>
#endif
#include <level/tlevelselector.h>
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


// HACK: Workaround to force qtandroiddeploy include QtMultimedia and QtAndroidExtras libs
#if defined (Q_OS_ANDROID)
  #include <QMediaPlayer>
  #include <QtAndroidExtras/QtAndroid>

    void fakeMultimediaDemander(QObject* parent) {
      QMediaPlayer dummyPlayer(parent);
      QtAndroid::androidActivity();
    }

  TtouchMessage *m_touchMessage;
#endif


MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  examResults(0),
  progress(0),
#if !defined (Q_OS_ANDROID)
  m_statusText(QString()),
  m_curBG(-1), m_prevBg(-1),
  m_lockStat(false),
#endif
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
//       QString newVersion = gl->config->value("version", QString()).toString();
      int supportDaysPass = gl->config->value("supportDate", QDate(2012, 12, 31)).toDate().daysTo(QDate::currentDate());
      gl->config->endGroup();
      if (/*newVersion != gl->version*/ supportDaysPass > 5) // display support dialog every five days
        QTimer::singleShot(2000, [=] { showSupportDialog(); });
#if !defined (Q_OS_ANDROID)
      else { // check for updates
        gl->config->beginGroup("Updates");
				m_updaterPlugin = new TpluginsLoader();
        if (gl->config->value("enableUpdates", true).toBool() && m_updaterPlugin->load(TpluginsLoader::e_updater)) {
            connect(m_updaterPlugin->node(), &TpluginObject::message, this, &MainWindow::updaterMessagesSlot);
            gl->config->endGroup(); // close settings note because updater need to open it again
            m_updaterPlugin->init("false", this); // string argument stops displaying update dialog when no news were send
        } else
						delete m_updaterPlugin;
      }
#endif
  }
  if (!gl->config->group().isEmpty()) // close settings group when was open
		gl->config->endGroup();

  Tnote::defaultStyle = gl->S->nameStyleInNoteName;
  sound = new Tsound(this);
#if !defined (Q_OS_ANDROID)
  m_messageTimer = new QTimer(this);
  connect(m_messageTimer, SIGNAL(timeout()), this, SLOT(restoreMessage()));
#endif

//-------------------------------------------------------------------
// Creating GUI elements
  bar = new TtoolBar(gl->version, this);
  bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // otherwise it follows global Qt style settings

  score = new TmainScore(this);
  noteName = score->noteName();
  pitchView = new TpitchView(sound->sniffer, this);
  sound->setPitchView(pitchView);
  pitchView->setVisible(gl->L->soundViewEnabled);
#if !defined (Q_OS_ANDROID)
// Hints - label with clues
  m_statLab = new TroundedLabel(this);
  m_statLab->setWordWrap(true);
  m_statLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_statLab->setContentsMargins(1, 1, 1, 1); // overwrite 5 px margins of TroundedLabel
  m_statLab->setVisible(gl->L->hintsBarEnabled);
#endif

  guitar = new TfingerBoard(this);
  guitar->setVisible(gl->L->guitarEnabled);

//-------------------------------------------------------------------		
// Setting a layout
  m_melButt = new TmelMan(score);
  bar->addScoreActions(score->scoreActions());
  bar->addMelodyButton(m_melButt);
#if defined (Q_OS_ANDROID)
  innerWidget = new TmainView(gl->L, bar, 0, pitchView, score, guitar, noteName, this);
  m_touchMessage = new TtouchMessage();
  innerWidget->scene()->addItem(m_touchMessage);
#else
  innerWidget = new TmainView(gl->L, bar, m_statLab, pitchView, score, guitar, noteName, this);
#endif
  setCentralWidget(innerWidget);
  Tmenu::setMainWidget(innerWidget);
//-------------------------------------------------------------------
  m_levelCreatorExist = false;

#if defined (Q_OS_ANDROID)
  connect(bar->aboutSimpleAct, &QAction::triggered, this, &MainWindow::aboutSlot);
  innerWidget->flyActions()->append(bar->playMelody()); // default quick actions for mobile
  innerWidget->flyActions()->append(bar->recordMelody());
  innerWidget->flyActions()->append(pitchView->pauseAction());
  bar->setFlyingActions(innerWidget->flyActions());
#else
  connect(bar->analyseAct, SIGNAL(triggered()), this, SLOT(analyseSlot()));
  connect(bar->aboutAct, &QAction::triggered, this, &MainWindow::aboutSlot);
  connect(score, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
  connect(innerWidget, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
#endif
  setSingleNoteMode(gl->S->isSingleNoteMode);

  connect(sound, &Tsound::noteStarted, this, &MainWindow::soundWasStarted);
  connect(sound, &Tsound::noteFinished, this, &MainWindow::soundWasFinished);
  connect(bar->levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));
  connect(bar->settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));
  connect(bar->startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));
  connect(score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
  connect(score, &TmainScore::clefChanged, this, &MainWindow::adjustAmbitus);
  connect(guitar, &TfingerBoard::guitarClicked, this, &MainWindow::guitarWasClicked);
  connect(innerWidget, &TmainView::sizeChanged, this, &MainWindow::updateSize);
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

void MainWindow::setStatusMessage(const QString& msg) {
#if !defined (Q_OS_ANDROID)
	if (!gl->L->hintsBarEnabled)
		return;
	if (!m_lockStat)
			m_statLab->setText(QLatin1String("<center>") + msg + QLatin1String("</center>"));
	else
			m_prevMsg = msg;
	m_statusText = msg;
#endif
}


void MainWindow::setStatusMessage(const QString& msg, int time) {
#if defined (Q_OS_ANDROID)
  m_touchMessage->setMessage(msg, time);
#else
	if (!gl->L->hintsBarEnabled)
		return;
	m_prevMsg = m_statusText;
	m_statLab->setText(QLatin1String("<center>") + msg + QLatin1String("</center>"));
	m_lockStat = true;
	m_messageTimer->start(time);
#endif
}


void MainWindow::setMessageBg(QColor bg) {
#if !defined (Q_OS_ANDROID)
	if (bg == -1) {
			m_statLab->setDefaultBackground();
	} else
			m_statLab->setBackroundColor(bg);
	m_curBG = bg;
#endif
}


void MainWindow::clearAfterExam(int examState) {
	bar->actionsAfterExam();
#if !defined (Q_OS_ANDROID)
	m_curBG = -1;
	m_prevBg = -1;
	setMessageBg(-1);
#endif
	if ((TexamExecutor::Estate)examState == TexamExecutor::e_openCreator) 
			openLevelCreator();
	else
			sound->go();
	innerWidget->takeExamViews();
	progress = 0;
	examResults = 0;
	if (score->insertMode() != TmultiScore::e_single)
		bar->setMelodyButtonVisible(true);
	updateSize(innerWidget->size());
  delete executor;
  m_deleteExecutor = true;
}

//##########################################################################################
//#######################     PUBLIC SLOTS       ###########################################
//##########################################################################################
void MainWindow::openFile(QString runArg) {
  if (executor || m_levelCreatorExist)
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
        prepareToExam();
        m_deleteExecutor = false;
        executor = new TexamExecutor(this, runArg);
        if (m_deleteExecutor)
          delete executor;
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
	if (score->isScorePlayed())
		m_melButt->playMelodySlot(); // stop playing
	QString args;
	if (executor) {
		if (executor->isExercise())
			args = QStringLiteral("exercise");
		else
			args = QStringLiteral("exam");
		executor->prepareToSettings();
	} else {
			if (score->insertMode() == TmultiScore::e_record)
				m_melButt->recordMelodySlot(); // switch to multi mode
			sound->prepareToConf();
	}
  TpluginsLoader *loader = new TpluginsLoader();
  if (loader->load(TpluginsLoader::e_settings)) {
    loader->init(args, this);
  }
	QString lastWord = loader->lastWord();
	delete loader;
		if (lastWord.contains(QLatin1String("Accepted"))) {
			if (executor) {
				executor->settingsAccepted();
				return;
			}
			m_isPlayerFree = false;
			sound->acceptSettings();
			setSingleNoteMode(gl->S->isSingleNoteMode);
			score->acceptSettings();
			updateSize(innerWidget->size());
			if (gl->L->guitarEnabled && gl->instrument != e_noInstrument)
					guitar->acceptSettings(); //refresh guitar
			bar->setBarIconStyle(gl->L->iconTextOnToolBar, bar->iconSize().width());
			innerWidget->setBarAutoHide(gl->L->toolBarAutoHide);
#if !defined (Q_OS_ANDROID)
			m_statLab->setVisible(gl->L->hintsBarEnabled);
#endif
			pitchView->setVisible(gl->L->soundViewEnabled);
			guitar->setVisible(gl->L->guitarEnabled);
      if (gl->S->isSingleNoteMode) // refresh note name
        noteName->setNoteName(noteName->getNoteName());
			m_isPlayerFree = true;
	} else if (lastWord.contains(QLatin1String("Reset"))) {
      resetConfig = true;
      close();
  } else { // settings not accepted
      if (!executor) // skip this when settings were called during exam
        sound->restoreAfterConf();
	}
}


void MainWindow::openLevelCreator(QString levelFile) {
  if (score->isScorePlayed())
    m_melButt->playMelodySlot(); // stop playing
  sound->wait(); // stops pitch detection
  sound->stopPlaying();
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
    TlevelSelector ls;
    ls.selectLevel(levelNr);
    m_level = ls.getSelectedLevel();
    prepareToExam();
    executor = new TexamExecutor(this, startExercise ? QLatin1String("exercise") : QString(), &m_level); // start exam
  }
  else
    sound->go(); // restore pitch detection
}


void MainWindow::startExamSlot() {
	prepareToExam();
  m_deleteExecutor = false;
	executor = new TexamExecutor(this);
  if (m_deleteExecutor)
    delete executor;
}


void MainWindow::aboutSlot() {
	if (score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
	sound->wait();
	sound->stopPlaying();
  TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_about))
    loader.init(QString(), this);
	sound->go();
}


void MainWindow::analyseSlot() {
#if !defined (Q_OS_ANDROID)
	if (score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
	sound->wait();
	sound->stopPlaying();
	TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_analyzer)) {
    loader.init(QString(), this);
  }
	sound->go();
#endif
}

void MainWindow::noteWasClicked(int index, Tnote note) {
	Q_UNUSED(index)
	if (m_isPlayerFree)
			sound->play(note);
		if (guitar->isVisible())
	guitar->setFinger(note);
}


void MainWindow::guitarWasClicked(const Tnote& note) {
	Tnote n = note;
	noteToKey(n, score->keySignature());
	sound->play(n);
	score->setNote(n);
}


void MainWindow::soundWasStarted(const Tnote& note) {
//   if (gl->instrument != e_noInstrument) { // quick reaction for guitar
    Tnote n = note;
    noteToKey(n, score->keySignature());
    score->setNote(n);
    m_startedSoundId = qMax<int>(score->currentIndex(), 0);
    if (guitar->isVisible())
      guitar->setFinger(note);
//   }
}


void MainWindow::soundWasFinished(Tchunk* chunk) {
//   if (gl->instrument == e_noInstrument) { // whole played note and average pitch for other instruments
    Tnote n = chunk->p();
    noteToKey(n, score->keySignature());
    score->setNote(m_startedSoundId, n);
    if (guitar->isVisible())
      guitar->setFinger(chunk->p());
//   }
}


void MainWindow::setSingleNoteMode(bool isSingle) {
	if (isSingle && score->insertMode() != TmultiScore::e_single) {
      if (!executor)
          bar->setMelodyButtonVisible(false);
//           m_melButt->melodyAction()->setVisible(false);
      innerWidget->addNoteName();
      score->setInsertMode(TmultiScore::e_single);
	} else if	(!isSingle && score->insertMode() == TmultiScore::e_single) {
      if (!executor)
          bar->setMelodyButtonVisible(true);
//           m_melButt->melodyAction()->setVisible(true);
      innerWidget->takeNoteName();
      noteName->setNoteName(Tnote(1, 0)); // unset buttons
      score->setInsertMode(TmultiScore::e_multi);
	}
}

//##########################################################################################
//#######################     PROTECTED SLOTS       ########################################
//##########################################################################################
void MainWindow::restoreMessage() {
#if !defined (Q_OS_ANDROID)
	m_messageTimer->stop();
	m_lockStat = false;
	setStatusMessage(m_prevMsg);
	setMessageBg(m_prevBg);
	m_prevMsg.clear();
#endif
}


void MainWindow::messageSlot(const QString& msg) {
#if !defined (Q_OS_ANDROID)
	if (msg.isEmpty()) {
//       m_statLab->setDefaultBackground();
//       m_statLab->setStyleSheet("color: palette(text)");
			setMessageBg(m_prevBg);
			m_statLab->setText(QLatin1String("<center>") + m_statusText + QLatin1String("</center>"));
			m_prevMsg = m_statusText;
	} else {
//       m_statLab->setBackroundColor(palette().highlightedText().color());
//       m_statLab->setStyleSheet("color: palette(highlight)");
			m_prevBg = m_curBG;
			setMessageBg(-1);
			m_statLab->setText(QLatin1String("<center>") + msg + QLatin1String("</center>"));
	}
#endif
}


void MainWindow::showSupportDialog() {
  sound->wait();
  sound->stopPlaying();
  TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_about))
    loader.init(QStringLiteral("support"), this);
  gl->config->beginGroup("General");
    gl->config->setValue("version", gl->version);
    gl->config->setValue("supportDate", QDate::currentDate());
  gl->config->endGroup();
  sound->go();
}


void MainWindow::updaterMessagesSlot(const QString& m) {
#if !defined (Q_OS_ANDROID)
  if (m.contains(QLatin1String("offline")) || m.contains(QLatin1String("No need"))
    || m.contains(QLatin1String("finished")) || m.contains(QLatin1String("error occurred"))) {
      m_updaterPlugin->deleteLater();
      if (m_updaterStoppedSound)
        sound->go();
  } else if (m.contains(QLatin1String("success")) && !sound->isSnifferPaused()) {
      sound->wait();
      m_updaterStoppedSound = true;
  }
  // It sends 'success' as well but it means that updater window is displayed, when user will close it - 'finished' is send
#endif
}


void MainWindow::adjustAmbitus() {
	if (!sound->sniffer)
		return;
	Tnote hiNote, loNote;
	const int noteOffset = 2; // major 2nd up and down 
	if (score->clef().type() != gl->S->clef) {
		if (score->highestNote().chromatic() < gl->hiNote().chromatic())
			hiNote = Tnote(gl->hiNote().chromatic() + noteOffset);
		else
			hiNote = Tnote(score->highestNote().chromatic() + noteOffset);
		if (score->lowestNote().chromatic() > gl->loNote().chromatic())
			loNote = Tnote(gl->loNote().chromatic() - noteOffset);
		else
			loNote = Tnote(score->lowestNote().chromatic() - noteOffset);
		sound->sniffer->setAmbitus(loNote, hiNote);
	} else
		sound->setDefaultAmbitus();
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void MainWindow::prepareToExam() {
	if (score->insertMode() != TmultiScore::e_single) {
		if (score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing when played
		bar->setMelodyButtonVisible(false);
	}
	sound->stopPlaying();
	examResults = new TexamView();
	examResults->setStyleBg(Tcolor::bgTag(gl->EanswerColor), Tcolor::bgTag(gl->EquestionColor), Tcolor::bgTag(gl->EnotBadColor));
	progress = new TprogressWidget();
	innerWidget->addExamViews(examResults, progress);
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
			pitchView->setDirection(QBoxLayout::TopToBottom);
			pitchView->setMaximumHeight(QWIDGETSIZE_MAX);
      pitchView->setMinimumHeight(0);
			pitchView->setFixedWidth(newS.width() * 0.4);
		} else {
			pitchView->setDirection(QBoxLayout::LeftToRight);
      pitchView->setMaximumWidth(QWIDGETSIZE_MAX);
      pitchView->setMinimumWidth(0);
			pitchView->setFixedHeight(newS.height() * 0.04);
		}
	}
#if !defined (Q_OS_ANDROID)
	int barIconSize = qMin(newS.width(), newS.height()) / 20;
  bar->setBarIconStyle(gl->L->iconTextOnToolBar, barIconSize);
#endif
	int baseH = qMin(newS.height(), newS.width());
	if (score->insertMode() == TmultiScore::e_single)
		noteName->setMaximumWidth(newS.width() / 2);
	else
    noteName->setMaximumWidth(QWIDGETSIZE_MAX);
#if defined (Q_OS_ANDROID)
  noteName->resize(qMin(baseH / 20, fontMetrics().height()));
#else
  noteName->resize(baseH / 40);
	m_statLab->setFixedHeight(newS.height() / 10);
	QFont f = m_statLab->font();
	f.setPointSize(m_statFontSize * 0.95);
	QFontMetrics fMetr(f);
	qreal fact = (qreal)(m_statFontSize * 1.4) / (qreal)fMetr.boundingRect("A").height();
	f.setPointSize(f.pointSize() * fact);
	m_statLab->setFont(f);
#endif
  if (progress) {
    progress->resize(m_statFontSize);
    examResults->setFontSize(m_statFontSize);
  }
	int newGuitH = (newS.height() - bar->height()) * 0.25;
	if (gl->instrument == e_electricGuitar || gl->instrument == e_bassGuitar) {
		QPixmap rosePix(gl->path + "picts/pickup.png");
		qreal pickCoef = ((newGuitH * 2.9) / 614.0) * 0.6;
		m_rosettePixmap = rosePix.scaled(rosePix.width() * pickCoef, rosePix.height() * pickCoef, Qt::KeepAspectRatio);
		pickCoef = (newGuitH * 3.3) / 535;
		int xPic = (newS.width()) * 0.8571428571 + 20 * pickCoef;;
    int yPic = (newS.height() - newGuitH) - 30 * pickCoef;
		if (!gl->GisRightHanded)
				xPic = newS.width() - xPic - m_rosettePixmap.width(); // reversed
		guitar->setPickUpRect(QRect(QPoint(xPic, yPic), m_rosettePixmap.size()));
	}
#if defined (Q_OS_ANDROID)
	guitar->setFixedHeight(newS.height() * 0.25);
#else
  guitar->setFixedHeight(newGuitH);
#endif
	if (gl->instrument != e_noInstrument && gl->L->guitarEnabled) {
		QPixmap bgPix;
		qreal guitH;
		qreal ratio;
		if (gl->instrument == e_classicalGuitar) {
			guitar->setPickUpRect(QRect());
			bgPix = QPixmap(gl->path + "picts/body.png"); // size 800x535
			guitH = qRound(((double)guitar->height() / 350.0) * 856.0);
			int guitW = centralWidget()->width() / 2;
			m_bgPixmap = bgPix.scaled(guitW, guitH, Qt::IgnoreAspectRatio);
		} else {
			if (gl->instrument == e_bassGuitar)
					bgPix = QPixmap(gl->path + "picts/body-bass.png"); // size 
			else
					bgPix = QPixmap(gl->path + "picts/body-electro.png");
			guitH = guitar->height() * 2.9;
			ratio = guitH / bgPix.height();
			m_bgPixmap = bgPix.scaled(qRound(bgPix.width() * ratio), guitH, Qt::KeepAspectRatio);
		}
	}
	setUpdatesEnabled(true);
// // // 	QTimer::singleShot(2, this, SLOT(update()));
}


void MainWindow::closeEvent(QCloseEvent *event) {
  sound->stopPlaying();
  sound->wait();
#if !defined (Q_OS_ANDROID)
  disconnect(score, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
  disconnect(innerWidget, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
#endif
	if (executor) {
      if (executor->closeNootka()) {
          event->accept();
      } else {
          event->ignore();
      }
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
      painter.drawPixmap(guitar->posX12fret() + 7, guitar->geometry().bottom() - m_bgPixmap.height() + 1, m_bgPixmap);
    } else {
      qreal ratio = (guitar->height() * 3.3) / 535;
      painter.drawPixmap(guitar->fbRect().right() - 235 * ratio, height() - m_bgPixmap.height() , m_bgPixmap);
      if (!gl->GisRightHanded)
        painter.resetTransform();
      painter.drawPixmap(guitar->pickRect()->x(), guitar->pickRect()->y(), m_rosettePixmap);
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

