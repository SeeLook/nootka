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
#include "tglobals.h"
#include "tsettingsdialog.h"
#include "tlevelcreatordlg.h"
#include "tlevelselector.h"
#include "taboutnootka.h"
#include "tfirstrunwizzard.h"
#include "tsupportnootka.h"
#include "tnootkalabel.h"
#include "widgets/troundedlabel.h"
#include "widgets/tanimedchbox.h"
#include "texamsettings.h"
#include <tupdateprocess.h>
#include <tcolor.h>
#include "tsound.h"
#include "tpushbutton.h"
#include "tmainscore.h"
#include "texam.h"
#include "tprogresswidget.h"
#include "texamview.h"
#include "taudioparams.h"
#include "tpitchview.h"
#include "tanalysdialog.h"
#include "tquestionpoint.h"
#include "tnotename.h"
#include "tfingerboard.h"
#include <QtGui>


extern Tglobals *gl;
extern bool resetConfig;


MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
    ex(0),
    m_isPlayerFree(true),
    m_pitchContainer(0),
    m_rightLay(0)
{
    Ttune::prepareDefinedTunes();
#if defined(Q_OS_MAC)
    TpushButton::setCheckColor(gl->SpointerColor, palette().base().color());
    TquestionPoint::setColors(QColor(gl->EanswerColor.name()), QColor(gl->EquestionColor.name()), 
                              QColor(gl->EnotBadColor.name()), QColor(100, 100, 100, 180), palette().window().color());
#else
    TpushButton::setCheckColor(palette().highlight().color(), palette().highlightedText().color() );
    TquestionPoint::setColors(QColor(gl->EanswerColor.name()), QColor(gl->EquestionColor.name()),
                              QColor(gl->EnotBadColor.name()), palette().shadow().color(), palette().base().color());
#endif
#if defined(Q_OS_LINUX)
    setWindowIcon(QIcon(gl->path+"picts/nootka.svg"));
#else
    setWindowIcon(QIcon(gl->path+"picts/nootka.png"));
#endif
    
    setMinimumSize(720, 480);
		gl->config->beginGroup("General");
    setGeometry(gl->config->value("geometry", QRect(50, 50, 750, 480)).toRect());
    
    if (gl->isFirstRun) {
        TfirstRunWizzard *firstWizz = new TfirstRunWizzard();
        firstWizz->exec();
        delete firstWizz;
        gl->isFirstRun = false;
    } else { // show support window once but not with first run wizard
				QString newVersion = gl->config->value("version", "").toString();
        if (newVersion != gl->version) {
          QTimer::singleShot(200, this, SLOT(showSupportDialog()));
				} else { // check for updates
          gl->config->endGroup();
          gl->config->beginGroup("Updates");
          if (gl->config->value("enableUpdates", true).toBool() && TupdateProcess::isPossible()) {
              TupdateProcess *process = new TupdateProcess(true, this);
              process->start();
          }
        }
    }
		gl->config->endGroup();
		
    TkeySignature::setNameStyle(gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);

    sound = new Tsound(this);
		
		
// 		qDebug() << style()->objectName();
//-------------------------------------------------------------------
// Creating GUI elements
    innerWidget = new QWidget(this);
    nootBar = new QToolBar(tr("main toolbar"), innerWidget);
		nootBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
		if (gl->hintsEnabled)
				nootBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		else
				nootBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    score = new TmainScore(innerWidget);
		score->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pitchView = new TpitchView(sound->sniffer, this);
    sound->setPitchView(pitchView);
 // Hints - label with clues
    QHBoxLayout *statLay = new QHBoxLayout;
    m_statLab = new TroundedLabel(innerWidget);
    m_statLab->setWordWrap(true);
    m_statLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
		m_statLab->setContentsMargins(1, 1, 1, 1); // overwrite 5 px margins of TroundedLabel
		statLay->addWidget(m_statLab, 0, Qt::AlignTop);
 // Expert corner
		autoRepeatChB = new TanimedChBox(innerWidget);
			autoRepeatChB->hide();
			autoRepeatChB->setStatusTip(TexamSettings::autoNextQuestTxt());
			autoRepeatChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
		expertAnswChB = new TanimedChBox(innerWidget);
			expertAnswChB->hide();
			expertAnswChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			expertAnswChB->setStatusTip(TexamSettings::expertsAnswerTxt());
		correctChB = new TanimedChBox(innerWidget);
			correctChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			correctChB->setStatusTip(tr("show correct answer for every mistake"));
			correctChB->hide();
    
    progress = new TprogressWidget(innerWidget);
    
    examResults = new TexamView(innerWidget);
    examResults->setStyleBg(Tcolor::bgTag(gl->EanswerColor), Tcolor::bgTag(gl->EquestionColor),
                            Tcolor::bgTag(gl->EnotBadColor));
		
		progress->hide();
    examResults->hide();
		QColor C(palette().text().color());
		C.setAlpha(15);
		C = Tcolor::merge(C, palette().window().color());
		nootLabel = new TnootkaLabel(gl->path + "picts/logo.png", innerWidget, C);
		
    noteName = new TnoteName(innerWidget);
    noteName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);

    guitar = new TfingerBoard(innerWidget);
		
