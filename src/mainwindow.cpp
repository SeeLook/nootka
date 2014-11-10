/***************************************************************************
 *   Copyright (C) 2011-2014 by Tomasz Bojczuk                             *
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
#include <tglobals.h>
#include <widgets/troundedlabel.h>
#include <tprocesshandler.h>
#include <tscoreparams.h>
#include <music/tchunk.h>
#include <widgets/tpitchview.h>
#include <tsound.h>
#include "score/tmainscore.h"
#include "score/tcornerproxy.h"
#include "guitar/tfingerboard.h"
#include "notename/tnotename.h"
// // #include "tsettingsdialog.h"
#include "exam/tstartexamdlg.h" // just temporary
// // #include "taboutnootka.h"
// // #include "tfirstrunwizzard.h"
// // #include "tsupportnootka.h"
// // #include "tnootkalabel.h"
// #include "texamsettings.h"
// #include <tupdateprocess.h>
// #include <tcolor.h>
#include "exam/tprogresswidget.h"
#include "exam/texamview.h"
#include "exam/texamexecutor.h"
#include "exam/tequalrand.h"
#include "gui/tmelman.h"
#include "gui/tmainview.h"
#include "gui/ttoolbar.h"
// #include "taudioparams.h"
// #include "tanalysdialog.h"
// #include "tquestionpoint.h"
#include <QtWidgets>


extern Tglobals *gl;
extern bool resetConfig;

QTimer *m_messageTimer;

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
		examResults(0),
		progress(0),
		m_statusText(""),
		m_curBG(-1), m_prevBg(-1),
		m_lockStat(false),
    ex(0),
    m_isPlayerFree(true)
//     m_pitchContainer(0),
//     m_rightLay(0),
//     m_extraFontOffset(0)
{
// #if defined(Q_OS_MAC)
//     TquestionPoint::setColors(QColor(gl->EanswerColor.name()), QColor(gl->EquestionColor.name()), 
//                               QColor(gl->EnotBadColor.name()), shadowC, palette().window().color());
// #else
//     TquestionPoint::setColors(QColor(gl->EanswerColor.name()), QColor(gl->EquestionColor.name()),
//                               QColor(gl->EnotBadColor.name()), palette().shadow().color(), palette().base().color());
// #endif
// #if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
//     setWindowIcon(QIcon(gl->path + "picts/nootka.svg"));
// #else
    setWindowIcon(QIcon(gl->path + "picts/nootka.png"));
// #endif
    
    setMinimumSize(720, 480);
		gl->config->beginGroup("General");
    setGeometry(gl->config->value("geometry", QRect(50, 50, 750, 480)).toRect());
		
// 		setAttribute(Qt::WA_AcceptTouchEvents);
    
//     if (gl->isFirstRun) {
//         TfirstRunWizzard *firstWizz = new TfirstRunWizzard();
//         firstWizz->exec();
//         delete firstWizz;
//         gl->isFirstRun = false;
//     } else { // show support window once but not with first run wizard
// 				QString newVersion = gl->config->value("version", "").toString();
//         if (newVersion != gl->version) {
//           QTimer::singleShot(200, this, SLOT(showSupportDialog()));
// 				} else { // check for updates
//           gl->config->endGroup();
//           gl->config->beginGroup("Updates");
//           if (gl->config->value("enableUpdates", true).toBool() && TupdateProcess::isPossible()) {
//               TupdateProcess *process = new TupdateProcess(true, this);
//               process->start();
//           }
//         }
//     }
		gl->config->endGroup();
		Tnote::defaultStyle = gl->S->nameStyleInNoteName;
		
    sound = new Tsound(this);
		
		m_messageTimer = new QTimer(this);
		connect(m_messageTimer, SIGNAL(timeout()), this, SLOT(restoreMessage()));
//-------------------------------------------------------------------
// Creating GUI elements
//     innerWidget = new QWidget(this);
    bar = new TtoolBar(this);
		if (gl->hintsEnabled)
				bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		else
				bar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    score = new TmainScore(this);
		noteName = score->noteName();
    pitchView = new TpitchView(sound->sniffer/*, this*/);
    sound->setPitchView(pitchView);
 // Hints - label with clues
    m_statLab = new TroundedLabel(/*innerWidget*/);
    m_statLab->setWordWrap(true);
    m_statLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		m_statLab->setContentsMargins(1, 1, 1, 1); // overwrite 5 px margins of TroundedLabel
