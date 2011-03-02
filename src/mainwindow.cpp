#include "settingsdialog.h"
#include "mainwindow.h"
#include "tglobals.h"


Tglobals *gl = new Tglobals();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    statusBar()->showMessage(tr("Nootka ") + gl->version);

//    m_actMenu = menuBar()->addMenu(tr("actions"));
    QWidget *widget = new QWidget(this);
    m_score = new TscoreWidget(3,widget);
    m_noteName = new TnoteName(widget);
    setMinimumSize(640,480);
    setCentralWidget(widget);

    createActions();
//    createMenus();
    createToolBar();
    QHBoxLayout *scoreAndNameLay = new QHBoxLayout;
    scoreAndNameLay->addWidget(m_score);
    scoreAndNameLay->addWidget(m_noteName);
    widget->setLayout(scoreAndNameLay);



}

MainWindow::~MainWindow()
{
    delete gl;
}

void MainWindow::createActions() {
    settingsAct = new QAction(tr("settings"),this);
    settingsAct->setStatusTip(tr("Application preferences"));
    settingsAct->setIcon(QIcon(":/picts/settings.svg"));
    connect(settingsAct, SIGNAL(triggered()), this, SLOT(createSettingsDialog()));
}

void MainWindow::createToolBar() {
    nootBar = addToolBar(tr("main toolbar"));
    nootBar->addAction(settingsAct);
}

void MainWindow::resizeEvent(QResizeEvent *) {
//    m_score->setFixedWidth(width()/2);
    m_score->setFixedHeight(2*height()/3);
//    m_noteName->setFixedWidth(width()/2);
}

//##########        SLOTS       ###############

void MainWindow::createSettingsDialog() {
    ScoreSettingsDlg *scoreSettDlg = new ScoreSettingsDlg();
    if (scoreSettDlg->exec() == QDialog::Accepted) {
        m_score->setEnabledDblAccid(gl->doubleAccidentalsEnabled);
        m_score->setEnableKeySign(gl->keySignatureEnabled);
        m_score->setEnableEnharmNotes(gl->showEnharmNotes);
        if (gl->keySignatureEnabled) m_score->refreshKeySignNameStyle();
    }

}

//void MainWindow::whenEnableDblAccStateChanged(bool isEnabled) {
//    useDblAccidAct->setChecked(isEnabled);
//}

//void MainWindow::createMenus() {
//    m_scoreMenu = menuBar()->addMenu(tr("score"));
//    m_scoreMenu->addAction(showOherNotesAct);
//    m_scoreMenu->addAction(useDblAccidAct);

//}
//void MainWindow::createActions() {
//    showOherNotesAct = new QAction(tr("show enharmonic notes"),this);
//    showOherNotesAct->setStatusTip(tr("Shows another enharmonic variants of a note"));
//    useDblAccidAct= new QAction(tr("use double accidentals"),this);
//    useDblAccidAct->setCheckable(true);
//    useDblAccidAct->setChecked(gl->doubleAccidentalsEnabled);
//    //    useDblaccidAct->setStatusTip(tr(""));
//    connect(useDblAccidAct, SIGNAL(triggered(bool)), m_score, SLOT(enableDblAccidsSlot(bool)));
//    m_score->contextMenu->addAction(showOherNotesAct);
//    m_score->contextMenu->addAction(useDblAccidAct);
//    connect(m_score, SIGNAL(enableDblAccWasChanged(bool)), this, SLOT(whenEnableDblAccStateChanged(bool)));

//}