//-------------------------------------------------------------------		
// Setting layout
		QVBoxLayout *chBlay = new QVBoxLayout;
			chBlay->addWidget(correctChB);
			chBlay->addWidget(autoRepeatChB);
			chBlay->addWidget(expertAnswChB);
			chBlay->addStretch(1);
			statLay->addLayout(chBlay);
		QHBoxLayout *toolAndHintLay = new QHBoxLayout;
			toolAndHintLay->addWidget(nootBar, 0, Qt::AlignTop);
			toolAndHintLay->addLayout(statLay);			
		m_scoreLay = new QVBoxLayout;
			m_scoreLay->addWidget(score);
			m_scoreLay->addWidget(pitchView);
		QVBoxLayout *rightPaneLay = new QVBoxLayout;
			rightPaneLay->addStretch(1);
			rightPaneLay->addWidget(progress);
			rightPaneLay->addWidget(examResults);
			rightPaneLay->addWidget(nootLabel);
      rightPaneLay->addStretch(1);
			rightPaneLay->addWidget(noteName);
			rightPaneLay->addStretch(1);
		m_rightLay = rightPaneLay;
		QHBoxLayout *scoreAndNameLay = new QHBoxLayout;
			scoreAndNameLay->addLayout(m_scoreLay);
			scoreAndNameLay->addLayout(rightPaneLay);
		QVBoxLayout *mainLay = new QVBoxLayout;
			mainLay->addLayout(toolAndHintLay);
			mainLay->addLayout(scoreAndNameLay);
			mainLay->addWidget(guitar);
    innerWidget->setLayout(mainLay);
    setCentralWidget(innerWidget);
//-------------------------------------------------------------------
    m_statusText = "";
    m_prevBg = -1;
    m_curBG = -1;
    m_lockStat = false;
    m_levelCreatorExist = false;

    createActions();

    connect(score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
    connect(noteName, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(noteNameWasChanged(Tnote)));
    connect(guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(guitarWasClicked(Tnote)));
    connect(sound, SIGNAL(detectedNote(Tnote)), this, SLOT(soundWasPlayed(Tnote)));

    if (gl->A->OUTenabled && !sound->isPlayable())
        QMessageBox::warning(this, "", tr("Problems with sound output"));
    
}

MainWindow::~MainWindow()
{
    gl->config->beginGroup("General");
      gl->config->setValue("geometry", geometry());
    gl->config->endGroup();
    delete gl;
}

//##########################################################################################
//#######################     METHODS       ################################################
//##########################################################################################

void MainWindow::createActions() {
    settingsAct = new QAction(tr("Settings"), this);
    settingsAct->setStatusTip(tr("Application preferences"));
//		settingsAct->setToolTip(settingsAct->statusTip());
    settingsAct->setIcon(QIcon(gl->path+"picts/systemsettings.png"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));

    levelCreatorAct = new QAction(this);
    connect(levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));

    startExamAct = new QAction(this);
    connect(startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));
    setStartExamActParams(); // set text and icon also for levelCreatorAct
    
    analyseAct = new QAction(tr("Analyze", "could be Chart as well"), this);
    analyseAct->setIcon(QIcon(gl->path+"picts/charts.png"));
    analyseAct->setStatusTip(tr("Analysis of exam results"));
//		analyseAct->setToolTip(analyseAct->statusTip());
    connect(analyseAct, SIGNAL(triggered()), this, SLOT(analyseSlot()));

    aboutAct = new QAction(tr("About"), this);
    aboutAct->setStatusTip(tr("About Nootka"));
