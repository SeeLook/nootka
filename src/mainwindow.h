#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <tscorewidget.h>
#include <QtGui>


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void whenEnableDblAccStateChanged(bool isEnabled);

private:
    TscoreWidget *m_score;
    QMenu *m_actMenu, *m_scoreMenu;
    QAction *showOherNotesAct, *useDblAccidAct;

    void createMenus();
    void createActions();

};

#endif // MAINWINDOW_H
