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

#include "mainwindow.h"
#include "tglobals.h"
#include "tsettingsdialog.h"
#include "tlevelcreatordlg.h"
#include "tlevelselector.h"
#include "taboutnootka.h"
#include "tfirstrunwizzard.h"
#include "tsupportnootka.h"
#include "tnootkalabel.h"
#include "texamsettings.h"
#include <tupdateprocess.h>
#include "tsound.h"
#include "tpushbutton.h"
#include "tmainscore.h"
#include "texam.h"
#include "tprogresswidget.h"
#include "texamview.h"
#include "taudioparams.h"
#include "tpitchview.h"
#include "tanalysdialog.h"
#include <tquestionpoint.h>
#include "tnotename.h"
#include "tfingerboard.h"
#include <QtGui>


extern Tglobals *gl;
extern bool resetConfig;

TnootkaLabel *nootLab;
bool m_isPlayerFree = true;

MainWindow::MainWindow(QWidget *parent) :
		QMainWindow(parent),
    ex(0)
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
    
    setMinimumSize(660, 480);
		gl->config->beginGroup("General");
    setGeometry(gl->config->value("geometry", QRect(50, 50, 750, 480)).toRect());
    
    if (gl->isFirstRun) {
        TfirstRunWizzard *firstWizz = new TfirstRunWizzard();
        firstWizz->exec();
        delete firstWizz;
        gl->isFirstRun = false;
    } else { // show support window once but not with first run wizzard
				QString newVersion = gl->config->value("version", "").toString();
        if (newVersion != gl->version) {
// 					if (newVersion == "0.8.9-beta") { // Transitional behaviour for clef deployment
// 						gl->Sclef = Tclef::e_treble_G_8down;
// 					}
          QTimer::singleShot(200, this, SLOT(showSupportDialog()));
				} else { // check for updates
          gl->config->endGroup();
          gl->config->beginGroup("Updates");
          if (gl->config->value("enableUpdates", true).toBool() && TupdateProcess::isPossible()) {
              TupdateProcess *process = new TupdateProcess(true, this);
              process->start();
          }
        }
//         gl->config->endGroup();
    }
		gl->config->endGroup();
		
    TkeySignature::setNameStyle(gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);

    sound = new Tsound(this);

    innerWidget = new QWidget(this);
    QVBoxLayout *mainLay = new QVBoxLayout;

    QHBoxLayout *scoreAndNameLay = new QHBoxLayout;
    QVBoxLayout *scoreLay = new QVBoxLayout;
    nootBar = new QToolBar(tr("main toolbar"), innerWidget);
    scoreLay->addWidget(nootBar);
    score = new TmainScore(innerWidget);
    scoreLay->addWidget(score);
    pitchView = new TpitchView(sound->sniffer, this);
    sound->setPitchView(pitchView);
    scoreLay->addWidget(pitchView);
    scoreAndNameLay->addLayout(scoreLay);

//-------------------------------------------------------------------
    QVBoxLayout *nameLay = new QVBoxLayout;
 // Hints - label with clues
    QHBoxLayout *statLay = new QHBoxLayout;
    m_statLab = new QLabel(innerWidget);
    m_statLab->setWordWrap(true);
    m_statLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
		statLay->addWidget(m_statLab);
#if defined(Q_OS_WIN32)
    QColor bgLight = palette().window().color().lighter(101);
#else
    QColor bgLight = palette().window().color().lighter(105);
#endif
    m_statLab->setStyleSheet(gl->getBGcolorText(bgLight) + "border-radius: 10px;");
 // Expert corner
    QVBoxLayout *chBlay = new QVBoxLayout;
    m_hintsChB = new QCheckBox(innerWidget);
    chBlay->addWidget(m_hintsChB);
    m_hintsChB->setChecked(gl->hintsEnabled);
    m_hintsChB->setStatusTip(tr("show or hide the hints"));
    m_hintsChB->setToolTip(m_hintsChB->statusTip());
    m_hintsChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    autoRepeatChB = new QCheckBox(innerWidget);
    autoRepeatChB->hide();
    chBlay->addWidget(autoRepeatChB);
    autoRepeatChB->setStatusTip(TexamSettings::autoNextQuestTxt());
    autoRepeatChB->setToolTip(TexamSettings::autoNextQuestTxt());
    autoRepeatChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    expertAnswChB = new QCheckBox(innerWidget);
    expertAnswChB->hide();
    chBlay->addWidget(expertAnswChB);
    expertAnswChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    expertAnswChB->setStatusTip(TexamSettings::expertsAnswerTxt());
    expertAnswChB->setToolTip(TexamSettings::expertsAnswerTxt());
    chBlay->addStretch(1);
    
    progress = new TprogressWidget(innerWidget);
