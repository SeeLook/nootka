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

#include "settingsdialog.h"
#include "mainwindow.h"
#include "tglobals.h"
#include "examsettingsdlg.h"
//#include <QDebug>

Tglobals *gl = new Tglobals();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    gl->path = Tglobals::getInstPath(qApp->applicationDirPath());

    setWindowIcon(QIcon(gl->path+"picts/nootka.svg"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *scoreAndNameLay = new QHBoxLayout;
    QVBoxLayout *scoreLay = new QVBoxLayout;
    nootBar = new QToolBar(tr("main toolbar"),widget);
    scoreLay->addWidget(nootBar);
    score = new TscoreWidget(3,widget);
    scoreLay->addWidget(score);
    scoreAndNameLay->addLayout(scoreLay);

    QVBoxLayout *nameLay = new QVBoxLayout;
    QGroupBox *statGr = new QGroupBox(widget);
    QVBoxLayout *statLay = new QVBoxLayout;
//    QHBoxLayout *statResultLay = new QHBoxLayout;
    m_statLab = new QLabel(widget);
    m_statLab->setWordWrap(true);
//    m_statLab->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
//    m_statLab->setFixedHeight(50);
    statLay->addWidget(m_statLab);
    statGr->setLayout(statLay);
    nameLay->addWidget(statGr);
//    statResultLay->addWidget(statGr);
//    nameLay->addLayout(statLay);
//     nameLay->addStretch(1);
    examResults = new TexamView(widget);
    nameLay->addWidget(examResults);
//    statResultLay->addWidget(examResults);
//    nameLay->addLayout(statResultLay);

//     nameLay->addStretch(1);
    noteName = new TnoteName(widget);
    nameLay->addWidget(noteName);
//    nameLay->addStretch(1);
    scoreAndNameLay->addLayout(nameLay);
    mainLay->addLayout(scoreAndNameLay);

    guitar = new TfingerBoard(widget);
    mainLay->addWidget(guitar);
    setMinimumSize(640,480);    
    widget->setLayout(mainLay);
    setCentralWidget(widget);

    m_statusText = "";
    m_prevBg = -1;

    createActions();


    connect(score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
    connect(noteName, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(noteNameWasChanged(Tnote)));
    connect(guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(guitarWasClicked(Tnote)));



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
    levelCreatorAct->setIcon(QIcon(gl->path+"picts/examSettings.png"));
    connect(levelCreatorAct, SIGNAL(triggered()), this, SLOT(createExamSettingsDlg()));

    startExamAct = new QAction(tr("Start exam"), this);
    startExamAct->setStatusTip(startExamAct->text());
    startExamAct->setIcon(QIcon(gl->path+"picts/startExam.png"));
    connect(startExamAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));

    aboutAct = new QAction(tr("about"), this);
    aboutAct->setStatusTip(tr("About Nootka"));
    aboutAct->setIcon(QIcon(gl->path+"picts/about.png"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(aboutSlot()));

    nootBar->addAction(settingsAct);
    nootBar->addAction(levelCreatorAct);
    nootBar->addAction(aboutAct);
    nootBar->addSeparator();
    nootBar->addAction(startExamAct);

    nootBar->setMovable(false);
}



void MainWindow::resizeEvent(QResizeEvent *) {
    nootBar->setIconSize(QSize(height()/21, height()/21));
    score->setFixedWidth((centralWidget()->width()/13)*6);
    m_statLab->setFixedHeight(height()/10);
    guitar->setFixedHeight(centralWidget()->height()/3);
    noteName->setFixedSize (QSize(centralWidget()->width()- score->width() -2, qRound(height() * 0.6)));
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
    if (bg == -1)
        m_statLab->setStyleSheet("background: transparent");
    else {
        m_statLab->setStyleSheet(
            QString("background: rgba(%1, %2, %3, 20)").arg(bg.red()).arg(bg.green()).arg(bg.blue()));
        m_prevBg = bg;
    }
}

//##########        SLOTS       ###############

void MainWindow::createSettingsDialog() {
    SettingsDialog *settings = new SettingsDialog(this);
    if (settings->exec() == QDialog::Accepted) {
        score->acceptSettings();
        noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        noteName->setEnabledEnharmNotes(gl->showEnharmNotes);
        noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
//        noteName->setAmbitus(gl->Gtune.lowest(),
//                               Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber));
        noteName->setAmbitus(gl->loString(),
                               Tnote(gl->hiString().getChromaticNrOfNote() + gl->GfretsNumber));
        noteWasClicked(0,noteName->getNoteName(0));//refresh name
        guitar->acceptSettings();;//refresh guitar
    }
}

void MainWindow::createExamSettingsDlg() {
    examSettingsDlg *examSettDlg = new examSettingsDlg(this);
    examSettDlg->exec();
}

void MainWindow::startExamSlot() {
    TexamExecutor *ex = new TexamExecutor(this);
}

void MainWindow::aboutSlot() {
    QMessageBox *msg = new QMessageBox(this);
    msg->setText("<center><b>Nootka " + gl->version + tr("</b></center><p>This is developers preview of Nootka. It works quitely stable, but has less functioinality yet.</p><p>See a <a href=\"http://nootka.sourceforge.net\">program site</a> for more details and furter relaces.</p><p>Any bugs, sugestions, translations and so on, report to: <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><p/><p style=\"text-align: right;\">with respects<br>Author</p>"));
    msg->exec();
}

void MainWindow::noteWasClicked(int index, Tnote note) {
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
    score->setNote(0, note);
    if (gl->showEnharmNotes) {
        score->setNote(1, noteName->getNoteName(1));
        score->setNote(2, noteName->getNoteName(2));
    }
    guitar->setFinger(note);
}

void MainWindow::guitarWasClicked(Tnote note) {
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
        } else {
            setMessageBg(-1);
            m_statLab->setText("<center>"+se->tip()+"</center>");
        }
    }
    return QMainWindow::event(event);
}

void MainWindow::restoreMessage() {
    m_lockStat = false;
    setStatusMessage(m_prevMsg);
    m_prevMsg = "";
}
