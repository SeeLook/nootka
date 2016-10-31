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
#include "exam/tprogresswidget.h"
#include "exam/texamview.h"
#include "exam/texamexecutor.h"
#include "exam/tequalrand.h"
#include "gui/tmelman.h"
#include "gui/tmainview.h"
#include "gui/ttoolbar.h"
#include "gui/tmenu.h"
#include "gui/tbgpixmap.h"
#include <tglobals.h>
#include <widgets/troundedlabel.h>
#include <tscoreparams.h>
#include <music/tchunk.h>
#include <tlayoutparams.h>
#include <level/tlevelselector.h>
#include <exam/texam.h>
#include <widgets/tpitchview.h>
#include <tsound.h>
#include <taboutnootka.h>
#include <tsupportnootka.h>
#include <plugins/tpluginsloader.h>
#include <QtWidgets>


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


static QTimer *m_messageTimer;

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		examResults(0),
		progress(0),
		m_statusText(QString()),
		m_curBG(-1), m_prevBg(-1),
		m_lockStat(false),
    m_isPlayerFree(true),
    m_updaterPlugin(0)
{
  setWindowIcon(QIcon(gl->path + "picts/nootka.png"));
  m_bgPix = new TbgPixmap;
  
  //    setAttribute(Qt::WA_AcceptTouchEvents);
  
  setMinimumSize(720, 480);
  gl->config->beginGroup("General");
  setGeometry(gl->config->value("geometry", QRect(50, 50, 750, 480)).toRect());
  
  if (gl->isFirstRun) {
      TpluginsLoader *loader = new TpluginsLoader();
      if (loader->load(TpluginsLoader::e_wizard)) {
        loader->init(QString(), this);
      }
      delete loader;
      gl->isFirstRun = false;
  } else { // show support window once but not with first run wizard
      QString newVersion = gl->config->value("version", QString()).toString();
      if (newVersion != gl->version) {
          QTimer::singleShot(2000, this, SLOT(showSupportDialog()));
      } else { // check for updates
          gl->config->endGroup();
          gl->config->beginGroup("Updates");
          m_updaterPlugin = new TpluginsLoader();
          if (gl->config->value("enableUpdates", true).toBool() && m_updaterPlugin->load(TpluginsLoader::e_updater)) {
            connect(m_updaterPlugin->node(), &TpluginObject::message, this, &MainWindow::updaterMessagesSlot);
            gl->config->endGroup(); // close settings note because updater need to open it again
            m_updaterPlugin->init("false", this); // string argument stops displaying update dialog when no news were send
          } else
              delete m_updaterPlugin;
      }
  }
  if (!gl->config->group().isEmpty()) // close settings group when was open
		gl->config->endGroup();
	
  Tnote::defaultStyle = gl->S->nameStyleInNoteName;
  
  sound = new Tsound(this);
  
  m_messageTimer = new QTimer(this);
  connect(m_messageTimer, SIGNAL(timeout()), this, SLOT(restoreMessage()));
//-------------------------------------------------------------------
// Creating GUI elements
  bar = new TtoolBar(gl->version, this);
  bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon); // otherwise it follows global Qt style settings

  score = new TmainScore(this);
  noteName = score->noteName();
  pitchView = new TpitchView(sound->sniffer, this);
  sound->setPitchView(pitchView);
  pitchView->setVisible(gl->L->soundViewEnabled);
// Hints - label with clues
  m_statLab = new TroundedLabel(this);
  m_statLab->setWordWrap(true);
  m_statLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  m_statLab->setContentsMargins(1, 1, 1, 1); // overwrite 5 px margins of TroundedLabel
  m_statLab->setVisible(gl->L->hintsBarEnabled);
#if defined (Q_OS_ANDROID)
  m_statLab->hide();
  nootBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
  showMaximized();
  nootBar->hide();
  nootBar->setAutoFillBackground(true);
#endif

  guitar = new TfingerBoard(this);
  guitar->setVisible(gl->L->guitarEnabled);
  
//-------------------------------------------------------------------		
// Setting layout
// #if defined (Q_OS_ANDROID)
// 	nootBar->setParent(0);
// #endif
// 	QVBoxLayout *mainLay = new QVBoxLayout;
// 	mainLay->setContentsMargins(0, 2, 0, 2);
// #if !defined (Q_OS_ANDROID)
// 		mainLay->addWidget(nootBar);
// #endif

  m_melButt = new TmelMan(score);
  bar->addScoreActions(score->scoreActions());
  bar->addMelodyButton(m_melButt);
  innerWidget = new TmainView(gl->L, bar, m_statLab, pitchView, score, guitar, noteName, this);
  setCentralWidget(innerWidget);
	Tmenu::setMainWidget(innerWidget);
