/***************************************************************************
 *   Copyright (C) 2011 by Tomasz Bojczuk  				   *
 *   tomaszbojczuk@gmail.com   						   *
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
 *  You should have received a copy of the GNU General Public License	   *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.  *
 ***************************************************************************/

#include "tglobals.h"
#include "settingsdialog.h"
#include "mainwindow.h"
#include "examsettingsdlg.h"
#include "taboutnootka.h"
#include "tfirstrunwizzard.h"
#include "examsettings.h"
#include "tplayer.h"
#include <QtGui>
//#include <QDebug>

Tglobals *gl = new Tglobals();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    gl->path = Tglobals::getInstPath(qApp->applicationDirPath());
    Ttune::prepareDefinedTunes();

    setWindowIcon(QIcon(gl->path+"picts/nootka.svg"));
    setMinimumSize(640,480);

    if (gl->isFirstRun) {
        TfirstRunWizzard *firstWizz = new TfirstRunWizzard(this);
        firstWizz->exec();
        delete firstWizz;
        gl->isFirstRun = false;
	TkeySignature::setNameStyle(
                gl->SnameStyleInKeySign, gl->SmajKeyNameSufix, gl->SminKeyNameSufix);
    }

    if (gl->AoutSoundEnabled)
        player = new Tplayer();
    else
        player = 0;

    QWidget *widget = new QWidget(this);
    QVBoxLayout *mainLay = new QVBoxLayout;

    QHBoxLayout *scoreAndNameLay = new QHBoxLayout;
    QVBoxLayout *scoreLay = new QVBoxLayout;
    nootBar = new QToolBar(tr("main toolbar"), widget);
    scoreLay->addWidget(nootBar);
    score = new TscoreWidget(3, widget);
    scoreLay->addWidget(score);
    scoreAndNameLay->addLayout(scoreLay);

//-------------------------------------------------------------------
    QVBoxLayout *nameLay = new QVBoxLayout;
//     QGroupBox *statGr = new QGroupBox(widget);
    QHBoxLayout *statLay = new QHBoxLayout;
    m_statLab = new QLabel(widget);
    m_statLab->setWordWrap(true);
    m_statLab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    statLay->addWidget(m_statLab);
    QVBoxLayout *chBlay = new QVBoxLayout;
    m_hintsChB = new QCheckBox(widget);
//    statLay->addWidget(m_hintsChB, 0, Qt::AlignRight);
    chBlay->addWidget(m_hintsChB);
    m_hintsChB->setChecked(gl->hintsEnabled);
    m_hintsChB->setStatusTip(tr("show or hide the hints"));
    m_hintsChB->setToolTip(m_hintsChB->statusTip());
    m_hintsChB->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    autoRepeatChB = new QCheckBox(widget);
    autoRepeatChB->hide();
    chBlay->addWidget(autoRepeatChB);
    autoRepeatChB->setStatusTip(ExamSettings::autoNextQuestTxt());
    autoRepeatChB->setToolTip(ExamSettings::autoNextQuestTxt());
    autoRepeatChB->setChecked(gl->EautoNextQuest);
    statLay->addLayout(chBlay);
    nameLay->addLayout(statLay);

    examResults = new TexamView(widget);
    nameLay->addWidget(examResults);
    noteName = new TnoteName(widget);
    noteName->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    nameLay->addWidget(noteName);
//    nameLay->addStretch(1);
    scoreAndNameLay->addLayout(nameLay);
    mainLay->addLayout(scoreAndNameLay);
//-------------------------------------------------------------------
    guitar = new TfingerBoard(widget);
    mainLay->addWidget(guitar);
    widget->setLayout(mainLay);
    setCentralWidget(widget);
//-------------------------------------------------------------------
    m_statusText = "";
    m_prevBg = -1;
    m_curBG = -1;
    m_lockStat = false;
    ex = 0;

    createActions();

    connect(score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
    connect(noteName, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(noteNameWasChanged(Tnote)));
    connect(guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(guitarWasClicked(Tnote)));
    connect(m_hintsChB, SIGNAL(clicked(bool)), this, SLOT(hintsStateChanged(bool)));

    if (gl->AoutSoundEnabled && !player->isPlayable())
        QMessageBox::warning(this, "", tr("Problems with sound output"));

}

MainWindow::~MainWindow()
{
    delete gl;
}

void MainWindow::createActions() {
    settingsAct = new QAction(tr("Settings"), this);
    settingsAct->setStatusTip(tr("Application preferences"));
    settingsAct->setIcon(QIcon(gl->path+"picts/systemsettings.png"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));

    levelCreatorAct = new QAction(tr("Levels' creator"), this);
    levelCreatorAct->setStatusTip(levelCreatorAct->text());
    levelCreatorAct->setIcon(QIcon(gl->path+"picts/levelCreator.png"));
    connect(levelCreatorAct, SIGNAL(triggered()), this, SLOT(openLevelCreator()));

    startExamAct = new QAction(this);
    setStartExamActParams();
    connect(startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));

    aboutAct = new QAction(tr("about"), this);
    aboutAct->setStatusTip(tr("About Nootka"));
