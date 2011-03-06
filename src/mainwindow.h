#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "tscorewidget.h"
#include "tnotename.h"
#include "tfingerboard.h"
#include "tnote.h"
//#include <QtGui>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
//    void whenEnableDblAccStateChanged(bool isEnabled);
    void createSettingsDialog();

    void noteWasClicked(int index, Tnote note);
    void noteNameWasChanged(Tnote note);

protected:
    void resizeEvent(QResizeEvent *);

private:
    TscoreWidget *m_score;
    TnoteName *m_noteName;
    TfingerBoard *m_guitar;

//    QMenu *m_actMenu, *m_scoreMenu;
//    QAction *showOherNotesAct, *useDblAccidAct;
    QAction *settingsAct;
    QToolBar *nootBar;

//    void createMenus();
    void createToolBar();
    void createActions();

};

#endif // MAINWINDOW_H