#if defined (Q_OS_ANDROID)
    m_statLab->hide();
		nootBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
		showMaximized();
		nootBar->hide();
		nootBar->setAutoFillBackground(true);
#endif
// 		nootBar->hide();
// 		nootBar->setAutoFillBackground(true);

//		QColor C(palette().text().color());
// #if defined (Q_OS_WIN)
// 		C.setAlpha(20);
// #else
// 		C.setAlpha(15);
// #endif
// 		C = Tcolor::merge(C, palette().window().color());
// 		nootLabel = new TnootkaLabel(gl->path + "picts/logo.png", innerWidget, C);
		
    guitar = new TfingerBoard();
		
//-------------------------------------------------------------------		
// Setting layout
// #if defined (Q_OS_ANDROID)
// 	nootBar->setParent(0);
// 	TcornerProxy *rhythmCorner = new TcornerProxy(score->scoreScene(), nootBar, Qt::TopRightCorner);
// 	rhythmCorner->setSpotColor(Qt::darkGreen);
// #endif
// 	QVBoxLayout *mainLay = new QVBoxLayout;
// 	mainLay->setContentsMargins(0, 2, 0, 2);
// #if !defined (Q_OS_ANDROID)
// 		mainLay->addWidget(nootBar);
// #endif

		m_melButt = new TmelMan(score);
		bar->addAction(m_melButt->melodyAction());
		innerWidget = new TmainView(bar, m_statLab, pitchView, score, guitar, this);
		setCentralWidget(innerWidget);
//-------------------------------------------------------------------
    m_levelCreatorExist = false;

		
    connect(bar->settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));
    connect(bar->levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));
    connect(bar->startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));
    connect(bar->analyseAct, SIGNAL(triggered()), this, SLOT(analyseSlot()));
//     connect(nootBar->aboutAct, SIGNAL(triggered()), this, SLOT(aboutSlot()));
		setSingleNoteMode(gl->S->isSingleNoteMode);
// 		if (gl->S->isSingleNoteMode)
// 				score->setInsertMode(TmultiScore::e_single);

    connect(score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
		connect(score, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));
// 		connect(score, SIGNAL(clefChanged(Tclef)), this, SLOT(adjustAmbitus()));
    connect(guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(guitarWasClicked(Tnote)));
		connect(sound, &Tsound::noteStarted, this, &MainWindow::soundWasStarted);
		connect(sound, &Tsound::noteFinished, this, &MainWindow::soundWasFinished);
		connect(innerWidget, SIGNAL(statusTip(QString)), this, SLOT(messageSlot(QString)));

//     if (gl->A->OUTenabled && !sound->isPlayable())
//         QMessageBox::warning(this, "", tr("Problems with sound output"));
// 		bar->hide();
// 		m_statLab->hide();
// 		pitchView->hide();
// 		guitar->hide();

}


MainWindow::~MainWindow()
{
    gl->config->beginGroup("General");
      gl->config->setValue("geometry", geometry());
    gl->config->endGroup();
    delete gl;
		qDebug() << "Settings object successfully deleted";
}

//##########################################################################################
//#######################     METHODS       ################################################
//##########################################################################################


void MainWindow::setStatusMessage(const QString& msg) {
	if (!m_lockStat)
			m_statLab->setText("<center>" + msg + "</center>");
	else
			m_prevMsg = msg;
	m_statusText = msg;
}


void MainWindow::setStatusMessage(const QString& msg, int time) {
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
	delete ex;
	ex = 0;
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
	if (score->insertMode() != TmultiScore::e_single) {
		m_melButt->melodyAction()->setVisible(true);
		m_melButt->button()->menu()->setDisabled(false);
	}
// 		nootLabel->show();
	updateSize(innerWidget->size());
}