//		aboutAct->setToolTip(aboutAct->statusTip());
    aboutAct->setIcon(QIcon(gl->path+"picts/about.png"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(aboutSlot()));

    nootBar->addAction(settingsAct);
    nootBar->addAction(levelCreatorAct);
    nootBar->addAction(analyseAct);
    nootBar->addAction(aboutAct);
    nootBar->addAction(startExamAct);
    
    nootBar->setMovable(false);
}

void MainWindow::setStartExamActParams() {
    levelCreatorAct->setText(tr("Level"));
    levelCreatorAct->setStatusTip(TlevelCreatorDlg::levelCreatorTxt());
//		levelCreatorAct->setToolTip(levelCreatorAct->statusTip());
    levelCreatorAct->setIcon(QIcon(gl->path+"picts/levelCreator.png"));
  
    startExamAct->setText(tr("Start!"));
    startExamAct->setStatusTip(tr("Start exercises or an exam"));
//		startExamAct->setToolTip(startExamAct->statusTip());
    startExamAct->setIcon(QIcon(gl->path+"picts/startExam.png"));
}

void MainWindow::setStatusMessage(QString msg) {
    if (!m_lockStat)
        m_statLab->setText("<center>" + msg + "</center>");
    else
        m_prevMsg = msg;
    m_statusText = msg;
}


void MainWindow::setStatusMessage(QString msg, int time) {
    m_prevMsg = m_statusText;
    m_statLab->setText("<center>" + msg + "</center>");
    m_lockStat = true;
    QTimer::singleShot(time, this, SLOT(restoreMessage()));
}


void MainWindow::setMessageBg(QColor bg) {
    if (bg == -1) {
				m_statLab->setDefaultBackground();
    }
    else
        m_statLab->setBackroundColor(bg);
    m_curBG = bg;
}


void MainWindow::clearAfterExam(TexamExecutor::Estate examState) {
    setStartExamActParams();
    delete ex;
    ex = 0;
    m_curBG = -1;
    m_prevBg = -1;
    setMessageBg(-1);
    if (examState == TexamExecutor::e_openCreator) 
				openLevelCreator();
		else
				sound->go();
		progress->hide();
		examResults->hide();
		nootLabel->show();
		updateSize(innerWidget->size());
}


QPoint MainWindow::relatedPoint() {
    return QPoint(noteName->geometry().x(), m_statLab->geometry().bottom() + 5);
}

//##########################################################################################
//#######################     SLOTS       ################################################
//##########################################################################################


void MainWindow::openFile(QString runArg) {
    if (ex || m_levelCreatorExist)
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
				if (Texam::couldBeExam(hdr)) {
					if (Texam::isExamVersion(hdr))
							ex = new TexamExecutor(this, runArg);
					else
							newerNootkaMessage(runArg, this);
				} else {
					if (Tlevel::couldBeLevel(hdr)) {
						if (Tlevel::isLevelVersion(hdr))
								openLevelCreator(runArg);
						else
								newerNootkaMessage(runArg, this);
					}
				}
    }
}


void MainWindow::createSettingsDialog() {
	TsettingsDialog *settings = new TsettingsDialog(this);
	sound->prepareToConf();
	if (settings->exec() == QDialog::Accepted) {
			delete settings;
			m_isPlayerFree = false;
			sound->acceptSettings();
			score->acceptSettings();
// 			if (gl->instrument == e_noInstrument) // Tsound sets ambitus to guitar range
// 				if (sound->isSniffable()) // but if no guitar - adjust it to score - clef range
// 					sound->sniffer->setAmbitus(score->lowestNote(), score->highestNote());
			noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
			noteName->setEnabledEnharmNotes(gl->showEnharmNotes);
			noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
			noteName->setStyle(gl->NnameStyleInNoteName);
				// set new colors in exam view
			examResults->setStyleBg(Tcolor::bgTag(gl->EanswerColor), Tcolor::bgTag(gl->EquestionColor),
															Tcolor::bgTag(gl->EnotBadColor));
			noteName->setAmbitus(gl->loString(),
															Tnote(gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber));
			updateSize(innerWidget->size());
			if (score->getNote(0).note != 0) {
				TnotesList nList;
				nList = score->getNote(0).getTheSameNotes(gl->doubleAccidentalsEnabled);
				if (nList[0].getChromaticNrOfNote() >= gl->loString().getChromaticNrOfNote() && 
					nList[0].getChromaticNrOfNote() <= gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber ) {
						if (gl->showEnharmNotes) { // refresh note name and score
								noteName->setNoteName(nList);
								if (nList.size() > 1)
										score->setNote(1, nList[1]);
								else {
										score->clearNote(1);
										score->clearNote(2);
								}
								if (nList.size() > 2)
										score->setNote(2, nList[2]);
								else
										score->clearNote(2);
						} else
								noteName->setNoteName(nList[0]);
				}
			}
			if (gl->instrument != e_noInstrument) {
					guitar->acceptSettings(); //refresh guitar
			}
			if (gl->hintsEnabled) {
				nootBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
			} else {
				m_prevBg = m_curBG;
				setStatusMessage(m_prevMsg);
				nootBar->setToolButtonStyle(Qt::ToolButtonIconOnly);				
			}
			m_isPlayerFree = true;
	} else { // settings not accepted
			delete settings;
			sound->restoreAfterConf();
	}
	if (resetConfig)
			close();
}


