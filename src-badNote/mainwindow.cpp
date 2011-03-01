//#include <QtGui>
#include "mainwindow.h"
#include "tglobals.h"

Tglobals *gl = new Tglobals();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    statusBar()->showMessage(tr("Nootka ") + gl->version);

    m_actMenu = menuBar()->addMenu(tr("actions"));

    m_score = new TscoreWidget(3);
    setMinimumSize(640,480);
    setCentralWidget(m_score);

    createActions();
    createMenus();

//    layout()->addWidget(score);
//    QHBoxLayout *scoreAndNameLay = new QHBoxLayout(this);
//    scoreAndNameLay->addWidget(score);

}

MainWindow::~MainWindow()
{
    delete gl;
}

void MainWindow::createMenus() {
    m_scoreMenu = menuBar()->addMenu(tr("score"));
    m_scoreMenu->addAction(showOherNotesAct);
    m_scoreMenu->addAction(useDblAccidAct);

}

void MainWindow::createActions() {
    showOherNotesAct = new QAction(tr("show enharmonic notes"),this);
    showOherNotesAct->setStatusTip(tr("Shows another enharmonic variants of a note"));
    useDblAccidAct= new QAction(tr("use double accidentals"),this);
    useDblAccidAct->setCheckable(true);
    useDblAccidAct->setChecked(gl->doubleAccidentalsEnabled);
    //    useDblaccidAct->setStatusTip(tr(""));
    connect(useDblAccidAct, SIGNAL(triggered(bool)), m_score, SLOT(enableDblAccidsSlot(bool)));
    m_score->contextMenu->addAction(showOherNotesAct);
    m_score->contextMenu->addAction(useDblAccidAct);
    connect(m_score, SIGNAL(enableDblAccWasChanged(bool)), this, SLOT(whenEnableDblAccStateChanged(bool)));

}


//##########        SLOTS       ###############

void MainWindow::whenEnableDblAccStateChanged(bool isEnabled) {
    useDblAccidAct->setChecked(isEnabled);
}