//     statLay->addStretch(1);
    statLay->addLayout(chBlay);
    nameLay->addLayout(statLay);
    
    examResults = new TexamView(innerWidget);
    examResults->setStyleBg(gl->getBGcolorText(gl->EanswerColor), gl->getBGcolorText(gl->EquestionColor),
                            gl->getBGcolorText(gl->EnotBadColor));
		
		progress->hide();
    examResults->hide();
		nameLay->addWidget(progress);
		nameLay->addWidget(examResults);
		
		nootLab = new TnootkaLabel(gl->path + "picts/logo.png", innerWidget);
		nameLay->addStretch(1);
		nameLay->addWidget(nootLab);
		nameLay->addStretch(1);
		
    noteName = new TnoteName(innerWidget);
    noteName->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
		nameLay->addWidget(noteName);
		nameLay->addStretch(1);
    scoreAndNameLay->addLayout(nameLay);
    mainLay->addLayout(scoreAndNameLay);
//-------------------------------------------------------------------
    guitar = new TfingerBoard(innerWidget);
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
    connect(m_hintsChB, SIGNAL(clicked(bool)), this, SLOT(hintsStateChanged(bool)));
    connect(sound, SIGNAL(detectedNote(Tnote)), this, SLOT(soundWasPlayed(Tnote)));

    if (gl->A->OUTenabled && !sound->isPlayable())
        QMessageBox::warning(this, "", tr("Problems with sound output"));
    
//     QTimer::singleShot(100, this, SLOT(analyseSlot()));
    
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
    settingsAct->setIcon(QIcon(gl->path+"picts/systemsettings.png"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));

    levelCreatorAct = new QAction(this);
    connect(levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));

    startExamAct = new QAction(this);
    connect(startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));
    setStartExamActParams(); // set text and icon also for levelCreatorAct
    
    analyseAct = new QAction(this);
    analyseAct->setIcon(QIcon(gl->path+"picts/charts.png"));
    analyseAct->setStatusTip(tr("Analyse of an exam results"));
    connect(analyseAct, SIGNAL(triggered()), this, SLOT(analyseSlot()));

    aboutAct = new QAction(tr("about"), this);
    aboutAct->setStatusTip(tr("About Nootka"));
    aboutAct->setIcon(QIcon(gl->path+"picts/about.png"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(aboutSlot()));

    nootBar->addAction(settingsAct);
    nootBar->addAction(levelCreatorAct);
    nootBar->addAction(analyseAct);
    nootBar->addAction(aboutAct);
    nootBar->addSeparator();
    nootBar->addAction(startExamAct);

    nootBar->setMovable(false);
}

void MainWindow::setStartExamActParams() {
    levelCreatorAct->setText(TlevelCreatorDlg::levelCreatorTxt());
    levelCreatorAct->setStatusTip(levelCreatorAct->text());
    levelCreatorAct->setIcon(QIcon(gl->path+"picts/levelCreator.png"));
  
    startExamAct->setText(tr("Start an exam"));
    startExamAct->setStatusTip(startExamAct->text());
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
//    m_prevBg = m_curBG;
    QTimer::singleShot(time, this, SLOT(restoreMessage()));
}

void MainWindow::setMessageBg(QColor bg) {
    if (bg == -1) {
      #if defined(Q_OS_WIN32)
        QColor bgLight = palette().window().color().lighter(101);
      #else
        QColor bgLight = palette().window().color().lighter(105);
      #endif
        m_statLab->setStyleSheet(gl->getBGcolorText(bgLight) + "border-radius: 10px;");
    }
    else
        m_statLab->setStyleSheet(gl->getBGcolorText(bg) + "border-radius: 10px;");
//    m_prevBg = m_curBG;
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
  nootLab->show();
}

QPoint MainWindow::relatedPoint() {
    return examResults->geometry().bottomLeft();
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
        if (hdr == Texam::examVersion || hdr == Texam::examVersion2)
            ex = new TexamExecutor(this, runArg);
        else if (hdr == TlevelSelector::levelVersion)
            openLevelCreator(runArg);
    }
}


void MainWindow::createSettingsDialog() {
    TsettingsDialog *settings = new TsettingsDialog(this);
    sound->prepareToConf();
//     qDebug("prepared");
    if (settings->exec() == QDialog::Accepted) {
        delete settings;
        m_isPlayerFree = false;
        sound->acceptSettings();
        score->acceptSettings();
				if (gl->instrument == e_noInstrument) // Tsound sets ambitus to guitar range
					if (sound->isSniffable()) // but if no guitar - adjust it to score - clef range
						sound->sniffer->setAmbitus(score->lowestNote(), score->highestNote());
        noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        noteName->setEnabledEnharmNotes(gl->showEnharmNotes);
        noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
        noteName->setStyle(gl->NnameStyleInNoteName);
          // set new colors in exam view
        examResults->setStyleBg(gl->getBGcolorText(gl->EanswerColor), gl->getBGcolorText(gl->EquestionColor), 
                                gl->getBGcolorText(gl->EnotBadColor));
        noteName->setAmbitus(gl->loString(),
                               Tnote(gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber));
        updsateSize();
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
// 						guitar->show();
						guitar->acceptSettings(); //refresh guitar
				} /*else*/
// 						guitar->hide();
        m_hintsChB->setChecked(gl->hintsEnabled);
        m_isPlayerFree = true;
    } else {
      delete settings;
      sound->restoreAfterConf();
    }
    if (resetConfig)
			close();