//    aboutAct->setMenuRole(QAction::AboutRole);
    aboutAct->setIcon(QIcon(gl->path+"picts/about.png"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(aboutSlot()));

    nootBar->addAction(settingsAct);
    nootBar->addAction(levelCreatorAct);
    nootBar->addAction(aboutAct);
    nootBar->addSeparator();
    nootBar->addAction(startExamAct);

    nootBar->setMovable(false);
}

void MainWindow::setStartExamActParams() {
    startExamAct->setText(tr("Start an exam"));
    startExamAct->setStatusTip(startExamAct->text());
    startExamAct->setIcon(QIcon(gl->path+"picts/startExam.png"));
}

void MainWindow::resizeEvent(QResizeEvent *) {
    nootBar->setIconSize(QSize(height()/21, height()/21));
    score->setFixedWidth((centralWidget()->width()/13)*6);
    m_statLab->setFixedHeight(height()/9);
    m_statFontSize = m_statLab->height()/4;
    QFont f = m_statLab->font();
    f.setPixelSize(m_statFontSize);
    m_statLab->setFont(f);
    guitar->setFixedHeight(centralWidget()->height()/3);
    examResults->setFixedHeight(height() / 7);
    examResults->setFontSize(m_statFontSize);
    noteName->setFixedSize (QSize(centralWidget()->width()- score->width() -2, qRound(height() * 0.4)));
    noteName->resize(m_statFontSize);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (!settingsAct->isEnabled() && ex) {
        if (ex->closeNootka())
            event->accept();
        else
            event->ignore();
    }
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

void MainWindow::clearAfterExam() {
    setStartExamActParams();
    delete ex;
    ex = 0;
    m_curBG = -1;
    m_prevBg = -1;
    setMessageBg(-1);
}


void MainWindow::openFile(QString runArg) {
    if (QFile::exists(runArg)) {
        QFile file(runArg);
        quint32 hdr = 0;
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            in.setVersion(QDataStream::Qt_4_7);
            in >> hdr; // check what file type
        }
        file.close();
        if (hdr == TexamExecutor::examVersion)
            ex = new TexamExecutor(this, runArg);
        else if (hdr == TlevelSelector::levelVersion)
            openLevelCreator(runArg);
    }
}


//#######################     SLOTS       ################################################

void MainWindow::createSettingsDialog() {
    SettingsDialog *settings = new SettingsDialog(this);
    if (settings->exec() == QDialog::Accepted) {
        score->acceptSettings();
        noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        noteName->setEnabledEnharmNotes(gl->showEnharmNotes);
        noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
        noteName->setAmbitus(gl->loString(),
                               Tnote(gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber));
        noteWasClicked(0, noteName->getNoteName(0)); //refresh name
        guitar->acceptSettings(); //refresh guitar
        m_hintsChB->setChecked(gl->hintsEnabled);
        if (gl->AoutSoundEnabled) {
            if (!player)
                player = new Tplayer();
            else // all this to avoid reading wav file every time
                player->setDevice();
        } else {
            delete player;
            player = 0;
        }
    }
    delete settings;
}

void MainWindow::openLevelCreator(QString levelFile) {
    examSettingsDlg *examSettDlg = new examSettingsDlg(this, levelFile);
    examSettDlg->exec();
    delete examSettDlg;
}

void MainWindow::startExamSlot() {
    ex = new TexamExecutor(this);
}

void MainWindow::aboutSlot() {
    TaboutNootka *ab = new TaboutNootka(this);
    ab->exec();
    delete ab;
}

void MainWindow::noteWasClicked(int index, Tnote note) {
    if (gl->AoutSoundEnabled)
        player->play(note);
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
    if (gl->AoutSoundEnabled)
        player->play(note);
    score->setNote(0, note);
    if (gl->showEnharmNotes) {
        score->setNote(1, noteName->getNoteName(1));
        score->setNote(2, noteName->getNoteName(2));
    }
    guitar->setFinger(note);
}

void MainWindow::guitarWasClicked(Tnote note) {
    if (gl->AoutSoundEnabled)
        player->play(note);
    if (gl->showEnharmNotes) {
        TnotesList noteList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        noteName->setNoteName(noteList);
        score->setNote(1, noteName->getNoteName(1));
        score->setNote(2, noteName->getNoteName(2));
    } else
        noteName->setNoteName(note);
    score->setNote(0, note);
}

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