void MainWindow::openLevelCreator(QString levelFile) {
    sound->wait(); // stops pitch detection
    sound->stopPlaying();
    m_levelCreatorExist = true;
    TlevelCreatorDlg *levelCreator= new TlevelCreatorDlg(this);
    bool shallExamStart = false;
    if (levelFile != "")
        levelCreator->loadLevelFile(levelFile);
    if (levelCreator->exec() == QDialog::Accepted) {
        m_level = levelCreator->selectedLevel();
        if (m_level.name != "")
            shallExamStart = true;
    }
    delete levelCreator;
    m_levelCreatorExist = false;
    if (shallExamStart) {
        nootLabel->hide();
        progress->show();
        examResults->show();
        ex = new TexamExecutor(this, "", &m_level); // start exam
    } else
        sound->go(); // restore pitch detection
}


void MainWindow::startExamSlot() {
    sound->stopPlaying();
		nootLabel->hide();
    ex = new TexamExecutor(this);
}


void MainWindow::aboutSlot() {
    sound->wait();
    sound->stopPlaying();
    TaboutNootka *ab = new TaboutNootka(this);
    ab->exec();
    delete ab;
    sound->go();
}


void MainWindow::analyseSlot() {
    sound->wait();
    sound->stopPlaying();
    TanalysDialog *ad = new TanalysDialog(0, this);
    ad->exec();
    delete ad;
    sound->go();
}


void MainWindow::noteWasClicked(int index, Tnote note) {
    Q_UNUSED(index)
    if (m_isPlayerFree)
        sound->play(note);
    if (gl->showEnharmNotes){
        TnotesList noteList;
        noteList.push_back(note);
        noteList.push_back(score->getNote(1));
        noteList.push_back(score->getNote(2));
        noteName->setNoteName(noteList);
    } else
        noteName->setNoteName(note);
		if (guitar->isVisible())
				guitar->setFinger(note);
}


void MainWindow::noteNameWasChanged(Tnote note) {
    sound->play(note);
    score->setNote(0, note);
    if (gl->showEnharmNotes) {
        score->setNote(1, noteName->getNoteName(1));
        score->setNote(2, noteName->getNoteName(2));
    }
    if (guitar->isVisible())
				guitar->setFinger(note);
}


void MainWindow::guitarWasClicked(Tnote note) {
    sound->play(note);
    if (gl->showEnharmNotes) {
        TnotesList noteList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        noteName->setNoteName(noteList);
        score->setNote(1, noteName->getNoteName(1));
				if (gl->doubleAccidentalsEnabled)
						score->setNote(2, noteName->getNoteName(2));
    } else
        noteName->setNoteName(note);
    score->setNote(0, note);
}


void MainWindow::soundWasPlayed(Tnote note) {
  if (gl->showEnharmNotes) {
      TnotesList noteList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
      noteName->setNoteName(noteList);
      score->setNote(1, noteName->getNoteName(1));
			if (gl->doubleAccidentalsEnabled)
					score->setNote(2, noteName->getNoteName(2));
  } else
      noteName->setNoteName(note);
  score->setNote(0, note);
	if (guitar->isVisible())
			guitar->setFinger(note);
}


