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
    m_score->setFixedWidth(centralWidget()->width()/2);
//    m_score->setFixedHeight(2*height()/3);
    m_guitar->setFixedHeight(centralWidget()->height()/3);
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