/*
QPoint MainWindow::relatedPoint() {
    return QPoint(noteName->geometry().x(), m_statLab->geometry().bottom() + 5);
}
*/
//##########################################################################################
//#######################     PUBLIC SLOTS       ###########################################
//##########################################################################################

void MainWindow::openFile(QString runArg) {
    if (/*ex || */m_levelCreatorExist)
        return;
    if (QFile::exists(runArg)) {
        QFile file(runArg);
        quint32 hdr = 0;
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            in.setVersion(QDataStream::Qt_4_7);
            in >> hdr; // check what file type
        }
        runArg = QDir(file.fileName()).absolutePath();
        file.close();
// 				if (Texam::couldBeExam(hdr)) {
// 					if (Texam::isExamVersion(hdr))
// 							ex = new TexamExecutor(this, runArg);
// 				} else {
					if (Tlevel::couldBeLevel(hdr)) {
						if (Tlevel::isLevelVersion(hdr))
								openLevelCreator(runArg);
					}
// 				}
    }
}


void MainWindow::createSettingsDialog() {
// 	TsettingsDialog *settings = new TsettingsDialog(this);
	if (score->isScorePlayed())
		m_melButt->playMelodySlot(); // stop playing
// 	sound->prepareToConf();
	gl->dumpToTemp();
	QStringList args;
	if (ex) {
		if (ex->isExercise())
			args << "exercise";
		else
			args << "exam";
		ex->prepareToSettings();
	} else
			sound->prepareToConf();
	TprocessHandler settProcess("nootka-settings", args, this);

// 	if (settings->exec() == QDialog::Accepted) {
// 	qDebug() << "lastWord" << settProcess.lastWord() << gl->grabFromTemp();
		if (settProcess.lastWord().contains("Accepted") && gl->grabFromTemp()) {
// 			delete settings;
			if (ex) {
				ex->settingsAccepted();
				return;
			}
			m_isPlayerFree = false;
			sound->acceptSettings();
			setSingleNoteMode(gl->S->isSingleNoteMode);
			score->acceptSettings();
// 			noteName->setAmbitus(gl->loString(),
// 															Tnote(gl->hiString().chromatic() + gl->GfretsNumber));
			updateSize(innerWidget->size());
// 			if (score->getNote(0).note != 0) {
// 				TnotesList nList;
// 				nList = score->getNote(0).getTheSameNotes(gl->S->doubleAccidentalsEnabled);
// 				if (nList[0].chromatic() >= gl->loString().chromatic() && 
// 					nList[0].chromatic() <= gl->hiString().chromatic() + gl->GfretsNumber ) {
// 						if (gl->S->showEnharmNotes) { // refresh note name and score
// // 								noteName->setNoteName(nList);
// 								if (nList.size() > 1)
// 										score->setNote(1, nList[1]);
// 								else {
// 										score->clearNote(1);
// 										score->clearNote(2);
// 								}
// 								if (nList.size() > 2)
// 										score->setNote(2, nList[2]);
// 								else
// 										score->clearNote(2);
// 						} else
// 								noteName->setNoteName(nList[0]);
// 				}
// 			}
			if (gl->instrument != e_noInstrument) {
					guitar->acceptSettings(); //refresh guitar
			}
			if (gl->hintsEnabled) {
				bar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			} else {
				m_prevBg = m_curBG;
				setStatusMessage(m_prevMsg);
				bar->setToolButtonStyle(Qt::ToolButtonIconOnly);				
			}
			m_isPlayerFree = true;
	} else { // settings not accepted
// 			delete settings;
			sound->restoreAfterConf();
	}
	if (resetConfig)
			close();
}


