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
#include <QDebug>

Tglobals *gl = new Tglobals();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    statusBar()->showMessage(tr("Nootka ") + gl->version);

    setWindowIcon(QIcon(":/picts/nootka.svg"));

//    m_actMenu = menuBar()->addMenu(tr("actions"));
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
    scoreAndNameLay->addStretch(1);
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
    settingsAct = new QAction(tr("settings"),this);
    settingsAct->setStatusTip(tr("Application preferences"));
    settingsAct->setIcon(QIcon(":/picts/systemsettings.svg"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));
}

void MainWindow::createToolBar() {
    nootBar = addToolBar(tr("main toolbar"));
    nootBar->addAction(settingsAct);
}

void MainWindow::resizeEvent(QResizeEvent *) {
    m_score->setFixedWidth(centralWidget()->width()/2);
    m_guitar->setFixedHeight(centralWidget()->height()/3);
//    m_guitar->resize();
//    m_guitar->setFixedWidth(centralWidget()->width());
    m_noteName->resize();
}

//##########        SLOTS       ###############

void MainWindow::createSettingsDialog() {
    SettingsDialog *settings = new SettingsDialog;
    if (settings->exec() == QDialog::Accepted) {
        m_score->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        m_noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        m_score->setEnableKeySign(gl->keySignatureEnabled);
        if (!gl->doubleAccidentalsEnabled) m_score->clearNote(2);
        m_score->setEnableEnharmNotes(gl->showEnharmNotes);
        m_noteName->setEnabledEnharmNotes(gl->showEnharmNotes);
        if (gl->keySignatureEnabled) m_score->refreshKeySignNameStyle();
        m_noteName->setNoteNamesOnButt(gl->NnameStyleInNoteName);
        m_noteName->setAmbitus(gl->Gtune.lowest(),
                               Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber));
        m_score->setAmbitus(gl->Gtune.lowest(),
                            Tnote(gl->Gtune.highest().getChromaticNrOfNote()+gl->GfretsNumber));
        noteWasClicked(0,m_noteName->getNoteName(0));//refresh name
        m_guitar->settingsWasChanged();//refresh guitar
    }

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
//        for (int i=0; i<noteList.size(); i++)
//            qDebug() << QString::fromStdString(noteList[i].getName());
        m_noteName->setNoteName(noteList);
        m_score->setNote(1, m_noteName->getNoteName(1));
        m_score->setNote(2, m_noteName->getNoteName(2));
    } else
        m_noteName->setNoteName(note);
    m_score->setNote(0, note);

}