//-------------------------------------------------------------------
  m_levelCreatorExist = false;

  
  connect(bar->settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));
  connect(bar->levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));
  connect(bar->startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));
  connect(bar->analyseAct, SIGNAL(triggered()), this, SLOT(analyseSlot()));
  connect(bar->aboutAct, &QAction::triggered, this, &MainWindow::aboutSlot);
  setSingleNoteMode(gl->S->isSingleNoteMode);

  connect(score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
  connect(score, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
  connect(score, &TmainScore::clefChanged, this, &MainWindow::adjustAmbitus);
  connect(guitar, &TfingerBoard::guitarClicked, this, &MainWindow::guitarWasClicked);
  connect(sound, &Tsound::noteStarted, this, &MainWindow::soundWasStarted);
  connect(sound, &Tsound::noteFinished, this, &MainWindow::soundWasFinished);
  connect(innerWidget, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
  connect(innerWidget, &TmainView::sizeChanged, this, &MainWindow::updateSize);
}


MainWindow::~MainWindow()
{
	gl->config->beginGroup("General");
		gl->config->setValue("geometry", geometry());
	gl->config->endGroup();
	Tmenu::deleteMenuHandler();
  Tmenu::setMainWidget(0);
  delete m_bgPix;
}

//##########################################################################################
//#######################     METHODS       ################################################
//##########################################################################################


void MainWindow::setStatusMessage(const QString& msg) {
	if (!gl->L->hintsBarEnabled)
		return;
	if (!m_lockStat)
			m_statLab->setText("<center>" + msg + "</center>");
	else
			m_prevMsg = msg;
	m_statusText = msg;
}


void MainWindow::setStatusMessage(const QString& msg, int time) {
	if (!gl->L->hintsBarEnabled)
		return;
	m_prevMsg = m_statusText;
	m_statLab->setText("<center>" + msg + "</center>");
	m_lockStat = true;
	m_messageTimer->start(time);
}


void MainWindow::setMessageBg(QColor bg) {
	if (bg == -1) {
			m_statLab->setDefaultBackground();
	} else
			m_statLab->setBackroundColor(bg);
	m_curBG = bg;
}


void MainWindow::clearAfterExam(int examState) {
  bar->actionsAfterExam();
  m_curBG = -1;
  m_prevBg = -1;
  setMessageBg(-1);
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
			args = "exercise";
		else
			args = "exam";
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
		if (lastWord.contains("Accepted")) {
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
			m_statLab->setVisible(gl->L->hintsBarEnabled);
			pitchView->setVisible(gl->L->soundViewEnabled);
			guitar->setVisible(gl->L->guitarEnabled);
      if (gl->S->isSingleNoteMode) // refresh note name
        noteName->setNoteName(noteName->getNoteName());
			m_isPlayerFree = true;
	} else if (lastWord.contains("Reset")) {
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
// 		setAttribute(Qt::WA_TransparentForMouseEvents, true);
		TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_level)) {
    loader.init(levelFile, this);
  }
  QString levelText = loader.lastWord();
  gl->config->sync(); // it is necessary to save recent levels list
  bool startExercise = false;
  if (levelText.contains("exam:"))
    levelText.remove("exam:");
  else {
    levelText.remove("exercise:");
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
    executor = new TexamExecutor(this, startExercise ? "exercise" : QString(), &m_level); // start exam
  }
// 		setAttribute(Qt::WA_TransparentForMouseEvents, false);
		
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
	TaboutNootka *ab = new TaboutNootka(this);
	ab->exec();
	delete ab;
	sound->go();
}


void MainWindow::analyseSlot() {
	if (score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
	sound->wait();
	sound->stopPlaying();
	TpluginsLoader loader;
  if (loader.load(TpluginsLoader::e_analyzer)) {
    loader.init(QString(), this);
  }
	sound->go();
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
			m_melButt->melodyAction()->setVisible(false);
		innerWidget->addNoteName();
		score->setInsertMode(TmultiScore::e_single);
	} else if	(!isSingle && score->insertMode() == TmultiScore::e_single) {
		if (!executor)
			m_melButt->melodyAction()->setVisible(true);
		innerWidget->takeNoteName();
		noteName->setNoteName(Tnote(1, 0)); // unset buttons
		score->setInsertMode(TmultiScore::e_multi);
	}
}

//##########################################################################################
//#######################     PROTECTED SLOTS       ########################################
//##########################################################################################

void MainWindow::restoreMessage() {
	m_messageTimer->stop();
	m_lockStat = false;
	setStatusMessage(m_prevMsg);
	setMessageBg(m_prevBg);
	m_prevMsg.clear();
}


void MainWindow::messageSlot(const QString& msg) {
	if (msg.isEmpty()) {
//       m_statLab->setDefaultBackground();
//       m_statLab->setStyleSheet("color: palette(text)");
			setMessageBg(m_prevBg);
			m_statLab->setText("<center>" + m_statusText + "</center>");
			m_prevMsg = m_statusText;
	} else {
//       m_statLab->setBackroundColor(palette().highlightedText().color());
//       m_statLab->setStyleSheet("color: palette(highlight)");
			m_prevBg = m_curBG;
			setMessageBg(-1);
			m_statLab->setText("<center>" + msg + "</center>");
	}
}


void MainWindow::showSupportDialog() {
  sound->wait();
  sound->stopPlaying();
  TsupportStandalone *supp = new TsupportStandalone(this);
  supp->exec();
  gl->config->beginGroup("General");
    gl->config->setValue("version", gl->version);
    gl->config->setValue("supportDate", QDate::currentDate());
  gl->config->endGroup();
  delete supp;
  sound->go();
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

bool updaterStoppedSound = false;
void MainWindow::updaterMessagesSlot(const QString& m) {
	if (m.contains(QLatin1String("offline")) || m.contains(QLatin1String("No need")) ||
      m.contains(QLatin1String("finished")) || m.contains(QLatin1String("error occurred"))) {
		m_updaterPlugin->deleteLater();
    if (updaterStoppedSound)
      sound->go();
  } else if (m.contains(QLatin1String("success")) && !sound->isSnifferPaused()) {
    sound->wait();
    updaterStoppedSound = true;
  }
	// It sends 'success' as well but it means that updater window is displayed, when user will close it - 'finished' is send
}


//#################################################################################################
//###################              PRIVATE             ############################################
//#################################################################################################

void MainWindow::prepareToExam() {
	if (score->insertMode() != TmultiScore::e_single) {
		if (score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing when played
    if (m_melButt->recordAction()->isChecked()) // exit record mode
      m_melButt->recordAction()->trigger();
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
	if (m_statFontSize < 0)
		return;
// 	qDebug() << "updateSize()";
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
// 	bar->setFixedWidth(newS.width());
#if defined (Q_OS_ANDROID)
	int barIconSize = qMin(newS.width(), newS.height()) / 10;
#else
	int barIconSize = qMin(newS.width(), newS.height()) / 20;
#endif
	bar->setBarIconStyle(gl->L->iconTextOnToolBar, barIconSize);
	int baseH = qMin(newS.height(), newS.width());
	if (score->insertMode() == TmultiScore::e_single)
		noteName->setMaximumWidth(newS.width() / 2);
	else
		noteName->setMaximumWidth(QWIDGETSIZE_MAX);
	noteName->resize(baseH / 40);
	m_statLab->setFixedHeight(newS.height() / 10);
	QFont f = m_statLab->font();
	f.setPointSize(m_statFontSize * 0.95);
	QFontMetrics fMetr(f);
	qreal fact = (qreal)(m_statFontSize * 1.4) / (qreal)fMetr.boundingRect("A").height();
	f.setPointSize(f.pointSize() * fact);
	m_statLab->setFont(f);
	int newGuitH = (newS.height() - bar->height()) * 0.25;
	if (progress) {
		progress->resize(m_statFontSize);
		examResults->setFontSize(m_statFontSize);
	}

  guitar->updateSize(QSize(innerWidget->width(), newGuitH));
  BG_PIX->update(size(), (int)gl->instrument, newGuitH, guitar->posX12fret(), guitar->fbRect().right(), gl->GisRightHanded);
  guitar->setFixedHeight(newGuitH);
	setUpdatesEnabled(true);
}


void MainWindow::closeEvent(QCloseEvent *event) {
  sound->stopPlaying();
  sound->wait();
  disconnect(score, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
  disconnect(innerWidget, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
	if (executor) {
		if (executor->closeNootka()) {
        qApp->quit(); // FIXME: force quick of the app during exercise, otherwise it hangs (it is dirty solution)
//         event->accept();
    } else
				event->ignore();
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