void MainWindow::openLevelCreator(QString levelFile) {
		if (score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
    sound->wait(); // stops pitch detection
    sound->stopPlaying();
    m_levelCreatorExist = true;
		gl->dumpToTemp();
		QStringList args;
		if (levelFile != "")
				args << levelFile;
// 		setAttribute(Qt::WA_TransparentForMouseEvents, true);
		TprocessHandler levelProcess("nootka-level", args, this);
		qDebug() << levelProcess.lastWord();
// 		setAttribute(Qt::WA_TransparentForMouseEvents, false);
		
//     TlevelCreatorDlg *levelCreator= new TlevelCreatorDlg(this);
//     bool shallExamStart = false;
//     if (levelFile != "")
//         levelCreator->loadLevelFile(levelFile);
//     if (levelCreator->exec() == QDialog::Accepted) {
//         m_level = levelCreator->selectedLevel();
//         if (m_level.name != "")
//             shallExamStart = true;
//     }
//     delete levelCreator;
    m_levelCreatorExist = false;
//     if (shallExamStart) {
			qDebug() << "Exam will be started: TODO"; // TODO
//         nootLabel->hide();
//         progress->show();
//         examResults->show();
//         ex = new TexamExecutor(this, "", &m_level); // start exam
//     } else
//         sound->go(); // restore pitch detection
}


void MainWindow::startExamSlot() {
	if (score->insertMode() != TmultiScore::e_single) {
		if (score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing when played
		m_melButt->button()->menu()->setDisabled(true);
		m_melButt->melodyAction()->setVisible(false);
	}
	sound->stopPlaying();
	examResults = new TexamView();
	examResults->setStyleBg(Tcolor::bgTag(gl->EanswerColor), Tcolor::bgTag(gl->EquestionColor), Tcolor::bgTag(gl->EnotBadColor));
	progress = new TprogressWidget();
	innerWidget->addExamViews(examResults, progress);
// 		nootLabel->hide();
	ex = new TexamExecutor(this);
}

/*
void MainWindow::aboutSlot() {
		if (score->isScorePlayed())
				playSlot(); // stop playing
    sound->wait();
    sound->stopPlaying();
    TaboutNootka *ab = new TaboutNootka(this);
    ab->exec();
    delete ab;
    sound->go();
}
*/

void MainWindow::analyseSlot() {
	if (score->isScorePlayed())
			m_melButt->playMelodySlot(); // stop playing
	sound->wait();
	sound->stopPlaying();
	QStringList args;
	TprocessHandler analyzeProcess("nootka-analyzer", args, this);
//     TanalysDialog *ad = new TanalysDialog(0, this);
//     ad->exec();
//     delete ad;
	sound->go();
}


void MainWindow::noteWasClicked(int index, Tnote note) {
	Q_UNUSED(index)
	if (m_isPlayerFree)
			sound->play(note);
// 		if (guitar->isVisible())
	guitar->setFinger(note);
}


void MainWindow::guitarWasClicked(Tnote note) {
	sound->play(note);
	score->setNote(note);
}


void MainWindow::soundWasStarted(const Tnote& note) {
  score->setNote(note);
	m_startedSoundId = score->currentIndex();
// 	if (guitar->isVisible())
	guitar->setFinger(note);
}


void MainWindow::soundWasFinished(Tchunk& chunk) {
	score->setNote(m_startedSoundId, chunk.p());
// 	if (guitar->isVisible())
		guitar->setFinger(chunk.p());
}


void MainWindow::setSingleNoteMode(bool isSingle) {
	if (isSingle && score->insertMode() != TmultiScore::e_single) {
		if (!ex)
				m_melButt->melodyAction()->setVisible(false);
		innerWidget->addNoteName(score->noteName());
		score->setInsertMode(TmultiScore::e_single);
	} else if	(!isSingle && score->insertMode() == TmultiScore::e_single) {
		if (!ex)
				m_melButt->melodyAction()->setVisible(true);
		innerWidget->takeNoteName();
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
	m_prevMsg = "";
}


void MainWindow::messageSlot(const QString& msg) {
	if (msg.isEmpty()) {
			setMessageBg(m_prevBg);
			m_statLab->setText("<center>" + m_statusText + "</center>");
			m_prevMsg = m_statusText;
	} else {
			m_prevBg = m_curBG;
			setMessageBg(-1);
			m_statLab->setText("<center>" + msg + "</center>");
	}
}


/*
void MainWindow::showSupportDialog() {
    sound->wait();
    sound->stopPlaying();
    TsupportStandalone *supp = new TsupportStandalone(gl->path, this);
    supp->exec();
    gl->config->beginGroup("General");
      gl->config->setValue("version", gl->version);
    gl->config->endGroup();
    delete supp;
    sound->go();
}


void MainWindow::setWidgetsFont() {
	progress->resize(m_statFontSize + m_extraFontOffset);
	examResults->setFontSize(m_statFontSize + m_extraFontOffset);
	noteName->resize(m_statFontSize + m_extraFontOffset);
}


void MainWindow::fixNoteNameSize() {
	if (!isMaximized()) {
		resize(width(), height() + noteName->smallSpace() + 2);
#if defined (Q_OS_WIN)
    noteName->setGeometry(noteName->x(), noteName->y(), noteName->width(), noteName->height() + noteName->smallSpace());
#endif
	}
}


void MainWindow::fixPitchViewPos() {
//   if (!windowState().testFlag(Qt::WindowMaximized)) {
//     // Lets hope user has no any abnormal desktop size and skip checking ratio for maximized
//     if (innerWidget->height() > innerWidget->width() * 0.8) {
//         resize(innerWidget->width(), innerWidget->width() * 0.75);
//         return;
//     } else if (innerWidget->width() > innerWidget->height() * 1.95) {
//         resize(innerWidget->height() * 1.9, innerWidget->height());
//         return;
//     }
//   }
  // when return occurred it will back here again from resizeEvent
  int maxPossH = innerWidget->height() - m_statLab->height() - guitar->height(); // max possible height of score
  int foreWidth = score->widthToHeight(maxPossH);
	if (gl->instrument != e_noInstrument) {
    if (foreWidth > innerWidget->width() / 2) {
      if (m_scoreLay->count() < 2) { // if it is under noteName
        m_rightLay->removeWidget(pitchView);
        m_scoreLay->insertWidget(1, pitchView);
      }
    } else {
      if (m_scoreLay->count() > 1) // if it is under score and there is enough horizontal space for new width
          if (score->widthToHeight(score->height() + pitchView->height() + m_scoreLay->spacing()) < innerWidget->width() / 2) {
           m_scoreLay->removeWidget(pitchView);
           m_rightLay->addWidget(pitchView);
      }
    }
	}
	if (m_scoreLay->count() > 1) //update possible height when pitchView went under score
			foreWidth = score->widthToHeight(maxPossH - pitchView->height());
	if (foreWidth > innerWidget->width() / 2) {
			qreal hScale = (innerWidget->width() / 2.0) / (qreal)foreWidth;
			score->setMaximumHeight((maxPossH - pitchView->height()) * hScale);
	} else
			score->setMaximumHeight(16777215);
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
*/


//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

void MainWindow::updateSize(QSize newS) {
	setUpdatesEnabled(false);
	m_statFontSize = (newS.height() / 10) / 4 - 2;
	if (m_statFontSize < 0)
		return;
// 	qDebug() << "updateSize()";
	pitchView->setFixedWidth(newS.width() * 0.4);
	pitchView->resize(m_statFontSize);
// 	nootBar->setFixedWidth(newS.width());
#if defined (Q_OS_ANDROID)
	int barIconSize = qMin(newS.width(), newS.height()) / 10;
#else
	int barIconSize = qMin(newS.width(), newS.height()) / 20;
#endif
	bar->setIconSize(QSize(barIconSize, barIconSize));
	m_melButt->button()->setIconSize(bar->iconSize());
	bar->adjustSize();
// 	nootBar->setIconSize(QSize(newS.height() / 22, height() / 22));	
	int baseH = qMin(newS.height(), newS.width());
	if (score->insertMode() == TmultiScore::e_single)
		noteName->setMaximumWidth(newS.width() / 2);
	else
		noteName->setMaximumWidth(QWIDGETSIZE_MAX);
	noteName->resize(baseH / 40);
	m_statLab->setFixedHeight(newS.height() / 10);
	QFont f = m_statLab->font();
	f.setPointSize(m_statFontSize);
	QFontMetrics fMetr(f);
	qreal fact = (qreal)(m_statFontSize * 1.5) / (qreal)fMetr.boundingRect("A").height();
	f.setPointSize(f.pointSize() * fact);
	m_statLab->setFont(f);
	int newGuitH = (newS.height() - bar->height()) * 0.25;
	if (progress) {
		progress->resize(m_statFontSize);
		examResults->setFontSize(m_statFontSize);
	}
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
	guitar->setFixedHeight((newS.height() - bar->height()) * 0.25);
// 	setWidgetsFont();
	
	if (gl->instrument != e_noInstrument) {
// 		pitchView->resize(m_statFontSize);
// 		if (m_pitchContainer) {
// 			m_pitchContainer->layout()->removeWidget(pitchView);
// 			m_pitchContainer->deleteLater();
// 			m_pitchContainer = 0;
// 			guitar->show();
// 			m_rightLay->addWidget(pitchView);
// 		}
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
		// 			if (gl->instrument == e_classicalGuitar) {
// 				QPixmap rosePix(gl->path + "picts/rosette.png"); // size 341x281
// 				m_rosettePixmap = rosePix.scaled(341 * ratio, 281 * ratio, Qt::KeepAspectRatio);
// 	} else { // no guitar - pitch view instead
// 			pitchView->resize(m_statFontSize * 1.7);
// 			if (!m_pitchContainer) {
// 				guitar->hide();
// 				m_pitchContainer = new QWidget(innerWidget);
// 				m_pitchContainer->setObjectName("m_pitchContainer");
// 				m_pitchContainer->setStyleSheet("QWidget#m_pitchContainer {" + Tcolor::bgTag(palette().window().color()) + 
// 					"border-radius: 10px;" + QString("background-image: url(%1);}").arg(gl->path + "picts/scoresettbg.png"));
// 				m_rightLay->removeWidget(pitchView);
// 				QVBoxLayout *pitchLay = new QVBoxLayout;
// 				pitchLay->addStretch(1);
// 				pitchLay->addWidget(pitchView);
// 				pitchLay->addStretch(1);
// 				m_pitchContainer->setLayout(pitchLay);
// 				innerWidget->layout()->addWidget(m_pitchContainer);
// 			}
	}
// 	if (m_pitchContainer)
// 		m_pitchContainer->setFixedHeight((height() - nootBar->height()) * 0.25);
// 	
	setUpdatesEnabled(true);
// 	fixPitchViewPos();
	QTimer::singleShot(2, this, SLOT(update()));
}


void MainWindow::resizeEvent(QResizeEvent * event) {
	Q_UNUSED(event)
// 	innerWidget->resize(contentsRect().size());
	updateSize(innerWidget->size());
// 	QTimer::singleShot(10, score, SLOT(resizeSlot()));
// 	emit sizeChanged(innerWidget->size());
//   QTimer::singleShot(3, this, SLOT(fixPitchViewPos()));
}

/*
void MainWindow::closeEvent(QCloseEvent *event) {
    if (!settingsAct->isEnabled() && ex) {
        if (ex->closeNootka())
            event->accept();
        else
            event->ignore();
    }
}
*/

void MainWindow::paintEvent(QPaintEvent* ) {
		if (gl->instrument != e_noInstrument) {
			QPainter painter(this);
			if (!gl->GisRightHanded) {
					painter.translate(width(), 0);
					painter.scale(-1, 1);
			}
			if (gl->instrument == e_classicalGuitar || gl->instrument == e_noInstrument) {
				painter.drawPixmap(guitar->posX12fret() + 7, guitar->geometry().bottom() - m_bgPixmap.height(), m_bgPixmap);
// 				painter.drawPixmap(width() - qRound(m_rosettePixmap.width() * 0.75), 
// 												height() - ratio * 250 - (height() - guitar->geometry().bottom()), m_rosettePixmap );
			} else {
					qreal ratio = (guitar->height() * 3.3) / 535;
					painter.drawPixmap(guitar->fbRect().right() - 235 * ratio, height() - m_bgPixmap.height() , m_bgPixmap);
          if (!gl->GisRightHanded)
							painter.resetTransform();
          painter.drawPixmap(guitar->pickRect()->x(), guitar->pickRect()->y(), m_rosettePixmap);
      }
		}
}

/*
bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::FileOpen) {
				QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(event);
				openFile(fileEvent->file());
				return true;
		} else { // standard event processing
				return QObject::eventFilter(obj, event);
		}
}
 */