void MainWindow::restoreMessage() {
    m_lockStat = false;
    setStatusMessage(m_prevMsg);
    setMessageBg(m_prevBg);
    m_prevMsg = "";
}


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


void MainWindow::fixPitchViewPos() {
  if (!windowState().testFlag(Qt::WindowMaximized)) {
    // Lets hope user has no any abnormal desktop size and skip checking ratio for maximized
    if (innerWidget->height() > innerWidget->width() * 0.8) {
        resize(innerWidget->width(), innerWidget->width() * 0.75);
        return;
    } else if (innerWidget->width() > innerWidget->height() * 1.95) {
        resize(innerWidget->height() * 1.9, innerWidget->height());
        return;
    }
  }
  // when return occurred it will back here again from resizeEvent
	if (gl->instrument != e_noInstrument) {
		if (score->width() < innerWidget->width() / 2) { // remove pitchView from under score
			if (m_scoreLay->count() > 1) // if it is under score
				// and there is enough horizontal space for new width
				if (score->widthToHeight(score->height() + pitchView->height() + m_scoreLay->spacing()) < innerWidget->width() / 2) {
						m_scoreLay->removeWidget(pitchView);
						m_rightLay->addWidget(pitchView);
			}
		} else { // move pitchView under score
			if (m_scoreLay->count() < 2) { // if it is under noteName
				m_rightLay->removeWidget(pitchView);
				m_scoreLay->insertWidget(1, pitchView);			
			}
		}
	}
	if (score->width() > (innerWidget->width() - noteName->width()))
		score->setMaximumHeight(score->height() * ((qreal)(innerWidget->width() - noteName->width()) / (qreal)score->width()));
	else
    score->setMaximumHeight(16777215);
}


//##########################################################################################
//#######################     EVENTS       ################################################
//##########################################################################################

bool MainWindow::event(QEvent *event) {
    if (gl->hintsEnabled && event->type() == QEvent::StatusTip && !m_lockStat) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        if (se->tip() == "") {
            setMessageBg(m_prevBg);
            m_statLab->setText("<center>" + m_statusText + "</center>");
            m_prevMsg = m_statusText;
        } else {
            m_prevBg = m_curBG;
            setMessageBg(-1);
            m_statLab->setText("<center>" + se->tip() + "</center>");
        }
    } else
      if (ex && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)) {
        ex->event(event);
      }
    return QMainWindow::event(event);
}


