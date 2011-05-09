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
#include "texamexecutor.h"
//#include <QDebug>

Tglobals *gl = new Tglobals();

QString hintText;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    gl->path = Tglobals::getInstPath(qApp->applicationDirPath());

//    statusBar()->showMessage("Nootka " + gl->version);
    hintText = "<center><b>Welcome !!</b></center>";

    setWindowIcon(QIcon(gl->path+"picts/nootka.svg"));

    QWidget *widget = new QWidget(this);
    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *scoreAndNameLay = new QHBoxLayout;
    QVBoxLayout *scoreLay = new QVBoxLayout;
    nootBar = new QToolBar(tr("main toolbar"),widget);
    scoreLay->addWidget(nootBar);
    m_score = new TscoreWidget(3,widget);
    scoreLay->addWidget(m_score);
    scoreAndNameLay->addLayout(scoreLay);

    QVBoxLayout *nameLay = new QVBoxLayout;
    QGroupBox *statGr = new QGroupBox(widget);
    QVBoxLayout *statLay = new QVBoxLayout;
    m_statLab = new QLabel(hintText, widget);
    m_statLab->setWordWrap(true);
    m_statLab->setFixedHeight(50);
    statLay->addWidget(m_statLab);
    statGr->setLayout(statLay);
    nameLay->addWidget(statGr);
//    nameLay->addLayout(statLay);
    nameLay->addStretch(1);
    m_examView = new TexamView(this);
    nameLay->addWidget(m_examView);

    nameLay->addStretch(1);
    m_noteName = new TnoteName(widget);
    nameLay->addWidget(m_noteName);
    scoreAndNameLay->addLayout(nameLay);
    mainLay->addLayout(scoreAndNameLay);

    m_guitar = new TfingerBoard(widget);
    mainLay->addWidget(m_guitar);
    setMinimumSize(640,480);    
    widget->setLayout(mainLay);
    setCentralWidget(widget);

    createActions();

//    scoreAndNameLay->addStretch(1);




    connect(m_score, SIGNAL(noteChanged(int,Tnote)), this, SLOT(noteWasClicked(int,Tnote)));
    connect(m_noteName, SIGNAL(noteNameWasChanged(Tnote)), this, SLOT(noteNameWasChanged(Tnote)));
    connect(m_guitar, SIGNAL(guitarClicked(Tnote)), this, SLOT(guitarWasClicked(Tnote)));



}

MainWindow::~MainWindow()
{
    delete gl;
}

void MainWindow::createActions() {
    settingsAct = new QAction(tr("Settings"),this);
    settingsAct->setStatusTip(tr("Application preferences"));
    settingsAct->setIcon(QIcon(gl->path+"picts/systemsettings.png"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));

    examSetAct = new QAction(tr("Exams settings"),this);
    examSetAct->setStatusTip(tr("Settings for exam's levels"));
    examSetAct->setIcon(QIcon(gl->path+"picts/examSettings.png"));
    connect(examSetAct, SIGNAL(triggered()), this, SLOT(createExamSettingsDlg()));

    startAct = new QAction(tr("Start exam"),this);
    startAct->setStatusTip(startAct->text());
    startAct->setIcon(QIcon(gl->path+"picts/startExam.png"));
    connect(startAct, SIGNAL(triggered()), this, SLOT(startExamSlot()));

    aboutAct = new QAction(tr("about"),this);
    aboutAct->setStatusTip(tr("About Nootka"));
    aboutAct->setIcon(QIcon(gl->path+"picts/about.png"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(aboutSlot()));

    nootBar->addAction(settingsAct);
    nootBar->addAction(examSetAct);
    nootBar->addAction(startAct);
    nootBar->addAction(aboutAct);

    nootBar->setMovable(false);
}



void MainWindow::resizeEvent(QResizeEvent *) {
    nootBar->setIconSize(QSize(height()/21, height()/21));
    m_score->setFixedWidth(centralWidget()->width()/2);
//    m_noteName->setFixedHeight((centralWidget()->height()/9)*4);
    m_guitar->setFixedHeight(centralWidget()->height()/3);
//    m_guitar->resize();
//    m_guitar->setFixedWidth(centralWidget()->width());
    m_noteName->resize();
}

//##########        SLOTS       ###############

void MainWindow::createSettingsDialog() {
    SettingsDialog *settings = new SettingsDialog;
    if (settings->exec() == QDialog::Accepted) {
        m_score->acceptSettings();
        m_noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        m_noteName->setEnabledEnharmNotes(gl->showEnharmNotes);
        m_noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
        m_noteName->setAmbitus(gl->Gtune.lowest(),
                               Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber));
        noteWasClicked(0,m_noteName->getNoteName(0));//refresh name
        m_guitar->acceptSettings();;//refresh guitar
    }
}

void MainWindow::createExamSettingsDlg() {
    examSettingsDlg *examSettDlg = new examSettingsDlg;
    examSettDlg->exec();
}

void MainWindow::startExamSlot() {
    TexamExecutor ex = TexamExecutor();
}

void MainWindow::aboutSlot() {
    QMessageBox msg;
    msg.setText("<center><b>Nootka " + gl->version + tr("</b></center><p>This is developers preview of Nootka. It works quitely stable, but has less functioinality yet.</p><p>See a <a href=\"http://nootka.sourceforge.net\">program site</a> for more details and furter relaces.</p><p>Any bugs, sugestions, translations and so on, report to: <a href=\"mailto:seelook.gmail.com\">seelook@gmail.com</a><p/><p style=\"text-align: right;\">with respects<br>Author</p>"));
    msg.exec();
}

void MainWindow::noteWasClicked(int index, Tnote note) {
    if (gl->showEnharmNotes){
        TnotesList noteList;
        noteList.push_back(note);
        noteList.push_back(m_score->getNote(1));
        noteList.push_back(m_score->getNote(2));
        m_noteName->setNoteName(noteList);
    } else
        m_noteName->setNoteName(note);
    m_guitar->setFinger(note);
}

void MainWindow::noteNameWasChanged(Tnote note) {
    m_score->setNote(0, note);
    if (gl->showEnharmNotes) {
        m_score->setNote(1, m_noteName->getNoteName(1));
        m_score->setNote(2, m_noteName->getNoteName(2));
    }
    m_guitar->setFinger(note);
}

void MainWindow::guitarWasClicked(Tnote note) {
    if (gl->showEnharmNotes) {
        TnotesList noteList = note.getTheSameNotes(gl->doubleAccidentalsEnabled);
        m_noteName->setNoteName(noteList);
        m_score->setNote(1, m_noteName->getNoteName(1));
        m_score->setNote(2, m_noteName->getNoteName(2));
    } else
        m_noteName->setNoteName(note);
    m_score->setNote(0, note);

}

bool MainWindow::event(QEvent *event) {
    if (event->type() == QEvent::StatusTip) {
        QStatusTipEvent *se = static_cast<QStatusTipEvent *>(event);
        if (se->tip() == "")
            m_statLab->setText(hintText);
        else
            m_statLab->setText("<center>"+se->tip()+"</center>");
    }
    return QMainWindow::event(event);
}