// 			qApp->quit();
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
        nootLab->hide();
        progress->show();
        examResults->show();
        ex = new TexamExecutor(this, "", &m_level); // start exam
    } else
        sound->go(); // restore pitch detection
}


void MainWindow::startExamSlot() {
    sound->stopPlaying();
    nootLab->hide();
    progress->show();
    examResults->show();
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
    guitar->setFinger(note);
}

void MainWindow::noteNameWasChanged(Tnote note) {
    sound->play(note);
    score->setNote(0, note);
    if (gl->showEnharmNotes) {
        score->setNote(1, noteName->getNoteName(1));
        score->setNote(2, noteName->getNoteName(2));
    }
    guitar->setFinger(note);
}

void MainWindow::guitarWasClicked(Tnote note) {
    sound->play(note);
    if (gl->showEnharmNotes) {
        TnotesList noteList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        noteName->setNoteName(noteList);
        score->setNote(1, noteName->getNoteName(1));
        score->setNote(2, noteName->getNoteName(2));
    } else
        noteName->setNoteName(note);
    score->setNote(0, note);
}

void MainWindow::soundWasPlayed(Tnote note) {
//   if (gl->hintsEnabled)
//     setStatusMessage(tr("Note was detected !!"), 750);
  if (gl->showEnharmNotes) {
      TnotesList noteList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
      noteName->setNoteName(noteList);
      score->setNote(1, noteName->getNoteName(1));
      score->setNote(2, noteName->getNoteName(2));
  } else
      noteName->setNoteName(note);
  score->setNote(0, note);
  guitar->setFinger(note);
}


void MainWindow::restoreMessage() {
    m_lockStat = false;
    setStatusMessage(m_prevMsg);
    setMessageBg(m_prevBg);
    m_prevMsg = "";
}

void MainWindow::hintsStateChanged(bool enable) {
    gl->hintsEnabled = enable;
    if (!enable) {
//        m_prevMsg = m_statusText;
        m_prevBg = m_curBG;
        setStatusMessage(m_prevMsg);
    }
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
            m_statLab->setText("<center>"+se->tip()+"</center>");
        }
    } else
      if (ex && (event->type() == QEvent::MouseButtonPress || event->type() == QEvent::MouseButtonRelease)) {
        ex->event(event);
      }
    return QMainWindow::event(event);
}



void MainWindow::updsateSize() {
    setUpdatesEnabled(false);
    m_statFontSize = (centralWidget()->height() / 9) / 4 - 2;
    nootBar->setIconSize(QSize(height() / 21, height() / 21));
    pitchView->resize(m_statFontSize);
		m_statLab->setFixedHeight(centralWidget()->height() / 9);
    QFont f = m_statLab->font();
    f.setPointSize(m_statFontSize);
    QFontMetrics fMetr(f);
    qreal fact = (qreal)(m_statFontSize * 1.5) / (qreal)fMetr.boundingRect("A").height();
    f.setPointSize(f.pointSize() * fact);
    m_statLab->setFont(f);
    guitar->setFixedHeight((centralWidget()->height() - nootBar->height()) * 0.25);
    progress->resize(m_statFontSize);
    examResults->setFontSize(m_statFontSize);
    noteName->resize(m_statFontSize);
		
		if (gl->instrument != e_noInstrument) {
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
				bgPix = QPixmap(gl->path + "picts/body-el.png"); // size 
				guitH = guitar->height() * 3.3;
				ratio = guitH / bgPix.height();
				m_bgPixmap = bgPix.scaled(qRound(bgPix.width() * ratio), guitH, Qt::KeepAspectRatio);
// 			if (gl->instrument == e_classicalGuitar) {
// 				QPixmap rosePix(gl->path + "picts/rosette.png"); // size 341x281
// 				m_rosettePixmap = rosePix.scaled(341 * ratio, 281 * ratio, Qt::KeepAspectRatio);
			}
		} 
// 		else {
// 			score->setFixedHeight(height() * 0.6);
// 		}
    
    setUpdatesEnabled(true);
    QTimer::singleShot(2, this, SLOT(update())); 
}



void MainWindow::resizeEvent(QResizeEvent * event) {
    updsateSize();
    emit sizeChanged(event->size());    
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
			if (gl->instrument == e_classicalGuitar) {
				painter.drawPixmap(guitar->posX12fret() + 7, guitar->geometry().bottom()/*height()*/ - m_bgPixmap.height(), m_bgPixmap);
// 				painter.drawPixmap(width() - qRound(m_rosettePixmap.width() * 0.75), 
// 												height() - ratio * 250 - (height() - guitar->geometry().bottom()), m_rosettePixmap );
			} else {
					qreal ratio = (guitar->height() * 3.3) / 535;
					painter.drawPixmap(guitar->fbRect().right() - 235 * ratio, height() - m_bgPixmap.height(), m_bgPixmap);
			}
		}
}



bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::FileOpen)
        {
            QFileOpenEvent* fileEvent = static_cast<QFileOpenEvent*>(event);
            openFile(fileEvent->file());
            return true;
        }
        else
        {
            // standard event processing
            return QObject::eventFilter(obj, event);
        }
}
