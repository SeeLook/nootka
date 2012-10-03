/***************************************************************************
 *   Copyright (C) 2011-2012 by Tomasz Bojczuk                             *
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

#include "tglobals.h"
#include "settingsdialog.h"
#include "mainwindow.h"
#include "tlevelcreatordlg.h"
#include "tlevelselector.h"
#include "taboutnootka.h"
#include "tfirstrunwizzard.h"
#include "tsupportnootka.h"
#include "examsettings.h"
#include "tsound.h"
#include "tpushbutton.h"
#include <tscorewidget.h>
#include "texam.h"
#include "tprogresswidget.h"
#include <texamview.h>
#include "taudioparams.h"
#include "taudioout.h"
#include "tpitchview.h"
#include "tanalysdialog.h"
#include "tnotename.h"
#include "tfingerboard.h"
#include <QtGui>


extern Tglobals *gl;


QLabel *nootLab;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    Ttune::prepareDefinedTunes();
#if defined(Q_OS_MAC)
    TpushButton::setCheckColor(gl->SpointerColor, palette().base().color());
    TquestionPoint::setColors(QColor(gl->EanswerColor.name()), QColor(gl->EquestionColor.name()), Qt::darkMagenta,
        QColor(100, 100, 100, 180), palette().window().color());
#else
    TpushButton::setCheckColor(palette().highlight().color().name(), palette().highlightedText().color() );
    TquestionPoint::setColors(QColor(gl->EanswerColor.name()), QColor(gl->EquestionColor.name()), Qt::darkMagenta,
        palette().shadow().color(), palette().base().color());
#endif
#if defined(Q_OS_LINUX)
    setWindowIcon(QIcon(gl->path+"picts/nootka.svg"));
#else
    setWindowIcon(QIcon(gl->path+"picts/nootka.png"));
#endif
    
    setMinimumSize(640, 480);
    
    gl->config->beginGroup("General");
#if defined(Q_OS_MAC)
    setGeometry(gl->config->value("geometry", QRect(50, 50, 960, 720)).toRect());
#else
    setGeometry(gl->config->value("geometry", QRect(50, 50, 800, 600)).toRect());
#endif
    gl->config->endGroup();

    if (gl->isFirstRun) {
        TfirstRunWizzard *firstWizz = new TfirstRunWizzard();
        firstWizz->exec();
        delete firstWizz;
        gl->isFirstRun = false;
    } else { // show support window once but not with first run wizzard
        gl->config->beginGroup("General");
        setGeometry(gl->config->value("geometry", QRect(50, 50, 800, 600)).toRect());
        if (gl->config->value("version", "").toString() != gl->version)
          QTimer::singleShot(200, this, SLOT(showSupportDialog()));
        gl->config->endGroup();
    }
    TkeySignature::setNameStyle(gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);

    sound = new Tsound(this);

    innerWidget = new QWidget(this);
    QVBoxLayout *mainLay = new QVBoxLayout;

    QHBoxLayout *scoreAndNameLay = new QHBoxLayout;
    QVBoxLayout *scoreLay = new QVBoxLayout;
    nootBar = new QToolBar(tr("main toolbar"), innerWidget);
    scoreLay->addWidget(nootBar);
    score = new TscoreWidget(3, innerWidget);
    scoreLay->addWidget(score);
    pitchView = new TpitchView(sound->sniffer, this);
    sound->setPitchView(pitchView);
    scoreLay->addWidget(pitchView);
    scoreAndNameLay->addLayout(scoreLay);

//-------------------------------------------------------------------
    QVBoxLayout *nameLay = new QVBoxLayout;
//     QGroupBox *statGr = new QGroupBox(widget);
 // Hints - label with clues
    QHBoxLayout *statLay = new QHBoxLayout;
    m_statLab = new QLabel(innerWidget);
    m_statLab->setWordWrap(true);
    m_statLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
//     statLay->addWidget(m_statLab);
 // Expert corner
    QVBoxLayout *chBlay = new QVBoxLayout;
    m_hintsChB = new QCheckBox(innerWidget);
//    statLay->addWidget(m_hintsChB, 0, Qt::AlignRight);
//     chBlay->addStretch();
    chBlay->addWidget(m_hintsChB);
    m_hintsChB->setChecked(gl->hintsEnabled);
    m_hintsChB->setStatusTip(tr("show or hide the hints"));
    m_hintsChB->setToolTip(m_hintsChB->statusTip());
    m_hintsChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    autoRepeatChB = new QCheckBox(innerWidget);
    autoRepeatChB->hide();
    chBlay->addWidget(autoRepeatChB);
    autoRepeatChB->setStatusTip(ExamSettings::autoNextQuestTxt());
    autoRepeatChB->setToolTip(ExamSettings::autoNextQuestTxt());
    autoRepeatChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    expertAnswChB = new QCheckBox(innerWidget);
    expertAnswChB->hide();
    chBlay->addWidget(expertAnswChB);
    expertAnswChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    expertAnswChB->setStatusTip(ExamSettings::expertsAnswerTxt());
    expertAnswChB->setToolTip(ExamSettings::expertsAnswerTxt());
    chBlay->addStretch(1);
//     statLay->addLayout(chBlay);
//     nameLay->addLayout(statLay);
    
//    QVBoxLayout *progresStatLay = new QVBoxLayout;
//     progresStatLay->addWidget(m_statLab);
    progress = new TprogressWidget(innerWidget);
//     progresStatLay->addWidget(progress);
//     statLay->addLayout(progresStatLay);
    statLay->addStretch(1);
    statLay->addLayout(chBlay);
    nameLay->addLayout(statLay);
    
//     nameLay->addWidget(progress);
//     nameLay->addStretch();

    examResults = new TexamView(innerWidget);
//     examResults->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    examResults->setStyleBg(gl->getBGcolorText(gl->EanswerColor), gl->getBGcolorText(gl->EquestionColor),
                            gl->getBGcolorText(Qt::magenta));
//     nameLay->addWidget(examResults);
    noteName = new TnoteName(innerWidget);
    noteName->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
//     nameLay->addWidget(noteName);
//     nameLay->addStretch(2);
    scoreAndNameLay->addLayout(nameLay);
    mainLay->addLayout(scoreAndNameLay);
//-------------------------------------------------------------------
    guitar = new TfingerBoard(innerWidget);
    mainLay->addWidget(guitar);
    innerWidget->setLayout(mainLay);
    setCentralWidget(innerWidget);
    
    progress->hide();
    examResults->hide();
    nootLab = new QLabel(QString("<center><img src=\"%1\"></center>").arg(gl->path + "picts/logo.png"), innerWidget);
//-------------------------------------------------------------------
    m_statusText = "";
    m_prevBg = -1;
    m_curBG = -1;
    m_lockStat = false;
    ex = 0;
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
    if (bg == -1)
        m_statLab->setStyleSheet("background: transparent");
    else
        m_statLab->setStyleSheet(gl->getBGcolorText(bg));
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
    SettingsDialog *settings = new SettingsDialog(this);
    sound->prepareToConf();
//     qDebug("prepared");
    if (settings->exec() == QDialog::Accepted) {
        sound->acceptSettings();
        score->acceptSettings();
        noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        noteName->setEnabledEnharmNotes(gl->showEnharmNotes);
        noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
          // set new colors in exam view
        examResults->setStyleBg(gl->getBGcolorText(gl->EanswerColor), gl->getBGcolorText(gl->EquestionColor)/*, 
                                gl->getBGcolorText(Qt::magenta)*/);
        noteName->setAmbitus(gl->loString(),
                               Tnote(gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber));
        noteWasClicked(0, noteName->getNoteName(0)); //refresh name
        guitar->acceptSettings(); //refresh guitar
        m_hintsChB->setChecked(gl->hintsEnabled);
    } else
      sound->restoreAfterConf();
    delete settings;
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
    } 
    return QMainWindow::event(event);
}