void MainWindow::updateSize(QSize newS) {
	setUpdatesEnabled(false);
	m_statFontSize = (newS.height() / 10) / 4 - 2;
	nootBar->setIconSize(QSize(newS.height() / 22, height() / 22));
//   nootBar->setFixedWidth(newS.width() * 0.4); // It avoids flickering of status label when tool bar content is changing
// 	pitchView->resize(m_statFontSize);
	m_statLab->setFixedHeight(newS.height() / 10);
	QFont f = m_statLab->font();
	f.setPointSize(m_statFontSize);
	QFontMetrics fMetr(f);
	qreal fact = (qreal)(m_statFontSize * 1.5) / (qreal)fMetr.boundingRect("A").height();
	f.setPointSize(f.pointSize() * fact);
	m_statLab->setFont(f);
	guitar->setFixedHeight((newS.height() - nootBar->height()) * 0.25);
	progress->resize(m_statFontSize);
	examResults->setFontSize(m_statFontSize);
	noteName->resize(m_statFontSize);
	
	if (gl->instrument != e_noInstrument) {
		pitchView->resize(m_statFontSize);
		if (m_pitchContainer) {
			m_pitchContainer->layout()->removeWidget(pitchView);
			m_pitchContainer->deleteLater();
			m_pitchContainer = 0;
			guitar->show();
			m_rightLay->addWidget(pitchView);
		}
		QPixmap bgPix;
		qreal guitH;
		qreal ratio;
		if (gl->instrument == e_classicalGuitar) {
			bgPix = QPixmap(gl->path + "picts/body.png"); // size 800x535
			guitH = qRound(((double)guitar->height() / 350.0) * 856.0);
			int guitW = centralWidget()->width() / 2;
			m_bgPixmap = bgPix.scaled(guitW, guitH, Qt::IgnoreAspectRatio);
		}
		else {
			if (gl->instrument == e_bassGuitar)
					bgPix = QPixmap(gl->path + "picts/body-bass.png"); // size 
			else
					bgPix = QPixmap(gl->path + "picts/body-electro.png");
			guitH = guitar->height() * 2.9;
			ratio = guitH / bgPix.height();
			m_bgPixmap = bgPix.scaled(qRound(bgPix.width() * ratio), guitH, Qt::KeepAspectRatio);
			QPixmap rosePix(gl->path + "picts/pickup.png");
// 				ratio = guitar->height() / rosePix.height();
// 				ratio = ((qreal)guitar->fbRect().height()) / (qreal)rosePix.height();
// 				qDebug() << ratio << (guitar->width() - guitar->fbRect().width()) / 2 << rosePix.size();
			if (gl->instrument == e_bassGuitar)
					ratio *= 0.5;
			else
					ratio *= 0.6;
			m_rosettePixmap = rosePix.scaled(rosePix.width() * ratio, rosePix.height() * ratio, Qt::KeepAspectRatio);
// 			if (gl->instrument == e_classicalGuitar) {
// 				QPixmap rosePix(gl->path + "picts/rosette.png"); // size 341x281
// 				m_rosettePixmap = rosePix.scaled(341 * ratio, 281 * ratio, Qt::KeepAspectRatio);
		}
	} else { // no guitar - pitch view instead
			pitchView->resize(m_statFontSize * 1.7);
			if (!m_pitchContainer) {
				guitar->hide();
				m_pitchContainer = new QWidget(innerWidget);
				m_pitchContainer->setObjectName("m_pitchContainer");
				m_pitchContainer->setStyleSheet("QWidget#m_pitchContainer {" + Tcolor::bgTag(palette().window().color()) + 
					"border-radius: 10px;" + QString("background-image: url(%1);}").arg(gl->path + "picts/scoresettbg.png"));
				m_rightLay->removeWidget(pitchView);
				QVBoxLayout *pitchLay = new QVBoxLayout;
				pitchLay->addStretch(1);
				pitchLay->addWidget(pitchView);
				pitchLay->addStretch(1);
				m_pitchContainer->setLayout(pitchLay);
				innerWidget->layout()->addWidget(m_pitchContainer);
			}
	}
	if (m_pitchContainer)
		m_pitchContainer->setFixedHeight((height() - nootBar->height()) * 0.25);
	
	setUpdatesEnabled(true);
	QTimer::singleShot(2, this, SLOT(update()));
}


void MainWindow::resizeEvent(QResizeEvent * event) {
	updateSize(innerWidget->size());
	emit sizeChanged(innerWidget->size());
  QTimer::singleShot(3, this, SLOT(fixPitchViewPos()));
}




void MainWindow::closeEvent(QCloseEvent *event) {
    if (!settingsAct->isEnabled() && ex) {
        if (ex->closeNootka())
            event->accept();
        else
            event->ignore();
    }
}


void MainWindow::paintEvent(QPaintEvent* ) {
		if (gl->instrument != e_noInstrument) {
			QPainter painter(this);
			if (!gl->GisRightHanded) {
					painter.translate(width(), 0);
					painter.scale(-1, 1);
			}
			if (gl->instrument == e_classicalGuitar || gl->instrument == e_noInstrument) {
				painter.drawPixmap(guitar->posX12fret() + 7, guitar->geometry().bottom()/*height()*/ - m_bgPixmap.height(), m_bgPixmap);
// 				painter.drawPixmap(width() - qRound(m_rosettePixmap.width() * 0.75), 
// 												height() - ratio * 250 - (height() - guitar->geometry().bottom()), m_rosettePixmap );
			} else {
					qreal ratio = (guitar->height() * 3.3) / 535;
					painter.drawPixmap(guitar->fbRect().right() - 235 * ratio, height() - m_bgPixmap.height() /*+ 20 * ratio*/, m_bgPixmap);
          if (gl->GisRightHanded) {
              painter.drawPixmap(guitar->fbRect().right() + 20 * ratio, guitar->y() - 15 * ratio, m_rosettePixmap);
          } else {
            painter.resetTransform();
            painter.drawPixmap(guitar->geometry().width() - (guitar->fbRect().right() + 20 * ratio) - m_rosettePixmap.width(),
                               guitar->y() - 15 * ratio, m_rosettePixmap);
          }
      }
		}
}


bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::FileOpen) {
				QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(event);
				openFile(fileEvent->file());
				return true;
		} else { // standard event processing
				return QObject::eventFilter(obj, event);
		}
}
