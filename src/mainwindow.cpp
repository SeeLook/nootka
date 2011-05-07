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

#include "settings/settingsdialog.h"
#include "mainwindow.h"
#include "tglobals.h"
#include "examsettingsdlg.h"
#include "texamexecutor.h"
//#include <QDebug>

Tglobals *gl = new Tglobals();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    gl->path = Tglobals::getInstPath(qApp->applicationDirPath());

    statusBar()->showMessage("Nootka " + gl->version);

    setWindowIcon(QIcon(gl->path+"picts/nootka.svg"));

    QWidget *widget = new QWidget(this);
    m_score = new TscoreWidget(3,widget);
    m_noteName = new TnoteName(widget);
    m_guitar = new TfingerBoard(widget);
    setMinimumSize(640,480);
    setCentralWidget(widget);

    createActions();
    createToolBar();

    QVBoxLayout *mainLay = new QVBoxLayout;
    QHBoxLayout *scoreAndNameLay = new QHBoxLayout;
    scoreAndNameLay->addWidget(m_score);
//    scoreAndNameLay->addStretch(1);
    QVBoxLayout *nameLay = new QVBoxLayout;
    nameLay->addWidget(m_noteName);
    nameLay->addStretch(1);
    scoreAndNameLay->addLayout(nameLay);
    mainLay->addLayout(scoreAndNameLay);
    mainLay->addWidget(m_guitar);
    widget->setLayout(mainLay);

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
}

void MainWindow::createToolBar() {
    nootBar = addToolBar(tr("main toolbar"));
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