void MainWindow::resizeEvent(QResizeEvent * event) {
//     setUpdatesEnabled(false);
    nootBar->setIconSize(QSize(height()/21, height()/21));
    score->setFixedWidth((centralWidget()->width()/14)*6);
    int posX = score->width() + 2;
    int gapY = 5;
//     m_statLab->setFixedHeight(centralWidget()->height()/9);
    m_statLab->setGeometry(posX, 7, centralWidget()->width() * 0.5, centralWidget()->height()/9);
#if defined(Q_OS_MAC)
   m_statFontSize = m_statLab->height()/4-2;
#else
    m_statFontSize = m_statLab->height()/4 - 2;
#endif
    QFont f = m_statLab->font();
    f.setPointSize(m_statFontSize);
    m_statLab->setFont(f);
    guitar->setFixedHeight((centralWidget()->height() - nootBar->height()) * 0.25);
    progress->resize(m_statFontSize);
    progress->setGeometry(posX, m_statLab->geometry().bottom() + gapY,
                          centralWidget()->width()- score->width() -2, centralWidget()->height() * 0.15);
    progress->setFixedHeight(centralWidget()->height() * 0.15);
//     examResults->setFixedHeight(centralWidget()->height() / 10);
    examResults->setFontSize(m_statFontSize);
    examResults->setGeometry(posX, progress->geometry().bottom() + gapY, centralWidget()->width()- score->width() -2, 
      centralWidget()->height() * 0.1);
    examResults->setFixedHeight(centralWidget()->height() * 0.08);
    noteName->resize(m_statFontSize);
//     examResults->setFixedHeight(qRound(centralWidget()->height() * 0.12));
//     noteName->setFixedSize (QSize(centralWidget()->width()- score->width() -2, qRound(centralWidget()->height() * 0.4)));
    noteName->setGeometry(posX, qRound(centralWidget()->height() * 0.35),
                          centralWidget()->width() - score->width() -2, qRound(centralWidget()->height() * 0.4));
    noteName->resize(m_statFontSize);
    pitchView->resize(m_statFontSize);
    nootLab->setGeometry(posX, qRound(centralWidget()->height() * 0.12), centralWidget()->width()- score->width() -2,
          qRound(centralWidget()->height() * 0.25));
    
    QPixmap bgPix(gl->path + "picts/body.png");
    int guitH = qRound(((double)guitar->height() / 350.0) * 856.0);
    int guitW = centralWidget()->width() / 2;
    m_bgPixmap = bgPix.scaled(guitW, guitH, Qt::IgnoreAspectRatio);
//    qDebug() << m_bgPixmap.size() /*<< centralWidget()->width() << guitar->geometry().x() << guitar->posX12fret()*/;
    
//     setUpdatesEnabled(true);
    emit sizeChanged(event->size());
    QTimer::singleShot(2, this, SLOT(update()));
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
    QPainter painter(this);
    if (!gl->GisRightHanded) {
        painter.translate(width(), 0);
        painter.scale(-1, 1);
    }
    painter.drawPixmap(guitar->posX12fret(), guitar->geometry().bottom() - m_bgPixmap.height(), m_bgPixmap);
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
