#include "settingsdialog.h"
#include "mainwindow.h"
#include "tglobals.h"
#include <iostream>

Tglobals *gl = new Tglobals();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    statusBar()->showMessage(tr("Nootka ") + gl->version);

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
    m_noteName->resize();
}

//##########        SLOTS       ###############

void MainWindow::createSettingsDialog() {
    ScoreSettingsDlg *scoreSettDlg = new ScoreSettingsDlg();
    if (scoreSettDlg->exec() == QDialog::Accepted) {
        m_score->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        m_noteName->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        m_score->setEnableKeySign(gl->keySignatureEnabled);
        if (!gl->doubleAccidentalsEnabled) m_score->clearNote(2);
        m_score->setEnableEnharmNotes(gl->showEnharmNotes);
        m_noteName->setEnabledEnharmNotes(gl->showEnharmNotes);
        if (gl->keySignatureEnabled) m_score->refreshKeySignNameStyle();
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
}

void MainWindow::noteNameWasChanged(Tnote note) {
    m_score->setNote(0, note);
    if (gl->showEnharmNotes) {
        m_score->setNote(1, m_noteName->getNoteName(1));
        m_score->setNote(2, m_noteName->getNoteName(2));
    }
}
